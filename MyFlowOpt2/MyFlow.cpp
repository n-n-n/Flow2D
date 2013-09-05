//--------------------------------------------------------------------------------------
// File: MyFlow.cpp
//
// Basic starting point for new Direct3D samples
//
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "MyFlow.h"
#include "CheckSSE.h"

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // 
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    InitApp();

    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTCreateWindow( L"MyFlow" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, X_SIZE, Y_SIZE, IsDeviceAcceptable, ModifyDeviceSettings );

    // 
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10; 
	g_SampleUI.AddButton( IDC_START,L"START/STOP",35,iY,100,20);

}


//--------------------------------------------------------------------------------------
//  IS DEVICE ACCEPTABLE
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
//  MODIFY DEVICE SETTING
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF ) {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF ) DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}

//--------------------------------------------------------------------------------------
//	CREATE
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

	// UI Creation
    V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );
    
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,  L"Arial", &g_pFont ) );

    // 
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"MyFlow.fx" ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags, NULL, &g_pEffect, NULL ) );

	//-- SSE Test Sector --//
	int div_count = 100;
	g_fPCP = Calc_PI_SSE_rcpps( div_count );
	g_fDIV = Calc_PI_SSE_divps( div_count );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// RESET
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,  const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnResetDevice() );
    V_RETURN( g_SettingsDlg.OnResetDevice() );

    if( g_pFont )  V_RETURN( g_pFont->OnResetDevice() );
    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    if( g_pEffect )  V_RETURN( g_pEffect->OnResetDevice() );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 100, 100 );
	g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-130, pBackBufferSurfaceDesc->Height-35 );
	g_SampleUI.SetSize( 100, 50 );

	
	V_RETURN( D3DXCreateTextureFromFileEx(pd3dDevice, L"random.bmp",g_iTextureWidth ,g_iTextureHeight , 0,
					D3DUSAGE_DYNAMIC, D3DFMT_R32F, D3DPOOL_DEFAULT,D3DX_DEFAULT, D3DX_FILTER_LINEAR, 0, NULL, NULL,&g_pTexture));
	
	//------- Load and Modify Scalar Density------	
	//g_dens = new float[g_iTextureWidth * g_iTextureHeight];
	g_dens.AllocateHeap(g_iTextureWidth * g_iTextureHeight, 16);

	for (int i = 0; i < g_iTextureWidth * g_iTextureHeight ; i++) g_dens.aligned[i] =0.0f;

	ReadTextureDensity8(g_dens.aligned);
	InverseDensity(g_dens.aligned);
	UpdateTexture8(g_dens.aligned);
	
	g_v[1].x =  (float)pBackBufferSurfaceDesc->Width;
	g_v[2].x =  (float)pBackBufferSurfaceDesc->Width; g_v[2].y = (float)pBackBufferSurfaceDesc->Height;
	g_v[3].y = (float)pBackBufferSurfaceDesc->Height;
	pd3dDevice->SetFVF(D3DFVF_RHW_TEX1_VERTEX);
	
	g_pF2D = new Flow2D;
	g_pF2D->Init(g_iNx,g_iNy,1);
	g_pF2D->SetParameters(1.0, 20, 0.0, 1.0, 0.0); // (time step, iteration, residual error, viscosity, confinement(vortex );
	
	g_iStart = 0;
	g_fTime = 0.0f;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// MOVE
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	if ( g_iStart%2 == 1){
		g_pF2D->NextStep();
		UpdateTexture8(g_pF2D->GetScalarField(0));
		g_fTime += 1.0f;
	//	g_iStart = 0;
	}	
}


//--------------------------------------------------------------------------------------
// RENDER
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
  
	// If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() ){

        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Clear the render target and the zbuffer 
   V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) ) {

		V( g_pEffect->SetTexture("g_pTexture", g_pTexture));
		
		UINT cPasses, iPass;

		V(g_pEffect->Begin(&cPasses, 0));

		for ( iPass = 0; iPass < cPasses; iPass++) {
			V(g_pEffect->BeginPass(iPass));
		
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_v, sizeof(RHW_TEX1_VERTEX));
			V(g_pEffect->EndPass());
		}

		V(g_pEffect->End());

		RenderText();
     // V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );
     
        V( pd3dDevice->EndScene() );
    }
}

//--------------------------------------------------------------------------------------
// Text
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 6 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 0.9f, 0.9f, 0.0f, 1.0f ) );
  //  txtHelper.DrawTextLine( DXUTGetFrameStats() );
  //  txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	txtHelper.DrawFormattedTextLine(L"SIZE: %dx%d, FPS: %3.2f, STEP: %f ",2* g_iNx,2* g_iNy, DXUTGetFPS(), g_fTime); 

	int iY = 5+10;
	txtHelper.SetInsertionPos( 5, iY);
	txtHelper.DrawFormattedTextLine(L"pi:%1.10f, %1.10f", g_fPCP, g_fDIV);
	
	txtHelper.End();
}


//--------------------------------------------------------------------------------------
//	MESSAGE PROCEDURE
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

   
    return 0;
}



//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
		case IDC_START:			  
			{
				if ( g_iStart == 0 ) {
					g_iStart =1;
					float addingForce = 5.0f;
					int forceBold = 5;
					g_pF2D->SetScalarField(0,g_dens.aligned, g_iNx, g_iNy);
					g_pF2D->AddVelocityOnLine(g_iNx/2, g_iNy/2,  10,g_iNy/2 , addingForce, forceBold);
					break;
				} else {
					g_iStart++;	
					break;
				}
			}
    }
}


//--------------------------------------------------------------------------------------
//	LOST
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnLostDevice();
    g_SettingsDlg.OnLostDevice();
    if( g_pFont )    g_pFont->OnLostDevice();
    if( g_pEffect )  g_pEffect->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );

	SAFE_RELEASE( g_pTexture );
	SAFE_RELEASE( g_pSurface );
	SAFE_DELETE(g_pF2D);

	
}

//--------------------------------------------------------------------------------------
// DESTORY
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnDestroyDevice();
    g_SettingsDlg.OnDestroyDevice();
    SAFE_RELEASE( g_pEffect );
    SAFE_RELEASE( g_pFont );

}


