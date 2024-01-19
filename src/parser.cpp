#include "parser.hpp"

#include "string_cache.hpp"

namespace keywords {

constexpr StringView kGraph{"graph: { "};
constexpr StringView kNode{"node: { "};
constexpr StringView kEdge{"edge: { "};

#if 0
constexpr StringView kOpenStr{"\""};
#endif
constexpr StringView kCloseStr{"\""};

#if 0
constexpr StringView kOpenBlk{"{"};
constexpr StringView kCloseBlk{"}"};
#endif

constexpr StringView kTitle{"title: \""};
constexpr StringView kLabel{"label: \""};
constexpr StringView kSourceName{"sourcename: \""};
constexpr StringView kTargetName{"targetname: \""};

#if 0
constexpr StringView kLabelDelim{"\\n"};
constexpr StringView kBytesTrail{" bytes (static)"};
#endif

}  // namespace keywords

namespace {

StringView getField(StringView const& view, StringView const& field) {
  auto beg = view.find(field);
  if (beg == StringView::npos) {
    return {};
  }
  beg += field.size();

  auto end = view.find(keywords::kCloseStr, beg);
  if (end == StringView::npos) {
    return {};
  }

  return view.substr(beg, end - beg);
}

}  // namespace

bool isEdgeLine(StringView const& line) {
  return line.starts_with(keywords::kEdge);
}

bool isNodeLine(StringView const& line) {
  return line.starts_with(keywords::kNode);
}

bool isGraphLine(StringView const& line) {
  return line.starts_with(keywords::kGraph);
}

StringView getLabel(StringView const& line) {
  return getField(line, keywords::kLabel);
}

StringView getSource(StringView const& line) {
  return getField(line, keywords::kSourceName);
}

StringView getTarget(StringView const& line) {
  return getField(line, keywords::kTargetName);
}

StringView getTitle(StringView const& line) {
  return getField(line, keywords::kTitle);
}
