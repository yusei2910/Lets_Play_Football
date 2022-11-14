#include "bfs.h"

bfs::bfs()
{
    bfsList.clear();                                              //always clears the list
	totalDist = 0;                                                //sets the total distance to 0
    cities = DBManager::instance()->GetNumTeams();                //gets the total number of teams from data table
    adjList = new std::vector<generalContainer::node>[cities + 1];//Creates adj list based on the number of teams + 1 so each index correlates to
                                                                  //    its respective id in the datatable
    visited = new bool [cities+1];                                //Parallel array that marks if an index has been visited
}
void bfs::AddEdges()
{
    //used to initialize the adj list
    //  each index is teams id and the element is a vector which contains
    //  the ending team and weight
    for(int index=1; index< cities+1 ; index++)
    {
        adjList[index] = DBManager::instance()->getAdjList(index);
    }
}

void bfs::bfsAlgo(int start)
{
    std::queue<int> tempQ;
    int vertex = 0;
    QString teamName; // used to hold the teamName

    //Initialize the visited array to false(all team unexplored)
    for(int index = 0; index < cities+1; index++)
    {
        visited[index] = false;
    }

    // UNCOMMENT LINES 36 TO DEBUG BFS INCASE TOTAL DISTANCE IS INCORRECT
    //qDebug() << "\n\n BFS TRIPS AND THEIR RESPECTIVE DISTANCES\n-------------------\n";
    vertex = start;
    tempQ.push(vertex);
    visited[vertex] = true;

    while(!tempQ.empty())
    {
        vertex = tempQ.front();
        tempQ.pop();

        //grabs the team name based on its id and pushes it to the QStringList
		teamName = DBManager::instance()->GetTeamName(vertex);
        bfsList << teamName;

        for(auto i: adjList[vertex])
        {
            if(visited[i.end] ==false)
            {
                tempQ.push(i.end);
				totalDist+=i.weight;

                //UNCOMMENT LINE 58 TO DEBUG IN CASE NUMBER IS WRONG
                //qDebug() << DBManager::instance()->getTeamName(vertex)<< " " << DBManager::instance()->getTeamName(i.end) << " " << i.weight;

                visited[i.end] = true;
            }
        }
    }

    //UNCOMMENT LINES 67 - 72 TO DEBUG INCASE BFS TRIP IS WRONG

    /*
    qDebug() << "\n\n THIS IS THE ORDER OF TEAMS IN THE BFS TRAVERSAL \n-----------------------\n";
    for(auto i: bfsList)
    {
        qDebug() << i;
    }
    */
}
QStringList bfs::GetBfsList()
{
    return bfsList;
}
int bfs::GetTotalDistance()
{
	return totalDist;
}
