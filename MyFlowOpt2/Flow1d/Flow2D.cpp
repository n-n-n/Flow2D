#include "Flow2D.h"
#include <iostream>
#include <cmath>


//---
//--- 逆行バージョン 
//--- バウンダリーバッファー導入したら遅くなった。肝心の計算の効率化がされていない。
//--- 

#ifndef MAX
#define MAX(a,b) (((a) > (b) ) ? (a) :(b))
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if (x) { delete (x); x = 0};}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) {delete [] (x); x = 0;};}
#endif

template <class Type> inline void Swap(Type &a, Type &b)
{
	Type tmp;
	tmp = a;
	a = b;
	b = tmp;
}

inline int Flow2D::GetIndex(const int x, const int y) const
{
	return x + m_Nx * y;
}

inline int Flow2D::GetIndexFull(const int x, const int y) const 
{
	return x+1 + (y+1) * m_NxFull;
}

inline int Flow2D::GetIndexFull(const int i) const
{
	return  (i%m_Nx)+1 + ( (i/m_Nx) + 1) * m_NxFull;
}
void Flow2D::SetVectorZero(AFVec2* pVec, const int Size)
{
	int Asize = Size/4;

	_asm {
		mov ecx, pVec;
		mov eax, (AFVec2) [ecx].px;
		mov edx, (AFVec2) [ecx].py;
		movaps xmm0, XMMWORD PTR [_0_0]
		movaps xmm1, XMMWORD PTR [_0_0]	
		mov ecx, [Asize]
		LOOP1:
		movups  [eax], xmm0 // aps はヒープのアライメントが保障されていないのでだめ。
		movups [edx], xmm1
		add	eax, 16
		add ebx, 16
		loop LOOP1
	}

}

void Flow2D::SetStateZero(Flow2State* pState, const int Size)
{
	int ASite = Size/4;

	__asm {
		mov eax, pState;
		mov edi, [eax].pvor;
		mov esi, [eax].paux;
		mov ebx, (Flow2State) [eax].px; // 同じメンバーがあるものを使うときには判るようにキャストする。
		mov edx, (Flow2State) [eax].py;
		mov ecx, [ASite];
		movaps xmm0, [_0_0];
	LOOP1:
		movups	[ebx], xmm0; // apsはヒープのアライメントが保障されていないのでだめ。
		movups  [edx], xmm0;
		movups	[edi], xmm0;
		movups  [esi], xmm0;
		add		ebx, 16;
		add		edx, 16;
		add		edi, 16;
		add     esi, 16;
		loop LOOP1
	}

}

void Flow2D::SetUniformVector(AFVec2& Vec, const int Size, float fVx, float fVy)
{
	for ( int i = 0; i < Size; i++) {
		Vec.px[i] = fVx;
		Vec.py[i] = fVy;
	}
}

void Flow2D::SetScalarZero(AFloat* pScalar, const int Size)
{
	int Asize = Size/4;

	_asm {
		mov eax, pScalar
		mov ecx, [Asize]
		movaps xmm0, XMMWORD PTR [_0_0]
		LOOP1:
		movups XMMWORD PTR [eax], xmm0 // ヒープのアライメントが保障されていないので。
		add	eax, 16
		loop LOOP1
	}

}


void Flow2D::SetScalarZero(AFloatPacked* pScalar, const int PackedSize)
{
	AFloatPacked* pAFloatP;

	for ( int i = 0; i < PackedSize; i++) {

		pAFloatP = &(pScalar[i]);

		_asm {
			mov edx, pAFloatP;
			movaps xmm0, XMMWORD PTR [_0_0]
			movups [edx], xmm0					// ヒープのアライメントが保障されていないので。
		}
	}

}

