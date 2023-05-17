#include <sstream>
#include <string_view>
#include <map>
#include "outcome/utils.hpp"
#include "utils.h"

#include "jpeg.h"

namespace
{
    using namespace std::string_view_literals;
#define JMESSAGE(code, string)  { code, #code },
    const std::map<J_MESSAGE_CODE, const std::string_view> _error_map {
#include "jerror.h"
    };

    std::string _format_message (j_common_ptr cinfo) {
        char buf[JMSG_LENGTH_MAX + 1] = {0};
        jpeg_error_mgr *err = cinfo->err;
        err->format_message(cinfo, buf);
        return buf;
    }

    extern "C"
    {
        void _error_exit (j_common_ptr cinfo)
        {
            std::system_error e(static_cast<J_MESSAGE_CODE>(cinfo->err->msg_code), _format_message(cinfo));
            throw e;
        }

        void _output_message (j_common_ptr cinfo)
        {
            log_error("JPEG ", cinfo->is_decompressor ? "decompression" : "compression", ": ", _format_message(cinfo));
        }

        void _emit_message (j_common_ptr cinfo, int msg_level)
        {
            jpeg_error_mgr *err = cinfo->err;
            if (msg_level < 0) [[unlikely]] { // Warning
                if (err->num_warnings == 0) {
                    log_info("JPEG Warning: ", _format_message(cinfo));
                }
                ++err->num_warnings;
            } else if (err->trace_level >= msg_level) [[unlikely]] {
                log_info("JPEG Info: ", _format_message(cinfo));
            }
        }
    }

}

std::string
detail::JpegErrc_category::message(int c) const
{
    auto it = _error_map.find(static_cast<J_MESSAGE_CODE>(c));
    if (it != _error_map.end()) {
        return std::string(it->second);
    } else {
        return utils::strcat("Unknown JPEG error "sv, c);
    }
}

const detail::JpegErrc_category& JpegErrc_category()
{
  static detail::JpegErrc_category c;
  return c;
}

std::error_code make_error_code(J_MESSAGE_CODE e)
{
  return {static_cast<int>(e), JpegErrc_category()};
}

namespace jpeg
{
    compressor::compressor() :
        outbuf(nullptr),
        outsize(0)
    {
        cinfo.err = jpeg_std_error(&jerr);
        cinfo.err->error_exit = _error_exit;
        cinfo.err->output_message = _output_message;
        cinfo.err->emit_message = _emit_message;
        cinfo.client_data = this;
        jpeg_create_compress(&cinfo);
        cinfo_p.reset(&cinfo);
        jpeg_mem_dest(&cinfo, &outbuf, &outsize);
    }
    outcome::result<std::span<const unsigned char>>
    compressor::compress_from(decompressor& decompressor, std::optional<int> quality, std::optional<int> scale)
    {
        try {
            jpeg_read_header(&decompressor.cinfo, TRUE);
            if (scale) {
                decompressor.cinfo.scale_denom = scale.value();
            }
            jpeg_calc_output_dimensions(&decompressor.cinfo);

            cinfo.image_width = decompressor.cinfo.output_width;
            cinfo.image_height = decompressor.cinfo.output_height;
            if (scale) {
                log_verbose("Scaling from ", decompressor.cinfo.image_height, "x", decompressor.cinfo.image_width, " to ",decompressor.cinfo.output_height, "x", decompressor.cinfo.output_width);
            }
            cinfo.input_components = decompressor.cinfo.output_components;
            cinfo.in_color_space = decompressor.cinfo.out_color_space;
            jpeg_set_defaults(&cinfo);
            cinfo.optimize_coding = TRUE;

            if (quality)
                jpeg_set_quality(&cinfo, quality.value(), TRUE);
            jpeg_start_decompress(&decompressor.cinfo);
            jpeg_start_compress(&cinfo, TRUE);
            const auto row_stride = decompressor.cinfo.output_width * decompressor.cinfo.output_components;
            auto buffer = (*decompressor.cinfo.mem->alloc_sarray) (reinterpret_cast<j_common_ptr>(&decompressor.cinfo), JPOOL_IMAGE, row_stride, 1);
            while (decompressor.cinfo.output_scanline < decompressor.cinfo.output_height) {
                auto dim_read = jpeg_read_scanlines(&decompressor.cinfo, buffer, 1);
                auto dim_wrote [[maybe_unused]] = jpeg_write_scanlines(&cinfo, buffer, dim_read);
            }
            jpeg_finish_decompress(&decompressor.cinfo);
            jpeg_finish_compress(&cinfo);
            return std::span<const unsigned char>(outbuf, outsize);
        } catch (...) {
            return outcome::error_from_exception();
        }
    }


    outcome::result<std::span<const unsigned char>>
    compressor::compress_rgb(JDIMENSION width, JDIMENSION height, std::span<JSAMPLE> buf, std::optional<int> quality)
    {
        try {
            cinfo.image_width = width;
            cinfo.image_height = height;
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            jpeg_set_defaults(&cinfo);
            cinfo.optimize_coding = TRUE;

            if (quality)
                jpeg_set_quality(&cinfo, quality.value(), TRUE);
            jpeg_start_compress(&cinfo, TRUE);
            const auto row_stride = cinfo.image_width * cinfo.input_components;

            auto rows = std::make_unique<JSAMPROW[]>(height);
            for (JDIMENSION i = 0; i < height; ++i) {
                const auto start = buf.data();
                auto p = start + row_stride * i;
                rows[i] = p;
            }

            auto res = jpeg_write_scanlines(&cinfo, rows.get(), height);
            if (res != height) {
                log_error("compress_rgb: write_scanlines wrote only ", res, " lines, whigh is less than height ", height);
                return std::errc::io_error;
            }

            jpeg_finish_compress(&cinfo);
            return std::span<const unsigned char>(outbuf, outsize);

        } catch (...) {
            return outcome::error_from_exception();
        }
    }

    decompressor::decompressor(std::span<const unsigned char> src)
    {
        cinfo.err = jpeg_std_error(&jerr);
        cinfo.err->error_exit = _error_exit;
        cinfo.err->output_message = _output_message;
        cinfo.err->emit_message = _emit_message;
        cinfo.client_data = this;
        jpeg_create_decompress(&cinfo);
        cinfo_p.reset(&cinfo);
        jpeg_mem_src(&cinfo, src.data(), utils::safe_int_cast<unsigned long>(src.size_bytes()));
    }

}
