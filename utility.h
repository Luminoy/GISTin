#pragma once
#include "edge.h"
#include <vector>

using namespace std;

//����һ����
Edge* makeEdge(void);

//ɾ��һ����
void deleteEdge(Edge* e);

//���ӻ�������
void splice(Edge* a, Edge* b);

//����������
Edge* connect(Edge* a, Edge* b);

//������
void swap(Edge* e);

double triarea(const Point2d& a, const Point2d& b, const Point2d& c);

//�ж�d�Ƿ���a��b��c�����Բ��
bool inCircle(const Point2d& a, const Point2d& b,
              const Point2d& c, const Point2d& d);

//��ʱ�뷽���ж�
bool ccw(const Point2d& a, const Point2d& b, const Point2d& c);

//�ж�x�Ƿ��ڱ�e���ұ�
bool rightOf(const Point2d& x, Edge* e);

////�ж�x�Ƿ��ڱ�e�����
bool leftOf(const Point2d& x, Edge* e);

//����e�Ƿ�Ϸ�
bool valid(Edge* e, Edge* basel);