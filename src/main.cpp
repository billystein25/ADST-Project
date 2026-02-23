#include <iostream>
#include <string> 
#include <sstream>

#include "boostGraphStruct.hpp"
#include "visitor_graph.hpp"

#include "checkMaxFlow.hpp"
#include "shortestAugmentedPath.hpp"

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/templates/max_flow.h> 
#include <LEDA/graph/graph_misc.h>
#include <LEDA/core/array2.h>

using namespace boost;

// prints every graph edge with its flow and capacity
void printGraphFlow(Graph& G);

void custom_graph();

void random_graph_evaluation(int num_nodes, int num_edges, int repetitions, bool print_progress = false, bool run_checker = false);

void random_grid_graph_evaluation(int num_nodes, int repetitions, bool print_progress = false, bool run_checker = false);


// returns true if there is a path from the source to the target node by calling itself
// only called by checkNodeConnected
bool runCheckNodeConnected(leda::graph& LG, leda::node& source_node, leda::node& target_node, leda::node_array<bool>& visited){
    // LG.print_node(source_node);
    if (source_node == target_node) return true;
    if (visited[source_node]) return false;
    visited[source_node] = true;
    leda::edge e;
    forall_out_edges(e, source_node){
        leda::node v = LG.target(e);
        if (runCheckNodeConnected(LG, v, target_node, visited)) return true;
    }
    return false;
}

// returns true if there is a path from the source node to the target node
bool checkNodeConnected(leda::graph& LG, leda::node& source_node, leda::node& target_node){
    
    leda::node_array<bool> visited(LG, LG.number_of_nodes(), false);
    
    return runCheckNodeConnected(LG, source_node, target_node, visited);
    
}


// transforms a LEDA graph to Boost. Given from the lab.
// modification: added [l_source_node], [l_sink_node], [b_source_node], and [b_sink_node]
// the former two are leda nodes mapped to the latter two so that the leda algorithms use
// the same nodes as the boost ones. That way all algorithms start from the same node

void LEDA_Graph_To_Boost(leda::graph& L_G, leda::edge_array<int>& lcap, leda::edge_array<int>& lf, leda::node& l_source_node, leda::node& l_sink_node, Graph& B_G, Vertex& b_source_node, Vertex& b_sink_node, leda::node_array<std::string>& names)
{
    leda::node_array<Vertex> Map(L_G);

    leda::edge e;
    leda::node n;

    forall_nodes(n, L_G)
    {
        Vertex v = add_vertex(B_G);
        Map[n] = v;
        B_G[v].name = names[n];
        if (n == l_source_node) b_source_node = v;
        else if (n == l_sink_node) b_sink_node = v;
    }

    forall_edges(e, L_G)
    {
        Edge u = add_edge(Map[L_G.source(e)], Map[L_G.target(e)], B_G).first;
        B_G[u].cap = lcap[e];
        B_G[u].f = lf[e];
    }
}


int main()
{

    // Part 1
    // implement and test

    // custom_graph();

    // Part 2
    // random graph evaluation

    // simple m = 2nlogn

    std::cout<<"Simple evaluation"<<std::endl;

    random_graph_evaluation(1000, 6000, 5, false, false);
    random_graph_evaluation(4000, 28816, 5, false, false);
    random_graph_evaluation(8000, 62449, 5, false, false);

    // dense m = 20nlogn

    std::cout<<"Dense evaluation"<<std::endl;

    random_graph_evaluation(1000, 60000, 5, false, false);
    random_graph_evaluation(4000, 288164, 5, true, false);
    // random_graph_evaluation(8000, 624494, 1, false, false);

    std::cout<<"Grid evaluation"<<std::endl;

    // custom grid. all horizontal edges point right. all vertical edges point down
    // top left is source, bottom right is sink
    // grid size is num_nodes x num_nodes

    // random_grid_graph_evaluation(100, 5, false, false);
    // random_grid_graph_evaluation(200, 5, false, false);
    // random_grid_graph_evaluation(300, 2, true, false);




    return 0;

}

void printGraphFlow(Graph& G){

    std::cout<<"Graph Flow: "<<std::endl;

    EdgeIterator e_it, e_it_end;

    for (tie(e_it, e_it_end) = edges(G); e_it != e_it_end; e_it++){
        Vertex s = source(*e_it, G);
        Vertex t = target(*e_it, G);
        
        std::cout<< "Edge: ("<<G[s].name<< ", "<< G[t].name<< ") flow: "<< G[*e_it].f<< " cap: "<< G[*e_it].cap<< std::endl;
    }

    std::cout<<std::endl;

}

