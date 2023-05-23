#include <ranges>

#include "outcome/try.hpp"
#include "libxml++/libxml++.h"

#include "book_searcher.h"
#include "log.h"
#include "libxml/tree.h"

namespace
{
    struct text_node_wrapper_deleter
    {
        void operator()(xmlpp::Node* node) const
        {
            /* Stolen xmlpp::Node::free_wrappers, since its broken for TextNodes */
            auto textnode = dynamic_cast<xmlpp::TextNode*>(node);
            if (!textnode) return;
            auto cobj = textnode->cobj();
            for (auto child = cobj->children; child; child = child->next) {
                xmlpp::Node::free_wrappers(child);
            }
            delete textnode;
            cobj->_private = nullptr;
        }
    };

    using text_node_p = std::unique_ptr<xmlpp::Node, text_node_wrapper_deleter>;
}

book_searcher::book_searcher(epub::books_t&& books,
                             epub::readers_t&& book_readers) :
    whitespace("[[:space:]]*"),
    src_books(std::move(books)), src_readers(std::move(book_readers))
{
}

OUTCOME_V2_NAMESPACE::result<std::vector<std::string>>
book_searcher::search() const
{
    if (!get_options()->search_pattern) {
        return std::errc::invalid_argument;
    }

    std::vector<std::string> matched_xpaths;

    for (const auto& [vol, reader_p] : src_readers) {
        OUTCOME_TRY(auto filenames, reader_p->zip.get_files());
        int matched = 0;
        for (auto filename : filenames | std::views::filter([] (const auto &f) { return f.ends_with(".xhtml"); })) {
            OUTCOME_TRY(auto idx, reader_p->zip.locate_file(filename.c_str()));
            OUTCOME_TRY(auto raw_xhtml, reader_p->zip.extract_raw(idx));
            xmlpp::TextReader text_reader(raw_xhtml.data(), raw_xhtml.size());
            try {
                auto&& pattern = get_options()->search_pattern.value();
                std::string previous_value;
                while (text_reader.read()) {
                    if (text_reader.has_value()) {
                        auto node_text = text_reader.get_value();
                        if(std::regex_search(node_text, pattern)) {
                            log_info("-----------");
                            if (!previous_value.empty()) log_info(vol, ' ', filename, ": ", previous_value);
                            log_info(vol, ' ', filename, ": ", node_text);
                            const text_node_p node(text_reader.get_current_node());
                            matched_xpaths.push_back(node->get_path());
                            matched = 1;
                        } else if (matched > 0) {
                            if (!std::regex_match(node_text, whitespace)) {
                                log_info(vol, ' ', filename, ": ", node_text);
                                --matched;
                            }
                        }

                        if (!std::regex_match(node_text, whitespace)) {
                            previous_value = node_text;
                        }
                    }
                }
            } catch (...) {
            }
        }
    }
    return matched_xpaths;
}
