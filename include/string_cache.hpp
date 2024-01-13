#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <variant>

using String = std::shared_ptr<std::string const>;

using StringView = std::string_view;

class StringCache final
{
public:
    StringCache();
    ~StringCache();

    String get(StringView const& str);

private:
    using Entry = std::variant<String, StringView>;

    struct Comparator final
    {
        bool operator()(Entry const& lhs, Entry const& rhs) const
        {
            if (std::holds_alternative<StringView>(lhs)) {
                if (std::holds_alternative<StringView>(rhs)) {
                    return std::get<StringView>(lhs) < std::get<StringView>(rhs);
                } else {
                    return std::get<StringView>(lhs) < *std::get<String>(rhs);
                }
            } else {
                if (std::holds_alternative<StringView>(rhs)) {
                    return *std::get<String>(lhs) < std::get<StringView>(rhs);
                } else {
                    return *std::get<String>(lhs) < *std::get<String>(rhs);
                }
            }
        }
    };

    std::set<Entry, Comparator> m_cache{};
    std::uint64_t m_hits{};
};

