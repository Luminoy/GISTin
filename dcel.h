#pragma once
#include "edge.h"

class DCEL
{
public:
    DCEL(void);
public:
    Edge   e[2];
	int    type;        // �ر�����
    bool   visited;     // ���ʱ�־
	double resistance;  // ͨ������
	double slope;       // �¶�ֵ
	double length;      // �ߵĳ���, km
};