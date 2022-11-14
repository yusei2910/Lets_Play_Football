#ifndef GRAPHDFS_H
#define GRAPHDFS_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "dbmanager.h"

namespace dfs {
/*!
 * \brief The Edge struct
 */
template<typename T>
struct Edge {
	T u;	/// start
	T v;	/// end
    bool discoverEdge;
    int weight;

    /***************************************************************
     * Edge();
     *   Constructor; Initialize class attributes
     *   Parameters: none
     *   Return: none
     ***************************************************************/
    Edge(): u{""}, v{""}, discoverEdge{false}, weight{0} {}

    /***************************************************************
     * Edge(T u, T v, int weight)
     *   Constructor; Initialize class attributes
     *   Parameters: none
     *   Return: none
     ***************************************************************/
    Edge(T u, T v, int weight)
    : u{u}, v{v}, discoverEdge{false}, weight{weight} {}

    bool operator==(const Edge<T> &rhs) const
    {
        return u == rhs.v && v == rhs.u;
    }

    bool operator!=(const Edge<T> &rhs) const { return !(this == rhs); }
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
    Vertex(): vert{T{}}, visited{false} {}

    /***************************************************************
     * Vertex(T vert, bool visited)
     *   Constructor; Initialize class attributes
     *   Parameters: none
     *   Return: none
     ***************************************************************/
    Vertex(T vert, bool visited): vert{vert}, visited{visited} {}

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
class GraphDFS {
public:
	/*!
	 * \brief Graph constructor
	 */
	GraphDFS() : dfsDistance{0} {}

	/*!
	 * \brief Graph destructor
	 */
	~GraphDFS() = default;

	/*!
	 * \brief generateGraph from db
	 */
	void GenerateGraph()
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
				AddVertex(teams[i - 1]);
				query.bindValue(":id", i);
				if(!query.exec()) {
					qDebug() << "DBManager::generateGraph vertex: " << i << " failed";
				} else {
					while(query.next())
						AddEdge(teams[i - 1], teams[query.value(0).toInt() - 1], query.value(1).toInt());
				}
			}
		} else if constexpr (std::is_same<T, int>::value) {
			int numTeams = DBManager::instance()->GetNumTeams();

			int i = 1;
			while (i <= numTeams) {
				AddVertex(i);
				query.bindValue(":id", i);
				if(!query.exec()) {
					qDebug() << "DBManager::generateGraph vertex: " << i << " failed";
				} else {
					while(query.next())
						AddEdge(i, query.value(0).toInt(), query.value(1).toInt());
					i++;
				}
			}
		}
	}


	/*!
	 * \brief checks if graph is empty
	 * \return bool
	 */
	bool Empty() const { return graph.empty(); }

	/*!
	 * \brief size of graph
	 * \return int
	 */
	int Size() const { return graph.size(); }

	/*!
	 * \brief addVertex
	 * \param vert; type T
	 */
	void AddVertex(T vert) {
		if (FindVertex(vert) == -1)
			graph.push_back(Vertex<T>{vert, false});
	}

	/*!
	 * \brief addEdge
	 * \param u; starting vert
	 * \param v; ending vert
	 * \param weight; "distance"
	 */
	void AddEdge(T u, T v, int weight) {
		int index = FindVertex(u);

		if (index == -1) {
			AddVertex(u);
			AddEdge(u, v, weight);
		} else {
			graph.at(index).edgeList.push_back(Edge<T>{u, v, weight});
		}
	}

	/*!
	 * \brief findVertex
	 * \param vert
	 * \return index in graph container
	 */
	int FindVertex(T vert) const {
		int index = 0;

		while (index < Size()) {
			if (graph.at(index).vert == vert)
				return index;
			index++;
		}
		return -1;
	}

	/*!
	 * \brief vertices
	 * \return list of vertices in graph
	 */
	std::vector<T> Vertices() const {
		std::vector<T> temp;
		for (auto a: graph)
			temp.push_back(a.vert);
		return temp;
	}

	/*!
	 * \brief edges
	 * \return list of edges in graph
	 */
	std::vector<std::string> Edges() const {
		std::vector<std::string> temp;
		for (int i = 0; i < Size(); i++) {
			if (graph.at(i).NumOfEdges() != 0) {
				for (unsigned int j = 0;
					 j < graph.at(i).NumOfEdges(); j++) {
					temp.push_back("(" + graph.at(i).edgeList.at(j).u + ", "
								   + graph.at(i).edgeList.at(j).v + ")");
				}
			}
		}
		return temp;
	}

	/*!
	 * \brief dfs depth first search
	 * \param start
	 * \param list
	 * \return total distance "cost"
	 */
	int dfs(T start, std::vector<T> &list) {
		int index = FindVertex(start);
		graph.at(index).visited = true;

		typename std::vector<T>::iterator itrNext = std::find(list.begin(),
															  list.end(),
															  start);

		if (itrNext == list.end())
			list.push_back(start);

		if (VerticesVisited() != Size()) {
			int next = SmallestEdgeDFS(index, list);
			dfs(graph.at(next).vert, list);
		}
		return dfsDistance;
	}

	/*!
	 * \brief getDiscoveryEdges
	 * \param dfs
	 * \return list of discovery edges
	 */
	std::vector<std::string> GetDiscoveryEdges(const std::vector<T> &dfs) {
		std::vector<Edge<T>> discEdges;

		for (int i = 0; i < Size(); i++) {
			int index = FindVertex(dfs.at(i));
			for (int j = 0; j < graph.at(index).NumOfEdges(); j++) {
				if (graph.at(index).edgeList.at(j).discoverEdge)
					discEdges.push_back(graph.at(index).edgeList.at(j));
			}
		}

		RemoveDuplicates(discEdges);
		typename std::vector<Edge<T>>::iterator itr = discEdges.begin();

		std::vector<std::string> discoveryEdges;

		while (itr != discEdges.end()) {
			discoveryEdges.push_back("(" + itr->u + ", " + itr->v + ")");
			itr++;
		}

		return discoveryEdges;
	}

	/*!
	 * \brief getBackEdges
	 * \param dfs; list containing dfs list
	 * \return list of back edges
	 */
	std::vector<std::string> GetBackEdges(const std::vector<T> &dfs) {
		std::vector<Edge<T>> backEdges;

		for (int i = 0; i < Size(); i++) {
			int index = FindVertex(dfs.at(i));
			for (int j = 0; j < graph.at(index).NumOfEdges(); j++) {
				if (!(graph.at(index).edgeList.at(j).discoverEdge))
					backEdges.push_back(graph.at(index).edgeList.at(j));
			}
		}

		RemoveDuplicates(backEdges);
		typename std::vector<Edge<T>>::iterator itr = backEdges.begin();

		std::vector<std::string> finalBackEdgeList;

		while (itr != backEdges.end()) {
			finalBackEdgeList.push_back("(" + itr->u + ", " + itr->v + ")");
			itr++;
		}

		return finalBackEdgeList;
	}

