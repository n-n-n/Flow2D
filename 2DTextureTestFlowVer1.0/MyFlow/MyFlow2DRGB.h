/*************************************************************************

 TEXTRUE REPRESENTED FIELDS 

**************************************************************************/
#ifndef _MYFLOW2DRGB_H
#define _MYFLOW2DRGB_H

#include "dxstdafx.h"
#include <iostream>
#include <cmath>
#include "MyUtil.h"

using namespace std;

static LPCTSTR pEffectFile= L"./MyFlow/MyFlow2DRGB.fx";	// effect file for this class

// Texture and Surface formats
/* To use the multi-target support, same textures are chosen */ 
static D3DFORMAT ScalarFormat	= D3DFMT_G32R32F;
static D3DFORMAT VectorFormat	= D3DFMT_G32R32F;
static D3DFORMAT ZBufferFormat	= D3DFMT_D16;

// Vertex declaration for the frame of picture
struct RHW_TEX1_VERTEX {float x, y, z;	float rhw; float u0, v0;};
#define D3DFVF_RHW_TEX1_VERTEX 	(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

// for safty check, MaxScala + MaxVector \leq 16 
const int  MaxScalar = 8;
const int  MaxVector = 8;

enum BoundaryCondition { BC_PERIODIC, BC_RIGID, BC_X_PERIODIC}; // boundary conditions
enum FilterSetting	   { FILTER_POINT, FILTER_LINEAR};			// filter	setting

const int g_NScalar = 3;
const int g_NScalarTex = 2 * 3;
const int g_NVector = 1;
const int g_NVectorTex = 2 * 1;

class MyFlow2DRGB 
{
private:

	int		m_Nx;			// the Number of Sites in the x Direction
	int		m_Ny;			// the Number of Sites in the y Direction
	int		m_N;			// the Number of Total Sites = the Size of Texcels

	BoundaryCondition	m_BC;


	int		m_ItrJacobiMethod;  // the number of iterations in Jacobi method

	LPDIRECT3DDEVICE9	m_pd3dDevice;			// Device whic are Fields(Textures) on 

	LPDIRECT3DTEXTURE9	*m_ppScalarTexture;		// Scalar Fields Configulations via Floating Point Textures 
	LPDIRECT3DSURFACE9	*m_ppScalarSurface;		//	Scalar Fields Surface
	
	LPDIRECT3DTEXTURE9	*m_ppVectorTexture;		// Vector Fields Configulations via Floating Point Textures
	LPDIRECT3DSURFACE9	*m_ppVectorSurface;		//	Scalar Fields Surface
	
	LPDIRECT3DSURFACE9	m_pZBuffer;				// Depth Stencil Surface
	
	LPDIRECT3DSURFACE9	m_pOldBackBuffer;		// To Keep Old Target
	LPDIRECT3DSURFACE9	m_pOldZBuffer;			//

	LPDIRECT3DTEXTURE9	*m_ppPaintVectorTexture;
	LPDIRECT3DTEXTURE9	*m_ppPaintScalarTexture;

	// Temporal entry for monitering 
	LPDIRECT3DTEXTURE9  m_pTmpScalarTexture;		// Depth Stencil Surface
	LPDIRECT3DSURFACE9	m_pTmpScalarSurface;

	LPDIRECT3DTEXTURE9  m_ppTmpScalarTexture[2];	
	LPDIRECT3DSURFACE9	m_ppTmpScalarSurface[2];

	

	// Front Id 
	int		*m_pScalarId; 
	int		*m_pVectorId;

	ID3DXEffect*		m_pEffect;		// D3DX effect interface, which is used the texture operation.

	RHW_TEX1_VERTEX m_Frame[4];			// vertices for the frame of pictures


	void InitDevice(LPDIRECT3DDEVICE9 pd3dDevice) { m_pd3dDevice = pd3dDevice;}
	
	void InitLattice(const int Nx, const int Ny, BoundaryCondition bc);		// Initialize the Lattice 
	

	void InitTextures();				// Initialize the texture pointers to NULL
	void InitEffect() { m_pEffect = 0;}
	void InitFrame();														// Initialize the frame of picutres
	void InitIndex();

	HRESULT SetTextureCondition(DWORD i, BoundaryCondition bc,FilterSetting filter);			// Set BoundaryCondtions

	HRESULT CreateEffect();													// Create The Effect
	HRESULT CreateTextures();												// Create Textures on the Device
	HRESULT CreatePaintTextures();
	HRESULT CreateFrame();													// Set Free Vertex Format as Frame 

	HRESULT SetParametersOnEffect();										// Set Parameters For the Effect file


	inline HRESULT TakeTarget();											// Preserve the  pointes for the Back Buffer 
	inline HRESULT ReturnTarget();											// Get the pointers for the Back Buffer Back

	HRESULT SetTmpScalars();	// Temporally Scalars Setting

public:
	MyFlow2DRGB();
	MyFlow2DRGB(const int Nx, const int Ny, BoundaryCondition bc, LPDIRECT3DDEVICE9 pd3dDeivce);
	~MyFlow2DRGB();
	
	// For initialization after MyFlow2DRGB() constructor
	void Init(const int Nx, const int Ny, BoundaryCondition bc, LPDIRECT3DDEVICE9 pd3dDeivce);


	// Interfaces for Framework (DXUT)
	void OnReset();
	void OnLost();
	void OnDestroy() {;}

	void ReCreatePaintTextures();


	void TakeTargetFromBackBuffer() {TakeTarget();}		// Preserve the pointers for the Back Buffer
	void ReturnTargetToBackBuffer()	{ReturnTarget();}	// Reset the pointers for the Back Buffer

	// m_pd3dDevice->BeginScene(),EndScene() included 
	HRESULT LoadTextureOnScalar(LPCTSTR pfilename);
	

	HRESULT MakeTextureOnScalar(const int index);
	HRESULT MakeTextureOnVector();

	// m_pd3dDevice->BeginScene(), EndScene() excluded, 
	// so the following functions must be inserted between  BeginScene() and EndScene(). 
	HRESULT DrawMixedScalar();
	HRESULT	DrawScalar(const int index);
	HRESULT DrawVector();

	HRESULT Display();
	
	// For Checking Basic routine
	HRESULT AdvectScalar();
	HRESULT AdvectVector();

	HRESULT AddVortex();
	HRESULT DiffuseVector();
	HRESULT MakeClosed();   // Make a vector field a closed form ( divergent free ) 

	
	HRESULT SaveVector();
	HRESULT SaveScalar(const int index);

	HRESULT	AddScalar(POINT StartPoint,const int index);
	HRESULT AddVector(POINT Point, D3DXVECTOR2 VectorVal);

};



#endif