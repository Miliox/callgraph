#pragma once

#include <map>
#include <ostream>

#include "string_cache.hpp"

class Graph final
{
public:
    Graph();

    ~Graph();

    void addEdge(StringView const& src, StringView const& dst);

    void setNodeLabel(StringView const& node, StringView const& label);

    void dump(std::ostream& out);

    void dump(std::ostream& out, StringView const& root);

private:
    std::map<StringView, std::set<StringView>> m_adj_list{};
    std::map<StringView, StringView> m_node_labels{};
};
