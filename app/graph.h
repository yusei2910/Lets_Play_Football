#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <QVector>
#include "dbmanager.h"


template<typename T>
/*!
 * \brief The Edge struct
 */
struct Edge {
	T u;    /// start
	T v;    /// end
	bool discoverEdge;
	int weight;

	/***************************************************************
	 * Edge();
	 *   Constructor; Initialize class attributes
	 *   Parameters: none
	 *   Return: none
	 ***************************************************************/
	Edge()
			: u(T{}), v(T{}), discoverEdge(false), weight(0) {}

	/***************************************************************
	 * Edge(T u, T v, int weight)
	 *   Constructor; Initialize class attributes
	 *   Parameters: none
	 *   Return: none
	 ***************************************************************/
	Edge(T u, T v, int weight)
			: u{u}, v{v}, discoverEdge{false}, weight{weight} {}

	bool operator==(const Edge<T> &rhs) const {
		return u == rhs.v && v == rhs.u;
	}

	bool operator!=(const Edge<T> &rhs) const { return !operator==(rhs); }
};


template<typename T>
/*!
 * \brief The Vertex struct
 */
struct Vertex {

	T vert;
	bool visited;
	std::vector<Edge<T>> edgeList;

	/***************************************************************
	 * Vertex();
	 *   Constructor; Initialize class attributes
	 *   Parameters: none
	 *   Return: none
	 ***************************************************************/
	Vertex() : vert(T{}), visited(false) {}

	/***************************************************************
	 * Vertex(T vert, bool visited)
	 *   Constructor; Initialize class attributes
	 *   Parameters: none
	 *   Return: none
	 ***************************************************************/
	Vertex(T vert, bool visited) : vert{vert}, visited{visited} {}

	/*******************************************************************
	 * int NumOfEdges() const
	 *
	 *   Accessor; number of edges
	 *------------------------------------------------------------------
	 *   Parameter: (none)
	 *------------------------------------------------------------------
	 *   Return: (int) - number of edges
	 *******************************************************************/
	int NumOfEdges() const { return edgeList.size(); }
};

template<typename T>
/*!
 * \brief The Graph class with dijkstra logic
 */
class Graph {
public:
	/*!
	 * \brief Graph constructor
	 */
	Graph();

	/*!
	 * \brief Graph destructor
	 */
	~Graph() = default;

	/*!
	 * \brief generateGraph from db
	 */
	void GenerateGraph();

	/*!
	 * \brief checks if graph is empty
	 * \return bool
	 */
	bool Empty() const;

	/*!
	 * \brief size of graph
	 * \return int
	 */
	int Size() const;

	/*!
	 * \brief insertVertex
	 * \param vert; type T
	 */
	void InsertVertex(const T &vert);

	/*!
	 * \brief findVertex
	 * \param vert
	 * \return index in graph container
	 */
	int FindVertex(const T &vert) const;

	/*!
	 * \brief insertEdge
	 * \param u; starting vert
	 * \param v; ending vert
	 * \param weight; "distance"
	 */
	void InsertEdge(const T &u, const T &v, int weight);

	/*!
	 * \brief vertices
	 * \return list of vertices in graph
	 */
	std::vector<T> Vertices() const;

	/*!
	 * \brief edges
	 * \return list of edges in graph
	 */
	std::vector<T> Edges() const;

	/*!
	 * \brief DijkstraPathFinder shortest path algorithm
	 * \param u; starting vert
	 * \param loadGraph
	 * \param costs; weights of the paths
	 * \param parent; holds previous vert needed to path tracing
	 */
	void DijkstraPathFinder(const T &u,
							std::vector<T> &loadGraph,
							int *costs, int *parent);

	/*!
	 * \brief returnPath
	 * \param u; starting vert
	 * \param v; ending vert
	 * \param parent; array of parent verts
	 * \return path
	 */
	std::vector<T> ReturnPath(const T &u,
							  const T &v,
							  const int parent[]);

private:
	std::vector<Vertex<T>> graph;	/// Graph container
	int distance;

	// fills the vector with the next closest vertex
	// used in dijkstra algorithm
	void FindClosest(std::vector<T> &loadGraph, int costs[], int parent[]);

	int DistanceFromStart(const T &u, int costs[], int parent[]);

	// returns the index for the smallest edge from a vertex
	int SmallestEdge(int vertex);

