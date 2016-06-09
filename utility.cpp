#include "stdafx.h"
#include <cmath>
#include "utility.h"
#include "dcel.h"

//生成一条边
Edge* makeEdge(void)
{
    DCEL* dcel = new DCEL();
    return dcel->e;
}

//链接或分离操作
void splice(Edge* a, Edge* b)
{
    a->prev()->eNext = b;
    b->prev()->eNext = a;
    Edge* t1 = a->prev();
    Edge* t2 = b->prev();
    a->ePrev = t2;
    b->ePrev = t1;
}

//删除一条边
void deleteEdge(Edge* e)
{
    if (e != NULL)
    {
        splice(e, e->oPrev());
        splice(e->sym(), e->sym()->oPrev());
        delete e->qEdge();
		e = NULL;      
    }
}

//连接两条边
Edge* connect(Edge* a, Edge* b)
{
    Edge* e = makeEdge();
    splice(e, a->lNext());
    splice(e->sym(), b);
    e->endPoints(a->dest(), b->org());
    return e;
}

//交换边
void swap(Edge* e)
{
    Edge* a = e->oPrev();
    Edge* b = e->sym()->oPrev();
    splice(e, a);
    splice(e->sym(), b);
    splice(e, a->lNext());
    splice(e->sym(), b->lNext());
    e->endPoints(a->dest(), b->dest());
}

double triarea(const Point2d& a, const Point2d& b, const Point2d& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

//判断d是否在a、b、c的外接圆内
bool inCircle(const Point2d& a, const Point2d& b,
              const Point2d& c, const Point2d& d)
{
    if (&d == &a || &d == &b || &d == &c)
        return false;

    return (a.x*a.x + a.y*a.y) * triarea(b, c, d) -
           (b.x*b.x + b.y*b.y) * triarea(a, c, d) +
           (c.x*c.x + c.y*c.y) * triarea(a, b, d) -
           (d.x*d.x + d.y*d.y) * triarea(a, b, c) > 0;
}

//逆时针方向判定
bool ccw(const Point2d& a, const Point2d& b, const Point2d& c)
{
    double multiply = triarea(a, b, c); // 叉积法
	if (multiply > 1.0e-6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//判定x是否在边e的右边
bool rightOf(const Point2d& x, Edge* e)
{
    return ccw(x, e->dest2d(), e->org2d());
}

//判定x是否在边e的左边
bool leftOf(const Point2d& x, Edge* e)
{
    return ccw(x, e->org2d(), e->dest2d());
}

//检测边e是否合法
bool valid(Edge* e, Edge* basel)
{
	return rightOf(*(e->dest()), basel);
}
