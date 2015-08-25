#include "danmu.h"
#include "math.h"
void* danmu=NULL;

void create(void* lp)
{
	danmu=lp;
	static long timeProc = 200;
	static num=0;
	if (getPassed() -  timeProc >=1800)
	{
	timeProc = getPassed();

	for(int i=0;i<6;i++)
	{
	XMFLOAT2 pos={184,125};
	polar *p=(polar*)(getLpMem(init(lp,pos,0x205,1)));
	if(p)
	{
	p->o=XM_2PI/6*i + num*XM_2PI/16.0f;

	}

	}
	num++;
	}
}

int safechk(XMFLOAT2 pos)
{
	if (pos.x>468 || pos.x<-100 || pos.y>440 || pos.y<-40)
	return 1;
	return 0;
}

void secCre(XMFLOAT2 pos)
{
	for(int i=0;i<36;i++)
{
	polar *p=(polar*)(getLpMem(init(danmu,pos,0x507,2)));
	if(p)
	{
	p->o=XM_2PI/36*i;

	}
}
}

int proc(void* lp,int catalogue)
{
XMFLOAT2 *pos=getLpPos(lp);
XMFLOAT2 *centre = getLpCentre(lp);

if(catalogue==1)
{
	polar *p=(polar*)(getLpMem(lp));
	p->r+=1.0f;
	XMFLOAT2 tp={p->r*cos(p->o),p->r*sin(p->o)};
	*pos=*centre;
	pos->x+=tp.x;
	pos->y+=tp.y;

if(getMyPassed(lp)>2000)
{
secCre(*pos);
return -1;
}

	
}else
{
polar *p=(polar*)(getLpMem(lp));

if(getMyPassed(lp)>2600)
{
return -1;
}else	
if(getMyPassed(lp)>1000)
{
p->rv+=0.1f;
p->r += p->rv;
}else
if(getMyPassed(lp)>500)
{
	p->r+=0.1f;
}else
{
	p->r+=1.0f;
}
	XMFLOAT2 tp={p->r*cos(p->o),p->r*sin(p->o)};
	*pos=*centre;
	pos->x+=tp.x;
	pos->y+=tp.y;
}

return 0;
}