#ifndef CHECKMAXFLOWT
#define CHECKMAXFLOWT

#include <iostream>

/* Functions */                    /* μπορούμε να χρησιμοποιήσουμε και array αντί για vector για το dist και το pred */
bool CHECK_MAX_FLOW_T(Graph& G, Vertex s, Vertex t, bool printErrorName = true);

#endif