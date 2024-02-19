#include "graph.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>

#include "string_cache.hpp"

Graph::Graph() = default;

Graph::~Graph() {
  std::uint64_t edges{};
  for (auto& adj : m_adj_list) {
    edges += adj.second.outgoing.size();
  }

  std::cerr << "Graph[" << static_cast<void*>(this)
            << "]: " << m_adj_list.size() << " nodes and " << edges
            << " edges.\n";
}

void Graph::addEdge(StringView const& src, StringView const& dst) {
  m_adj_list[dst].incoming.emplace(src);
  m_adj_list[src].outgoing.emplace(dst);
}

void Graph::setLabel(StringView const& vertex, StringView const& label) {
  m_adj_list[vertex].label = label;
}

void Graph::dump(std::ostream& out) {
  out << "digraph {\n";
  out << "rankdir=\"LR\"\n";
  out << "node [shape=Mrecord]\n";

  for (auto const& adj : m_adj_list) {
    if (!adj.second.incoming.empty() ||
        !adj.second.outgoing.empty()) {
      out << '"' << adj.first << "\" [label=\"" << adj.second.label << "\"]\n";
    }
  }

  for (auto const& adj : m_adj_list) {
    auto& base = adj.first;
    auto& adjs = adj.second.outgoing;
    if (adjs.empty()) {
      continue;
    }

    out << '"' << base << '"' << " -> ";

    if (adjs.size() > 1U) {
      out << "{";
    }

    for (auto const& a : adjs) {
      out << ' ' << '"' << a << '"';
    }

    if (adjs.size() > 1U) {
      out << " }";
    }

    out << '\n';
  }

  out << "}\n";
}

void Graph::dump(std::ostream& out, StringView const& root,
                 bool const outgoing) {
  std::set<StringView> visited{};

  std::queue<StringView> to_visit{};
  to_visit.push(root);

  std::stringstream ss{};
  while (!to_visit.empty()) {
    StringView visiting = to_visit.front();
    to_visit.pop();

    if (visited.find(visiting) != visited.end()) {
      continue;
    }
    visited.emplace(visiting);

    auto& adjacents = outgoing ? m_adj_list[visiting].outgoing
                               : m_adj_list[visiting].incoming;
    if (adjacents.empty()) {
      continue;
    }

    if (outgoing) {
      ss << '"' << visiting << '"' << " -> ";
    }

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

    if (not outgoing) {
      ss << " -> " << '"' << visiting << '"';
    }

    ss << '\n';
  }

  out << "digraph {\n";
  out << "rankdir=\"LR\"\n";
  out << "node [shape=Mrecord]\n";

  std::vector<std::pair<StringView, StringView>> id_label_pairs{};
  for (auto const& id : visited) {
    StringView const label = m_adj_list.at(id).label;
    id_label_pairs.emplace_back(id, label);
  }
  std::sort(id_label_pairs.begin(), id_label_pairs.end(),
            [](std::pair<StringView, StringView> const& lhs,
               std::pair<StringView, StringView> const& rhs) -> bool {
              return lhs.second > rhs.second;
            });

  for (auto const& id_label_pair : id_label_pairs) {
    out << '"' << id_label_pair.first << "\" [label=\"" << id_label_pair.second << "\"]\n";
  }

  out << ss.str();
  out << "}\n";
}
