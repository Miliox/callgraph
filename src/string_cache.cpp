#include "string_cache.hpp"

#include <iostream>

StringCache::StringCache() = default;

StringCache::~StringCache()
{
    std::cerr << "StringCache[" << static_cast<void*>(this)
              << "] cached " << m_cache.size() << " strings, hits "
              << m_hits << ", misses " << m_misses << "\n";
}

String StringCache::get(StringView const& str)
{
    auto result = m_cache.emplace(std::make_shared<std::string>(str));
    if (result.second) {
        ++m_misses;
    } else {
        ++m_hits;
    }
    return *result.first;
}
