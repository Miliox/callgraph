#pragma once

#include "string_cache.hpp"

bool isEdgeLine(StringView const& line);
bool isNodeLine(StringView const& line);
bool isGraphLine(StringView const& line);

StringView getLabel(StringView const& line);
StringView getSource(StringView const& line);
StringView getTarget(StringView const& line);
StringView getTitle(StringView const& line);