//
Flow2D::Flow2D()
{
	m_Vel.px = NULL;
	m_Vel.py = NULL;

	m_Vel.pvor = NULL;
	m_Vel.paux = NULL;
	
	m_TmpVel.px = NULL;
	m_TmpVel.py = NULL;
	
	
	m_TmpA = NULL;
	m_TmpB = NULL;

	for (int i = 0; i < SCALAR_MAX_NUM; i++) {
		m_Scalar[i] = NULL;
		m_TmpScalar[i] = NULL;
	}
}

Flow2D::~Flow2D()
{
	SAFE_DELETE_ARRAY(m_Vel.px);
	SAFE_DELETE_ARRAY(m_Vel.py);

	SAFE_DELETE_ARRAY(m_Vel.pvor);
	SAFE_DELETE_ARRAY(m_Vel.paux);

	SAFE_DELETE_ARRAY(m_TmpVel.px);
	SAFE_DELETE_ARRAY(m_TmpVel.py);
	
	SAFE_DELETE_ARRAY(m_TmpA);
	SAFE_DELETE_ARRAY(m_TmpB);

	for (int i = 0; i < m_Nscalar; i++) {
		SAFE_DELETE_ARRAY(m_Scalar[i]);
		SAFE_DELETE_ARRAY(m_TmpScalar[i]);
	}
}

//------------------------------------------
void Flow2D::Clear()
{
	m_time = 0;

	SetStateZero(&m_Vel, m_NsiteFull);

	SetVectorZero(&m_TmpVel, m_NsiteFull);

	
	SetScalarZero(m_TmpA, m_Nsite);
	SetScalarZero(m_TmpB, m_Nsite);

	for (int j = 0; j < m_Nscalar; j++) {
		SetScalarZero(m_Scalar[j],	m_Nsite);
		SetScalarZero(m_TmpScalar[j],m_Nsite);
	}
}

void Flow2D::Init(const int nx, const int ny, const int scaNum)
{
	int i;

	m_dtime = 1.0f;

	m_iteration = 20;
	m_residual = 0.0f;
	m_viscosity = 0.0f;
	m_improve = 0.15f;

	m_Nx = nx;
	m_Ny = ny;
	
	m_Nsite = m_Nx * m_Ny;

	m_NxFull = m_Nx + 2;
	m_NyFull = m_Ny + 2;

	m_NsiteFull = m_NxFull * m_NyFull;

	m_Nscalar = scaNum;

	try {

		m_Vel.px = new float[m_NsiteFull];
		m_Vel.py = new float[m_NsiteFull];
		m_Vel.pvor = new float[m_NsiteFull];
		m_Vel.paux = new float[m_NsiteFull];

		m_TmpVel.px = new float[m_NsiteFull];
		m_TmpVel.py = new float[m_NsiteFull];

		m_TmpA = new float[m_Nsite];
		m_TmpB = new float[m_Nsite];
		
		for (i = 0; i < m_Nscalar; i++) {
			m_Scalar[i]		= new float[m_Nsite];
			m_TmpScalar[i]  = new float[m_Nsite];
		}
	
	} catch (std::bad_alloc xa) {
		exit(0);
	}

	Clear();
}


void Flow2D::NextStep()
{
	/*	
	if (m_viscosity > 0.0) {
		Diffuse();
	}
	
	if (m_improve > 0.0) {
		Improve();
	}
	*/
	Advect();
	SolvePoissonEq();
	
	m_time++;

}