	// returns distance as an int between two vertices in the graph
	// u denotes starting point and v is ending point
	int DistanceBetween(int u, int v);

	// returns the number of verts visited
	// used to check if search is completed
	int VerticesVisited();

	// returns the number of edges visited
	// returns 0 is nothing found
	int EdgesDiscovered(int currentVert);
};



/*
 *
 *
 *	IMPLEMENTATION SECTION
 *
 *
 */



template<typename T>
Graph<T>::Graph() : distance(0) {}

template<typename T>
void Graph<T>::GenerateGraph()
{
	QSqlQuery query;
	QString queryString = "SELECT id,distanceTo "
						  "FROM distance "
						  "WHERE beginStadium "
						  "IN(SELECT endStadium FROM distance WHERE id = :id) "
						  "AND endStadium = (SELECT beginStadium FROM distance WHERE id = :id) "
						  "AND id!=:id ";

	query.prepare(queryString);

	if constexpr (std::is_same<T, QString>::value) {
		QStringList teams;
		DBManager::instance()->GetTeams(teams);

		for (int i = 1; i <= teams.size(); i++) {
			InsertVertex(teams[i - 1]);
			query.bindValue(":id", i);
			if(!query.exec()) {
				qDebug() << "DBManager::generateGraph vertex: " << i << " failed";
			} else {
				while(query.next())
					InsertEdge(teams[i - 1], teams[query.value(0).toInt() - 1], query.value(1).toInt());
			}
		}
	} else if constexpr (std::is_same<T, int>::value) {
		int numTeams = DBManager::instance()->GetNumTeams();

		int i = 1;
		while (i <= numTeams) {
			InsertVertex(i);
			query.bindValue(":id", i);
			if(!query.exec()) {
				qDebug() << "DBManager::generateGraph vertex: " << i << " failed";
			} else {
				while(query.next())
					InsertEdge(i, query.value(0).toInt(), query.value(1).toInt());
				i++;
			}
		}
	}
}

template<typename T>
bool Graph<T>::Empty() const { return graph.empty(); }

template<typename T>
int Graph<T>::Size() const { return graph.size(); }

template<typename T>
void Graph<T>::InsertVertex(const T &vert) {
	if (FindVertex(vert) == Size())
		graph.push_back(Vertex<T>{vert, false});
}

template<typename T>
int Graph<T>::FindVertex(const T &vert) const {
	int index = 0;

	while (index < Size()) {
		if (graph.at(index).vert == vert)
			return index;
		index++;
	}
	return index;
}

template<typename T>
void Graph<T>::InsertEdge(const T &u, const T &v, int weight) {
	int index = FindVertex(u);

	if (index == Size()) {
		InsertVertex(u);
		InsertEdge(u, v, weight);
	} else {
		graph.at(index).edgeList.push_back(Edge<T>{u, v, weight});
	}
}

template<typename T>
std::vector<T> Graph<T>::Vertices() const {
	std::vector<T> temp;
	for (auto a: graph)
		temp.push_back(a.vert);
	return temp;
}

template<typename T>
std::vector<T> Graph<T>::Edges() const {
	std::vector<std::string> temp;
	for (size_t i = 0; i < Size(); i++) {
		if (graph.at(i).NumOfEdges() != 0) {
			for (size_t j = 0;
				 j < graph.at(i).NumOfEdges(); j++) {
				temp.push_back(
						"(" + std::to_string(graph.at(i).edgeList.at(j).u) +
						", " +
						std::to_string(graph.at(i).edgeList.at(j).v) + ")");
			}
		}
	}
	return temp;
}

template<typename T>
void Graph<T>::DijkstraPathFinder(const T &u, std::vector<T> &loadGraph,
								  int *costs, int *parent) {
	if (VerticesVisited() == Size()) {
		for (auto &i : graph) {
			i.visited = false;

			for (auto &j : i.edgeList)
				j.discoverEdge = false;
		}
	}

	int currVert = FindVertex(u);

	loadGraph.push_back(graph[currVert].vert);

	costs[currVert] = 0;
	parent[currVert] = -1;

	graph[currVert].visited = true;

	while ((int) loadGraph.size() != Size())
		FindClosest(loadGraph, costs, parent);
}

