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
	resistance[0] = 1.0;
	resistance[1] = 1.0;
	resistance[2] = 1.0;
	slope = 0.0;
	length = 0;
}
