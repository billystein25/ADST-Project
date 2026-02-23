#include "boostGraphStruct.hpp"
#include "checkMaxFlow.hpp"
#include "visitor_graph.hpp"
#include <queue>
#include <vector>
// #include <boost/graph/depth_first_search.hpp>

using namespace boost;

bool CHECK_MAX_FLOW_T(Graph& G, Vertex s, Vertex t, bool printErrorName){
	
	// Vertex v;
	// Edge e;
	int n = num_vertices(G);
	Edge edge_nil = *(edges(G).second);
	Vertex vertex_nil = graph_traits<Graph>::null_vertex();

	VertexIterator v_it, v_it_end;
	EdgeIterator e_it, e_it_end;
	OutEdgeIterator out_e_it, out_e_it_end;
	InEdgeIterator in_e_it, in_e_it_end;

	// illegal flow value error
	
	for (tie(e_it, e_it_end) = edges(G); e_it != e_it_end; e_it++){
		if (!(G[*e_it].f >= 0 && G[*e_it].f <= G[*e_it].cap)){
			if (printErrorName){
				std::cout<< "Illegal flow value"<<std::endl;
				std::cout<<"Edge: ("<<G[source(*e_it, G)].name<<", "<<G[target(*e_it, G)].name<<")"<<std::endl;

			}
			return false;
		}
	}

	// calculate excess for all nodes/vertexes

	std::vector<long> excess(n, 0);

	for (tie(e_it, e_it_end) = edges(G); e_it != e_it_end; e_it++){
		Vertex v = source(*e_it, G);
		Vertex w = target(*e_it, G);
		excess[v] -= G[*e_it].f;
		excess[w] += G[*e_it].f;
	}

	// node with non-zero excess error
	
	for (tie(v_it, v_it_end) = vertices(G); v_it != v_it_end; v_it++){
		if (!(*v_it == s || *v_it == t || excess[*v_it] == 0)){
			if (printErrorName){
				std::cout<< "Node with non-zero excess"<<std::endl;
				std::cout<< "Node: "<<G[*v_it].name<<std::endl;

			}
			return false;
		}
	}

	// Compute nodes reachable from s using BFS

	std::vector<bool> reached(n, false);
	std::queue<Vertex> Q;

	Q.push(s);
	reached[s] = true;

	while (!Q.empty()){

		Vertex v = Q.front();
		Q.pop();
		// for all out edges
		for (tie(out_e_it, out_e_it_end) = out_edges(v, G); out_e_it != out_e_it_end; out_e_it++){
			Vertex w = target(*out_e_it, G);
			if (G[*out_e_it].f < G[*out_e_it].cap && !reached[w]){
				reached[w] = true;
				Q.push(w);
			}
		}
		// for all in edges
		for (tie(in_e_it, in_e_it_end) = in_edges(v, G); in_e_it != in_e_it_end; in_e_it++){
			Vertex w = source(*in_e_it, G);
			if (G[*in_e_it].f > 0 && !reached[w]){
				reached[w] = true;
				Q.push(w);
			}
		}
	}
	if (reached[t]){
		if (printErrorName){
			std::cout<<"t is reachable in G_f"<<std::endl;
		}
		return false;
	}

	return true;
}