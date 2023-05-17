#pragma once
#include <memory>
#include <span>
#include <string_view>

#include "png.h"

#include "outcome/result.hpp"
#include "outcome/utils.hpp"

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace png
{
    class reader
    {
    public:
        reader(std::span<const unsigned char> src);

        outcome::result<std::span<unsigned char>> read();
        outcome::result<std::span<unsigned char>> scale(int scale);

        bool is_grayscale() const;
        bool is_linear() const;
        bool is_colormapped() const;
        bool has_alpha() const;
        png_uint_32 get_height() const;
        png_uint_32 get_width() const;
        std::string_view get_format() const;


    private:
        struct png_image_deleter {
            void operator()(png_image *i) const noexcept {
                png_image_free(i);
            }
        };
        std::span<const unsigned char> src;
        png_image image;
        std::unique_ptr<png_image, png_image_deleter> imagep;
        std::unique_ptr<unsigned char[]> out_datap;
    };

}
