#include <algorithm>
#include <cstring>
#include "png_reader.h"
#include "log.h"
#include "utils.h"

using namespace std::string_view_literals;

png::reader::reader(std::span<const unsigned char> src) :
    src(src),
    image(),
    imagep(&image)
{
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    image.opaque = nullptr;
}

bool
png::reader::is_grayscale() const
{
    return !(image.format & PNG_FORMAT_FLAG_COLOR);
}

bool
png::reader::is_linear() const
{
    return image.format & PNG_FORMAT_FLAG_LINEAR;
}

bool
png::reader::is_colormapped() const
{
    return image.format & PNG_FORMAT_FLAG_COLORMAP;
}

bool
png::reader::has_alpha() const
{
    return image.format & PNG_FORMAT_FLAG_ALPHA;
}

png_uint_32
png::reader::get_height() const
{
    return image.height;
}

png_uint_32
png::reader::get_width() const
{
    return image.width;
}

std::string_view
png::reader::get_format() const
{
    switch (image.format)
    {
        case PNG_FORMAT_GRAY:
            return "PNG_FORMAT_GRAY"sv;
        case PNG_FORMAT_GA:
            return "PNG_FORMAT_GA"sv;
        case PNG_FORMAT_AG:
            return "PNG_FORMAT_AG"sv;
        case PNG_FORMAT_LINEAR_Y:
            return "PNG_FORMAT_LINEAR_Y"sv;
        case PNG_FORMAT_LINEAR_Y_ALPHA:
            return "PNG_FORMAT_LINEAR_Y_ALPHA"sv;
        case PNG_FORMAT_RGB:
            return "PNG_FORMAT_RGB"sv;
        case PNG_FORMAT_RGBA:
            return "PNG_FORMAT_RGBA"sv;
        case PNG_FORMAT_LINEAR_RGB:
            return "PNG_FORMAT_LINEAR_RGB"sv;
        case PNG_FORMAT_LINEAR_RGB_ALPHA:
            return "PNG_FORMAT_LINEAR_RGB_ALPHA"sv;
        case PNG_FORMAT_RGB_COLORMAP:
            return "PNG_FORMAT_RGB_COLORMAP"sv;
        case PNG_FORMAT_RGBA_COLORMAP:
            return "PNG_FORMAT_RGBA_COLORMAP"sv;
        default:
            return "oopsie"sv;
    };

    return "oops"sv;
}

outcome::result<std::span<unsigned char>>
png::reader::read()
{
    auto log_png_error = [this]() {
        if (0 != image.warning_or_error) {
            decltype(image.warning_or_error) flag = image.warning_or_error & 0x3;
            if (flag == 1) {
                log_verbose("warning: libpng: ", static_cast<std::decay_t<decltype(image.message)>>(image.message));
            } else if (flag == 2) {
                log_error("libpng: ", static_cast<std::decay_t<decltype(image.message)>>(image.message));
                return true;
            } else if (flag == 3) {
                log_error("libpng: (warning before error) ", static_cast<std::decay_t<decltype(image.message)>>(image.message));
                return true;
            }
        }
        return false;
    };

    auto res = png_image_begin_read_from_memory(&image, src.data(), src.size_bytes());
    if (log_png_error()) {
        // TODO: error codes
        return std::errc::io_error;
    }

    log_verbose("res: ", res, ". This image is ", image.width, " wide ", image.height, " height. Grayscale: ",
             is_grayscale(), " Linear: ", is_linear(), " Alpha: ", has_alpha(),
             " Colormapped: ", is_colormapped(), " Format: ", get_format());
    image.format = PNG_FORMAT_RGB;

    const auto row_stride = PNG_IMAGE_ROW_STRIDE(image);
    const auto image_size = PNG_IMAGE_SIZE(image);
    out_datap = std::make_unique<unsigned char[]>(image_size);
    const png_color_struct background {0xFF, 0xFF, 0xFF};

    // alloc buffer ??
    // alloc color map???
    res = png_image_finish_read(&image, &background, out_datap.get(), row_stride, nullptr);
    if (log_png_error()) {
        out_datap.reset();
        return std::errc::io_error;
    }
    return std::span(out_datap.get(), image_size);
}

outcome::result<std::span<unsigned char>>
png::reader::scale(int scale)
{
    if (!out_datap) {
        return std::errc::invalid_argument;
    }
    const auto new_width = get_width() / scale;
    const auto old_width_limit = new_width * scale;
    const auto new_height = get_height() / scale;
    const auto old_height_limit = new_height * scale;
    const auto channels = PNG_IMAGE_PIXEL_CHANNELS(image.format);
    const auto old_stride_bytes = static_cast<std::size_t>(get_width()) * channels;
    unsigned char* out_p = out_datap.get();

    /* Box Sampling:
       If scale = 3, for a 3x3 image:
       [rgb rgb rgb
        rgb rgb rgb
        rgb rgb rgb]
        Will be a single pixel [rgb] where r = sum(all the r)/9;
        If that 3x3 is given to scale = 2, we will crop the right&bottom.
        For 4x4, scale = 2:
        [rgb1 rgb1  rgb2 rgb2
        rgb1 rgb1  rgb2 rgb2  --> [rgb1 rgb2
        rgb3 rgb3  rgb4 rgb4       rgb3 rgb4]
        rgb3 rgb3  rgb4 rgb4]

        We are writing inplace.
    */

    auto sum_p = std::make_unique<unsigned int[]>(channels);
    for (size_t row = 0; row < old_height_limit; row += scale) {
        for (size_t column = 0; column < old_width_limit; column += scale) {
            auto p = out_datap.get();
            p += row * old_stride_bytes;
            p += column * channels;
            // p now pointing at top left of sample box

            for (unsigned int component = 0; component < channels; ++component) {
                sum_p[component] = 0;
            }
            // For each pixel in the sample box:
            for (int i = 0; i < scale; ++i) {
                for (int j = 0; j < scale; ++j) {
                    unsigned char *subsample_p = p;
                    subsample_p += j * old_stride_bytes;
                    subsample_p += i * channels;
                    // Add up each component
                    for (unsigned int component = 0; component < channels; ++component) {
                        sum_p[component] += subsample_p[component];
                    }
                }
            }

            // Get the average. The denominator is scale squared.
            for (unsigned int component = 0; component < channels; ++component) {
                out_p[component] = utils::clamping_int_cast<unsigned char>(sum_p[component] / (scale*scale));
            }
            out_p += channels;
        }
    }

    image.width = new_width;
    image.height = new_height;
    ptrdiff_t out_size = out_p - out_datap.get();

    return std::span<unsigned char>(out_datap.get(), out_size);
}
