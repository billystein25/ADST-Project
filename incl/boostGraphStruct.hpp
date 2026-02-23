#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>

/* Boost Graph */
#include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/bellman_ford_shortest_paths.hpp>

struct myEdge
{
	int cap;
    int f;
    // int reversef;
};

struct myVertex
{
    std::string name;

    // Constructor
    myVertex() : name("noVertexName") {}

};

/* 1o όρισμα: σε τι container αποθηκεύουμε τις ακμές (vecS, listS) */
/* 2ο όρισμα: σε τι container αποθηκεύουμε τις κορυφές (vecS, listS) */
/* 3ο όρισμα: ορίζουμε αν το γράφημα είναι κατευθυνόμενο (directedS / bidirectionalS) ή μη κατευθυνόμενο (undirectedS) */
/* 4ο όρισμα: το struct με τις ιδιότητες των κορυφών ή boost::no_property αν δεν έχει καμία ιδιότητα. Θα βάλουμε no_property 
              γιατί οι κορυφές στην Boost έχουν αρίθμηση από 0 έως n-1. Άρα ό,τι πληροφορία θέλουμε για τις κορυφές μπορούμε
              να την αποθηκεύσουμε σε ένα vector ή σε ένα array. */
/* 5ο όρισμα: το struct με τις ιδιότητες των ακμών ή boost::no_property αν δεν έχει καμία ιδιότητα. Για τον αλγόριθμο χρειαζόμαστε
              μόνο ένα βάρος weight για τις ακμές, οπότε κατασκευάζουμε το struct myEdge. */
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, myVertex, myEdge> Graph;

/* Vertex descriptor */
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

/* Edge descriptor */
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

/* Iterators */
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;
typedef boost::graph_traits<Graph>::in_edge_iterator InEdgeIterator;
typedef boost::graph_traits<Graph>::adjacency_iterator AdjIterator;

/* Functions */                    /* μπορούμε να χρησιμοποιήσουμε και array αντί για vector για το dist και το pred */
// in specific files

#endif