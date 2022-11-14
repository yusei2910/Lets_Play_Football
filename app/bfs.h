#ifndef BFS_H
#define BFS_H

#include <vector>
#include <QStringList>
#include <dbmanager.h>
#include <queue>
#include <bfsNamespace.h>

class bfs
{
private:
    /*!
     * @brief ADJ LIST with the index of the array representing the teams id
     *          the element of the array is a vector the list of adj teams and their weights
     */
    std::vector<generalContainer::node>* adjList;

    /*!
     * @brief holds the total distance travelled as a int
     */
	int totalDist;

    /*!
     * @brief contains the names of the team to visit in bfs order
     */
    QStringList bfsList;

    /*!
     * @brief number of cities to visit
     */
    int cities;

    /*!
     * @brief bool array to check if the city has been visited
     */
    bool *visited;
public:
    /*!
     * @brief default Constructor
     */
    bfs();

    /*!
     * @brief initializes the ADJ list from the values in the data table
     */
	void AddEdges(); // initializes the adj list

    /*!
     * @brief preforms a bfs traversal through the team
     *
     * @param start; The team you want to start at
     */
    void bfsAlgo(int start);// actual bfs Algo

    /*!
     * @brief returns the total distance traveled
     * @return  int
     */
	int GetTotalDistance(); // returns the total distance

    /*!
     * @brief returns a QStringList of all the teams in their bfs order
     * @return QStringList
     */
	QStringList GetBfsList();
};

#endif // BFS_H
