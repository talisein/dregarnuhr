#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <set>
#include <string>
#include <fstream>

#include "bookmaker.h"
#include "book_writer.h"
#include "volumes.h"
#include "log.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"
#include "omnibus.h"
#include "omnibus_defs.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"
#include "utils.h"

namespace {
    constexpr auto defined_volumes = std::views::transform(get_omnibus_definition_r(),
                                                           &std::ranges::range_value_t<decltype(get_omnibus_definition_r())>::name) | std::views::transform([](const auto& v) -> volume { return std::get<volume>(v); });


    constexpr auto get_definition_view(omnibus o)
    {
        auto it = std::ranges::find(omnibus_defs::omnibus_arr,
                                    o,
                                    [](const auto& def) { return std::get<omnibus>(def.name); });
        if (it == std::ranges::end(omnibus_defs::omnibus_arr)) {
            return omnibus_defs::omnibus_def;
        }

        return *it;
    }

    constexpr auto get_definition_view(volume v)
    {
        auto it = std::ranges::find(omnibus_defs::omnibus_def,
                                    v,
                                    [](const auto& def) { return std::get<volume>(def.name); });
        if (it == std::ranges::end(omnibus_defs::omnibus_def)) {
            assert(false);
            return definition_view_t(std::views::single(omnibus_defs::omnibus_def.front()), v);

        }

        return definition_view_t(std::views::single(*it), v);
    }

    template<std::ranges::input_range R>
    auto get_filtered_defs(R&& defs, const epub::books_t& src_books, const epub::readers_t& src_readers)
    {
        return std::views::filter(std::forward<R>(defs), [&src_books, &src_readers](const volume_definition& def) {
            // Filter out sources we don't have available
            const auto& src_book_iter = src_books.find(def.vol);
            if (std::end(src_books) == src_book_iter) {
                return false;
            }

            const auto& src_reader = src_readers.at(def.vol);
            const auto& src_book = src_books.at(def.vol);
            const auto root = src_book.rootfile_path.substr(0, src_book.rootfile_path.find_first_of('/')+1);
            const auto src = utils::strcat(root, to_string_view(def.vol), '/', def.href);

            if (get_options()->size_filter) {
                const auto src_idx = src_reader->zip.locate_file(src.c_str()).value();
                const auto src_stat = src_reader->zip.stat(src_idx).value();
                if (src_stat.m_uncomp_size > get_options()->size_filter.value()) {
                    return false;
                }
            }

            if (get_options()->name_filter) {
                if (std::regex_search(src, get_options()->name_filter.value())) {
                    return false;
                }
            }
            return true;
        });
    }

}

namespace epub {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template<std::ranges::input_range R>
    result<void>
    bookmaker::make_books_impl(R&& view, std::variant<omnibus, volume> book)
    {
        if (std::holds_alternative<volume>(book)) {
            if (!src_books.contains(std::get<volume>(book))) {
                log_verbose("Info: ", std::get<volume>(book), " unavailable, skipping.");
                return std::errc::no_such_file_or_directory;
            }
        }

        if (std::ranges::none_of(view, [&](const auto &def) {
            return src_books.contains(std::get<volume>(def.name));
        })) {
            log_verbose("Info: No inputs to make ", book);
            return std::errc::no_such_file_or_directory;
        }

        try {
            auto res = std::visit(overloaded {
                        [&](omnibus o) -> result<fs::path> {
                            // x is a view of a list of (defs, volume_name) definiition_view_t
                            auto omnibus_def = utils::make_omnibus_def(get_filtered_defs(std::views::join(view), src_books, src_readers));
                            //auto first_chapter = std::ranges::find(omnibus_def, CHAPTER, &volume_definition::get_chapter_type);
                            auto base_volume = omnibus_def.front().vol;
                            auto writer = book_writer(base_volume, src_books, src_readers, o, std::move(omnibus_def));
                            return writer.make_book();
                        },
                            [&](volume v) -> result<fs::path> {
                            // x is a filter(views::single) of a (defs, volume_name) definiition_view_t
                            auto volume_def = get_filtered_defs(view.front(), src_books, src_readers);
                            auto writer = book_writer(v, src_books, src_readers, v, volume_def);
                            return writer.make_book();
                        }
                }, book);
            if (res.has_error()) {
                std::visit(overloaded {
                        [&](omnibus o) {
                            log_error("Couldn't make omnibus ", o, ": ", res.error().message());
                        },
                        [&](volume v) {
                            log_error("Couldn't make ", v, ": ", res.error().message(), ". Moving on...");
                        }
                }, book);
                return res.as_failure();
            } else {
                std::visit([&](auto&& arg) { log_info("Created chronologically ordered ", arg, ": ", res.value() ); }, book);
                return outcome::success();
            }
        } catch (std::system_error& e) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg, ": ", e.what()); }, book);
            return e.code();
        } catch (...) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg); }, book);
            return outcome::error_from_exception();
        }
    }

    result<void>
    bookmaker::make_books()
    {
        int created_books = 0;

        if (get_options()->omnibus_type) {
            auto defs = get_definition_view(get_options()->omnibus_type.value());
            OUTCOME_TRY(make_books_impl(std::move(defs), *get_options()->omnibus_type));
            ++created_books;
        } else {
            for (volume defined_volume : defined_volumes) {
                OUTCOME_TRY(make_books_impl(get_definition_view(defined_volume), defined_volume));
                ++created_books;
            }
        }

        log_info("Created ", created_books, " new books.");
        return outcome::success();
    }

    bookmaker::bookmaker(books_t&& books,
                         readers_t&& book_readers) :
            src_books(std::move(books)),
            src_readers(std::move(book_readers))
    {
    }
}
