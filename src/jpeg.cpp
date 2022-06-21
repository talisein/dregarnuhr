#include <sstream>
#include <string_view>
#include <map>
#include "outcome/utils.hpp"

#include "jpeg.h"

namespace
{
    using namespace std::string_view_literals;
    typedef std::pair<const std::string_view, const std::string_view> _pair_t;
#define JCOPYRIGHT_SHORT "Copyright (C) @COPYRIGHT_YEAR@ The libjpeg-turbo Project and many others"
#define JCOPYRIGHT "Copyright (C) @COPYRIGHT_YEAR@ The libjpeg-turbo Project and many others"
#define JVERSION "8d  15-Jan-2012"
#define JMESSAGE(code, string)  { code, std::make_pair(#code, string) },
    const std::map<J_MESSAGE_CODE, const _pair_t> _error_map {
#include "jerror.h"
    };
#undef JMESSAGE
#undef JVERSION
#undef JCOPYRIGHT
#undef JCOPYRIGHT_SHORT


    extern "C"
    {
        void _error_exit (j_common_ptr cinfo)
        {
            jpeg_error_mgr *err = cinfo->err;
            std::system_error e(static_cast<J_MESSAGE_CODE>(err->msg_code));
            throw e;
        }

        void _output_message (j_common_ptr cinfo)
        {
            jpeg_error_mgr *err = cinfo->err;
            log_error("JPEG ", cinfo->is_decompressor ? "decompression" : "compression", " error: ", err->last_jpeg_message);
        }
        void _format_message (j_common_ptr cinfo, char *buffer);
        void _emit_message (j_common_ptr cinfo, int msg_level);
    }

}

std::string
detail::JpegErrc_category::message(int c) const
{
    std::stringstream ss;
    auto it = _error_map.find(static_cast<J_MESSAGE_CODE>(c));
    if (it != _error_map.end()) {
        ss << it->second.first << ": " << it->second.second;
    } else {
        ss << "Unknown JPEG error " << c;
    }
    return ss.str();
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
                log_info("Scaling from ", decompressor.cinfo.image_height, "x", decompressor.cinfo.image_width, " to ",decompressor.cinfo.output_height, "x", decompressor.cinfo.output_width);
                std::cout << std::flush;
            }
            cinfo.input_components = decompressor.cinfo.output_components;
            cinfo.in_color_space = decompressor.cinfo.out_color_space;
            jpeg_set_defaults(&cinfo);
            if (quality)
                jpeg_set_quality(&cinfo, quality.value(), TRUE);
            jpeg_start_decompress(&decompressor.cinfo);
            jpeg_start_compress(&cinfo, TRUE);
            const auto row_stride = decompressor.cinfo.output_width * decompressor.cinfo.output_components;
            auto buffer = (*decompressor.cinfo.mem->alloc_sarray) ((j_common_ptr)&decompressor.cinfo, JPOOL_IMAGE, row_stride, 1);
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
    decompressor::decompressor(std::span<const unsigned char> src)
    {
        cinfo.err = jpeg_std_error(&jerr);
        cinfo.err->error_exit = _error_exit;
        cinfo.err->output_message = _output_message;
        cinfo.client_data = this;
        jpeg_create_decompress(&cinfo);
        cinfo_p.reset(&cinfo);
        jpeg_mem_src(&cinfo, src.data(), src.size_bytes());
    }

}
