#include "stdafx.h"

#include "dcel.h"

DCEL::DCEL(void)
{
    e[0].num = 0;
    e[1].num = 1;
    e[0].ePrev = &(e[1]);
    e[0].eNext = &(e[1]);
    e[1].ePrev = &(e[0]);
    e[1].eNext = &(e[0]);
    visited = false;
	walkable = true;
}
