#include <type_traits>
#include <utility>
#include <iostream>
#include <cstdlib>
#include "zip.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"
#include "log.h"
#include "utils.h"

namespace {
    static constexpr time_t ZIP_TIME_UNSET = 312796800;

    extern "C" {
        size_t
        _extract_to_std_string(void *opaque, mz_uint64, const void *buf, size_t n)
        {
            auto str = static_cast<std::string*>(opaque);
            try {
                str->append(static_cast<const char*>(buf), n);
                return n;
            } catch (...) {
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
            } catch (...) {
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
    namespace {
        size_t _zip_file_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
        {
            auto istream = static_cast<std::ifstream*>(pOpaque);
            auto desired_pos = std::ifstream::traits_type::pos_type(file_ofs);
            auto cur_ofs = istream->tellg();
            if (std::cmp_not_equal(desired_pos - cur_ofs, 0)) {
                istream->seekg(desired_pos);
            }
            istream->read(static_cast<char *>(pBuf), n);
            return istream->gcount();
        }
    }
    reader::reader(const fs::path& filename) :
        path(filename)
    {
        auto file_size = std::filesystem::file_size(filename);
        mz_zip_zero_struct(&zip);
        istream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        istream.open(filename, std::ios_base::in | std::ios_base::binary);
        zip.m_pRead = _zip_file_read_func;
        zip.m_pIO_opaque = &istream;
        if (MZ_FALSE == mz_zip_reader_init(&zip, file_size, 0)) [[unlikely]] {
            mz_zip_error err = mz_zip_get_last_error(&zip);
            throw std::system_error(err);
        }
        p_zip.reset(&zip);
    }

    result<bool>
    reader::is_readable()
    {
        try {
            auto num_files = mz_zip_reader_get_num_files(&zip);
            auto err = mz_zip_get_last_error(&zip);
            if (err != MZ_ZIP_NO_ERROR) {
                return outcome::failure(err);
            }
            for (decltype(num_files) i = 0; i < num_files; ++i) {
                if (MZ_TRUE != mz_zip_validate_archive(&zip, MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY))
                    return false;
            }

            return true;
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    result<void>
    reader::print_files()
    {
        try {
            auto num_files = mz_zip_reader_get_num_files(&zip);
            for (decltype(num_files) i = 0U; i < num_files; ++i) {
                OUTCOME_TRY(auto st, stat(i));
                std::cout << "File: " << st.m_filename << "\n";
            }
            return outcome::success();
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }
    result<std::list<std::string>>
    reader::get_files()
    {
        try {
            std::list<std::string> res;
            auto num_files = mz_zip_reader_get_num_files(&zip);
            for (decltype(num_files) i = 0U; i < num_files; ++i) {
                OUTCOME_TRY(auto st, stat(i));
                res.push_back(st.m_filename);
            }
            return res;
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    result<mz_uint>
    reader::locate_file(const char *filename)
    {
        try {
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
        } catch (std::system_error& e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    result<std::string>
    reader::extract(mz_uint idx, mz_uint flags)
    {
        OUTCOME_TRY(auto st, stat(idx));

        std::string res;
        try {
            res.reserve(st.m_uncomp_size);
            if (!mz_zip_reader_extract_to_callback(&zip, idx, &_extract_to_std_string,
                                                   &res, flags)) {
                return mz_zip_get_last_error(&zip);
            } else {
                return res;
            }
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    result<std::basic_string<unsigned char>>
    reader::extract_raw(mz_uint idx, mz_uint flags)
    {
        OUTCOME_TRY(auto st, stat(idx));

        std::basic_string<unsigned char> res;
        try {
            res.reserve(st.m_uncomp_size);
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }

        try {
            if (!mz_zip_reader_extract_to_callback(&zip, idx, &_extract_to_std_basic_string_uchar,
                                                   &res, flags)) {
                return mz_zip_get_last_error(&zip);
            } else {
                return res;
            }
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
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
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    result<mz_zip_archive_file_stat>
    reader::stat(mz_uint idx)
    {
        mz_zip_archive_file_stat st{};
        try {
            if (MZ_FALSE == mz_zip_reader_file_stat(&zip, idx, &st)) {
                return mz_zip_get_last_error(&zip);
            }
            return st;
        } catch (std::system_error &e) {
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    namespace {
        size_t _file_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
        {
            auto ostream = static_cast<std::ofstream*>(pOpaque);
            auto cur_pos = ostream->tellp();
            auto desired_pos = std::ifstream::traits_type::pos_type(file_ofs);
            if (std::cmp_not_equal(desired_pos - cur_pos, 0)) {
                ostream->seekp(file_ofs);
            }
            ostream->write(static_cast<const char *>(pBuf), n);
            return n; // an exception throws if less than n
        }
    }
    writer::writer(const fs::path& filename) :
        path(filename)
    {
        mz_zip_zero_struct(&zip);
        ostream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ostream.open(filename, std::ios_base::out | std::ios_base::binary);
        zip.m_pWrite = _file_write_func;
        zip.m_pIO_opaque = &ostream;
        if (MZ_FALSE == mz_zip_writer_init(&zip, 0)) [[unlikely]] {
            mz_zip_error err = mz_zip_get_last_error(&zip);
            throw std::system_error(err);
        }
        p_zip.reset(&zip);
    }

    namespace {
        void _log_level_and_flags(std::string_view filename, mz_zip_archive_file_stat stat)
        {
            mz_uint16 flags = stat.m_bit_flag;
            std::stringstream ss;
            mz_uint level = flags & 0xF;
            if (flags & MZ_ZIP_FLAG_IGNORE_PATH) ss << "MZ_ZIP_FLAG_IGNORE_PATH" << ' ';
            if (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ss << "MZ_ZIP_FLAG_COMPRESSED_DATA ";
            if (flags & MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) ss << "MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY ";
            if (flags & MZ_ZIP_FLAG_VALIDATE_LOCATE_FILE_FLAG) ss << "MZ_ZIP_FLAG_VALIDATE_LOCATE_FILE_FLAG ";
            if (flags & MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY) ss << "MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY ";
            if (flags & MZ_ZIP_FLAG_WRITE_ZIP64) ss << "MZ_ZIP_FLAG_WRITE_ZIP64 ";
            if (flags & MZ_ZIP_FLAG_WRITE_ALLOW_READING) ss << "MZ_ZIP_FLAG_WRITE_ALLOW_READING ";
            if (flags & MZ_ZIP_FLAG_ASCII_FILENAME) ss << "MZ_ZIP_FLAG_ASCII_FILENAME ";
            if (flags & MZ_ZIP_FLAG_WRITE_HEADER_SET_SIZE) ss << "MZ_ZIP_FLAG_WRITE_HEADER_SET_SIZE ";
            log_verbose("File: ", filename, " Level: ", level, " Flags: 0x", std::hex, (flags & 0xF0), std::dec, " ", ss.view(),
                        " crc: ", std::hex, stat.m_crc32, std::dec, " comp_size: ", stat.m_comp_size, " uncomp_size: ", stat.m_uncomp_size,
                        " method: ", stat.m_method, " external_attr: ", std::hex, stat.m_external_attr, std::dec,
                        " modified: ", stat.m_time, " hex modified: ", std::hex, stat.m_time, std::dec);
        }
    }


    result<void>
    writer::copy_from(reader& reader,
                      const std::string& src_filename,
                      const std::string& dst_filename)
    {
        OUTCOME_TRY(auto src_idx, reader.locate_file(src_filename.c_str()));
        OUTCOME_TRY(auto stat, reader.stat(src_idx));
        _log_level_and_flags(src_filename, stat);
        auto level = stat.m_bit_flag & 0xF;
        if (0 == stat.m_method)
            level = 0;
        OUTCOME_TRY(auto buf, reader.extract(src_idx, (0 == level) ? 0 : MZ_ZIP_FLAG_COMPRESSED_DATA));

        try {
            time_t modified = stat.m_time;
            auto res = mz_zip_writer_add_mem_ex_v2(&zip,
                                                   dst_filename.c_str(),
                                                   buf.data(),
                                                   buf.size(),
                                                   stat.m_comment,
                                                   utils::safe_int_cast<mz_uint16>(stat.m_comment_size),
                                                   0 == level ? 0 : (level | MZ_ZIP_FLAG_COMPRESSED_DATA),
                                                   0 == level ? 0 : stat.m_uncomp_size,
                                                   0 == level ? 0 : stat.m_crc32,
                                                   modified == ZIP_TIME_UNSET ? NULL : &modified,
                                                   nullptr, 0, nullptr, 0);
            if (modified != stat.m_time) {
                log_verbose("Writer changed the modified time?? from ", stat.m_time, " to ", modified);
            }
            if (MZ_FALSE == res) {
                std::system_error e(mz_zip_get_last_error(&zip));
                log_error("returned false: ", e.code(), ' ', e.what());
                return mz_zip_get_last_error(&zip);
            }
        } catch (std::system_error& e) {
            log_error("Exception: ", e.what());
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }

        return outcome::success();
    }

    result<void>
    writer::finish()
    {
        try {
            if (MZ_FALSE == mz_zip_writer_finalize_archive(&zip)) {
                auto err = mz_zip_get_last_error(&zip);
                std::system_error e(err);
                log_error("Finalization error: ", e.code(), ' ', e.what());
                return err;
            }
        } catch (std::system_error& e) {
            log_error("Exception: ", e.what());
            return e.code();
        } catch (...) {
            return outcome::error_from_exception();
        }
        return outcome::success();
    }

    void
    archive_deleter::operator()(mz_zip_archive *zip) const noexcept
    {
        if (!mz_zip_end(zip)) {
            std::system_error err {mz_zip_get_last_error(zip)};
            std::cerr << "Unable to close zip_reader: " << err.code()
                      << ' ' << err.what() << '\n';
        }
    }
}
