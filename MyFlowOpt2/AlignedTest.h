#ifndef _ALIGNED_TEST_
#define _ALIGNED_TEST_
#include <windows.h>

struct pDColor {
	DWORD *pR;
	DWORD *pG;
	DWORD *pB;
	DWORD *pA;
};

namespace SSE_CONST {
static const __declspec(align(32)) float _0_0[4] = { 0.0f, 0.0f, 0.0f,0.0f};
static const __declspec(align(32)) float _1_0[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
static const __declspec(align(32)) float _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f};
static const __declspec(align(32)) float _0_25[4] = { 0.25f, 0.25f, 0.25f, 0.25f};
};

//typedef __declspec(align(32)) float AFloat; // APSを使いたいので16BYTEにalignさせる

typedef struct _VERTEX_AOS {  float x, y,z, color;} Vertex_aos;
typedef struct _VERTEX_SOA {  float x[4], y[4], z[4], color[4];} Vertex_soa;

// ダイナミカルな使用を想定しているが、アライメントは保障されない。
typedef struct _VERTEX_SOP 
	{	 float* px;
		 float* py;
		 float* pz;
		 float* pcolor;
		 float* pxBody;
		 float* pyBody;
		 float* pzBody;
		 float* pcolorBody;
	} Vertex_sop;


void MMX_Deswizzle(pDColor ApColor, DWORD* pColors, const int Size);
void SSE_AOStoSOA(Vertex_aos* aosInn, Vertex_soa* soaOut);
void SSE_AOStoSOP(Vertex_aos* aosIn,Vertex_sop* sopOut, const int size);
void SSE_SOAtoAOS(Vertex_soa* soaIn, Vertex_aos* aosOut);
void SSE_SOAtoAOS2(Vertex_soa* soaIn, Vertex_aos* aosOut);
void SSE_SOPtoAOS(Vertex_sop* sopIn, Vertex_aos* aosOut, const int Asize);
void SSE_SetZero(Vertex_sop* sop, int size);

#endif // _ALIGNED_TEST_