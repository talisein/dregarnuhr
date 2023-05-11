#pragma once
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <span>
#include "jpeglib.h"
#include "jerror.h"
#include "log.h"

#include "outcome/result.hpp"
#include "outcome/utils.hpp"

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace std
{
  template <> struct is_error_code_enum<J_MESSAGE_CODE> : true_type
  {
  };
}

namespace detail
{
    class JpegErrc_category : public std::error_category
    {
    public:
        virtual const char *name() const noexcept override final { return "JpegError"; }
        virtual std::string message(int c) const override final;

        virtual std::error_condition default_error_condition(int c) const noexcept override final
        {
            switch (static_cast<J_MESSAGE_CODE>(c))
            {
                case JERR_NOTIMPL:
                    return make_error_condition(std::errc::function_not_supported);
                default:
                    return std::error_condition(c, *this);
            }
        }
    };
}

const detail::JpegErrc_category& JpegErrc_category();
std::error_code make_error_code(J_MESSAGE_CODE e);

namespace jpeg
{

    class compressor;

    class decompressor
    {
    public:
        decompressor(std::span<const unsigned char> src);

    private:
        struct decompress_deleter {
            void operator()(jpeg_decompress_struct *info) const {
                jpeg_destroy_decompress(info);
            }
        };

        std::unique_ptr<jpeg_decompress_struct, decompress_deleter> cinfo_p;
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        friend class compressor;
    };

    class compressor
    {
    public:
        compressor();

        outcome::result<std::span<const unsigned char>> compress_from(decompressor& decompressor, std::optional<int> quality, std::optional<int> scale);
        outcome::result<std::span<const unsigned char>> compress_rgb(JDIMENSION width, JDIMENSION height, std::span<unsigned char> buf, std::optional<int> quality);

    private:
        struct compress_deleter {
            void operator()(jpeg_compress_struct *info) const {
                jpeg_destroy_compress(info);
            }
        };

        std::unique_ptr<jpeg_compress_struct, compress_deleter> cinfo_p;
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        unsigned char *outbuf;
        unsigned long outsize;

        friend class decompressor;
    };

}
