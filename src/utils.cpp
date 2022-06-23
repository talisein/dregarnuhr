#include "utils.h"

namespace utils
{
    bool filter_chapter_stylesheet::operator()(const volume_definition& def)
    {
        if (get_uniqueness(def.get_chapter_type()) == chapter_uniqueness::SINGLE ) {
            if (chap_set.contains(def.get_chapter_type())) {
                return false;
            } else {
                chap_set.insert(def.get_chapter_type());
                return true;
            }
        } else {
            if (def.get_chapter_type() == STYLESHEET) {
                if (style_set.contains(def.vol)) {
                    return false;
                } else {
                    style_set.insert(def.vol);
                    return true;
                }
            } else {
                return true;
            }
        }
    }

    volume_definition transform_unique_ids::operator()(volume_definition def)
    {

        if (std::string_view::npos != def.mediatype.find("dtbncx") || std::string_view::npos != def.href.find("toc.xhtml"))
            return def;
        ss.str("");
        ss << m_prefix << '-' << unique++ << '-' << def.id;
        auto iter = m_set.insert(ss.str());
        def.id = *iter.first;
        return def;
    }

    void foreach_label::operator()(volume_definition &def)
    {
        if (!prev_label) {
            prev_label = def.toc_label;
            return;
        }

        if (def.toc_label) {
            if (prev_label.value() == def.toc_label.value()) {
                def.toc_label = std::nullopt;
            } else {
                prev_label = def.toc_label;
            }
        }
    }
}