void Flow2D::Advect()
{
	float Weights[4];
	int PlaquetteIndex[4];
	int PlaquetteIndexFull[4];
	Int2 Plaquette[2];
	int indexFull;

	Swap(m_Vel.px, m_TmpVel.px);
	Swap(m_Vel.py, m_TmpVel.py);

	for (int i = 0; i < m_Nscalar; i++) {
		Swap(m_Scalar[i], m_TmpScalar[i]);
	}

	for (int i = 0; i < m_Nsite; i++) {

		indexFull = GetIndexFull(i);

		BackTrace(Plaquette, Weights,i);
		
		PlaquetteIndex[0] = GetIndex(Plaquette[0].x, Plaquette[0].y);
		PlaquetteIndex[1] = GetIndex(Plaquette[1].x, Plaquette[0].y);
		PlaquetteIndex[2] = GetIndex(Plaquette[0].x, Plaquette[1].y);
		PlaquetteIndex[3] = GetIndex(Plaquette[1].x, Plaquette[1].y);

		PlaquetteIndexFull[0] = GetIndexFull(Plaquette[0].x, Plaquette[0].y);
		PlaquetteIndexFull[1] = GetIndexFull(Plaquette[1].x, Plaquette[0].y);
		PlaquetteIndexFull[2] = GetIndexFull(Plaquette[0].x, Plaquette[1].y);
		PlaquetteIndexFull[3] = GetIndexFull(Plaquette[1].x, Plaquette[1].y);

		m_Vel.px[indexFull] = 0.0f;
		m_Vel.py[indexFull] = 0.0f;

		for (int j = 0; j < 4; j++) {
			m_Vel.px[indexFull] += Weights[j] * m_TmpVel.px[ PlaquetteIndexFull[j] ];
			m_Vel.py[indexFull] += Weights[j] * m_TmpVel.py[ PlaquetteIndexFull[j] ];
		}

		for (int k = 0; k < m_Nscalar; k++) { 
			m_Scalar[k][i] = 0.0f;
			for (int j = 0; j < 4 ; j++) {
				m_Scalar[k][i] += Weights[j] * m_TmpScalar[k][ PlaquetteIndex[j] ];
			}
		}	
	}

	SetBoundary(m_Vel.px);
	SetBoundary(m_Vel.py);

}

void Flow2D::SolvePoissonEq()
{
//	bool loopEnd;
	const float h2 = 2.0;
	const float h4 = 4.0;
	int iX, iY;
	for (int i = 0; i < m_Nsite; i++) {
		iX = i%m_Nsite;
		iY = i/m_Nsite;

		m_TmpA[i] = (m_Vel.px[GetIndexFull(iX+1,iY)] - m_Vel.px[GetIndexFull(iX-1, iY)] 
				+ m_Vel.py[GetIndexFull(iX,iY+1)] - m_Vel.py[GetIndexFull(iX, iY-1)]) / h2;
	}

	for (int j = 0; j< m_iteration; j++) {
		Swap(m_Vel.paux, m_Vel.pvor);
//		loopEnd = true;
		for (int i = 0; i < m_Nsite; i++) {	
			iX = i%m_Nsite;
			iY = i/m_Nsite;

			m_Vel.paux[GetIndexFull(iX, iY)] = (m_Vel.pvor[GetIndexFull(iX+1,iY)] + m_Vel.pvor[GetIndexFull(iX-1,iY)]
					+ m_Vel.pvor[GetIndexFull(iX, iY+1)] + m_Vel.pvor[GetIndexFull(iX, iY-1)] - m_TmpA[i]) / h4;
//			if ( loopEnd && (m_Aux[j] - m_TmpB[j]) > residual * m_Aux[j] ) {
//				loopEnd = false;
//			}
		}

		SetBoundary(m_Vel.paux);

//		if (loopEnd) {
//			break;
//		}
	}

	for (int i = 0; i < m_Nsite; i++) {
		iX = i%m_Nsite;
		iY = i/m_Nsite;
		m_Vel.px[GetIndexFull(iX, iY)] -= (m_Vel.paux[GetIndexFull(iX+1, iY)] - m_Vel.paux[GetIndexFull(iX-1, iY)]) / h2;
		m_Vel.py[GetIndexFull(iX, iY)] -= (m_Vel.paux[GetIndexFull(iX, iY +1)] - m_Vel.paux[GetIndexFull(iX, iY-1)]) / h2;
	}

	SetBoundary(m_Vel.px);
	SetBoundary(m_Vel.py);
}


