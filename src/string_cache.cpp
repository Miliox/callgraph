#include "string_cache.hpp"

String StringCache::get(StringView const& str)
{
    return *m_cache.emplace(std::make_shared<std::string>(str)).first;
}
