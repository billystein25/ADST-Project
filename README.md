# ADST-Project
This project was made for the undergraduate elective course Algorithmic Data Science Techniques (CEID1524). The goal of this project was to implement a shortest augmented path algorithm on a Boost graph. The project runs my implementation of the shortest augmented path algorithm on a Boost graph and compares its execution time to the execution time of the MAX_FLOW_T function from the Leda library on the same graph. The graph is generated using Leda and then translated to a Boost graph.

# Setup
This project was made on a Red Hat Linux Distribution and was compiled using GCC 4.4.7

This project requires the LEDA and Boost libraries to be installed.

# Usage 
Navigate to the bin/ directory and run the `make` command. This will create an executable file called `main` that you can execute with `make run`. To change the details of the execution you will have to change which functions are called in `main.cpp` on the lines 98~118 and recompile the project.

# File Structure
The project includes 3 main directories. The bin/ directory is where the objects and compiled executables are stored, the incl/ directory is where the .hpp files are stored, and the src/ directory is where the main .cpp files are stored.

# Implementation
**Note:** The boost graph struct and the visitor graph were provided by the course lab and were not implemented by me.

The original boost graph struct that was provided by the course lab was modified to include flow and capacity values for each edge. Each vertex also includes a name property though that was only used for debugging. The graphs that are used are generated with LEDA and they are directional loop free graphs where a connection of type A->B and B->A is not allowed. Instead the reverse edges are simulated using the flow of the respective edges. For every vertex an out edge only exists if `cap - f > 0` for that edge. Similarly an in edge only exists if `f > 0`. These conditions are checked regardless of if a real edge exists in G. That way I can use all the out edges in G_f without modifing G by adding or removing edges. 

In addition to the shortest augmented path algorithm, I also implemented a checker that returns `true` or `false` based on the flow of the input graph. This checker was not part of the project and was implemented for debugging purposes.

# Shortcomings
- Having to recompile the executable every time you need to run the program with different inputs is obviously not ideal. Should I remake this today I would feed the input either as command line arguments, or through a config file.
