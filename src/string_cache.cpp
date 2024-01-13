#include "string_cache.hpp"

#include <iostream>

StringCache::StringCache() = default;

StringCache::~StringCache()
{
    std::cerr << "StringCache[" << static_cast<void*>(this)
              << "] cached " << m_cache.size() << " strings, hits "
              << m_hits << ".\n";
}

String StringCache::get(StringView const& str)
{
    auto it = m_cache.find({str});
    if (it != m_cache.end()) {
        ++m_hits;
        return std::get<String>(*it);
    }

    auto result = m_cache.emplace(std::make_shared<std::string>(str));
    if (!result.second) {
        std::cerr << "error: failed to insert new cache entry";
        std::abort();
    }
    return std::get<String>(*result.first);
}
