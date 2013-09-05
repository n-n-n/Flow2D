/*************************************************************************

 TEXTRUE REPRESENTED FIELDS 

**************************************************************************/
#ifndef _MYUTIL_H
#define _MYUTIL_H
#include "dxstdafx.h"
const double Pi = 3.14159265358979323846264338327;


// values which each field takes
typedef float ScalarVal;
struct fVector2 {float  x; float y;};
typedef fVector2 VectorVal;
struct iVector2 {int x;int y;};

// Data Set Utilities 

float	Norm(D3DXVECTOR2 Vec2);
float	Gaussian1D(float x, float Sigma);
float	DistortedGaussian1D(float x , float Sigma , float a);
float	Gaussian2D(D3DXVECTOR2 Vec2, D3DXVECTOR2 Sigma2);
float	FermiDirac(float x, float mu);

void   D3DXFloatVector2_16To32Array(fVector2 *pOut, const D3DXVECTOR2_16F *pIn, UINT n); // Read From 16 TO 32
void   D3DXFloatVector2_32To16Array(D3DXVECTOR2_16F *pOut,const  fVector2 *pIn, UINT n);


#endif