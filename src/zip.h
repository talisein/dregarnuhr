#pragma once

#include <list>
#include <filesystem>
#include <fstream>
#include <system_error> // bring in std::error_code et al
#include <span>
#include "miniz.h"
#include "outcome/result.hpp"
#include "outcome/utils.hpp"

namespace outcome = OUTCOME_V2_NAMESPACE;
using outcome::result;

namespace std
{
  template <> struct is_error_code_enum<mz_zip_error> : true_type
  {
  };
}

namespace detail
{
    class MzZipErrc_category : public std::error_category
    {
    public:
        virtual const char *name() const noexcept override final { return "MiniZipError"; }
        virtual std::string message(int c) const override final
        {
            return mz_zip_get_error_string(static_cast<mz_zip_error>(c));
        }

        virtual std::error_condition default_error_condition(int c) const noexcept override final
        {
            switch (static_cast<mz_zip_error>(c))
            {
                case MZ_ZIP_FILE_TOO_LARGE:
                    return make_error_condition(std::errc::file_too_large);
                case MZ_ZIP_ALLOC_FAILED:
                    return make_error_condition(std::errc::not_enough_memory);
                case MZ_ZIP_FILE_NOT_FOUND:
                    return make_error_condition(std::errc::no_such_file_or_directory);
                default:
                    return std::error_condition(c, *this);
            }
        }
    };
}

const detail::MzZipErrc_category& MzZipErrc_category();
std::error_code make_error_code(mz_zip_error e);

namespace zip
{
    namespace fs = std::filesystem;
    struct archive_deleter {
        void operator ()(mz_zip_archive *zip) const noexcept;
    };
    using zip_ptr = std::unique_ptr<mz_zip_archive, archive_deleter>;


    class zipstreambuf : public std::streambuf {
    public:
        zipstreambuf(mz_zip_archive *pZip, mz_uint idx, mz_uint flags);

    protected:
        virtual int_type underflow() override final;
        virtual int_type uflow() override final;
        virtual std::streamsize showmanyc() override final;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override final;
    private:
        struct _extract_iter_deleter
        {
            void operator()(mz_zip_reader_extract_iter_state* state) const noexcept {
                mz_zip_reader_extract_iter_free(state);
            }
        };
        using extract_iter_ptr = std::unique_ptr<mz_zip_reader_extract_iter_state, _extract_iter_deleter>;
        extract_iter_ptr iter;
    };

    class reader
    {
    public:
        reader(const fs::path& filename);

        result<bool> is_readable();

        result<void> print_files();

        result<mz_uint> locate_file(const char *filename);

        result<std::string> extract(mz_uint idx, mz_uint flags = 0);
        result<std::basic_string<unsigned char>> extract_raw(mz_uint idx, mz_uint flags = 0);
        result<zipstreambuf> extract_stream(mz_uint idx, mz_uint flags = 0);
        inline result<void> extract_to_cb(mz_uint idx, mz_file_write_func pCallback, void *pOpaque, mz_uint flags = 0) {
            try {
                auto res = mz_zip_reader_extract_to_callback(&zip, idx, pCallback, pOpaque, flags);
                if (MZ_FALSE == res) {
                    return mz_zip_get_last_error(&zip);
                }
            } catch (std::system_error &e) {
                return e.code();
            } catch (...) {
                return outcome::error_from_exception();
            }
            return outcome::success();
        }
        result<mz_zip_archive_file_stat> stat(mz_uint idx);
        result<std::list<std::string>> get_files();
    private:
        fs::path path;
        mz_zip_archive zip;
        zip_ptr p_zip;
        std::ifstream istream;
    };

    class writer
    {
    public:
        writer(const fs::path& filename);

        result<void> copy_from(reader& reader,
                               const std::string& src_filename,
                               const std::string& dst_filename);

        template <typename T>
        result<void> add(const std::string& filename,
                         std::span<const T> data)
        {
            try {
                if (MZ_FALSE == mz_zip_writer_add_mem(&zip, filename.c_str(), data.data(), data.size_bytes(), 8)) {
                    auto err = mz_zip_get_last_error(&zip);
                    return err;
                }
            } catch (std::system_error &e) {
                return e.code();
            } catch (...) {
                return outcome::error_from_exception();
            }

            return outcome::success();
        }

        result<void> finish();

    private:
        fs::path path;
        mz_zip_archive zip;
        zip_ptr p_zip;
        std::ofstream ostream;
    };
}
