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

    void setLabel(StringView const& node, StringView const& label);

    void dump(std::ostream& out);

    void dump(std::ostream& out, StringView const& root);

private:
    struct Vertex final
    {
        StringView label{};
        std::set<StringView> incoming{};
        std::set<StringView> outgoing{};
    };

    std::map<StringView, Vertex> m_adj_list{};
};
