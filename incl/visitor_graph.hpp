#ifndef VIS
#define VIS

#include "checkMaxFlow.hpp"
#include "shortestAugmentedPath.hpp"
#include <boost/graph/depth_first_search.hpp>
#include <boost/shared_ptr.hpp>
#include <LEDA/graph/graph.h>
#include <vector>

using namespace boost;

class my_dfs_visitor : public default_dfs_visitor 
{
public:
    my_dfs_visitor(Graph& g) 
    {
        in_R = boost::shared_ptr<std::vector<bool> >(new std::vector<bool>(num_vertices(g), false));
    }

    void discover_vertex(Vertex u, const Graph&) 
    {
        (*in_R)[u] = true;
    }

    std::vector<bool> return_vector() const 
    {
        return *in_R;
    }

private:
    boost::shared_ptr<std::vector<bool> > in_R;
};

// void LEDA_Graph_To_Boost(leda::graph& L_G, leda::edge_array<int>& wt, Graph& B_G) 
// {
//     leda::node_array<Vertex> Map(L_G);

//     leda::edge e;
//     leda::node n;

//     forall_nodes(n, L_G)
//     {
//         Vertex v = add_vertex(B_G);
//         Map[n] = v;
//     }

//     forall_edges(e, L_G)
//     {
//         Edge u = add_edge(Map[L_G.source(e)], Map[L_G.target(e)], B_G).first;
//         B_G[u].weight = wt[e];
//     }
// }

#endif