#ifndef _FLOW_H_
#define _FLOW_H_

//---
//--- �t�s�o�[�W���� 
//--- �o�E���_���[�o�b�t�@�[����������x���Ȃ����B�̐S�̌v�Z�̌�����������Ă��Ȃ��B
//--- 
#pragma once
//#include "VectorStruct.h"
//#include <iostream>
//#include <fstream>
#include <cmath>
#ifndef NULL
#define NULL 0
#endif

static const int SCALAR_MAX_NUM = 2; // 9, 10	�łȂ񂩂ւ�B

template <class X> inline void Swap(X &a, X &b);


#define MAX_SCALAR  1

// Heap �̃A���C�����g�͕ۏႳ��Ȃ��̂Œ���
static const int AlignedSize = 4; // 4 Packed Single-Precision FP��p����
typedef __declspec(align(16)) float AFloat; // APS���g�������̂�16BYTE��align������

static const AFloat _0_0[4] = { 0.0f, 0.0f, 0.0f,0.0f};
static const AFloat _1_0[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
static const AFloat _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f};
static const AFloat _0_25[4] = { 0.25f, 0.25f, 0.25f, 0.25f};


typedef struct _F_VECTOR2	{ 
								float* px;
								float* py;
								} AFVec2;

typedef struct _F_STATE2 { 
							float* px;
							float* py;
							float* pvor;
							float* paux;
						} Flow2State;


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
	
	bool SetParameters(const float dt, const int ite, const float res, const float vis, const float con); // (time step, iteration, residual error, viscosity, confinement(vortex );
    void NextStep();
	
	const float * GetScalarField(const int scalarIndex);
	void SetScalarField(const int scalarIndex, const float *dens, const int Nx, const int Ny);
	void AddVelocityOnLine(int fromPx, int fromPy,  int toPx, int toPy,float vel, const int bold);
	void SetScalarField(const int scalarIndex, const float *dens);

private:
		
	void BackTrace(Int2 Plaquette[2], float Weights[4], const int index);
	void BackTrace();

	void AddVelocity(const int index, const float Vx, const float Vy);

protected:
	
	int		m_Nx;					//�Q�̔{��
	int		m_Ny;					//�Q�̔{��

	int		m_Nsite;

	int		m_NxFull;
	int		m_NyFull;
	int		m_NsiteFull;

	Flow2State	m_Vel;

	AFVec2  m_TmpVel;
	AFVec2	m_TmpVec;

	AFloat* m_TmpA;
	AFloat* m_TmpB;
	
	// Quantites on Dynamics ( m_NSite )
	int		m_Nscalar;					//	�g�p���镪�̏�̐�
	AFloat* m_Scalar[MAX_SCALAR];	
	AFloat* m_TmpScalar[MAX_SCALAR];

	// Parameters
	float	m_dtime;
	int		m_iteration;				
	float	m_residual;				
	float	m_viscosity;			
	float	m_improve;			
	float   m_difftime;

	int		m_time;				//time step

	void static SetVectorZero(AFVec2* pVec, const int Size);
	void static SetScalarZero(AFloat* pScalar, const int Size);
	void static SetUniformVector(AFVec2 &pVec, const int Size, const float fVx, const float fVy);
	void static SetStateZero(Flow2State* pState, const int Size);


	void SetBoundary(AFloat* pFull);// boundary �������Ⴄ�A���E�����ɉ����ĕς���
	
	void Advect();	// use semi-Lagrangian method
	void SolvePoissonEq();

	inline int GetIndex(const int x, const int y) const;		
	inline int GetIndexFull(const int x, const int y) const;
	inline int GetIndexFull(const int i) const;
private:
	
	inline void SearchNeighbor(Neighbor &outNei, const int index);
//	void SearchNeighbor(Neighbor &outNei, const int ix, const int iy);
};
#endif