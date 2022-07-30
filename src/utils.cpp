#include "utils.h"

namespace utils
{
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
