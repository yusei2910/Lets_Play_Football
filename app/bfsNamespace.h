#ifndef BFSNAMESPACE_H
#define BFSNAMESPACE_H

#include <algorithm>

namespace generalContainer
{
    /*!
     * @brief node is a container for each element in the adj list
     * @param end; id of the ending team based on index in adj list
     * @param weight; weight between index in the adj list and the end variable
     */
    struct node
    {
        int end; // id of ending team
        int weight; //weight from vertex to ending team
        node():end(0),weight(0){};
    };
}

#endif // BFSNAMESPACE_H