void random_graph_evaluation(int num_nodes, int num_edges, int repetitions, bool print_progress, bool run_checker){

    const bool PRINT_DEBUG = false;
    const bool PRINT_SOURCE_AND_SINK = false;

    std::cout<<"Running "<<repetitions<< " repetitions on custom grid graph with "<<num_nodes<<" nodes and "<<num_edges<<" edges\n";

    float complete_time_L = 0;
    float complete_time_B = 0;

    for (int rep_num = 0; rep_num < repetitions; rep_num++){

        float start_T, finish_T;

        if (print_progress) std::cout<< "\nIteration: "<< rep_num<<"\nCreating Graph\n\n";

        // create random graph with leda

        leda::graph LG;
        leda::edge e;
        leda::node v;

        random_simple_loopfree_graph(LG, num_nodes, num_edges);
        Make_Connected(LG);

        leda::node source_node = LG.choose_node();
        leda::node sink_node = LG.choose_node();

        // sink != source
        while (sink_node == source_node){
            sink_node = LG.choose_node();
        }

        while(!checkNodeConnected(LG, source_node, sink_node)){
            random_simple_loopfree_graph(LG, num_nodes, num_edges);
            Make_Connected(LG);

            source_node = LG.choose_node();
            sink_node = LG.choose_node();

            // sink != source
            while (sink_node == source_node){
                sink_node = LG.choose_node();
            }

        }

        if (PRINT_DEBUG || PRINT_SOURCE_AND_SINK){
            std::cout<<"source: ";
            LG.print_node(source_node);
            std::cout<<" sink: ";
            LG.print_node(sink_node);
            std::cout<<std::endl;

        }

        leda::edge_array<int> flow(LG, LG.number_of_edges(), 0);
        leda::edge_array<int> cap(LG, LG.number_of_edges(), 0);

        // give edge capacities random values
        leda::random_source S; 

        forall_edges(e, LG){
            cap[e] = S(100, 10000);
        }

        // name nodes

        int number = 0;
        leda::node_array<std::string> names(LG, LG.number_of_nodes(), "unnamed");
        forall_nodes(v, LG){
            std::stringstream gstream;
            gstream << number;
            names[v]=gstream.str();
            number++;
        }

        if (PRINT_DEBUG) LG.print();
        if (PRINT_DEBUG){
            if (checkNodeConnected(LG, source_node, sink_node)){
                std::cout<<"source and sink are connected"<<std::endl;
            }
            else{
                std::cout<<"source and sink are not connected"<<std::endl;
            }

        }


        if (print_progress) std::cout<< "Running MAX_FLOW_T\n\n";

        start_T = leda::used_time();
        MAX_FLOW_T(LG, source_node, sink_node, cap, flow);
        finish_T = leda::used_time(start_T);

        complete_time_L += finish_T;
        
        if (print_progress) std::cout<< "MAX_FLOW_T finished with time: "<< finish_T<<" seconds\n\n";

        Graph BG;

        Vertex b_source, b_sink;

        LEDA_Graph_To_Boost(LG, cap, flow, source_node, sink_node, BG, b_source, b_sink, names);

        if (PRINT_DEBUG) printGraphFlow(BG);

        if (run_checker){
            bool res = CHECK_MAX_FLOW_T(BG, b_source, b_sink, false);
            if (res){
                std::cout<<"Graph has max flow\n"<<std::endl;
            }
            else{
                std::cout<<"Graph does not have max flow\n"<<std::endl;
            }
        }

        if (print_progress) std::cout<<"Running shortestAugmentedPath\n\n";

        start_T = leda::used_time();
        shortestAugmentedPath(BG, b_source, b_sink);
        finish_T = leda::used_time(start_T);

        complete_time_B += finish_T;

        if (print_progress) std::cout<< "shortestAugmentedPath finished with time: "<< finish_T<<" seconds\n\n";

        if (PRINT_DEBUG) printGraphFlow(BG);

        if (run_checker){
            bool res = CHECK_MAX_FLOW_T(BG, b_source, b_sink, false);
            if (res){
                std::cout<<"Graph has max flow"<<std::endl;
            }
            else{
                std::cout<<"Graph does not have max flow"<<std::endl;
            }
        }

    }

    float avrg_L = complete_time_L / repetitions;
    float avrg_B = complete_time_B / repetitions;

    std::cout<< repetitions<< " repetitions of LEDA MAX_FLOW_T took total "<< complete_time_L<< " seconds with average time "<< avrg_L<<" seconds\n\n";
    std::cout<< repetitions<< " repetitions of shortestAugmentedPath took total "<< complete_time_B<< " seconds with average time "<< avrg_B<<" seconds\n\n";

}

