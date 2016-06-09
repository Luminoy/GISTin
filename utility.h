#pragma once
#include "edge.h"
#include <vector>

using namespace std;

//生成一条边
Edge* makeEdge(void);

//删除一条边
void deleteEdge(Edge* e);

//链接或分离操作
void splice(Edge* a, Edge* b);

//连接两条边
Edge* connect(Edge* a, Edge* b);

//交换边
void swap(Edge* e);

double triarea(const Point2d& a, const Point2d& b, const Point2d& c);

//判断d是否在a、b、c的外接圆内
bool inCircle(const Point2d& a, const Point2d& b,
              const Point2d& c, const Point2d& d);

//逆时针方向判定
bool ccw(const Point2d& a, const Point2d& b, const Point2d& c);

//判定x是否在边e的右边
bool rightOf(const Point2d& x, Edge* e);

////判定x是否在边e的左边
bool leftOf(const Point2d& x, Edge* e);

//检测边e是否合法
bool valid(Edge* e, Edge* basel);