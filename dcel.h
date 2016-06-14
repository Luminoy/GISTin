#pragma once
#include "edge.h"

class DCEL
{
public:
    DCEL(void);
public:
    Edge e[2];
    bool visited;
	bool walkable;
};
