#pragma once
#include "edge.h"

class DCEL
{
public:
    DCEL(void);
public:
    Edge   e[2];
	int    type;        // 地表类型
    bool   visited;     // 访问标志
	double resistance;  // 通行阻力
	double slope;       // 坡度值
	double length;      // 边的长度, km
};