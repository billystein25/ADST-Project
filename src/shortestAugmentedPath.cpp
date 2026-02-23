#include <iostream>
#include "boostGraphStruct.hpp"
#include "shortestAugmentedPath.hpp"
#include "visitor_graph.hpp"
#include <queue>
#include <climits>

// Boost Graph 
#include <boost/graph/adjacency_list.hpp>

void calcDist(Graph& G, std::vector<bool>& visited, std::vector<int>& d, std::queue<Vertex>& currentVertices, int currentDist);


// returns true if there is a path from the source to the target node by calling itself
// only called by checkNodeConnected
bool runCheckVertexConnected(Graph& BG, Vertex source_vertex, Vertex target_vertex, std::vector<bool>& visited);

// returns true if there is a path from the source node to the target node
bool checkVertexConnected(Graph& BG, Vertex source_vertex, Vertex target_vertex);


bool shortestAugmentedPath(Graph& G, Vertex s, Vertex t){
    
    bool PRINT_PROGRESS = false;
    const bool PRINT_DISTANCE = false;
    const int ASK_FOR_INPUT_EVERY_LOOPS = 1000;

    // pseudocode algo

    // while d(s) < n
    // {
    
    //     if i has an admissible arc then
    //     {
    //         advance (i)
    //         if i == t then augment and set i = s
    //     }
    //     else
    //     {
    //         retreat (i)
    //     }

    // }
    // end

    // advance (i){
    //     let (i, j) be an admissible arc in A(i)
    //     pred(j) = i and i = j
    // }

    // retreat (i){
    //     d(i) = min (d(j) + 1 : (i, j) E A(i) and rij > 0)
    //     if i != s then i = pred(i)
    // }

    // augment {
    //     using the predecessor indices identify an augmenting path P from the source to the sink
    //     delta = min (rij : (i, j) E P)
    //     augment delta units of flow along path P
    // }
    
    if (!(checkVertexConnected(G, s, t))){
        std::cout<<"t is not reachable from s"<<std::endl;
        return false;
    }

    int n = num_vertices(G);
	Edge edge_nil = *(edges(G).second);
	Vertex vertex_nil = graph_traits<Graph>::null_vertex();
	
    VertexIterator v_it, v_it_end;
	EdgeIterator e_it, e_it_end;
	OutEdgeIterator out_e_it, out_e_it_end;
	InEdgeIterator in_e_it, in_e_it_end;

    // main

    // flow = 0
    for (tie(e_it, e_it_end) = edges(G); e_it != e_it_end; e_it++){
        G[*e_it].f = 0;
        // G[*e_it].reversef = 0;
    }

    // obtain exact distance labels d(i) using reverse bfs

    std::vector<int> d(n, -1);
    std::vector<bool> visited(n, false);

    d[t] = 0;

    visited[t] = true;

    std::queue<Vertex> currentVertices;
    currentVertices.push(t);

    calcDist(G, visited, d, currentVertices, 0);

    // print Vertex name and its distance

    if (PRINT_DISTANCE){
        for (tie(v_it, v_it_end) = vertices(G); v_it != v_it_end; v_it++){
            std::cout<< G[*v_it].name << " d: "<< d[*v_it]<< std::endl;
        }

    }

    Vertex i = s;

    std::vector<Vertex> pred(n, t);
    std::vector<bool> predIsReal(n, true);

    int m = num_edges(G);
    int loop = 0;

    if (PRINT_PROGRESS) std::cout<<std::endl<<"starting d[s]: "<<d[s]<<std::endl;

    while (d[s] < n){
        // if i has an admissible arc 
        // an arc with source i is admissible if for the arc's target j the condition d[i] = d[j] + 1 is true

        // first find the admissible arc if one exists

        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"Loop: "<<loop<<" i: "<<G[i].name<<" d[s]: "<<d[s]<<std::endl;

        Edge admissibleArc;
        bool arcFound = false;

        // if true then the direction of the virtual edge we're traveling with matches the real edge in G.
        bool currPredIsReal;

        // simulate the existence of reverse edges using f
        // an out_edge exists only if cap - f > 0
        // a reverse in_edge exists only if f > 0

        //                                          5                           5/8     8/8
        // imagine graph: A -> B -> C with flow: A --> B     C   flow in G:  A ----> B ----> C
        // all edges have cap = 8                  <--   <--
        // we simulate the existence of the         3     8
        // revese edges in G_f using f. that way we can use all the out edges in G_f without having to modify G by adding and removing edges
        // similarly an out edge in G_f exists only if cap - f > 0 in G

        // first search out edges

        for (tie(out_e_it, out_e_it_end) = out_edges(i, G); out_e_it != out_e_it_end; out_e_it++){
            Vertex j = target(*out_e_it, G);

            // ignore edges that cannot reach t
            if (d[j] == -1) continue;
            currPredIsReal = true;
            // this arc is admissible

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) {
                std::cout<<"Looking for Admissible Arc: (i, j): "<<std::endl;
                std::cout<<"d["<<G[i].name<<"] = "<<d[i]<<" d["<<G[j].name<<"] = "<<d[j]<<std::endl;
                std::cout<<"cap ("<<G[i].name<<", "<<G[j].name<<"): "<<G[*out_e_it].cap<<" f ("<<G[i].name<<", "<<G[j].name<<"): "<<G[*out_e_it].f<<std::endl;

            }

            // simulate the existence of an edge using cap - f

            int r = G[*out_e_it].cap - G[*out_e_it].f;

            if ((d[i] == d[j] + 1) && r > 0){

                admissibleArc = *out_e_it;
                arcFound = true;
                break;

            }
        }

        // then in edges that simulate out edges

        if (!arcFound){
            for (tie(in_e_it, in_e_it_end) = in_edges(i, G); in_e_it != in_e_it_end; in_e_it++){
                Vertex j = source(*in_e_it, G);
                
                // ignore edges that cannot reach t
                if (d[j] == -1) continue;
                currPredIsReal = false;
                
                // this arc is admissible

                if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) {
                    std::cout<<"Looking for Admissible Arc: (j, i): "<<std::endl;
                    std::cout<<"d["<<G[i].name<<"] = "<<d[i]<<" d["<<G[j].name<<"] = "<<d[j]<<std::endl;
                    std::cout<<"cap ("<<G[j].name<<", "<<G[i].name<<"): "<<G[*in_e_it].cap<<" f ("<<G[j].name<<", "<<G[i].name<<"): "<<G[*in_e_it].f<<std::endl;

                }

                // simulate the existence of an edge using f

                if ((d[i] == d[j] + 1) && G[*in_e_it].f > 0){

                    admissibleArc = *in_e_it;
                    arcFound = true;
                    break;

                }
            }
        }

        // if arc found
        if (arcFound){

            // advance and augment

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"Admissible Arc found. admissibleArc: ("<<G[source(admissibleArc, G)].name<<", "<<G[target(admissibleArc, G)].name<<")"<<std::endl;

            Vertex j;
            if (currPredIsReal){
                j = target(admissibleArc, G);
            }
            else{
                j = source(admissibleArc, G);
            }
            pred[j] = i;
            predIsReal[j] = currPredIsReal;
            i = j;

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"pred set. i advanced. i=j: "<<G[i].name<<" pred[j]: "<<G[pred[j]].name<<std::endl;

            if (i != t){
                continue;
            }
            // augment and set i = s

            // use pred to find identify the augmenting path P.
            // set f of all edges in path to min{r(ji) E P}

            int delta = INT_MAX;

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"i == t. Augment."<<std::endl;

            // use pred to reach s and find min delta for arcs (j, i)

            std::queue<Edge> pathEdges;
            std::queue<bool> addFlow;

            while (i != s){

                j = pred[i];
                if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"i: "<<G[i].name<<" j (pred[i]): "<<G[j].name<<std::endl;

                // find the edge. account for the direction of the real edge in G since we simulate G_f

                if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)){
                    if (predIsReal[i]){
                        std::cout<<G[i].name<<" has real pred"<<std::endl;
                    }
                    else{
                        std::cout<<G[i].name<<" has reverse pred"<<std::endl;
                    }
                } 

                if (predIsReal[i]){
                    for (tie(in_e_it, in_e_it_end) = in_edges(i, G); in_e_it != in_e_it_end; in_e_it++){
                        if (source(*in_e_it, G) != j) continue;

                        int r = G[*in_e_it].cap - G[*in_e_it].f;

                        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"push edge: ("<<G[source(*in_e_it, G)].name<<", "<<G[target(*in_e_it, G)].name<<") cap: "<<G[*in_e_it].cap<<" f: "<<G[*in_e_it].f<<std::endl;
                        
                        pathEdges.push(*in_e_it);
                        addFlow.push(true);

                        if (r < delta){
                            delta = r;
                        }

                        break;

                    }

                    i = j;

                }
                else{
                    for (tie(out_e_it, out_e_it_end) = out_edges(i, G); out_e_it != out_e_it_end; out_e_it++){
                        if (target(*out_e_it, G) != j) continue;

                        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"push reverse edge: ("<<G[source(*out_e_it, G)].name<<", "<<G[target(*out_e_it, G)].name<<") cap: "<<G[*out_e_it].cap<<" f: "<<G[*out_e_it].f<<std::endl;
                        // if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"push reverse edge: ("<<G[source(*out_e_it, G)].name<<", "<<G[target(*out_e_it, G)].name<<") reversef: "<<G[*out_e_it].reversef<<std::endl;

                        pathEdges.push(*out_e_it);
                        addFlow.push(false);

                        int r = G[*out_e_it].f;

                        if (r < delta){
                            delta = r;
                        }

                        break;
                    }

                    i = j;
                }

            }

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"delta: "<<delta<<std::endl;

            // augment edges

            while (!pathEdges.empty()){
                Edge e = pathEdges.front();
                pathEdges.pop();
                
                if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"Augmenting Edge: ("<<G[source(e, G)].name<<", "<<G[target(e, G)].name<<") flow: "<<G[e].f<<std::endl;
                
                if (addFlow.front()){
                    G[e].f += delta;
                    // G[e].reversef += delta;
                }
                else{
                    G[e].f -= delta;
                    // G[e].reversef -= delta;
                }
                addFlow.pop();

            }

            i = s;

        }
        else{

            if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"Admissible Arc not found. Retreat. i = "<<G[i].name<<std::endl;

            // retreat

            // A(i) : out edges
            // r(ij) : residual capacity = cap(ij) - f(ij)

            // find the min [d(j) + 1] from in edges of i and r(ij) > 0

            int mind = INT_MAX;
            bool mind_found = false;

            if (predIsReal[i]){

                for (tie(in_e_it, in_e_it_end) = in_edges(i, G); in_e_it != in_e_it_end; in_e_it++){
                    if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"in edge: ("<<G[source(*in_e_it, G)].name<<", "<<G[target(*in_e_it, G)].name<<") cap: "<<G[*in_e_it].cap<<" f: "<<G[*in_e_it].f<<std::endl;
                    
                    if (!(G[*in_e_it].f > 0)) continue;
    
                    int newD = d[source(*out_e_it, G)] + 1;
    
                    if (newD == 0) continue;
    
                    if (newD < mind){
                        mind = newD;
                        mind_found = true;
    
                        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"min d found in in_edges. mind: "<<mind<<std::endl;
                    }
                }
                
            }
            else{
                
                for (tie(out_e_it, out_e_it_end) = out_edges(i, G); out_e_it != out_e_it_end; out_e_it++){
    
                    if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"out edge: ("<<G[source(*out_e_it, G)].name<<", "<<G[target(*out_e_it, G)].name<<") cap: "<<G[*out_e_it].cap<<" f: "<<G[*out_e_it].f<<std::endl;
    
                    int r = G[*out_e_it].cap - G[*out_e_it].f;
                    if (!(r > 0)) continue;
    
                    int newD = d[target(*out_e_it, G)] + 1;
    
                    if (newD == 0) continue;
    
                    if (newD < mind){
                        mind = newD;
                        mind_found = true;
    
                        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)) std::cout<<"min d found in out_edges. mind: "<<mind<<std::endl;
                    }
                }

                
            }


            if (mind_found) {
                d[i] = mind;
            }
            else{
                d[i] += 1;
            }

            if (i != s){
                i = pred[i];
            }

        }

        if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)){
            char charr;
            std::cout<<"Enter S to skip. Anything else for next loop. ";
            std::cin>>charr;
            if (charr == 's' || charr == 'S') PRINT_PROGRESS = false;
            std::cout<<std::endl;

        }

        // emergency loop break if it gets trapped
        // if (loop > n*n*m) {
        //     // std::cout<<d[s]<<std::endl;
        //     std::cout<<"escaped"<<std::endl;
        //     std::cout<<"trapped in node: "<<G[i].name<<std::endl;
        //     break;
        // };

        loop++;

        
    }

    // print Vertex name and its distance

    if (PRINT_PROGRESS && (loop % ASK_FOR_INPUT_EVERY_LOOPS == 0)){
        for (tie(v_it, v_it_end) = vertices(G); v_it != v_it_end; v_it++){
            std::cout<< G[*v_it].name << " d: "<< d[*v_it]<< std::endl;
        }

    }

    return true;
    
}

