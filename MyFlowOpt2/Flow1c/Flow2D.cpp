#include "Flow2D.h"
#include <iostream>
#include <cmath>

// -- 最速バージョン
//　* バウンダリーバッファーなし。

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

void Flow2D::SetVectorZero(AFVec2& Vec, const int Size)
{

	for (int i = 0; i < Size; i++) {
		Vec.px.aligned[i] = 0.0f;
		Vec.py.aligned[i] = 0.0f;
	}

}

void Flow2D::SetUniformVector(AFVec2& Vec, const int Size, float fVx, float fVy)
{
	for ( int i = 0; i < Size; i++) {
		Vec.px.aligned[i] = fVx;
		Vec.py.aligned[i] = fVy;
	}

}

void Flow2D::SetScalarZero(AlignedHeapFloat* pScalar, const int Size)
{

	for (int i = 0; i < Size; i++) {
		pScalar->aligned[i] = 0.0f;
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

	//SetUniformVector(m_Vel, m_Nsite, 1.0f, 0.0f);
	
	SetVectorZero(m_Vel, m_Nsite);
	SetVectorZero(m_TmpVel, m_Nsite);
	SetVectorZero(m_TmpVec, m_Nsite);

	SetScalarZero(&m_Vor, m_Nsite);
	SetScalarZero(&m_Aux, m_Nsite);
	SetScalarZero(&m_TmpA, m_Nsite);
	SetScalarZero(&m_TmpB, m_Nsite);

	for (int j = 0; j < m_Nscalar; j++) {
		SetScalarZero(&(m_Scalar[j]),	m_Nsite);
		SetScalarZero(&(m_TmpScalar[j]),m_Nsite);
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


	m_Nscalar = scaNum;

	try {

		m_Vel.px.AllocateHeap(m_Nsite, 16);
		m_Vel.py.AllocateHeap(m_Nsite, 16);
		m_Vor.AllocateHeap(m_Nsite,16);
		m_Aux.AllocateHeap(m_Nsite,16);

		m_TmpVel.px.AllocateHeap(m_Nsite, 16);
		m_TmpVel.py.AllocateHeap(m_Nsite, 16);

		m_TmpVec.px.AllocateHeap(m_Nsite, 16);
		m_TmpVec.py.AllocateHeap(m_Nsite, 16);

		m_TmpA.AllocateHeap(m_Nsite, 16);
		m_TmpB.AllocateHeap(m_Nsite, 16);
		
		for (i = 0; i < m_Nscalar; i++) {
			m_Scalar[i].AllocateHeap(m_Nsite, 16);
			m_TmpScalar[i].AllocateHeap(m_Nsite, 16);
		}
	
	} catch (std::bad_alloc xa) {
		exit(0);
	}

	Clear();
}

void Flow2D::SearchNeighbor(Neighbor &nei, const int index)
{
	int ix, iy;
	//
	nei.right = index + 1;
	nei.left = index - 1;
	nei.up = index + m_Nx;
	nei.down = index - m_Nx;

	ix = index%m_Nx;
	iy = index/m_Nx;

	//right
	if (ix == m_Nx - 1) {
		nei.right = GetIndex(0, iy);
	}

	//left
	else if (ix == 0) {
		nei.left = GetIndex(m_Nx - 1, iy);
	}

	//up
	if (iy == m_Ny - 1) {
		nei.up = GetIndex(ix, 0);
	}

	//down
	else if (iy == 0) {
		nei.down = GetIndex(ix, m_Ny - 1);
	}

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
	Int2 Plaquette[2];

	Swap(m_Vel.px.aligned, m_TmpVel.px.aligned);
	Swap(m_Vel.py.aligned, m_TmpVel.py.aligned);

	for (int i = 0; i < m_Nscalar; i++) {
		Swap(m_Scalar[i].aligned, m_TmpScalar[i].aligned);
	}

	for (int i = 0; i < m_Nsite; i++) {

		//BackTrace(PlaquetteIndex, Weights, i);
		BackTrace(Plaquette, Weights,i);
		PlaquetteIndex[0] = Plaquette[0].x + m_Nx * Plaquette[0].y;
		PlaquetteIndex[1] = Plaquette[1].x + m_Nx * Plaquette[0].y;
		PlaquetteIndex[2] = Plaquette[0].x + m_Nx * Plaquette[1].y;
		PlaquetteIndex[3] = Plaquette[1].x + m_Nx * Plaquette[1].y;

		m_Vel.px.aligned[i] = 0.0f;
		m_Vel.py.aligned[i] = 0.0f;

		for (int j = 0; j < 4; j++) {
			m_Vel.px.aligned[i] += Weights[j] * m_TmpVel.px.aligned[ PlaquetteIndex[j] ];
			m_Vel.py.aligned[i] += Weights[j] * m_TmpVel.py.aligned[ PlaquetteIndex[j] ];
		}

		for (int k = 0; k < m_Nscalar; k++) { 
			m_Scalar[k].aligned[i] = 0.0f;
			for (int j = 0; j < 4 ; j++) {
				m_Scalar[k].aligned[i] += Weights[j] * m_TmpScalar[k].aligned[ PlaquetteIndex[j] ];
			}
		}	
	}
}

void Flow2D::SolvePoissonEq()
{
	int i, j;
//	bool loopEnd;
	const float h2 = 2.0;
	const float h4 = 4.0;
	Neighbor nei;
	for (i = 0; i < m_Nsite; i++) {
		SearchNeighbor(nei, i);
		m_TmpA.aligned[i] = (m_Vel.px.aligned[nei.right] - m_Vel.px.aligned[nei.left] + m_Vel.py.aligned[nei.up] - m_Vel.py.aligned[nei.down]) / h2;
	}

	for (i = 0; i < m_iteration; i++) {
		Swap(m_Aux.aligned, m_TmpB.aligned);
//		loopEnd = true;
		for (j = 0; j < m_Nsite; j++) {	
			SearchNeighbor(nei, j);
			m_Aux.aligned[j] = (m_TmpB.aligned[nei.right] + m_TmpB.aligned[nei.left] + m_TmpB.aligned[nei.up] + m_TmpB.aligned[nei.down]
						- m_TmpA.aligned[j]) / h4;
//			if ( loopEnd && (m_Aux[j] - m_TmpB[j]) > residual * m_Aux[j] ) {
//				loopEnd = false;
//			}
		}

//		if (loopEnd) {
//			break;
//		}
	}

	for (i = 0; i < m_Nsite; i++) {
		SearchNeighbor(nei, i);
		m_Vel.px.aligned[i] -= (m_Aux.aligned[nei.right] - m_Aux.aligned[nei.left]) / h2;
		m_Vel.py.aligned[i] -= (m_Aux.aligned[nei.up] - m_Aux.aligned[nei.down]) / h2;
	}
}

void Flow2D::BackTrace(int Plaquette[4], float Weights[4], const int index)
{
	float fBackPtX = float(index % m_Nx) - m_TmpVel.px.aligned[index] * m_dtime;
	float fBackPtY = float(index / m_Nx) - m_TmpVel.py.aligned[index] * m_dtime;

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
	int iBackPtX, iBackPtY;
	int iX, iY;

	fdx		=  modff(fBackPtX ,&(fidx));
	fdy		=  modff(fBackPtY, &(fidy));

	iBackPtX = (int)fidx;
	iBackPtY = (int)fidy;

	// Set Sites On A Plaquette
	Plaquette[0] = iBackPtX + iBackPtY * m_Nx;

	//right
	if (iBackPtX == m_Nx - 1) {
		Plaquette[1] = iBackPtY * m_Nx;
		iX = 0;
	} else { 
		Plaquette[1] = Plaquette[0]+1;
		iX = iBackPtX + 1;
	}
	//top
	if (iBackPtY == m_Ny - 1) {
		Plaquette[2] = iBackPtX;
		iY = 0;
	} else {
		Plaquette[2] = Plaquette[0]+ m_Nx;
		iY = iBackPtY + 1;
	}

	Plaquette[3] = GetIndex(iX, iY);
	
	Weights[3] = fdx * fdy;
	Weights[2] = fdy - Weights[3];
	Weights[1] = fdx - Weights[3];
	Weights[0] = 1.0f - Weights[1] - fdy;
}

void Flow2D::BackTrace(Int2 Plaquette[2], float Weights[4], const int index)
{
	float fBackPtX = float(index % m_Nx) - m_TmpVel.px.aligned[index] * m_dtime;
	float fBackPtY = float(index / m_Nx) - m_TmpVel.py.aligned[index] * m_dtime;

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

//
void Flow2D::Improve()
{
	int i;
	Neighbor nei;
	float norm;
	float condt = m_improve * m_dtime;

	for (i = 0; i < m_Nsite; i++) {
		SearchNeighbor(nei, i);
		m_Vor.aligned[i]
			=  (m_Vel.py.aligned[ nei.right ] - m_Vel.py.aligned[ nei.left ]- m_Vel.px.aligned[ nei.up ] + m_Vel.px.aligned[ nei.down ]) / 2.0f;
		m_TmpA.aligned[i] = fabs(m_Vor.aligned[i]);
	}

	for (i = 0; i < m_Nsite; i++) {
		SearchNeighbor(nei, i);
		m_TmpVec.px.aligned[i] = (m_TmpA.aligned[ nei.right ] - m_TmpA.aligned[ nei.left ]) / 2.0f;
		m_TmpVec.py.aligned[i] = (m_TmpA.aligned[ nei.up ] - m_TmpA.aligned[ nei.down ]) / 2.0f;
		
		norm = sqrtf(powf(m_TmpVec.px.aligned[i],2) + powf(m_TmpVec.py.aligned[i],2));
		m_TmpVec.px.aligned[i] /=norm; 
		m_TmpVec.py.aligned[i] /=norm;
	}

	for (i = 0; i < m_Nsite; i++) {
		m_Vel.px.aligned[i] += condt * (m_TmpVec.py.aligned[i] * m_Vor.aligned[i]);
		m_Vel.py.aligned[i] += condt * (-m_TmpVec.px.aligned[i] * m_Vor.aligned[i]);
	}
}


void Flow2D::AddVelocity(const int index, const float fVx, const float fVy)
{
	if ( 0>index || index > m_Nsite) {
		return;
	}
	m_Vel.px.aligned[ index ] += fVx;
	m_Vel.py.aligned[ index ] += fVy;
}

void Flow2D::Diffuse()
{
	Neighbor nei;

	for (int i = 0; i < m_Nsite; i++) {
		SearchNeighbor(nei, i);
		m_TmpVel.px.aligned[i] = m_Vel.px.aligned[nei.right] + m_Vel.px.aligned[nei.left] + m_Vel.px.aligned[nei.up] + m_Vel.px.aligned[nei.down] - m_Vel.px.aligned[i] * 4.0f;
		m_TmpVel.py.aligned[i] = m_Vel.py.aligned[nei.right] + m_Vel.py.aligned[nei.left] + m_Vel.py.aligned[nei.up] + m_Vel.py.aligned[nei.down] - m_Vel.py.aligned[i] * 4.0f;
	}

	for (i = 0; i < m_Nsite; i++) {
		m_Vel.px.aligned[i] += m_difftime * m_TmpVel.px.aligned[i];
		m_Vel.py.aligned[i] += m_difftime * m_TmpVel.py.aligned[i];
	}
}


//
//
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
}