private:
	// returns the index of the next smallest weighted path from vert
	// recursively backtracks if none is found
	int SmallestEdgeDFS(int vert, std::vector<T> &dfs) {
		if (EdgesDiscovered(vert) != graph.at(vert).NumOfEdges()) {
			int smallestIndex = 0;
			int compIndex = smallestIndex + 1;
			int size = graph.at(vert).NumOfEdges();

			while (compIndex < size) {
				int smallestVert = FindVertex(
						graph.at(vert).edgeList.at(smallestIndex).v);
				int compVertex = FindVertex(
						graph.at(vert).edgeList.at(compIndex).v);

				if (graph.at(smallestVert).visited) {
					smallestIndex++;
				} else {
					if (!(graph.at(compVertex).visited)) {
						if (graph.at(vert).edgeList.at(
								smallestIndex).weight >=
							graph.at(vert).edgeList.at(compIndex).weight)
							smallestIndex = compIndex;
					}
				}
				compIndex++;
			}

			graph.at(vert).edgeList.at(smallestIndex).discoverEdge = true;

			dfsDistance += graph.at(vert).edgeList.at(smallestIndex).weight;

			T next = graph.at(vert).edgeList.at(smallestIndex).v;

			smallestIndex = FindVertex(next);

			for (int i = 0; i < graph.at(smallestIndex).NumOfEdges(); i++) {
				if (graph.at(vert).vert ==
					graph.at(smallestIndex).edgeList.at(i).v) {
					graph.at(smallestIndex).edgeList.at(
							i).discoverEdge = true;
				}
			}

			return smallestIndex;
		} else {
			typename std::vector<T>::iterator itr =
					std::find(dfs.begin(), dfs.end(), graph.at(vert).vert);
			itr--;
			int backIndex = FindVertex(*itr);
			return SmallestEdgeDFS(backIndex, dfs);
		}
	}

	// returns the number of verts visited
	// used to check if search is completed
	int VerticesVisited() {
		std::size_t count = 0;

		typename std::vector<Vertex<T>>::iterator itr = graph.begin();

		while (itr != graph.end()) {
			if (itr->visited)
				count++;
			itr++;
		}
		return count;
	}

	// returns the number of edges visited
	// returns 0 is nothing found
	int EdgesDiscovered(int currVertex) {
		int numVisited = 0;

		for (int i = 0; i < graph.at(currVertex).NumOfEdges(); i++) {
			if (graph.at(FindVertex(
					graph.at(currVertex).edgeList.at(i).v)).visited)
				numVisited++;
		}
		return numVisited;
	}

	// removes duplicates from the discovery edges method and
	// the backedges method. This occurs because edges can go both ways
	void RemoveDuplicates(std::vector<Edge<T>> &vec) {
		typename std::vector<Edge<T>>::iterator itr = vec.begin();

		while (itr != vec.end()) {
			typename std::vector<Edge<T>>::iterator compIt = itr + 1;
			bool deleted = false;

			while (compIt != vec.end() && !deleted) {
				if (itr->u == compIt->v && itr->v == compIt->u) {
					vec.erase(compIt);
					deleted = true;
				} else {
					compIt++;
				}
			}
			itr++;
		}
	}

	std::vector<Vertex<T>> graph;	/// graph container
	int dfsDistance;
};
}
#endif //GRAPHDFS_H