void random_grid_graph_evaluation(int num_nodes, int repetitions, bool print_progress, bool run_checker){
    
    const bool PRINT_DEBUG = false;
    const bool PRINT_SOURCE_AND_SINK = false;

    std::cout<<"Running "<<repetitions<< " repetitions on random graphs with "<<num_nodes<<" nodes\n";

    float complete_time_L = 0;
    float complete_time_B = 0;

    for (int rep_num = 0; rep_num < repetitions; rep_num++){

        float start_T, finish_T;

        if (print_progress) std::cout<< "\nIteration: "<< rep_num<<"\nCreating Graph\n\n";

        // create random graph with leda

        leda::graph LG;
        leda::edge e;
        leda::node v;

        leda::array2< leda::node > nodes_array(num_nodes, num_nodes);

        for (int y = 0; y < num_nodes; y++){
            for (int i = 0; i < num_nodes; i++){
                nodes_array(i, y) = LG.new_node();
            }
        }

        for (int y = 0; y < num_nodes; y++){
            for (int i = 0; i < num_nodes-1; i++){
                LG.new_edge(
                            nodes_array(i, y),  /*source node*/
                            nodes_array(i+1, y) /*target node*/
                            );
            }
        }

        for (int y = 0; y < num_nodes-1; y++){
            for (int i = 0; i < num_nodes; i++){
                LG.new_edge(
                            nodes_array(i, y),  /*source node*/
                            nodes_array(i, y+1) /*target node*/
                            );
            }
        }

        leda::node source_node = nodes_array(0, 0);
        leda::node sink_node = nodes_array(num_nodes-1, num_nodes-1);

        if (PRINT_DEBUG || PRINT_SOURCE_AND_SINK){
            std::cout<<"source: ";
            LG.print_node(source_node);
            std::cout<<" sink: ";
            LG.print_node(sink_node);
            std::cout<<std::endl;

        }

        leda::edge_array<int> flow(LG, LG.number_of_edges(), 0);
        leda::edge_array<int> cap(LG, LG.number_of_edges(), 0);

        // give edge capacities random values
        leda::random_source S; 

        forall_edges(e, LG){
            cap[e] = S(100, 10000);
        }

        // name nodes

        int number = 0;
        leda::node_array<std::string> names(LG, LG.number_of_nodes(), "unnamed");
        forall_nodes(v, LG){
            std::stringstream gstream;
            gstream << number;
            names[v]=gstream.str();
            number++;
        }

        if (PRINT_DEBUG) LG.print();
        if (PRINT_DEBUG){
            if (checkNodeConnected(LG, source_node, sink_node)){
                std::cout<<"source and sink are connected"<<std::endl;
            }
            else{
                std::cout<<"source and sink are not connected"<<std::endl;
            }

        }


        if (print_progress) std::cout<< "Running MAX_FLOW_T\n\n";

        start_T = leda::used_time();
        MAX_FLOW_T(LG, source_node, sink_node, cap, flow);
        finish_T = leda::used_time(start_T);

        complete_time_L += finish_T;
        
        if (print_progress) std::cout<< "MAX_FLOW_T finished with time: "<< finish_T<<" seconds\n\n";

        Graph BG;

        Vertex b_source, b_sink;

        LEDA_Graph_To_Boost(LG, cap, flow, source_node, sink_node, BG, b_source, b_sink, names);

        if (PRINT_DEBUG) printGraphFlow(BG);

        if (run_checker){
            bool res = CHECK_MAX_FLOW_T(BG, b_source, b_sink, false);
            if (res){
                std::cout<<"Graph has max flow\n"<<std::endl;
            }
            else{
                std::cout<<"Graph does not have max flow\n"<<std::endl;
            }
        }

        if (print_progress) std::cout<<"Running shortestAugmentedPath\n\n";

        start_T = leda::used_time();
        shortestAugmentedPath(BG, b_source, b_sink);
        finish_T = leda::used_time(start_T);

        complete_time_B += finish_T;

        if (print_progress) std::cout<< "shortestAugmentedPath finished with time: "<< finish_T<<" seconds\n\n";

        if (PRINT_DEBUG) printGraphFlow(BG);

        if (run_checker){
            bool res = CHECK_MAX_FLOW_T(BG, b_source, b_sink, false);
            if (res){
                std::cout<<"Graph has max flow"<<std::endl;
            }
            else{
                std::cout<<"Graph does not have max flow"<<std::endl;
            }
        }

    }

    float avrg_L = complete_time_L / repetitions;
    float avrg_B = complete_time_B / repetitions;

    std::cout<< repetitions<< " repetitions of LEDA MAX_FLOW_T took total "<< complete_time_L<< " seconds with average time "<< avrg_L<<" seconds\n\n";
    std::cout<< repetitions<< " repetitions of shortestAugmentedPath took total "<< complete_time_B<< " seconds with average time "<< avrg_B<<" seconds\n\n";

}


