//--------------------------------------------------------------------------------------
// File: MyFlow.cpp
//
// Basic starting point for new Direct3D samples
//
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "flow1cASM/flow2d.h"
#include "AlignedHeapFloat.h"


//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

#define ASIZE 16

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*					g_pFont			= NULL;         // Font for drawing text
ID3DXSprite*				g_pTextSprite	= NULL;			// Sprite for batching draw text calls
ID3DXEffect*				g_pEffect		= NULL;			// D3DX effect interface


bool						g_bShowHelp		= true;			// If true, it renders the UI control text
CDXUTDialogResourceManager	g_DialogResourceManager;		// manager for shared resources of dialogs
CD3DSettingsDlg				g_SettingsDlg;					// Device settings dialog
CDXUTDialog					g_HUD;							// dialog for standard controls
CDXUTDialog					g_SampleUI;						// dialog for sample specific controls

float						g_fTime =0.0f;

// SSE Test Variables
float g_fPCP; 
float g_fDIV; 

struct RHW_TEX1_VERTEX {float x, y, z;	float rhw; float u0, v0;};
#define D3DFVF_RHW_TEX1_VERTEX 	(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))
RHW_TEX1_VERTEX g_v[4] =
	{
	{ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{ 0.0f,	0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	{ 0.0f,	0.0f, 0.0f, 1.0f, 0.0f, 1.0f}
	};

IDirect3DSurface9*		g_pSurface		=	NULL;

const int X_SIZE = (256 * 2);
const int Y_SIZE = (256 * 2);

//---------- For Drawing -----------
LPDIRECT3DTEXTURE9		g_pTexture		= NULL;
int						g_iTextureWidth = X_SIZE;
int						g_iTextureHeight= Y_SIZE;

//------- For flow calculating -----
Flow2D* g_pF2D	= NULL;
int		g_iNx = X_SIZE;
int		g_iNy = Y_SIZE;


//float* g_dens = NULL;
AlignedHeapFloat g_dens;
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
int g_iStart =  0;

enum {IDC_TOGGLEFULLSCREEN  = 0, IDC_TOGGLEREF, IDC_CHANGEDEVICE};
enum {IDC_START = 100};

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

void    InitApp();
void    RenderText();

void	InverseDensity(float* dens);

HRESULT TextUpdateTexture();

HRESULT ReadTextureDensity( float* dens);
HRESULT UpdateTexture(const float* dens);

HRESULT ReadSurfaceDensity32(float* dens);
HRESULT UpdateSurface32(const float* dens);


//=========================================================================================
#define B8	255
#define B32 4294967295
//=========================================================================================

void InverseDensity(float* dens)
{
	const unsigned int texSize = g_iNx * g_iNy;
	for ( unsigned int i = 0; i <  texSize; i++) dens[i] = 1.0f - dens[i];
	return;
}

//===================================================
//  Texture
//====================================================
HRESULT ReadTextureDensity8(float* dens)
{
	HRESULT hr;

	if ( dens == NULL ) return E_FAIL;
	const int texSize = g_iNx * g_iNy;

	D3DLOCKED_RECT Rect;
	RECT LockRect;

	LockRect.top = 0;
	LockRect.left = 0;
	LockRect.right = g_iNx;
	LockRect.bottom = g_iNy;

	hr = g_pTexture->LockRect(0,&Rect, &LockRect, D3DLOCK_NO_DIRTY_UPDATE);

	if ( SUCCEEDED(hr)) {

		float* pTex = (float*)Rect.pBits;

		memcpy(dens, pTex, sizeof(float) * texSize);

		g_pTexture->UnlockRect(0);

	} else {
		return hr;
	}

	return S_OK;
}

HRESULT UpdateTexture8(const float *dens)
{
	HRESULT hr;
	
	D3DLOCKED_RECT Rect;

	RECT ThisRect;
	ThisRect.left = 0; ThisRect.top =0;
	ThisRect.right = g_iNx;
	ThisRect.bottom = g_iNy;
	const unsigned int texSize = g_iNx * g_iNy;

	//hr = g_pTexture->LockRect(0,&Rect,NULL,D3DLOCK_DISCARD);
	 hr = g_pTexture->LockRect(0, &Rect, &ThisRect, D3DLOCK_NO_DIRTY_UPDATE);
	if (SUCCEEDED(hr)) {

		float *pTex = (float *)Rect.pBits;

		memcpy(pTex, dens, sizeof(float)*texSize);
	/*
		for (unsigned int i = 0; i < texSize; i++) {

			if (dens[i] > 1.0) 	pTex[i] = B8;
			else if (dens[i] < 0.0) pTex[i] = 0;
			else pTex[i] = (BYTE)(B8 * dens[i]);
		}
	*/
		g_pTexture->UnlockRect(0);
		
	} else {
		return hr;
	}

	return S_OK;
}



