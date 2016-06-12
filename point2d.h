
#pragma once

//坐标类
class Point2d
{
public:
    Point2d(void);
    Point2d(double tx, double ty); 
    Point2d(const Point2d& p);	
    bool operator==(const Point2d& p) const;
	bool operator<(const Point2d& p) const;

public:
    double x;  
    double y;
};

// 为Point2d 写的两个函数，以供unordered_map使用
struct hash_func {
	size_t operator() (const Point2d& PNT2d) const
	{
		return PNT2d.x * 9999 + PNT2d.y;
	}
};

struct hash_cmp {
	bool operator() (const Point2d& P1, const Point2d& P2) const
	{
		return P1.x == P2.x && P1.y == P2.y;
	}
};
