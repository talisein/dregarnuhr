#include <type_traits>
#include <iostream>
#include <cstdlib>
#include "zip.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"

namespace {
    extern "C" {
        size_t
        _extract_to_std_string(void *opaque, mz_uint64, const void *buf, size_t n)
        {
            auto str = static_cast<std::string*>(opaque);
            try {
                str->append(static_cast<const char*>(buf), n);
                return n;
            } catch (std::exception &e) {
                return 0;
            }
        }

        size_t
        _extract_to_std_basic_string_uchar(void *opaque, mz_uint64, const void *buf, size_t n)
        {
            auto str = static_cast<std::basic_string<unsigned char>*>(opaque);
            try {
                str->append(static_cast<const unsigned char*>(buf), n);
                return n;
            } catch (std::exception &e) {
                return 0;
            }
        }
    }

}

const detail::MzZipErrc_category& MzZipErrc_category()
{
  static detail::MzZipErrc_category c;
  return c;
}

std::error_code make_error_code(mz_zip_error e)
{
  return {static_cast<int>(e), MzZipErrc_category()};
}

namespace zip
{
    reader::reader(const fs::path& filename)
    {
        mz_zip_zero_struct(&zip);
        auto filename_string = filename.string(); // Windows' c_str() returns wchar_t*
        if (MZ_FALSE == mz_zip_reader_init_file(&zip, filename_string.c_str(), 0)) [[unlikely]] {
            mz_zip_error err = mz_zip_get_last_error(&zip);
            throw std::system_error(err);
        }
        p_zip.reset(&zip);
    }

    bool
    reader::is_readable()
    {
        auto num_files = mz_zip_reader_get_num_files(&zip);
        for (decltype(num_files) i = 0; i < num_files; ++i) {
            if (MZ_TRUE != mz_zip_validate_archive(&zip, MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY))
                return false;
        }

        return true;
    }

    result<void>
    reader::print_files()
    {
        auto num_files = mz_zip_reader_get_num_files(&zip);
        for (decltype(num_files) i = 0U; i < num_files; ++i) {
            OUTCOME_TRY(auto st, stat(i));
            std::cout << "File: " << st.m_filename << "\n";
        }

        return outcome::success();
    }

    result<mz_uint>
    reader::locate_file(const char *filename)
    {
        mz_uint32 idx;
        auto found = mz_zip_reader_locate_file_v2(&zip, filename, nullptr, 0, &idx);
        if (!found) {
            auto err = mz_zip_get_last_error(&zip);
            if (err != MZ_ZIP_NO_ERROR) {
                return outcome::failure(err);
            } else {
                return std::errc::no_such_file_or_directory;
            }
        }

        return outcome::success(idx);
    }

    result<std::string>
    reader::extract(mz_uint idx, mz_uint flags)
    {
        OUTCOME_TRY(auto st, stat(idx));

        std::string res;
        try {
            res.reserve(st.m_uncomp_size);
        } catch (std::exception &e) {
            return outcome::error_from_exception();
        }

        if (!mz_zip_reader_extract_to_callback(&zip, idx, &_extract_to_std_string,
                                               &res, flags)) {
            return mz_zip_get_last_error(&zip);
        } else {
            return res;
        }
    }

    result<std::basic_string<unsigned char>>
    reader::extract_raw(mz_uint idx, mz_uint flags)
    {
        OUTCOME_TRY(auto st, stat(idx));

        std::basic_string<unsigned char> res;
        try {
            res.reserve(st.m_uncomp_size);
        } catch (std::exception &e) {
            return outcome::error_from_exception();
        }

        if (!mz_zip_reader_extract_to_callback(&zip, idx, &_extract_to_std_basic_string_uchar,
                                               &res, flags)) {
            return mz_zip_get_last_error(&zip);
        } else {
            return res;
        }
    }

    zipstreambuf::zipstreambuf(mz_zip_archive *pZip, mz_uint idx, mz_uint flags) :
        iter(extract_iter_ptr(mz_zip_reader_extract_iter_new(pZip, idx, flags)))
    {
        if (nullptr == iter) {
            throw std::system_error(mz_zip_get_last_error(pZip));
        }
    }

    std::streambuf::int_type
    zipstreambuf::underflow()
    {
        if (iter->comp_remaining == 0 && iter->out_blk_remain == 0) return traits_type::eof();

        if ((iter->flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!iter->file_stat.m_method)) {
            if (iter->read_buf_avail == 0) {
                char unused[1];
                mz_zip_reader_extract_iter_read(iter.get(), unused, 0);
            }
        } else {
            if (iter->out_blk_remain == 0) {
                char unused[1];
                mz_zip_reader_extract_iter_read(iter.get(), unused, 0);
            }
        }

        if ((iter->flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!iter->file_stat.m_method)) {
            // peek readbuf
            return traits_type::to_int_type(*(char*)iter->pRead_buf);
        } else {
            // peek writebuf
            auto buf = (mz_uint8*)iter->pWrite_buf + (iter->out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
            return traits_type::to_int_type(*buf);
        }
    }

    std::streambuf::int_type
    zipstreambuf::uflow()
    {
        if (iter->comp_remaining == 0 && iter->out_blk_remain == 0) return traits_type::eof();

        char buf[1];
        auto res = mz_zip_reader_extract_iter_read(iter.get(), buf, 1);
        if (1 != res) {
            throw std::system_error(mz_zip_get_last_error(iter->pZip));
        }
        return traits_type::to_int_type(buf[0]);
    }

    std::streamsize
    zipstreambuf::showmanyc()
    {
        if (iter->comp_remaining == 0 && iter->out_blk_remain == 0) return -1;
        if (iter->out_blk_remain == 0) {
            char unused[1];
            mz_zip_reader_extract_iter_read(iter.get(), unused, 0);
        }
        return iter->out_blk_remain;
    }

    std::streamsize
    zipstreambuf::xsgetn( char* s, std::streamsize count )
    {
        auto res = mz_zip_reader_extract_iter_read(iter.get(), s, count);
        return res;
    }

    result<zipstreambuf>
    reader::extract_stream(mz_uint idx, mz_uint flags)
    {
        try {
            return zipstreambuf(&zip, idx, flags);
        } catch (std::exception &e) {
            return outcome::error_from_exception();
        }
    }

    result<mz_zip_archive_file_stat>
    reader::stat(mz_uint idx)
    {
        mz_zip_archive_file_stat st;
        if (MZ_FALSE == mz_zip_reader_file_stat(&zip, idx, &st)) {
            return mz_zip_get_last_error(&zip);
        }
        return st;
    }

    void
    reader::free_deleter::operator()(char *p) const noexcept
    {
        std::free(p);
    }

    void
    reader::archive_deleter::operator()(mz_zip_archive *zip) const noexcept
    {
        if (!mz_zip_end(zip)) {
            std::system_error err {mz_zip_get_last_error(zip)};
            std::cerr << "Unable to close zip_reader: " << err.code()
                      << ' ' << err.what() << '\n';
        }
    }
}
