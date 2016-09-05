
#pragma once

//×ø±êÀà
class Point2d
{
public:
    Point2d(void);
    Point2d(double tx, double ty, double tz = 1.0); 
    Point2d(const Point2d& p);	
    bool operator==(const Point2d& p) const;
	bool operator<(const Point2d& p) const;

public:
    double x;  
    double y;
	double z;
};

