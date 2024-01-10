callgraph:
	g++-13 -std=c++20 -o callgraph -fcallgraph-info=su,da -fstack-usage -O2 callgraph.cpp
