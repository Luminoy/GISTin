#include "stdafx.h"
#include "point2d.h"
#include <cmath>
const double TOLERANCE = 1e-10;

Point2d::Point2d(void)
    : x(0), y(0), z(1.0)
{
}

Point2d::Point2d(double tx, double ty, double tz)
    : x(tx), y(ty), z(tz)
{
}

Point2d::Point2d(const Point2d& p)
{
    x = p.x;
    y = p.y;
	z = p.z;
}

bool Point2d::operator==(const Point2d& p) const
{
    if (fabs(x - p.x) >= TOLERANCE || fabs(y - p.y) >= TOLERANCE || fabs( z - p.z) >= TOLERANCE)
        return false;

    return true;
}

bool Point2d::operator<(const Point2d& p) const
{
	return (x < p.x) || ((x == p.x) && ((y < p.y) || (y == p.y) && (z < p.z))); //有点复杂，但总体是先比较x再比较y最后比较z
}
