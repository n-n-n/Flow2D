#include "Flow2D.h"
#include <iostream>
#include <cmath>
#include "AlignedHeapFloat.h"

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

	// AlignedHeapFloatのオフセットが出来なかったのでローカル変数を使う
	float	*AlignedX = pVec->x.aligned;
	float   *AlignedY = pVec->y.aligned;
	_asm {
		push ecx
		push eax
		push ebx
		mov	 eax, AlignedX
		mov  ebx, AlignedY
		movaps xmm0, XMMWORD PTR [_0_0]
		movaps xmm1, XMMWORD PTR [_0_0]	
		mov ecx, [Asize]
		LOOP1:
		movaps XMMWORD PTR [eax], xmm0 
		movaps XMMWORD PTR [ebx], xmm1
		add	eax, 16
		add ebx, 16
		loop LOOP1

		pop	ebx
		pop	eax
		pop ecx
	}
/*
	_asm {
		push ecx
		push eax
		push ebx
		push edx
		mov edx, pVec
		mov ecx, (AFVec2) [edx].x
		mov eax, ecx //  [ecx].aligned にしていたら、混合モードでは、[ecx]になってた, localでみて、先頭にalingedがあるので。オフセットの取り方がわからん
		mov ecx, (AFVec2) [edx].y
		mov ebx, ecx//
		movaps xmm0, XMMWORD PTR [_1_0]
		movaps xmm1, XMMWORD PTR [_1_0]	
		mov ecx, [Asize]
		LOOP1:
		movaps XMMWORD PTR [eax], xmm0 // 不満 apsを使いたい-ここでは頭なのにだめ？XMMWORD PTRいれてもだめ。
		movaps XMMWORD PTR [ebx], xmm1
		add	eax, 16
		add ebx, 16
		loop LOOP1
		pop edx
		pop	ebx
		pop	eax
		pop ecx
	}
	*/
/*
	for (int i = 0; i < Size; i++) {
		Vec.px[i] = 0.0f;
		Vec.py[i] = 0.0f;
	}
*/
}

void Flow2D::SetStateZero(Flow2State* pState, const int Size)
{
	int ASite = Size/4;

	// .px, .py でエラー出すけどだしてないものもある、どうなってるのか判らん-> 型を教えてやると良いみたい。
	// aligned が先頭に来てるのでオフセットがいらなかった。
	__asm {
		push ecx
		push eax
		push ebx
		push edx
		push edi

		mov ecx, pState;
		mov eax, [ecx].vor;
		mov ebx, [ecx].aux;
		mov edx, (Flow2State) [ecx].x;
		mov edi, (Flow2State) [ecx].y;
		mov ecx, [ASite];
		movaps xmm0, [_0_0];
	LOOP1:
		movaps	[eax], xmm0; 
		movaps  [ebx], xmm0;
		movaps	[edx], xmm0;
		movaps  [edi], xmm0;
		add		eax, 16;
		add		ebx, 16;
		add		edx, 16;
		add     edi, 16;
		loop LOOP1
	
		pop edi
		pop edx
		pop ebx
		pop eax
		pop ecx

	}

}

void Flow2D::SetUniformVector(AFVec2& Vec, const int Size, float fVx, float fVy)
{
	for ( int i = 0; i < Size; i++) {
		Vec.x.aligned[i] = fVx;
		Vec.y.aligned[i] = fVy;
	}
}

void Flow2D::SetScalarZero(AlignedHeapFloat* pScalar, const int Size)
{
	int Asize = Size/4;
	float* afloat = pScalar->aligned;

	__asm {
		push eax
		push ecx
		mov eax, afloat
		mov ecx, [Asize]
		movaps xmm0, XMMWORD PTR [_0_0]
		LOOP1:
		movaps XMMWORD PTR [eax], xmm0
		add	eax, 16
		loop LOOP1
		pop ecx
		pop eax
	}

}



//
Flow2D::Flow2D()
{
	
}

Flow2D::~Flow2D()
{


}

