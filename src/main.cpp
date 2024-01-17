#include "graph.hpp"
#include "parser.hpp"
#include "string_cache.hpp"

#include <cxxabi.h>

#include <fstream>
#include <iostream>

static StringView getSymbol(StringView view) {
    auto const pos = view.find(':');
    return (pos == StringView::npos)
        ? view
        : view.substr(pos + 1U);
}

static StringView formatLabel(StringCache& cache, StringView const raw) {
    std::string escaped{};
    for (auto const& c : raw) {
        if (c == '<' || c == '>' ||
            c == '{' || c == '}' ||
            c == '|' || c == ' ')
        {
            escaped += '\\';
        }
        escaped += c;

    }
    return cache.get(escaped);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " in1.ci [in2.ci [...]]\n";
        return EXIT_FAILURE;
    }

    StringCache cache{};
    Graph graph{};

    for (int i = 1; i < argc; ++i) {
        std::ifstream infile(argv[i]);
        if (!infile.is_open()) {
            std::cerr << "error: cannot open " << argv[i] << '\n';
            continue;
        }

        for (std::string line; std::getline(infile, line);) {
            StringView const view{line};

            if (isEdgeLine(view)) {
                auto const src = cache.get(getSource(view));
                auto const dst = cache.get(getTarget(view));
                graph.addEdge(src, dst);
            } else if (isNodeLine(view)) {
                auto const node = getTitle(view);
                auto const label = getLabel(view);
                auto const line_break_pos = label.find("\\n");
                auto const first_label =
                    (line_break_pos != StringView::npos) ? label.substr(0U, line_break_pos) : label;

                if (!first_label.starts_with(")") &&
                    !first_label.starts_with("cpp)") &&
                    (first_label.find("/") == StringView::npos)) {

                    auto const tpl_pos = first_label.find(" [with");
                    if (tpl_pos == StringView::npos) {
                        graph.setLabel(cache.get(node), formatLabel(cache, first_label));
                    } else {
                        StringView parts[2] = {
                            first_label.substr(0, tpl_pos),
                            first_label.substr(tpl_pos + 1U)
                        };
                        std::string aux{parts[0]};
                        aux += "\\n";
                        aux += parts[1];
                        graph.setLabel(cache.get(node), formatLabel(cache, aux));
                    }
                } else {
                    std::string c_symbol{getSymbol(node)};

                    int status{};
                    char* cpp_symbol =  abi::__cxa_demangle(c_symbol.data(), nullptr, nullptr, &status);

                    if (status == 0) {
                        graph.setLabel(cache.get(node), formatLabel(cache, cpp_symbol));
                    }

                    free(cpp_symbol);
                }
            }
        }
    }

    graph.dump(std::cout, cache.get({"main"}));

    return 0;
}
