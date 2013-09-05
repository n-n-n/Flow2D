#include "MyUtil.h"

float Norm(D3DXVECTOR2 Vec2)
{
	return sqrtf(Vec2.x * Vec2.x + Vec2.y * Vec2.y); 
}

float Gaussian1D(float x, float Sigma)
{
	if ( Sigma == 0.0f ) return 0.0f;

	return  expf( -x * x / (2.0f * Sigma * Sigma) ) ;

}

float FermiDirac(float x, float mu)
{
	if ( mu <= 0.0f ) return 0.0f;


	float fTmp = expf(x/mu-1.0f) +1.0f;
	
	return 2.0f/fTmp;

}

float Gaussian2D(D3DXVECTOR2 Vec2, D3DXVECTOR2 Sigma2)
{
	if ( Sigma2.x == 0.0f ||  Sigma2.y == 0.0f ) return 0.0f;
	
	D3DXVECTOR2 vTmp;
	vTmp.x = Vec2.x/ Sigma2.x;
	vTmp.y = Vec2.y/ Sigma2.y;

	return  expf( -vTmp.x * vTmp.x / 2.0f  -vTmp.y * vTmp.y / 2.0f );

}

float DistortedGaussian1D(float x, float Sigma, float a)
{
	if ( Sigma == 0.0f ) return 0.0f;

	if ( x >= 0 ) return expf( -x * x / (2.0f * Sigma * Sigma) );
	else  return expf( -a * x * x / (2.0f * Sigma * Sigma) );

}

void   D3DXFloatVector2_16To32Array(fVector2 *pOut, const D3DXVECTOR2_16F *pIn, UINT n)
{

	float	*pTex32X, *pTex32Y;
	D3DXFLOAT16 *pTex16X, *pTex16Y;

	pTex32X = new float[n];
	pTex32Y = new float[n];
	
	pTex16X	= new D3DXFLOAT16[n];
	pTex16Y = new D3DXFLOAT16[n]; 

	UINT i;
	for ( i = 0; i < n ; i++) {
			pTex16X[i] = pIn[i].x;
			pTex16Y[i] = pIn[i].y;
	}
	
	
	D3DXFloat16To32Array(pTex32X, pTex16X, n);
	D3DXFloat16To32Array(pTex32Y, pTex16Y, n); 

	for ( i = 0; i < n; i++) {
			pOut[i].x = pTex32X[i];
			pOut[i].y = pTex32Y[i];
		}

	delete [] pTex32X;
	delete [] pTex32Y;
	delete [] pTex16X;
	delete [] pTex16Y;	
}


void   D3DXFloatVector2_32To16Array(D3DXVECTOR2_16F *pOut, const fVector2 *pIn, UINT n)
{
	float		*pTex32X, *pTex32Y;
	D3DXFLOAT16 *pTex16X, *pTex16Y;

	pTex32X = new float[n];
	pTex32Y = new float[n];
	
	pTex16X	= new D3DXFLOAT16[n];
	pTex16Y = new D3DXFLOAT16[n]; 

	UINT i;
	for ( i = 0; i < n; i++) {
			pTex32X[i] = pIn[i].x;
			pTex32Y[i] = pIn[i].y;
		}
	
	D3DXFloat32To16Array(pTex16X, pTex32X, n);
	D3DXFloat32To16Array(pTex16Y, pTex32Y, n); 

	for ( i = 0; i < n; i++) {
			pOut[i].x = pTex16X[i];
			pOut[i].y = pTex16Y[i];
		}
	delete [] pTex32X;
	delete [] pTex32Y;
	delete [] pTex16X;
	delete [] pTex16Y;

}


