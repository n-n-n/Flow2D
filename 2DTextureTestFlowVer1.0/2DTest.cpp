//--------------------------------------------------------------------------------------
// File: 2DTest.cpp
//
// Basic starting point for new Direct3D samples
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "2DTest.h"


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
	
    //
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

    DXUTCreateWindow( L"2DTest" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, g_iWidth, g_iHeight, IsDeviceAcceptable, ModifyDeviceSettings );
	
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
    g_HUD.Init(		&g_DialogResourceManager );
	g_MyUI.Init(	&g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 30, iY, 100, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 30, iY += 24, 100, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 30, iY += 24, 100, 22, VK_F2 );

	g_MyUI.SetCallback( OnGUIEvent ); iY = 10;
	g_MyUI.AddButton( MYUI_SWITCH1, L"Change Display", 30,iY,100,22);
	g_MyUI.AddButton( MYUI_SWITCH2, L"Start/Stop", 30,iY+=24,100,22);
	g_MyUI.AddButton( MYUI_SWITCH3,	L"Check/Hide"	, 30, iY += 24, 100,22);
	g_MyUI.AddButton( MYUI_SWITCH4,	L"Save"	, 30, iY += 24, 100,22);

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
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
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
    V_RETURN( D3DXCreateFont( pd3dDevice, 12, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,   L"Arial", &g_pFont ) );
 
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

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-150, 0 );
    g_HUD.SetSize( 100, 100 );
	
	g_iWidth	= pBackBufferSurfaceDesc->Width;
	g_iHeight	= pBackBufferSurfaceDesc->Height;

	g_MyUI.SetLocation(g_iWidth-150, g_iHeight -120);
	g_MyUI.SetSize(100,100);
		

	g_iSizeX = 512;
	g_iSizeY = 512;
	

	BoundaryCondition this_BC = BC_PERIODIC;
	g_Flow.Init(g_iSizeX,g_iSizeY,this_BC,pd3dDevice);
    
	g_Flow.OnReset();


	g_Flow.LoadTextureOnScalar(L"./Waterlilies.jpg");
	//g_Flow.LoadTextureOnScalar(L"./random.bmp",0);
	//g_Flow.LoadTextureOnScalar(L"./SunSet.jpg",0);		
	
//	g_Flow.MakeTextureOnScalar(0);
//	g_Flow.MakeTextureOnScalar(1);
//	g_Flow.MakeTextureOnScalar(2);

	g_Flow.MakeTextureOnVector();


	int UnitWidth = g_iSizeX/32;
	for ( int i = 0 ;i < 32; i++ ) {
			g_pVectorInd[i].x = g_iSizeY/2;
			g_pVectorInd[i].y = i * UnitWidth ;
	}

	g_Flow.ReCreatePaintTextures();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// MOVE
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	
}

