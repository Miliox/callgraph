#include "string_cache.hpp"

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <iterator>

namespace {

constexpr size_t kArenaSize{4 * 1024 * 1024};

char* allocateArena() {
    auto const ret = sysconf(_SC_PAGE_SIZE);
    if (ret <= 0) {
        std::cerr << "error: failed to read page size " << std::endl;
        std::abort();
    }
    size_t const page_size = static_cast<size_t>(ret);

    void* arena{};
    int const error = posix_memalign(&arena, page_size, kArenaSize);
    if (error != 0 && arena != nullptr) {
        std::cerr << "error: failed to allocate cache with error " << error
                << std::endl;
        std::abort();
    }
    return static_cast<char*>(arena);
}

}

StringCache::StringCache()
    : m_arena_begin{allocateArena()},
      m_arena_end{m_arena_begin + kArenaSize},
      m_arena_cursor{m_arena_begin} {}

StringCache::~StringCache() {
  size_t cache_size{m_used_arenas.size() * kArenaSize};
  cache_size += static_cast<size_t>(m_arena_cursor - m_arena_begin);

  std::cerr << "StringCache[" << static_cast<void*>(this) << "] "
            << m_cache.size() << " strings, "
            << cache_size << " bytes, "
            << (1U + m_used_arenas.size()) << " buckets, "
            << m_hits << " hits.\n";

  for (char* used_arena : m_used_arenas) {
    free(used_arena);
  }
  free(m_arena_begin);
}

StringView StringCache::get(StringView const& str) {
  auto it = m_cache.find(str);
  if (it != m_cache.end()) {
    ++m_hits;
    return *it;
  }

  if (static_cast<ptrdiff_t>(str.size()) > (m_arena_end - m_arena_cursor)) {
    if (str.size() >= kArenaSize) {
        std::cerr << "error: string of " << str.size()
                  << " bytes is too larger to fit in the cache." << std::endl;
        std::abort();
    }
    m_used_arenas.push_back(m_arena_begin);

    m_arena_begin  = allocateArena();
    m_arena_cursor = m_arena_begin;
    m_arena_end    = m_arena_begin + kArenaSize;
  }

  char* cursor = std::copy(str.begin(), str.end(), m_arena_cursor);
  *cursor = '\0';
  cursor += 1U;

  auto ret = m_cache.emplace(m_arena_cursor, str.size());
  if (!ret.second) {
    std::cerr << "error: failed to cache value" << std::endl;
    std::abort();
  }
  m_arena_cursor = cursor;

  return *ret.first;
}