void Flow2D::BackTrace(Int2 Plaquette[2], float Weights[4], const int index)
{
	int iX = index % m_Nx;
	int iY = index / m_Nx;
	int indexFull = GetIndexFull(iX, iY);
	
	float fBackPtX = float(iX) - m_TmpVel.px[indexFull] * m_dtime;
	float fBackPtY = float(iY) - m_TmpVel.py[indexFull] * m_dtime;

	if (fBackPtX < 0.0f) {
		fBackPtX = m_Nx-1 + fmodf(fBackPtX, (float)m_Nx);
	} else if (fBackPtX >= m_Nx) {
		fBackPtX = fmodf(fBackPtX, (float)m_Nx);
	}

	if (fBackPtY < 0.0f) {
		fBackPtY = m_Ny-1 + fmodf(fBackPtY, (float)m_Ny);
	} else if (fBackPtY >= m_Ny) {
		fBackPtY = fmodf(fBackPtY, (float)m_Ny); 
	}

	float fdx,fdy; 
	float fidx, fidy;

	fdx	= modff(fBackPtX ,&(fidx));
	fdy	= modff(fBackPtY, &(fidy));

	// Set Sites Co On A Plaquette
	Plaquette[0].x =  (int)fidx;
	Plaquette[0].y =  (int)fidy;

	//right
	if (Plaquette[0].x == m_Nx - 1) {
		Plaquette[1].x = 0;
	} else { 
		Plaquette[1].x = Plaquette[0].x+ 1;
	}
	//top
	if (Plaquette[0].y == m_Ny - 1) {
		Plaquette[1].y = 0;
	} else {
		Plaquette[1].y = Plaquette[0].y + 1;
	}
	
	Weights[3] = fdx * fdy;
	Weights[2] = fdy - Weights[3];
	Weights[1] = fdx - Weights[3];
	Weights[0] = 1.0f - Weights[1] - fdy;
}

void Flow2D::SetBoundary(AFloat* pFull)
{

	for ( int i = 0; i < m_Nx; i++ ) {
		pFull[GetIndexFull(i,-1)] = pFull[GetIndexFull(i,m_Ny-1)];
		pFull[GetIndexFull(i, m_Ny)] = pFull[GetIndexFull(i,0)];
	}
	
	for ( int j = 0; j < m_Ny; j++ ) {
		pFull[GetIndexFull(-1,j)] = pFull[GetIndexFull(m_Nx-1, j)];
		pFull[GetIndexFull( m_Ny,j)] = pFull[GetIndexFull(0,j)];
	}

	pFull[GetIndexFull(-1,-1)]		= pFull[GetIndexFull(m_Nx-1, m_Ny-1)];
	pFull[GetIndexFull(m_Nx,-1)]	= pFull[GetIndexFull(m_Ny-1,0)];
	pFull[GetIndexFull(-1, m_Ny)]	= pFull[GetIndexFull(0,m_Ny-1)];
	pFull[GetIndexFull(m_Nx, m_Ny)] = pFull[GetIndexFull(0,0)];
}

bool Flow2D::SetParameters(const float dt, const int ite, const float res, const float vis, const float con)
{
	bool result = true;

	if (dt <= 1.0 || dt > 0.0) {
		m_dtime = dt;
	} else {
		result = false;
	}

	if (ite > 0) {
		m_iteration = ite;
	} else {
		result = false;
	}

	if (res <= 0.01 || res >= 0.0) {
		m_residual = res;
	} else {
		result = false;
	}

	if (vis <= 1.0 || vis >= 0.0) {
		m_viscosity = vis;
	} else {
		result = false;
	}

	if (con <= 1.0 || con >= 0.0) {
		m_improve = con;
	} else {
		result = false;
	}

	m_difftime = m_viscosity * m_dtime;

	return result;
}

