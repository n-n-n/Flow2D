#ifndef _FLOW_H_
#define _FLOW_H_

#include "../AlignedHeapFloat.h"

// -- 最速バージョン
//　* バウンダリーバッファーなし。

#pragma once
//#include "VectorStruct.h"
//#include <iostream>
//#include <fstream>
#include <cmath>
#ifndef NULL
#define NULL 0
#endif

static const int SCALAR_MAX_NUM = 2; // 9, 10	でなんかへん。

template <class X> inline void Swap(X &a, X &b);

#define MAX_SCALAR  1

static const int AlignedSize = 4; // 4 Packed Single-Precision FPを用いる
typedef __declspec(align(16)) float AFloat; // APSを使いたいので16BYTEにalignさせる

static const AFloat _0_0[4] = { 0.0f, 0.0f, 0.0f,0.0f};
static const AFloat _1_0[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
static const AFloat _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f};
static const AFloat _0_25[4] = { 0.25f, 0.25f, 0.25f, 0.25f};

template <class X> inline void Swap(X &a, X &b);

struct Int2 {
	int x;
	int y;
};

struct Neighbor{
	int right, left, up, down;
};

class Flow2D
{
public:
	Flow2D();		
	~Flow2D();		
	void Init(const int nx, const int ny, const int scaNum);
	void Create();
	void Release();
	void Clear();
	void Improve();
	
	bool SetParameters(const float dt, const int ite, const float res, const float vis, const float con); // (time step, iteration, residual error, viscosity, confinement(vortex );
    void NextStep();
	
	const float * GetScalarField(const int scalarIndex);
	void SetScalarField(const int scalarIndex, const float *dens, const int Nx, const int Ny);
	void AddVelocityOnLine(int fromPx, int fromPy,  int toPx, int toPy,float vel, const int bold);
	void SetScalarField(const int scalarIndex, const float *dens);

private:
		
	struct AFVec2 { 
		AlignedHeapFloat px;
		AlignedHeapFloat py;
	};
/*
	struct AFVec2{
		float * px;
		float * py;
	};
*/	
	void BackTrace(int Plaquette[4], float Weights[4], const int index);
	void BackTrace(Int2 Plaquette[2], float Weights[4], const int index);

	void AddVelocity(const int index, const float Vx, const float Vy);

protected:
	
	int		m_Nx;					//２の倍数
	int		m_Ny;					//２の倍数

	int		m_Nsite;
	int		m_NsitePacked;

	
	AFVec2	m_Vel;
	AFVec2  m_TmpVel;
	AFVec2	m_TmpVec;

	AlignedHeapFloat m_Aux;
	AlignedHeapFloat m_Vor;

	AlignedHeapFloat m_TmpA;
	AlignedHeapFloat m_TmpB;
	
	// Quantites on Dynamics ( m_NSite )
	int		m_Nscalar;					//	使用する分の場の数
	AlignedHeapFloat m_Scalar[MAX_SCALAR];	
	AlignedHeapFloat m_TmpScalar[MAX_SCALAR];

	// Parameters
	float	m_dtime;
	int		m_iteration;				
	float	m_residual;				
	float	m_viscosity;			
	float	m_improve;			
	float   m_difftime;

	int		m_time;				//time step

	void static SetVectorZero(AFVec2 &pVec, const int Size);
	void static SetScalarZero(AlignedHeapFloat* pScalar, const int Size);
	void static SetUniformVector(AFVec2 &pVec, const int Size, const float fVx, const float fVy);

	void Diffuse();
	void SetBoundary();// boundary 処理が違う、境界条件に応じて変える
	
	void Advect();	// use semi-Lagrangian method
	void SolvePoissonEq();

	inline int GetIndex(const int x, const int y) const;		

private:
	
	inline void SearchNeighbor(Neighbor &outNei, const int index);
//	void SearchNeighbor(Neighbor &outNei, const int ix, const int iy);
};
#endif