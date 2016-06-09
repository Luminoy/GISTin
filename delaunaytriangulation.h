#pragma once
#include "edge.h"
#include "point2d.h"
#include "dcel.h"
#include <vector>
using namespace std;
struct MaxEdge
{
	MaxEdge(void)
	{
		le = NULL;
        re = NULL;
	}
    Edge* le; //左极值点
    Edge* re; //右极值点
    Edge* be; //下极值点
    Edge* te; //上极值点
};

void initial(Point2d* ps, int& nPoints);

MaxEdge doDelaunayTriangulation(Point2d* ps, int& nPoints);

MaxEdge delaunay(Point2d *ps, int begin, int end);

MaxEdge MergeDelaunay(MaxEdge leftRet, MaxEdge rightRet);

void collectDcel(Edge* e, DCEL **pEdge, long &nCount);

MaxEdge MergeDelaunay(MaxEdge leftRet, MaxEdge rightRet, bool bLRMerge);

void SetConvexHullExtremum(MaxEdge& ret, Edge *pCWE);

void SetBottomTopExtremum(MaxEdge& ret, Edge *pCWE);

void SetLeftRightExtremum(MaxEdge& ret, Edge *pCWE);

void SetDcelVisit(vector<DCEL*>&dcelCollection, bool bVisited);