template<typename T>
std::vector<T>
Graph<T>::ReturnPath(const T& /*u*/, const T &v, const int *parent) {
	std::vector<T> path;

	int vertex = FindVertex(v);

	while (parent[vertex] != -1) {
		path.push_back(graph[vertex].vert);
		vertex = parent[vertex];
	}

	path.push_back(graph[vertex].vert);

	std::reverse(path.begin(), path.end());

	return path;
}

template<typename T>
void
Graph<T>::FindClosest(std::vector<T> &loadGraph, int *costs, int *parent) {
	if (loadGraph.size() == 1) {
		int frontVer = FindVertex(loadGraph.front());

		int nextVer = SmallestEdge(frontVer);

		costs[nextVer] = DistanceBetween(frontVer, nextVer);
		parent[nextVer] = frontVer;

		graph[nextVer].visited = true;

		loadGraph.push_back(graph[nextVer].vert);
	} else {
		int smallID = 0;
		int comparatorID = smallID + 1;

		int smallDistance;
		int comparatorDistance;

		int size = loadGraph.size();

		while (comparatorID < size) {
			int smallVer = FindVertex(loadGraph[smallID]);
			int compVer = FindVertex(loadGraph[comparatorID]);

			if (graph[smallVer].NumOfEdges() == EdgesDiscovered(smallVer)) {
				smallID++;
			} else {
				if (graph[compVer].NumOfEdges() !=
					EdgesDiscovered(compVer)) {
					smallDistance = DistanceBetween(smallVer,
													SmallestEdge(smallVer))
									+
									DistanceFromStart(graph[smallVer].vert,
													  costs,
													  parent);

					comparatorDistance =
							DistanceBetween(compVer, SmallestEdge(compVer))
							+ DistanceFromStart(graph[compVer].vert, costs,
												parent);

					if (smallDistance > comparatorDistance) {
						smallID = comparatorID;
						smallDistance = comparatorDistance;
					}
				}
			}
			comparatorID++;
		}

		int smallestVertex = SmallestEdge(FindVertex(loadGraph[smallID]));

		costs[smallestVertex] = smallDistance;
		parent[smallestVertex] = FindVertex(loadGraph[smallID]);

		graph[smallestVertex].visited = true;

		loadGraph.push_back(graph[smallestVertex].vert);
	}
}

template<typename T>
int Graph<T>::DistanceFromStart(const T &u, int *costs, int *parent) {
	int distance = 0;

	int vertex = FindVertex(u);

	while (costs[vertex] != 0) {
		distance += DistanceBetween(vertex, parent[vertex]);
		vertex = parent[vertex];
	}

	return distance;
}

template<typename T>
int Graph<T>::SmallestEdge(int vertex) {
	int smallestIndex = 0;
	int compIndex = smallestIndex + 1;

	int size = graph.at(vertex).NumOfEdges();

	while (compIndex < size) {
		int smallestVertex = FindVertex(
				graph.at(vertex).edgeList.at(smallestIndex).v);

		int compVertex = FindVertex(
				graph.at(vertex).edgeList.at(compIndex).v);

		if (graph.at(smallestVertex).visited) {
			smallestIndex++;
		} else {
			if (!(graph.at(compVertex).visited)) {
				if (graph.at(vertex).edgeList.at(smallestIndex).weight >=
					graph.at(vertex).edgeList.at(compIndex).weight)
					smallestIndex = compIndex;
			}
		}
		compIndex++;
	}
	smallestIndex = FindVertex(
			graph.at(vertex).edgeList.at(smallestIndex).v);

	return smallestIndex;
}

template<typename T>
int Graph<T>::DistanceBetween(int u, int v) {
	int i = 0;
	while (graph[u].edgeList[i].v != graph[v].vert) i++;
	return graph[u].edgeList[i].weight;
}

template<typename T>
int Graph<T>::VerticesVisited() {
	int visits = 0;

	typename std::vector<Vertex<T>>::iterator graphIt = graph.begin();

	while (graphIt != graph.end()) {
		if (graphIt->visited) visits++;
		graphIt++;
	}

	return visits;
}

template<typename T>
int Graph<T>::EdgesDiscovered(int currentVert) {
	int visits = 0;

	for (int i = 0; i < graph.at(currentVert).NumOfEdges(); i++) {
		if (graph.at(
				FindVertex(graph.at(currentVert).edgeList.at(i).v)).visited)
			visits++;
	}

	return visits;
}
#endif //GRAPH_H
