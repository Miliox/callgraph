#include <iostream>
#include <queue>
#include <set>
#include <sstream>

#include "graph.hpp"
#include "string_cache.hpp"

Graph::Graph() = default;

Graph::~Graph()
{
    std::uint64_t edges{};
    for (auto& adj : m_adj_list) {
        edges += adj.second.size();
    }

    std::cerr << "Graph[" << static_cast<void*>(this) <<  "]: "
              << m_adj_list.size() << " nodes and "
              << edges << " edges.\n";
}

void Graph::addEdge(StringView const& src, StringView const& dst)
{
    m_adj_list[src].emplace(dst);
    static_cast<void>(m_adj_list[dst]);
}

void Graph::setNodeLabel(StringView const& node, StringView const& label)
{
    m_node_labels[node] = label;
}

void Graph::dump(std::ostream& out)
{
    out << "digraph {\n";
    out << "rankdir=\"LR\"\n";
    out << "node [shape=Mrecord]\n";

    for (auto const& adj : m_adj_list) {
        if (adj.second.empty()) {
            continue;
        }

        out << '"' << adj.first << '"' << " -> ";

        if (adj.second.size() > 1U) {
            out << "{";
        }

        for (auto const& node : adj.second) {
            out << ' ' << '"' << node << '"';
        }

        if (adj.second.size() > 1U) {
            out << " }";
        }

        out << '\n';
    }

    out << "}\n";
}

void Graph::dump(std::ostream& out, StringView const& root)
{
    std::set<StringView> visited{};

    std::queue<StringView> to_visit{};
    to_visit.push(root);

    std::stringstream ss{};
    while (!to_visit.empty()) {
        StringView visiting = to_visit.front();
        to_visit.pop();

        if (visited.find(visiting) != visited.end())
        {
            continue;
        }
        visited.emplace(visiting);

        auto& adjacents = m_adj_list[visiting];
        if (adjacents.empty()) {
            continue;
        }

        ss << '"' << visiting << '"' << " -> ";

        if (adjacents.size() > 1U) {
            ss << "{";
        }

        for (auto const& adj : adjacents) {
            to_visit.push(adj);
            ss << ' ' << '"' << adj << '"';
        }

        if (adjacents.size() > 1U) {
            ss << " }";
        }

        ss << '\n';
    }

    out << "digraph {\n";
    out << "rankdir=\"LR\"\n";
    out << "node [shape=Mrecord]\n";

    for (auto const& v : visited) {
        auto it = m_node_labels.find(v);
        if (it != m_node_labels.end()) {
            out << '"' << v << "\" [label=\"" << it->second << "\"]\n";
        }
    }

    out << ss.str();
    out << "}\n";
}