//------------------------------------------
void Flow2D::Clear()
{
	m_time = 0;

	SetStateZero(&m_Vel, m_NsiteFull);

	SetVectorZero(&m_TmpVel, m_NsiteFull);

	SetScalarZero(&m_TmpA, m_Nsite);
	SetScalarZero(&m_TmpB, m_Nsite);

	for (int j = 0; j < m_Nscalar; j++) {
		SetScalarZero(&m_Scalar[j],	m_Nsite);
		SetScalarZero(&m_TmpScalar[j],m_Nsite);
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

		m_Vel.x.AllocateHeap(m_NsiteFull, AlignedBoundary);		
		m_Vel.y.AllocateHeap(m_NsiteFull, AlignedBoundary);
		m_Vel.vor.AllocateHeap(m_NsiteFull, AlignedBoundary);
		m_Vel.aux.AllocateHeap(m_NsiteFull, AlignedBoundary);

		m_TmpVel.x.AllocateHeap(m_NsiteFull, AlignedBoundary);
		m_TmpVel.y.AllocateHeap(m_NsiteFull, AlignedBoundary);
	
		m_TmpA.AllocateHeap(m_Nsite, AlignedBoundary);
		m_TmpB.AllocateHeap(m_Nsite, AlignedBoundary);
		
		for (i = 0; i < m_Nscalar; i++) {
			m_Scalar[i].AllocateHeap(m_Nsite, AlignedBoundary);
			m_TmpScalar[i].AllocateHeap(m_Nsite, AlignedBoundary);
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

	Swap(m_Vel.x.aligned, m_TmpVel.x.aligned);
	Swap(m_Vel.y.aligned, m_TmpVel.y.aligned);

	for (int i = 0; i < m_Nscalar; i++) {
		Swap(m_Scalar[i].aligned, m_TmpScalar[i].aligned);
	}

	for (int i = 0; i < m_Nsite; i++) {
//
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

		m_Vel.x.aligned[indexFull] = 0.0f;
		m_Vel.y.aligned[indexFull] = 0.0f;

		for (int j = 0; j < 4; j++) {
			m_Vel.x.aligned[indexFull] += Weights[j] * m_TmpVel.x.aligned[ PlaquetteIndexFull[j] ];
			m_Vel.y.aligned[indexFull] += Weights[j] * m_TmpVel.y.aligned[ PlaquetteIndexFull[j] ];
		}

		for (int k = 0; k < m_Nscalar; k++) { 
			m_Scalar[k].aligned[i] = 0.0f;
			for (int j = 0; j < 4 ; j++) {
				m_Scalar[k].aligned[i] += Weights[j] * m_TmpScalar[k].aligned[ PlaquetteIndex[j] ];
			}
		}	
	}

	SetBoundary(m_Vel.x.aligned);
	SetBoundary(m_Vel.y.aligned);

}

void Flow2D::SolvePoissonEq()
{
//	bool loopEnd;
	const float h2 = 2.0;
	const float h4 = 4.0;
	int iX, iY;
//	float* tmpA;
//	float* velX;
//	float* velY;
	for (int i = 0; i < m_Nsite; i++) {
		iX = i%m_Nsite;
		iY = i/m_Nsite;

		m_TmpA.aligned[i] = (m_Vel.x.aligned[GetIndexFull(iX+1,iY)] - m_Vel.x.aligned[GetIndexFull(iX-1, iY)] 
				+ m_Vel.y.aligned[GetIndexFull(iX,iY+1)] - m_Vel.y.aligned[GetIndexFull(iX, iY-1)]) / h2;
	
	/*
		tmpA = m_TmpA.aligned+i;
		velX = m_Vel.x.aligned;
		velY = m_Vel.y.aligned;

		__asm {
			push eax
			mov eax, 

			mov eax, tmpA

}	
*/	
	
	}

	for (int j = 0; j< m_iteration; j++) {
		Swap(m_Vel.aux.aligned, m_Vel.vor.aligned);
//		loopEnd = true;
		for (int i = 0; i < m_Nsite; i++) {	
			iX = i%m_Nsite;
			iY = i/m_Nsite;

			m_Vel.aux.aligned[GetIndexFull(iX, iY)] = (m_Vel.vor.aligned[GetIndexFull(iX+1,iY)] + m_Vel.vor.aligned[GetIndexFull(iX-1,iY)]
					+ m_Vel.vor.aligned[GetIndexFull(iX, iY+1)] + m_Vel.vor.aligned[GetIndexFull(iX, iY-1)] - m_TmpA.aligned[i]) / h4;
//			if ( loopEnd && (m_Aux[j] - m_TmpB[j]) > residual * m_Aux[j] ) {
//				loopEnd = false;
//			}
		}

		SetBoundary(m_Vel.aux.aligned);

//		if (loopEnd) {
//			break;
//		}
	}

	for (int i = 0; i < m_Nsite; i++) {
		iX = i%m_Nsite;
		iY = i/m_Nsite;
		m_Vel.x.aligned[GetIndexFull(iX, iY)] -= (m_Vel.aux.aligned[GetIndexFull(iX+1, iY)] - m_Vel.aux.aligned[GetIndexFull(iX-1, iY)]) / h2;
		m_Vel.y.aligned[GetIndexFull(iX, iY)] -= (m_Vel.aux.aligned[GetIndexFull(iX, iY +1)] - m_Vel.aux.aligned[GetIndexFull(iX, iY-1)]) / h2;
	}

	SetBoundary(m_Vel.x.aligned);
	SetBoundary(m_Vel.y.aligned);
}

inline void Flow2D::BackTrace(Int2 Plaquette[2], float Weights[4], const int index)
{
	int iX = index % m_Nx;
	int iY = index / m_Nx;
	int indexFull = GetIndexFull(iX, iY);
	
	float fBackPtX = float(iX) - m_TmpVel.x.aligned[indexFull] * m_dtime;
	float fBackPtY = float(iY) - m_TmpVel.y.aligned[indexFull] * m_dtime;

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

	return (const float *)(m_Scalar[scalarIndex].aligned);
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
			if ( dens[SrcIndex] > 1.0f) m_Scalar[scalarIndex].aligned[DstIndex] =1.0f;
			else if ( dens[SrcIndex] < 0.0f ) m_Scalar[scalarIndex].aligned[DstIndex] = 0.0f;
			else  m_Scalar[scalarIndex].aligned[SrcIndex] = dens[DstIndex];
		
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

	SetBoundary(m_Vel.x.aligned);
	SetBoundary(m_Vel.y.aligned);
}


void Flow2D::AddVelocity(const int index, const float fVx, const float fVy)
{
	if ( 0>index || index > m_Nsite) {
		return;
	}
	m_Vel.x.aligned[ GetIndexFull(index) ] += fVx;
	m_Vel.y.aligned[ GetIndexFull(index) ] += fVy;
}


