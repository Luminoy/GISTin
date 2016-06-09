#include "stdafx.h"
#include "point2d.h"
#include <cmath>
const double TOLERANCE = 1e-10;

Point2d::Point2d(void)
    : x(0), y(0)
{
}

Point2d::Point2d(double tx, double ty)
    : x(tx), y(ty)
{
}

Point2d::Point2d(const Point2d& p)
{
    x = p.x;
    y = p.y;
}

bool Point2d::operator==(const Point2d& p) const
{
    if (fabs(x - p.x) >= TOLERANCE || fabs(y - p.y) >= TOLERANCE)
        return false;

    return true;
}

bool Point2d::operator<(const Point2d& p) const
{
	if(x < p.x)
	{
	    return true;
	}
	else
	{
	    if(x > p.x)
		    return false;
		else
		    return (y < p.y);
	}
}
