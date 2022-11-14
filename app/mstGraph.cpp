#include"mstGraph.h"

mstGraph::mstGraph(){
    int vertexNum = DBManager::instance()->GetNumTeams();
    numVertex = vertexNum;
    vertices.resize(vertexNum);
	InitializeMatrix();
}

mstGraph::~mstGraph(){
    //deleteMatrix();
}

void mstGraph::AddEdge(int start, int end, int weight){
    adjMatrix[start][end] = weight;
}

void mstGraph::InitializeMatrix(){
    adjMatrix = new int*[numVertex];
    for (int i = 0; i < numVertex; i++)
        adjMatrix[i] = new int[numVertex];

    for (int i = 0; i < numVertex; i++)
        for (int j = 0; j < numVertex; j++)
            adjMatrix[i][j] = -1;

    QSqlQuery query;
    query.prepare("SELECT id,distanceTo "
                  "FROM distance "
                  "WHERE beginStadium "
                  "IN(SELECT endStadium "
                  "FROM distance WHERE id = :teamID) "
                  "AND endStadium = (SELECT beginStadium "
                  "FROM distance WHERE id = :teamID) AND id!= :teamID");

    QLocale c(QLocale::C);  // variable to convert a formatted string into an int
    for (int i = 0; i <numVertex; i++)
    {
        query.bindValue(":teamID", i);
        if(query.exec())
        {
            while(query.next())
            {
				AddEdge(i,query.value(0).toInt(),c.toInt(query.value(1).toString()));
            }
        }
    }
}

void mstGraph::DeleteMatrix(){
    for (int i = 0; i < numVertex; i++)
        delete[] adjMatrix[i];
    delete[] adjMatrix;
}

bool mstGraph::isExist(mstVertex search)
{
	for (int i = 0; i < (int) mstVec.size(); i++)
        if (mstVec[i]->index == search.index)
            return true;
    return false;
}

mstVertex* mstGraph::ExtractMin() {
    if(!mstVec.empty()) {
        mstVertex* temp = mstVec[0];
        int smallestIndex = 0;
		for (int i = 0; i < (int) mstVec.size(); i++) {
            if (temp->key > mstVec[i]->key) {
                temp = mstVec[i];
                smallestIndex = i;
            }
        }
        swap(mstVec[smallestIndex], mstVec[mstVec.size()-1]);
        mstVec.pop_back();
        return temp;
    }
    return nullptr;
}

void mstGraph::PrimMST(int start){
    mstVec.clear();
	for (int i = 0; i < (int) vertices.size(); i++){
        vertices[i].index = i;
        vertices[i].key = INT_MAX;
        vertices[i].parent = nullptr;
    }
    vertices[start].key = 0;

	for (int i = 0; i < (int) vertices.size();i++)
        mstVec.push_back(&vertices[i]);

    while(!mstVec.empty()){
		mstVertex* smallestVerPtr = ExtractMin();
        for (int i = 0; i < numVertex; i++){
            if (adjMatrix[smallestVerPtr->index][i] != -1){
                if(isExist(vertices[i]) && adjMatrix[smallestVerPtr->index][i] < vertices[i].key){
                    vertices[i].parent = smallestVerPtr;
                    vertices[i].key = adjMatrix[smallestVerPtr->index][i];
                }
            }
        }
    }
}

void mstGraph::GetMST(vector<mstEdge>& edgeVec){
	PrimMST(0);
	for (int i = 0; i < (int) vertices.size(); i++){
        if (vertices[i].parent != nullptr)
            edgeVec.push_back(mstEdge(vertices[i].parent->index, vertices[i].index, vertices[i].key));
    }
}

long mstGraph::GetMSTdistance(){
    long sum = 0;
	for (int i = 0; i < (int) vertices.size(); i++)
    {
        if (vertices[i].parent != nullptr)
            sum += vertices[i].key;
    }
    return sum;
}
