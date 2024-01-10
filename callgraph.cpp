#include <cxxabi.h>

#include <array>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace keywords {

constexpr std::string_view kGraph{"graph: { "};
constexpr std::string_view kNode{"node: { "};
constexpr std::string_view kEdge{"edge: { "};

constexpr std::string_view kOpenStr{"\""};
constexpr std::string_view kCloseStr{"\""};

constexpr std::string_view kOpenBlk{"{"};
constexpr std::string_view kCloseBlk{"}"};

constexpr std::string_view kTitle{"title: \""};
constexpr std::string_view kLabel{"label: \""};
constexpr std::string_view kSourceName{"sourcename: \""};
constexpr std::string_view kTargetName{"targetname: \""};

constexpr std::string_view kLabelDelim{"\\n"};
constexpr std::string_view kBytesTrail{" bytes (static)"};

}  // namespace keywords

struct StringCacheLookup final {
    bool operator()(std::shared_ptr<std::string> const& lhs,
                    std::shared_ptr<std::string> const& rhs) const {
        return *lhs < *rhs;
    }
};

struct Node final {
    std::optional<std::shared_ptr<std::string>> file_path;
    std::optional<size_t> file_line;
    std::optional<size_t> file_column;
    std::optional<size_t> static_size;
    std::optional<size_t> dynamic_objects;
    std::shared_ptr<std::string> label;
    std::optional<std::shared_ptr<std::string>> function;
    std::set<std::shared_ptr<std::string>> adjacents;
};

std::string_view get_field(std::string_view const& view,
                           std::string_view const& field) {
    auto beg = view.find(field);
    if (beg == std::string_view::npos) {
        return {};
    }
    beg += field.size();

    auto end = view.find(keywords::kCloseStr, beg);
    if (end == std::string_view::npos) {
        return {};
    }

    return view.substr(beg, end - beg);
}

std::string_view skip_file_prefix(std::string_view view) {
    auto const pos = view.find(':');
    return (pos == std::string_view::npos)
        ? view
        : view.substr(pos + 1U);
}

std::string escape_string(std::string_view const view) {
    std::string escaped{};
    for (auto const& ch : view) {
        if (ch == '<' || ch == '>' || ch == '{' || ch == '}' || ch == '|' || ch == ' ')
        {
            escaped += '\\';
        }
        escaped += ch;
    }
    return escaped;
}

std::vector<std::string_view> split_view(std::string_view const& view,
                                         std::string_view const& delim) {
    std::vector<std::string_view> tokens{};

    size_t beg = 0;
    size_t end = view.find(delim);

    while (end != std::string_view::npos) {
        tokens.push_back(view.substr(beg, end - beg));

        beg = end + keywords::kLabelDelim.size();
        end = view.find(keywords::kLabelDelim, beg);
    }

    tokens.push_back(view.substr(beg));

    return tokens;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input.ci output.dot\n";
        return EXIT_FAILURE;
    }

    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "error: cannot open " << argv[1] << '\n';
        return EXIT_FAILURE;
    }

    std::ofstream outfile(argv[2]);
    if (!outfile.is_open()) {
        std::cerr << "error: cannot open " << argv[2] << '\n';
        return EXIT_FAILURE;
    }

    std::set<std::shared_ptr<std::string>, StringCacheLookup> string_cache{};

    std::map<std::shared_ptr<std::string>,  // source
             Node                           // targets
             >
        adj_list{};

    for (std::string line; std::getline(infile, line);) {
        std::string_view view{line};

        if (view.starts_with(keywords::kGraph)) {
            // std::cout << "graph " <<  get_field(view, keywords::kTitle) <<
            // '\n';
        } else if (view.starts_with(keywords::kNode)) {
            // std::cout << "node " << get_field(view, keywords::kTitle) << " "
            // << get_field(view, keywords::kLabel) << '\n';

            std::shared_ptr<std::string> id =
                *string_cache
                     .emplace(std::make_shared<std::string>(
                         get_field(view, keywords::kTitle)))
                     .first;

            std::string_view label = get_field(view, keywords::kLabel);

            std::vector<std::string_view> label_parts{
                split_view(label, keywords::kLabelDelim)};

            for (auto const& part : label_parts) {
                if (part.ends_with(keywords::kBytesTrail)) {
                    std::size_t static_size{};
                    auto [ptr, ec] =
                        std::from_chars(part.data(),
                                        part.data() + part.size() -
                                            keywords::kBytesTrail.size(),
                                        static_size);

                    if (ec == std::errc{}) {
                        adj_list[id].static_size = static_size;
                    }
                }
            }

            std::string label_str{"{"};
            
            auto mangled_id = skip_file_prefix(*id);
            label_str += escape_string(mangled_id);
            label_str += '|';

            int demangle_status{};
            char* demangled_id =  abi::__cxa_demangle(mangled_id.data(), nullptr, nullptr, &demangle_status);
            if (demangle_status == 0) {
                label_str += escape_string(demangled_id);
                label_str += '|';
            } else {
                std::cerr << "Demangling error: " << *id << " " << demangle_status << "\n";
            }
            free(demangled_id);


            for (auto const& part : label_parts) {
                label_str += escape_string(part);

                if (part.data() != label_parts.back().data())
                {
                    label_str += '|';
                } else {
                    label_str += '}';
                }
            }
            adj_list[id].label = *string_cache.emplace(std::make_shared<std::string>(label_str)).first;

        } else if (view.starts_with(keywords::kEdge)) {
            std::shared_ptr<std::string> source =
                *string_cache
                     .emplace(std::make_shared<std::string>(
                         get_field(view, keywords::kSourceName)))
                     .first;
            std::shared_ptr<std::string> target =
                *string_cache
                     .emplace(std::make_shared<std::string>(
                         get_field(view, keywords::kTargetName)))
                     .first;
            static_cast<void>(adj_list[source].adjacents.emplace(target));
        } else if (view.starts_with(keywords::kCloseBlk)) {
            // std::cout << "graph-end\n";
        }
    }

    outfile << "digraph {\n";
    outfile << "node [shape=record margin=\"0.5,0.0\"]\n";

    std::set<std::string_view> node_ids{};

    for (auto const& node : adj_list) {
        outfile << '"' << *node.first << '"' << " [label=\"" << *node.second.label << "\"]\n";
    }

    for (auto const& adj : adj_list) {
        if (adj.second.adjacents.empty()) {
            continue;
        }

        outfile << '"' << *adj.first << '"' << " -> ";

        if (adj.second.adjacents.size() > 1U) {
            outfile << "{";
        }

        for (auto const& node : adj.second.adjacents) {
            outfile << ' ' << '"' << *node << '"';
        }

        if (adj.second.adjacents.size() > 1U) {
            outfile << " }";
        }

        outfile << '\n';
    }

    outfile << "}\n";

    return EXIT_SUCCESS;
}

