//--------------------------------------------------------------------------------------
// File: MyFlow.cpp
//
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"
#include "./MyFlow/MyFlow2DRGB.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*					g_pFont			= NULL;         // Font for drawing text
ID3DXSprite*				g_pTextSprite	= NULL;			// Sprite for batching draw text calls

MyFlow2DRGB					g_Flow;							// Flow Object


bool						g_bShowHelp		= true;			// If true, it renders the UI control text
CDXUTDialogResourceManager	g_DialogResourceManager;		// manager for shared resources of dialogs
CD3DSettingsDlg				g_SettingsDlg;					// Device settings dialog
CDXUTDialog					g_HUD;							// dialog for standard controls
CDXUTDialog					g_MyUI;							// 

int							g_Switch1		= 0;
int							g_OldSwitch1	= 0;

int							g_Switch2		= 0;
int							g_OldSwitch2	= 0;

int							g_Switch3		=0;


int g_iWidth = 640;
int g_iHeight = 520;

int g_iSizeX;
int g_iSizeY;


long long g_Time  = 0;


D3DXVECTOR2 g_pVectorVal[32];
iVector2	g_pVectorInd[32];


POINT						g_CurrentPoint;
POINT						g_DownPoint;
POINT						g_UpPoint;
float						g_DownTime;
float						g_UpTime;
float						g_DeltaTime;

D3DXVECTOR2					g_VectorVal;


bool						g_bDrawScalar = false;
bool						g_bDrawVector = false;
bool						g_bRDown	  = false;

int							g_Color =0;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
int g_iStart =  0;
enum {IDC_TOGGLEFULLSCREEN  = 0, IDC_TOGGLEREF, IDC_CHANGEDEVICE};
enum {MYUI_SWITCH1 =100, MYUI_SWITCH2, MYUI_SWITCH3, MYUI_SWITCH4};

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