//
const float * Flow2D::GetScalarField(const int scalarIndex)
{
	if (scalarIndex >= m_Nscalar) {
		return NULL;
	}

	return (const float *)m_Scalar[scalarIndex];
}


void Flow2D::SetScalarField(const int scalarIndex, const float *dens, const int Nx, const int Ny)
{
	if (dens == NULL ) return;

	if ( scalarIndex <0 || scalarIndex >= m_Nscalar ) return;
	 
	int ClipNx, ClipNy;
	ClipNx = ( Nx > m_Nx ) ? m_Nx : Nx;
	ClipNy = ( Ny > m_Ny ) ? m_Ny : Ny;

	int SrcIndex;
	int DstIndex;
	for ( int j = 0; j <  ClipNy; j++ ){
		SrcIndex = j * Nx;
		DstIndex = j * m_Nx;
		for ( int i = 0; i < Nx; i++) { 
			if ( dens[SrcIndex] > 1.0f) m_Scalar[scalarIndex][DstIndex] =1.0f;
			else if ( dens[SrcIndex] < 0.0f ) m_Scalar[scalarIndex][DstIndex] = 0.0f;
			else  m_Scalar[scalarIndex][SrcIndex] = dens[DstIndex];
		
			SrcIndex++;
			DstIndex++;
		}
	}
	
	return;
}


void Flow2D::AddVelocityOnLine(int fromPx, int fromPy,  int toPx, int toPy,float vel, const int bold)
{
	if (vel<= 0.0) 	return;

	int iVx = toPx - fromPx;
	int iVy = toPy - fromPy;

	vel = vel/sqrtf((float)(iVx * iVx + iVy * iVy));
	if ( vel == 0.0f ) return;

	float mx, my;
	int px,py;
	float fVx = float(iVx) * vel;
	float fVy = float(iVy) * vel;
	
	if (fVx == 0.0f) {
		if (fromPy > toPy) {
			Swap(fromPx, toPx);
			Swap(fromPy, toPy);
		}
		for (int j = fromPy; j <= toPy; j++) {
	    	for (int i = fromPx - bold; i <= fromPx + bold; i++){
				AddVelocity(GetIndex(i, j), fVx, fVy);
			}
		}
	} else if (fVy == 0.0f) {
		if (fromPx > toPx) {
			Swap(fromPx, toPx);
			Swap(fromPy, toPy);
		}
		for (int i = fromPx; i < toPx; i++) {
	    	for (int j = fromPy - bold; j <= fromPy + bold; j++) {
                AddVelocity(GetIndex(i, j), fVx, fVy);
			}
		}
	} else {
		mx = (float)(fVy / fVx);	
		if (fabs(mx) < 1.0) {
			if (fromPx > toPx) {
				Swap(fromPx, toPx);
				Swap(fromPy, toPy);
			}
			for (int i = fromPx; i < toPy; i++) {
				py = (int)( mx * (i - fromPx) + fromPy);
				for (int j = py - bold; j <= py + bold; j++) {
                    AddVelocity(GetIndex(i, j),fVx, fVy);
				}
			}
		} else {
			if (fromPy > toPy) {
				Swap(fromPx, toPx);
				Swap(fromPy, toPy);
			}
			my = (float)(fVx / fVy);
			for (int j = fromPy; j < toPy; j++) {
				px = (int)(my * (j - fromPy) + fromPx);
				for(int i = px - bold ; i <= px + bold; i++){
					AddVelocity(GetIndex(i, j), fVx, fVy);
				}
			}
		}
	}

	SetBoundary(m_Vel.px);
	SetBoundary(m_Vel.py);
}


void Flow2D::AddVelocity(const int index, const float fVx, const float fVy)
{
	if ( 0>index || index > m_Nsite) {
		return;
	}
	m_Vel.px[ GetIndexFull(index) ] += fVx;
	m_Vel.py[ GetIndexFull(index) ] += fVy;
}


