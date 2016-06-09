#pragma once
#include "point2d.h"

//�������
class Edge
{
public:
    Edge(void);
    Edge* twin(void);
    Edge* next(void);
    Edge* prev(void);
    Edge* sym(void);
    Edge* oNext(void);  //origin next?
    Edge* oPrev(void);  //origin prev?
    Edge* dNext(void);  //dest next?
    Edge* dPrev(void);  //dest prev?
    Edge* lNext(void);  //left next?
    Edge* lPrev(void);  //left prev?
    Edge* rNext(void);  //right next?
    Edge* rPrev(void);  //right prev?

    Point2d* org(void);  //�߶ε����
    Point2d* dest(void); //�߶ε��յ�
    const Point2d& org2d(void) const;
    const Point2d& dest2d(void) const;
    void endPoints(Point2d* org, Point2d* des);
    void* qEdge(void);
public:
    int num;
    Edge* eNext;
    Edge* ePrev;
    Point2d* oData;
};