// calculates the exact distance labels for each vertex and stores them in d

void calcDist(Graph& G, std::vector<bool>& visited, std::vector<int>& d, std::queue<Vertex>& currentVertices, int currentDist){
    
    InEdgeIterator in_e_it, in_e_it_end;
    std::queue<Vertex> nextVertices;
    currentDist++;

    while (!currentVertices.empty()){

        Vertex y = currentVertices.front();
        currentVertices.pop();

        // if (y == s) continue;

        // std::cout<<"curr vertex: "<<G[y].name<<std::endl;

        for (tie(in_e_it, in_e_it_end) = in_edges(y, G); in_e_it != in_e_it_end; in_e_it++){
            
            Vertex i = source(*in_e_it, G);
            // std::cout<< "now checking source i: "<<G[i].name<<std::endl;

            if (visited[i]){
                // std::cout<<G[i].name<<" is already visited"<<std::endl;
                continue;
            } 
            
            visited[i] = true;
            d[i] = currentDist;
            nextVertices.push(i);

            // std::cout<<"visited["<<G[i].name<<"] = true dist: "<<d[i]<<std::endl;
            
        }
        
    }
    while(!nextVertices.empty()){
        calcDist(G, visited, d, nextVertices, currentDist);
    }
    
}


// returns true if there is a path from the source to the target node by calling itself
// only called by checkNodeConnected
bool runCheckVertexConnected(Graph& BG, Vertex source_vertex, Vertex target_vertex, std::vector<bool>& visited){
    if (source_vertex == target_vertex) return true;
    if (visited[source_vertex]) return false;
    visited[source_vertex] = true;
    OutEdgeIterator out_e_it, out_e_it_end;

    for (tie(out_e_it, out_e_it_end) = out_edges(source_vertex, BG); out_e_it != out_e_it_end; out_e_it++){
        Vertex v = target(*out_e_it, BG);
        if (runCheckVertexConnected(BG, v, target_vertex, visited)) return true;
    }
    return false;

    // forall_out_edges(e, source_vertex){
    //     leda::node v = LG.target(e);
    //     if (runCheckNodeConnected(LG, v, target_vertex, visited)) return true;
    // }
    // return false;
}

// returns true if there is a path from the source node to the target node
bool checkVertexConnected(Graph& BG, Vertex source_vertex, Vertex target_vertex){
    
    std::vector<bool> visited(num_vertices(BG), false);
    
    return runCheckVertexConnected(BG, source_vertex, target_vertex, visited);
    
}

