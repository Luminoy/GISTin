#include "stdafx.h"
#include "delaunaytriangulation.h"
#include <cassert>
#include <algorithm>
#include <stack>
#include "utility.h"
#include "dcel.h"
using namespace std;

void initial(Point2d* ps, int& nPoints)
{
    std::sort(ps, ps + nPoints);
    //去除重复点
    int j = 1;
    for (int i = 1; i < nPoints; ++i)
    {
        if (ps[i].x != ps[i-1].x || ps[i].y != ps[i-1].y)
        {
            ps[j++] = ps[i];
        }
    }
	nPoints = j;
}

MaxEdge doDelaunayTriangulation(Point2d* ps, int& nPoints)
{
	MaxEdge maxEdge;
    if (nPoints >= 2)
    {
		initial(ps, nPoints); //去重
        maxEdge = delaunay(ps, 0, nPoints - 1);    
    }
	return maxEdge;
}

MaxEdge delaunay(Point2d *ps, int begin, int end)
{
    //点的数量
    int size = end - begin + 1;
    MaxEdge ret;
    //分治算法开始
    if(size == 2) //如果只有两个点
    {
        //创建一条边
        Edge* a = makeEdge();
        a->endPoints(ps + begin, ps + end);
        ret.le = a;
        ret.re = a->twin();
        return ret;
    }
    else if(size == 3) //如果有3个点
    {
        //创建两条边
        Edge* a = makeEdge();
        Edge* b = makeEdge();
        splice(a->twin(), b);
        a->endPoints(ps + begin, ps + (begin + 1));
        b->endPoints(ps + (begin + 1), ps + end);
        Edge* c;
        if(ccw(ps[begin], ps[begin + 1], ps[end]))
        {
            c = connect(b, a);

            ret.le = a; 
            ret.re = b->twin(); 

            return ret;
        }
        else if(ccw(ps[begin], ps[end], ps[begin + 1]))
        {
            c = connect(b, a);

            ret.le = c->twin();
            ret.re = c;        

            return ret;
        }
        else  
        {
            ret.le = a;         
            ret.re = b->twin();  
            return ret;
        }
    }
    else if(size >= 4) //如果多于或等于4个点
    {
        //递归计算L部分与R部分
        MaxEdge leftRet = delaunay(ps, begin, begin + (size / 2) - 1);
        MaxEdge rightRet = delaunay(ps, begin + (size / 2), end);
        return MergeDelaunay(leftRet, rightRet);
    }
	//如果只有一个点
	else 
	{
		return ret;
	}
}

MaxEdge MergeDelaunay(MaxEdge leftRet, MaxEdge rightRet)
{
	Edge *ldo, *ldi; 
	Edge *rdo, *rdi;  
	ldo = leftRet.le; 
    ldi = leftRet.re; 
    rdi = rightRet.le; 
    rdo = rightRet.re; 
    //寻找左L凸包与右R凸包的下公共切线
    while(1)
    {
        if(leftOf(*(rdi->org()), ldi))
        {
            ldi = ldi->lNext(); 
        }
        else if(rightOf(*(ldi->org()), rdi))
        {
            rdi = rdi->rPrev();  
        }
        else
        {
            break; 
        }
    }
    Edge* basel = connect(rdi->twin(), ldi);
	Edge* pCWE = basel;
    //开始合并L、R部分
    while(1)
    {
        Edge* lcand = basel->twin()->oNext();
        Edge* t;
        if(valid(lcand, basel))
        {
            while(inCircle(*(basel->dest()), *(basel->org()), *(lcand->dest()), *(lcand->oNext()->dest())))
            {
                t = lcand->oNext();
                deleteEdge(lcand);
                lcand = t;
            }
        }
        Edge* rcand = basel->oPrev();
        if(valid(rcand, basel))
        {
            while(inCircle(*(basel->dest()), *(basel->org()), *(rcand->dest()), 
                           *(rcand->oPrev()->dest())))
            {
                t = rcand->oPrev();
                deleteEdge(rcand);
                rcand = t;
            }
        }
        if((!valid(lcand, basel)) && (!valid(rcand, basel)))
        {
            break;
        }
        if((!valid(lcand, basel)) || (valid(rcand, basel) && 
				                           inCircle(*(lcand->dest()), *(lcand->org()),*(rcand->org()), *(rcand->dest()))))
        {
            basel = connect(rcand, basel->twin());
        }
        else
        {
            basel = connect(basel->twin(), lcand->twin());
        }
    }
	//重新设置极值边
	MaxEdge ret;
    SetLeftRightExtremum(ret, pCWE);
    return ret;
}

