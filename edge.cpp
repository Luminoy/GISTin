#include "stdafx.h"
#include "edge.h"

Edge::Edge(void)
    :oData(NULL), eNext(NULL), ePrev(NULL), num(-1)
{
}
Edge* Edge::twin(void)
{
    return (num > 0) ? this - 1 : this + 1;
}
Edge* Edge::next(void)
{
    return eNext;
}
Edge* Edge::prev(void)
{
    return ePrev;
}
Edge* Edge::sym(void)
{
    return (num > 0) ? this - 1 : this + 1;
}
Edge* Edge::oNext(void)
{
    return ePrev->twin();
}
Edge* Edge::oPrev(void)
{
    return twin()->next();
}
Edge* Edge::dNext(void)
{
    return twin()->prev();
}
Edge* Edge::dPrev(void)
{
    return eNext->twin();
}
Edge* Edge::lNext(void)
{
    return eNext;
}
Edge* Edge::lPrev(void)
{
    return ePrev;
}
Edge* Edge::rNext(void)
{
    return twin()->next()->twin();
}
Edge* Edge::rPrev(void)
{
    return twin()->prev()->twin();
}
Point2d* Edge::org(void)
{
    return oData;
}
Point2d* Edge::dest(void)
{
    return twin()->oData;
}
const Point2d& Edge::org2d(void) const
{
    return *oData;
}
const Point2d& Edge::dest2d(void) const
{
    return (num > 0) ? *((this - 1)->oData) : *((this + 1)->oData);
}
void Edge::endPoints(Point2d *org, Point2d *des)
{
    oData = org;
    twin()->oData = des;
}
void* Edge::qEdge(void)
{
    return (void*)(this - num);
}
