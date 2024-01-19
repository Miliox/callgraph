#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <variant>

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

  char* const m_arena_begin;
  char* const m_arena_end;
  char* m_arena_cursor;
};