//完成左右、上下合并
MaxEdge MergeDelaunay(MaxEdge leftRet, MaxEdge rightRet, bool bLRMerge)
{
	if (leftRet.le == NULL)
	{
		return rightRet;
	}
	else if (rightRet.le == NULL)
	{
		return leftRet;
	}
	Edge *ldi; 
	Edge *rdi; 
	//如果是左右合并
	if (bLRMerge)
	{
        ldi = leftRet.re; 
        rdi = rightRet.le; 
	}
	//如果是上下合并
    else
	{
        ldi = leftRet.te;    
        rdi = rightRet.be;   
	}
    //1.寻找左(下)凸包与右(上)凸包的下(右)公共切线
    while(1)
    {
        if(leftOf(*(rdi->org()), ldi))
        {
            ldi = ldi->lNext(); 
        }
        else if(rightOf(*(ldi->org()), rdi))
        {
            rdi = rdi->rPrev();  
        }
        else
        {
            break; 
        }
    }
    //2.创建一条边
    Edge* basel = connect(rdi->twin(), ldi);
	Edge* pCWE = basel;
    //开始合并L、R部分
    while(1)
    {
        Edge* lcand = basel->twin()->oNext();
        Edge* t;
        if(valid(lcand, basel))
        {
            while(inCircle(*(basel->dest()), *(basel->org()), *(lcand->dest()), *(lcand->oNext()->dest())))
            {
                t = lcand->oNext();
                deleteEdge(lcand);
                lcand = t;
            }
        }
        Edge* rcand = basel->oPrev();
        if(valid(rcand, basel))
        {
            while(inCircle(*(basel->dest()), *(basel->org()), *(rcand->dest()), 
                           *(rcand->oPrev()->dest())))
            {
                t = rcand->oPrev();
                deleteEdge(rcand);
                rcand = t;
            }
        }
        if((!valid(lcand, basel)) && (!valid(rcand, basel)))
        {
            break;
        }
        if((!valid(lcand, basel)) || (valid(rcand, basel) && 
				                           inCircle(*(lcand->dest()), *(lcand->org()),*(rcand->org()), *(rcand->dest()))))
        {
            basel = connect(rcand, basel->twin());
        }
        else
        {
            basel = connect(basel->twin(), lcand->twin());
        }
    }
	MaxEdge ret;
	SetConvexHullExtremum(ret, pCWE);
    return ret;
}

//设置凸包的所有极值
void SetConvexHullExtremum(MaxEdge& ret, Edge *pCWE)
{
	Edge *pEdge = pCWE;
	ret.le = ret.re = ret.be = ret.te = pEdge;
	pEdge = pEdge->next();
	while (pEdge != pCWE)
	{
		if (ret.le->org()->x > pEdge->org()->x ||
			(ret.le->org()->x == pEdge->org()->x && ret.le->org()->y > pEdge->org()->y))
		{
			ret.le = pEdge;
		}
		if (ret.re->org()->x < pEdge->org()->x ||
			(ret.re->org()->x == pEdge->org()->x && ret.re->org()->y < pEdge->org()->y))
		{
			ret.re = pEdge;
		}
		if (ret.be->org()->y > pEdge->org()->y ||
			(ret.be->org()->y == pEdge->org()->y && ret.be->org()->x > pEdge->org()->x))
		{
			ret.be = pEdge;
		}
		if (ret.te->org()->y < pEdge->org()->y ||
			(ret.te->org()->y == pEdge->org()->y && ret.te->org()->x < pEdge->org()->x))
		{
			ret.te = pEdge;
		}
		pEdge = pEdge->next();
	}
	ret.le = ret.le->prev()->twin();
	ret.be = ret.be->prev()->twin();
}

//设置凸包的上下极值点
void SetBottomTopExtremum(MaxEdge& ret, Edge *pCWE)
{
	Edge *pEdge = pCWE;
	ret.be = ret.te = pEdge;
	pEdge = pEdge->next();
	while (pEdge != pCWE)
	{
		if (ret.be->org()->y > pEdge->org()->y ||
			(ret.be->org()->y == pEdge->org()->y && ret.be->org()->x > pEdge->org()->x))
		{
			ret.be = pEdge;
		}
		if (ret.te->org()->y < pEdge->org()->y ||
			(ret.te->org()->y == pEdge->org()->y && ret.te->org()->x < pEdge->org()->x))
		{
			ret.te = pEdge;
		}
		pEdge = pEdge->next();
	}
	ret.be = ret.be->prev()->twin();
}

//设置凸包的左右极值
void SetLeftRightExtremum(MaxEdge& ret, Edge *pCWE)
{
	Edge *pEdge = pCWE;
	ret.le = ret.re = pEdge;
	pEdge = pEdge->next();
	while (pEdge != pCWE)
	{
		if (ret.le->org()->x > pEdge->org()->x || 
			(ret.le->org()->x == pEdge->org()->x && ret.le->org()->y > pEdge->org()->y))
		{
			ret.le = pEdge;
		}
		if (ret.re->org()->x < pEdge->org()->x ||
			(ret.re->org()->x == pEdge->org()->x && ret.re->org()->y < pEdge->org()->y))
		{
			ret.re = pEdge;
		}
		pEdge = pEdge->next();
	}
	ret.le = ret.le->prev()->twin();
}

//收集边
void collectDcel(Edge* e, DCEL **pEdge, long &nCount)
{
	DCEL *pDcel;
    Edge *edge;	
	std::stack<Edge *>dcelStack;
	dcelStack.push(e);
	while(!dcelStack.empty())
	{
		edge = dcelStack.top();
		pDcel = (DCEL *)(edge->qEdge());
		dcelStack.pop();
	    if(!pDcel->visited)
		{
			pEdge[nCount ++] = pDcel;
			pDcel->visited = true;
			dcelStack.push(edge->oNext());
			dcelStack.push(edge->oPrev());
			dcelStack.push(edge->dNext());
			dcelStack.push(edge->dPrev());
		}
	}
}

//设置边是否已经访问
void SetDcelVisit(vector<DCEL*>&dcelCollection, bool bVisited)
{
	vector<DCEL *>::iterator iter;
	for(iter = dcelCollection.begin(); iter != dcelCollection.end(); iter++)
	{
		DCEL *pdecl = *iter;
		pdecl->visited = bVisited;
	}
}