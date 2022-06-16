#pragma once

#include <filesystem>
#include <system_error> // bring in std::error_code et al
#include "miniz.h"
#include "outcome/result.hpp"

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

        bool is_readable();

        result<void> print_files();

        result<mz_uint> locate_file(const char *filename);

        result<std::string> extract(mz_uint idx, mz_uint flags = 0);
        result<std::basic_string<unsigned char>> extract_raw(mz_uint idx, mz_uint flags = 0);
        result<zipstreambuf> extract_stream(mz_uint idx, mz_uint flags = 0);

        result<mz_zip_archive_file_stat> stat(mz_uint idx);

    private:
        struct archive_deleter {
            void operator ()(mz_zip_archive *zip) const noexcept;
        };
        using zip_ptr = std::unique_ptr<mz_zip_archive, archive_deleter>;

        fs::path path;
        mz_zip_archive zip;
        zip_ptr p_zip;
    };
}
