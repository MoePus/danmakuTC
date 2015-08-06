#define XM_2PI              6.283185307f


typedef struct
{
	float x;
	float y;
} XMFLOAT2;


typedef struct 
{
	float r;
	float ra;
	float rv;

	float o;
	float oa;
	float ov;
}polar;

XMFLOAT2 getZijiPos()
{
	XMFLOAT2* pos = getZijiLpPos();
	return *pos;
	
}
