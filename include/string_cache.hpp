#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using StringView = std::string_view;

class StringCache final {
 public:
  StringCache();
  ~StringCache();

  StringCache(const StringCache&) = delete;
  StringCache& operator=(const StringCache&) = delete;

  StringView get(StringView const& str);

 private:
  std::set<StringView> m_cache{};
  std::uint64_t m_hits{};

  char* m_arena_begin;
  char* m_arena_end;
  char* m_arena_cursor;

  std::vector<char*> m_used_arenas{};
};