void custom_graph(){
    /*
      B - D
     / \ / \
    A   X   F
     \ / \ /
      C - E
    */


    Graph G;
    Vertex A = add_vertex(G); // 0 // source
    Vertex B = add_vertex(G); // 1
    Vertex C = add_vertex(G); // 2 
    Vertex D = add_vertex(G); // 3
    Vertex E = add_vertex(G); // 4
    Vertex F = add_vertex(G); // 5 // sink
    // Vertex H = add_vertex(G); // 6 
    // Vertex J = add_vertex(G); // 7 
    // Vertex K = add_vertex(G); // 8 
    // Vertex I = add_vertex(G); // 9 
    
    // add_edge(A, B, G); // 1
    // add_edge(A, C, G);
    // add_edge(B, D, G); // 3
    // add_edge(B, E, G);
    // add_edge(C, D, G); // 5
    // add_edge(C, E, G);
    // add_edge(D, F, G); // 7
    // add_edge(E, F, G);

    add_edge(A, B, G);
    add_edge(B, C, G);
    add_edge(C, D, G);
    add_edge(D, F, G);
    add_edge(A, E, G);
    add_edge(E, F, G);

    // add_edge(E, I, G); // 9
    // add_edge(F, J, G);
    // add_edge(J, K, G); // 11
    // add_edge(K, I, G);

    // set names
    VertexIterator v_it, v_it_end;
    int number = 0;
    for (tie(v_it, v_it_end) = vertices(G); v_it != v_it_end; v_it++){
        std::stringstream gstream;
        gstream << number;
        G[*v_it].name = gstream.str();
        number++;

    }

    EdgeIterator e_it, e_it_end;

    int n = num_vertices(G);
    std::vector<int> cap(n, 10);
    std::vector<int> f(n, 10);

    int edge_num = 0;

    // manually set capacities for testing 

    for (tie(e_it, e_it_end) = edges(G); e_it != e_it_end; e_it++){
        
        edge_num++;

        switch (edge_num){

            case 1: // A - B
                G[*e_it].cap = 3;
                G[*e_it].f = 8;
                break;
            case 2: // A - C
                G[*e_it].cap = 4;
                G[*e_it].f = 8;
                break;
            case 3: // B - D
                G[*e_it].cap = 3;
                G[*e_it].f = 3;
                break;
            case 4: // B - E
                G[*e_it].cap = 1;
                G[*e_it].f = 3;
                break;
            case 5: // C - D
                G[*e_it].cap = 1;
                G[*e_it].f = 3;
                break;
            case 6: // C - E
                G[*e_it].cap = 2;
                G[*e_it].f = 0;
                break;
            case 7: // D - F
                G[*e_it].cap = 3;
                G[*e_it].f = 3;
                break;
            case 8: // E - F
                G[*e_it].cap = 5;
                G[*e_it].f = 3;
                break;
            // case 9: // E - I
            //     G[*e_it].cap = 6;
            //     G[*e_it].f = 3;
            //     break;
            // case 10: // F - J
            //     G[*e_it].cap = 10;
            //     G[*e_it].f = 5;
            //     break;
            // case 11: // J - K
            //     G[*e_it].cap = 5;
            //     G[*e_it].f = 5;
            //     break;
            // case 12: // K - I
            //     G[*e_it].cap = 5;
            //     G[*e_it].f = 6;
            //     break;
        }

        // std::cout<<*e_it<<std::endl;

    }
    // G[edge()].f = 5;

    printGraphFlow(G);

    /****************************
      DON'T FORGET TO CHANGE ME
    ****************************/
    Vertex source = A;
    Vertex sink = F;

    if (CHECK_MAX_FLOW_T(G, source, sink)){
        std::cout<<"Checker returned true. Flow is max."<<std::endl;
    }
    else{
        std::cout<<"Checker returned false. Flow is not max."<<std::endl;
    }

    std::cout<<"Running shortest augmented path algorithm."<<std::endl;

    shortestAugmentedPath(G, source, sink);
    
    printGraphFlow(G);

    if (CHECK_MAX_FLOW_T(G, source, sink)){
        std::cout<<"Checker returned true. Flow is max."<<std::endl;
    }
    else{
        std::cout<<"Checker returned false. Flow is not max."<<std::endl;
    }
}