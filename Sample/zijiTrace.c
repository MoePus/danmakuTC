#include "danmu.h"
#include "math.h"

void create(void* lp)
{
	
	static long timeProc = 400;

	if (getPassed() -  timeProc >=16)
	{

	XMFLOAT2 zijiPos= getZijiPos();
	XMFLOAT2 pos={zijiPos.x,zijiPos.y};
	polar *p=(polar*)(getLpMem(init(lp,pos,1,1)));
/*	if(p)
	{
	XMFLOAT2 zijiPos= getZijiPos();
	p->o=atan((zijiPos.x-184)/(zijiPos.y-120))-XM_2PI/4;
	p->r=0;	
	if(zijiPos.y<120)
	{
		p->o += XM_2PI/2;
	}
	}*/

	
	
	}
}

int safechk(XMFLOAT2 pos)
{
	if (pos.x>820 || pos.x<-80 || pos.y>620 || pos.y<-80)
	return 1;
return 0;
}

int proc(void* lp,int catalogue)
{
	XMFLOAT2 *pos=getLpPos(lp);
	XMFLOAT2 *centre = getLpCentre(lp);

	polar *p=(polar*)(getLpMem(lp));
	p->r+=1.0f;
	
	XMFLOAT2 tp={p->r*cos(p->o),p->r*sin(p->o)};
	*pos=*centre;
	pos->x+=tp.x;
	pos->y+=-tp.y;

if (safechk(*pos))
return -1;

	return 0;
}