//--------------------------------------------------------------------------------------
// RENDER
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    
    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() ){
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) ) {
		
		if (g_bDrawScalar == true ){
			if(abs(g_Color)==0||  g_Switch1%6==1) {				g_Flow.AddScalar(g_DownPoint, 0);}
			else if (abs(g_Color)==1 ||  g_Switch1%6==2 ) {		g_Flow.AddScalar(g_DownPoint, 1);}
			else if (abs(g_Color) ==2 ||  g_Switch1%6==3 ) {		g_Flow.AddScalar(g_DownPoint, 2);}
			
			g_bDrawScalar = false;
		}

	
		if (g_bDrawVector == true ) {
			g_Flow.AddVector(g_DownPoint, g_VectorVal);
			g_bDrawVector = false;
		}
		

		if ( g_Switch2%2 ==1 ) {
			g_Time++; 
			
			V(g_Flow.AdvectScalar());
			V(g_Flow.AdvectVector());
			V(g_Flow.DiffuseVector());	
			V(g_Flow.AddVortex());
			V(g_Flow.MakeClosed());
		
		}

		switch (g_Switch1%6){	
			case 0:	
				V(g_Flow.DrawMixedScalar());
				break;
			case 1:
				V(g_Flow.DrawScalar(0));
				break;
			case 2:
				V(g_Flow.DrawScalar(1));
				break;
			case 3:
				V(g_Flow.DrawScalar(2));
				break;
			case 4:
				V(g_Flow.DrawVector());
				break;
			case 5:
				V(g_Flow.Display());
				break;
		}
		 

		 
		RenderText();
		 V( g_HUD.OnRender( fElapsedTime ) );
		 V( g_MyUI.OnRender(fElapsedTime ) );
     
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
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 9 );
	CDXUTTextHelper	txtInfo(g_pFont, g_pTextSprite, 12);
	

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.f, 1.f, 1.0f ) );
	txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	txtHelper.DrawFormattedTextLine(L"Test Drawing, FPS: %3.2f ", DXUTGetFPS());

	txtHelper.DrawFormattedTextLine(L"---Parameters----\n");
	txtHelper.DrawFormattedTextLine(L"Time Step; %d", g_Time);
	
	txtHelper.End();

	//
	txtInfo.Begin();
	txtInfo.SetInsertionPos(g_iWidth -120, g_iHeight -250);
	txtInfo.SetForegroundColor( D3DXCOLOR( 0.5f, 0.5f, 0.f, 1.0f ) );


	txtInfo.DrawFormattedTextLine(L"Time Delta; %1.1e", g_DeltaTime);
	txtInfo.DrawFormattedTextLine(L"( %1.1e, %1.1e )", g_VectorVal.x, g_VectorVal.y);
	txtInfo.DrawFormattedTextLine(L"Norm =%1.1e", Norm(g_VectorVal));

	txtInfo.DrawFormattedTextLine(L"\n");
	txtInfo.DrawFormattedTextLine(L"Begin ( %d, %d )", g_DownPoint.x, g_DownPoint.y);
	txtInfo.DrawFormattedTextLine(L"End ( %d, %d )",   g_UpPoint.x, g_UpPoint.y);
	txtInfo.DrawFormattedTextLine(L"Current (%d, %d)", g_CurrentPoint.x, g_CurrentPoint.y);

	WCHAR *switch2[2] ={L"Stop", L"Start"};
	WCHAR *colors[3] ={L"Red",L"Green",L"Blue"};
	WCHAR *switch1[6] ={L"Full", L"Red",L"Green", L"Blue",L"Vector",L"Multi"};

	
	txtInfo.DrawFormattedTextLine(colors[abs(g_Color)]);
	txtInfo.DrawFormattedTextLine(switch2[g_Switch2%2]); 
	txtInfo.DrawFormattedTextLine(switch1[g_Switch1%6]); 
	
	txtInfo.End();

	

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

    if( g_SettingsDlg.IsActive() ){
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
  
	// Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_MyUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
  
    // Pass all remaining windows messages to camera so it can respond to user input
 

	if ( uMsg == WM_MOUSEWHEEL ) {
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		g_Color += int(zDelta/120);
		g_Color %=3;
	}


	 if(   uMsg == WM_LBUTTONDOWN ){
		GetCursorPos(&g_DownPoint);
		ScreenToClient(hWnd, &g_DownPoint);
		g_bDrawScalar = true;
	 }
	 
	
	 if ( uMsg == WM_RBUTTONDOWN ) {
		GetCursorPos(&g_DownPoint);
		ScreenToClient(hWnd, &g_DownPoint);
		g_DownTime = (float) DXUTGetTime();
		g_bDrawVector = false;
		g_bRDown =true;
	 }

	 if ( uMsg == WM_RBUTTONUP ) {
		GetCursorPos(&g_UpPoint);
		ScreenToClient(hWnd, &g_UpPoint);
		g_UpTime  = (float) DXUTGetTime();
		g_DeltaTime = g_UpTime - g_DownTime;
	
		if (g_bRDown && g_DeltaTime > 0) {
		
			g_bDrawVector = true;
			g_VectorVal.x = float(g_UpPoint.x -g_DownPoint.x)/ g_DeltaTime;
			g_VectorVal.y = float(g_UpPoint.y -g_DownPoint.y)/ g_DeltaTime;
		
		} else g_bDrawVector = false;
		
		g_bRDown = false;
	 } 

	GetCursorPos(&g_CurrentPoint);	
	ScreenToClient(hWnd,&g_CurrentPoint);
	if ( !(0 <= g_DownPoint.x && g_DownPoint.x <= g_iSizeX && 0 <= g_DownPoint.y && g_DownPoint.y < g_iSizeY)) {

		g_bDrawScalar = false;
		g_bDrawVector = false;
	}

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
	
		case MYUI_SWITCH1:	
				g_Switch1++ ;
				break;
		
		case MYUI_SWITCH2:
				g_Switch2++ ;
				break;
		case MYUI_SWITCH3:
				g_Switch3++;
				if ( g_Switch3%2 == 1 ) {
					g_Flow.SaveVector();
					g_Flow.SaveScalar(0);
		
				}
				break;
		case MYUI_SWITCH4:
				break;
    }
}

//--------------------------------------------------------------------------------------
//	LOST
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnLostDevice();
    g_SettingsDlg.OnLostDevice();
    if( g_pFont )  g_pFont->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );
 
	g_Flow.OnLost();
}

//--------------------------------------------------------------------------------------
// DESTORY
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnDestroyDevice();
    g_SettingsDlg.OnDestroyDevice();
  
   SAFE_RELEASE( g_pFont );
	
	g_Flow.OnDestroy();
	
}



