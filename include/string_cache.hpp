#pragma once

#include <memory>
#include <set>
#include <string>
#include <string_view>

using String = std::shared_ptr<std::string>;

using StringView = std::string_view;

class StringCache final
{
public:
    String get(StringView const& str);

private:
    struct StringCacheComparator final
    {
        bool operator()(String const& lhs, String const& rhs) const
        {
            return *lhs < *rhs;
        }
    };

    std::set<String, StringCacheComparator> m_cache;
};

