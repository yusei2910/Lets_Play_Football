#ifndef MSTGRAPH_H
#define MSTGRAPH_H

#include<vector>
#include<iostream>
#include<string>
#include<climits>
#include<iomanip>
#include"dbmanager.h"

using namespace std;

/**
 * @brief The mstEdge struct represents an edge in the graph
 */
struct mstEdge{
    int start;
    int end;
    int weight;
    mstEdge(int start, int end, int weight):start(start),end(end),weight(weight){}
};

/**
 * @brief The mstVertex struct represents a vertex in the graph
 */
struct mstVertex{
    int key;
    int index;
    mstVertex* parent; //parent node
};

/**
 * @brief The mstGraph class represent the graph of stadiums
 */
class mstGraph{
private:
    vector<mstVertex> vertices;    //Vector to store the vertices
    int numVertex; // number of vertices
    int** adjMatrix;    // adjacency matrix

    vector<mstVertex*> mstVec; // vector working as a priority queue

    /**
     * @brief Function to create the adjacency matrix and initialize all the element to -1
     **/
	void InitializeMatrix();

    /**
     * @brief Function to delete the matrix
     **/
	void DeleteMatrix();

    /**
     * @brief Function to find the minimum value for MST and remove from the "queue"
     * @return the pointer to the vertex with the minimum key
     **/
	mstVertex* ExtractMin();

    /**
     * @brief Function to do the MST
     * @param start: starting location
     */
	void PrimMST(int start);



    /**
     * @brief Function to check if the vertex is in the mstVec
     * @param search: vertex to be checked
     **/
    bool isExist(mstVertex search);

public:
    /**
     * @brief Default constructor
     **/
    mstGraph();

    /**
     * @brief Function to add an edge into the adjacency matrix
     * @param start: starting location
     * @param end: ending location
     * @param weight: distance between 2 states
     **/
	void AddEdge(int start, int end,int weight);

    /**
     * @brief Function to get the total mileage of the MST
     * @return total mileage of MST
     **/
	long GetMSTdistance();

    /**
     * @brief Function to do the MST and store the edges into a vector
     */
	void GetMST(vector<mstEdge>&);

    /**
     * Class destructor
     **/
    ~mstGraph();
};

#endif // MSTGRAPH_H
