#include "string_cache.hpp"

#include <cstring>
#include <iostream>
#include <iterator>

#include <unistd.h>

constexpr size_t kCacheCapacity{1024 * 1024};

StringCache::StringCache() :
    m_arena_begin{
        [](){
            auto const page_size = sysconf(_SC_PAGE_SIZE);
            if (page_size <= 0) {
                std::cerr << "error: failed to read page size " << std::endl;
                std::abort();
            }

            void* arena{};
            int const error = posix_memalign(&arena, static_cast<size_t>(page_size), kCacheCapacity);
            if (error != 0 && arena != nullptr) {
                std::cerr << "error: failed to allocate cache with error " << error << std::endl;
                std::abort();
            }
            return static_cast<char*>(arena);
        }()},
    m_arena_end{m_arena_begin + kCacheCapacity},
    m_arena_cursor{m_arena_begin}
{
}

StringCache::~StringCache() 
{
    std::cerr << "StringCache[" << static_cast<void*>(this)
              << "] " << m_cache.size() << " strings, "
              << (m_arena_cursor - m_arena_begin) << " bytes, "
              << m_hits << " hits.\n";
    
    free(m_arena_begin);
}

StringView StringCache::get(StringView const& str)
{
    auto it = m_cache.find(str);
    if (it != m_cache.end()) {
        ++m_hits;
        return *it;
    }

    if (static_cast<ptrdiff_t>(str.size()) > (m_arena_end - m_arena_cursor)) {
        std::cerr << "error: cache ran out of space" << std::endl;
        std::abort();
    }

    static_cast<void>(std::memcpy(m_arena_cursor, str.data(), str.size()));
    m_arena_cursor[str.size()] = '\0';

    auto ret = m_cache.emplace(m_arena_cursor, str.size());
    m_arena_cursor += str.size() + 1U;

    if (!ret.second) {
        std::cerr << "error: failed to cache value" << std::endl;
        std::abort();
    }

    return *ret.first;
}
