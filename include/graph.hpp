#pragma once

#include <map>
#include <ostream>

#include "string_cache.hpp"

class Graph final
{
public:
    void addEdge(String const& src, String const& dst);

    void setNodeLabel(String const& node, String const& label);

    void dump(std::ostream& out);

    void dump(std::ostream& out, String const& root);

private:
    std::map<String, std::set<String>> m_adj_list{};
    std::map<String, String> m_node_labels;
};
