#include "MyFlow2DRGB.h"


//=========================================================================================
// For Initialization
//=========================================================================================
void MyFlow2DRGB::InitLattice(const int Nx, const int Ny, BoundaryCondition bc)
{
	m_Nx = Nx;
	m_Ny = Ny;

	m_N = m_Nx * m_Ny;

	m_BC = bc;
}
void MyFlow2DRGB::InitTextures()
{

	int i;

	delete [] m_ppScalarTexture;	delete [] m_ppScalarSurface;

	delete [] m_ppVectorTexture;	delete [] m_ppVectorSurface;

	/* NEW VERSION */
	try {
		m_ppScalarTexture = new LPDIRECT3DTEXTURE9[g_NScalarTex];
		m_ppScalarSurface = new LPDIRECT3DSURFACE9[g_NScalarTex];

		m_ppVectorTexture = new LPDIRECT3DTEXTURE9[g_NVectorTex];
		m_ppVectorSurface = new LPDIRECT3DSURFACE9[g_NVectorTex];

		m_ppPaintVectorTexture = new LPDIRECT3DTEXTURE9[g_NVector];
		m_ppPaintScalarTexture = new LPDIRECT3DTEXTURE9[g_NScalar];


	} catch (bad_alloc xa) {
		cerr << "Allocation Failed (bad_alloc)" << endl;
		exit(0);
	}
		
	// initialized via NULL = 0
	for (i= 0; i < g_NScalar; i++ ){
		m_ppScalarTexture[ 2 * i ] = 0;
		m_ppScalarTexture[ 2 * i + 1] = 0;

		m_ppScalarSurface[ 2 * i] = 0;
		m_ppScalarSurface[ 2 * i + 1] = 0;

		m_ppPaintScalarTexture[i] = 0;
	}

	for (i= 0; i < g_NVector; i++ ) {
		m_ppVectorTexture[ 2 * i ] = 0;
		m_ppVectorTexture[ 2 * i + 1] = 0;
		
		m_ppVectorSurface[ 2 * i] = 0;
		m_ppVectorSurface[ 2 * i + 1] =0;

		m_ppPaintVectorTexture[i] = 0;
	}

	
	m_pZBuffer = 0;
}

void MyFlow2DRGB::InitIndex()
{
	delete [] m_pScalarId;  
	delete [] m_pVectorId;
	
	try { 
		m_pScalarId = new int[g_NScalar];
		m_pVectorId = new int[g_NVector];
	} catch ( bad_alloc xa ) {
		cerr << "Allocation Failed (bad_alloc)" << endl;
		exit(0);
	}

	int i;
	for ( i= 0; i < g_NScalar; i++ ) m_pScalarId[i] = 0;
	for ( i= 0; i < g_NVector; i++ ) m_pVectorId[i] = 0;

}
void MyFlow2DRGB::InitFrame()
{
	for ( int i = 0; i< 4; i++ ){
	
		m_Frame[i].x = 0.0f;
		m_Frame[i].y = 0.0f;
		m_Frame[i].z = 0.0f;
		m_Frame[i].rhw = 1.0f;

	}

	m_Frame[0].u0 = 0.0f; m_Frame[0].v0 = 0.0f;
	m_Frame[1].u0 = 1.0f; m_Frame[1].v0 = 0.0f;
	m_Frame[2].u0 = 1.0f; m_Frame[2].v0 = 1.0f;
	m_Frame[3].u0 = 0.0f; m_Frame[3].v0 = 1.0f;
	
	// Set Texture Coordinate and Vertex declaration
	m_Frame[1].x = float(m_Nx);
	m_Frame[2].x = float(m_Nx); m_Frame[2].y = float(m_Ny);
	m_Frame[3].y = float(m_Ny);
}

HRESULT MyFlow2DRGB::CreateTextures()
{
	HRESULT hr;
	int i;
	for ( i = 0; i < g_NScalarTex; i++) {
		V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_RENDERTARGET,ScalarFormat,D3DPOOL_DEFAULT,&(m_ppScalarTexture[i]), NULL));	
		V( m_ppScalarTexture[i]->GetSurfaceLevel(0,&(m_ppScalarSurface[i])));
	}
	
	for ( i = 0; i < g_NVectorTex; i++) {
		V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_RENDERTARGET,VectorFormat,D3DPOOL_DEFAULT,&(m_ppVectorTexture[i]), NULL));	
		V( m_ppVectorTexture[i]->GetSurfaceLevel(0,&(m_ppVectorSurface[i])));
	}

	V(m_pd3dDevice->CreateDepthStencilSurface(m_Nx, m_Ny,ZBufferFormat, D3DMULTISAMPLE_NONE, 0, true, &m_pZBuffer, NULL));

	return hr;

}

HRESULT MyFlow2DRGB::CreatePaintTextures()
{
	HRESULT hr= S_OK;
	int i;

	for ( i = 0; i < g_NScalar ; i++ )
		V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_DYNAMIC,ScalarFormat, D3DPOOL_DEFAULT, &(m_ppPaintScalarTexture[i]), NULL));
	
	
	for ( i = 0; i < g_NVector; i ++ )
		V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_DYNAMIC, VectorFormat, D3DPOOL_DEFAULT, &(m_ppPaintVectorTexture[i]), NULL));

	return hr;

}

HRESULT MyFlow2DRGB::CreateEffect()
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
    
	#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

	HRESULT hr;
	// Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, pEffectFile));

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
     
	V_RETURN(D3DXCreateEffectFromFile( m_pd3dDevice, str, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, NULL ));
	
	return hr;
}


HRESULT MyFlow2DRGB::SetParametersOnEffect()
{
	HRESULT hr;

	float du  =1.0f/(float(m_Nx));
	float dv  =1.0f/(float(m_Ny));
	V( m_pEffect->SetFloat("g_du",du));
	V( m_pEffect->SetFloat("g_dv", dv));
	V( m_pEffect->SetFloat("g_SizeX", (float)m_Nx));
	V( m_pEffect->SetFloat("g_SizeY", (float)m_Ny));
	V( m_pEffect->SetFloat("g_du2", du*du));
	V( m_pEffect->SetFloat("g_dv2", dv*dv));

	float dt =du;
	V( m_pEffect->SetFloat("g_dt", dt));
	
	float da = 0.50f/( float(m_Nx) * float(m_Nx) + float(m_Ny) *float(m_Ny));
	//da *=0.1f;
	V( m_pEffect->SetFloat("g_da", da));

	return hr;
}

HRESULT MyFlow2DRGB::CreateFrame()
{
	HRESULT hr;
	V(m_pd3dDevice->SetFVF(D3DFVF_RHW_TEX1_VERTEX));
	return hr;
}		


inline HRESULT MyFlow2DRGB::TakeTarget()
{
	HRESULT hr;
	V(m_pd3dDevice->GetRenderTarget(0, &m_pOldBackBuffer));
	V(m_pd3dDevice->GetDepthStencilSurface(&m_pOldZBuffer));
	V(m_pd3dDevice->SetDepthStencilSurface(m_pZBuffer));

	return hr;
}

inline HRESULT MyFlow2DRGB::ReturnTarget()
{	
	HRESULT hr;
	
	V(m_pd3dDevice->SetRenderTarget(0, m_pOldBackBuffer));
	V(m_pd3dDevice->SetDepthStencilSurface(m_pOldZBuffer));
	V(m_pOldBackBuffer->Release());
	V(m_pOldZBuffer->Release());
	
	return hr;

}

HRESULT MyFlow2DRGB::SetTextureCondition(DWORD i, BoundaryCondition bc, FilterSetting filter)
{
	HRESULT	hr; 
	
	D3DTEXTUREADDRESS UCondition = D3DTADDRESS_WRAP;
	D3DTEXTUREADDRESS VCondition = D3DTADDRESS_WRAP;
	D3DTEXTUREFILTERTYPE Filter = D3DTEXF_POINT;

	Filter = D3DTEXF_POINT;

	switch (bc) { 
		case BC_PERIODIC:
			break;
		case BC_RIGID:
			UCondition = D3DTADDRESS_CLAMP;
			VCondition = D3DTADDRESS_CLAMP;
			break;
	}


	switch (filter) {
		case FILTER_POINT:
			break;
		case FILTER_LINEAR:
			Filter = D3DTEXF_LINEAR;
			break;
	}


	V( m_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, UCondition));
	V( m_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, VCondition));
	V( m_pd3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, Filter));
	V( m_pd3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, Filter));
	V( m_pd3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, Filter));

	return hr;
}


MyFlow2DRGB::MyFlow2DRGB()
{
	
	// TEXTURES
	m_ppScalarTexture = 0;	m_ppVectorTexture = 0;
	m_ppScalarSurface = 0;	m_ppVectorSurface = 0;

}

MyFlow2DRGB::MyFlow2DRGB(const int Nx, const int Ny, BoundaryCondition bc,LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_ppScalarTexture = 0; m_ppVectorTexture = 0;
	m_ppScalarSurface = 0; m_ppVectorSurface = 0;

	Init( Nx,  Ny, bc,pd3dDevice);
}

MyFlow2DRGB::~MyFlow2DRGB()
{
	delete [] m_pScalarId;
	delete [] m_pVectorId;

	// Release the array of D3D pointers 
	delete [] m_ppScalarTexture;
	delete [] m_ppScalarSurface;

	delete [] m_ppVectorTexture;
	delete [] m_ppVectorSurface;

	delete [] m_ppPaintVectorTexture;
	delete [] m_ppPaintScalarTexture;

}

void MyFlow2DRGB::Init(const int Nx, const int Ny, BoundaryCondition bc,LPDIRECT3DDEVICE9 pd3dDevice)
{

	InitDevice( pd3dDevice);
	InitLattice(Nx,Ny,bc);
	InitTextures();
	InitEffect();
	InitFrame();
	InitIndex();

	CreateFrame();
	CreateTextures();
	CreatePaintTextures();
	CreateEffect();

	SetParametersOnEffect();

	SetTmpScalars();

	m_ItrJacobiMethod = 50;
}

void MyFlow2DRGB::OnReset()
{
	if( m_pEffect )   m_pEffect->OnResetDevice() ;
}
void MyFlow2DRGB::OnLost()
{
	
	// TEXUTRES & SURFACE 
	int i;
	for (i = 0; i < g_NScalar; i++ ) {
		SAFE_RELEASE(m_ppScalarSurface[2 * i]);
		SAFE_RELEASE(m_ppScalarSurface[2 * i+1]);

		SAFE_RELEASE(m_ppScalarTexture[2 * i]);	
		SAFE_RELEASE(m_ppScalarTexture[2 * i+1]);	

		SAFE_RELEASE(m_ppPaintScalarTexture[i]);

	}

	for (i = 0; i < g_NVector; i++ )	{

		SAFE_RELEASE(m_ppVectorSurface[2 * i]);
		SAFE_RELEASE(m_ppVectorSurface[2 * i+1]);

		SAFE_RELEASE(m_ppVectorTexture[2 * i]);
		SAFE_RELEASE(m_ppVectorTexture[2 * i+1]);

		SAFE_RELEASE(m_ppPaintVectorTexture[i]);
	}

	// NOTICE: THIS OBJECT ENTRYED TEMPOLARY!
	SAFE_RELEASE(m_pTmpScalarSurface);
	SAFE_RELEASE(m_pTmpScalarTexture);

	SAFE_RELEASE(m_pZBuffer);
	SAFE_RELEASE(m_pEffect);

	

}

void MyFlow2DRGB::ReCreatePaintTextures()
{
	for (int i = 0; i < g_NScalar;i++)
		SAFE_RELEASE(m_ppPaintScalarTexture[i]);

	for (int i = 0; i < g_NVector;i++)
		SAFE_RELEASE(m_ppPaintVectorTexture[i]);

	CreatePaintTextures();


}


HRESULT MyFlow2DRGB::SetTmpScalars()
{
	HRESULT hr;
	TakeTarget();

	V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_RENDERTARGET,ScalarFormat,D3DPOOL_DEFAULT,&(m_pTmpScalarTexture), NULL));	
	V( m_pTmpScalarTexture->GetSurfaceLevel(0, &(m_pTmpScalarSurface)));
	
	V(m_pd3dDevice->SetRenderTarget(0,m_pTmpScalarSurface));
	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 255, 0,0), 1.0f, 0) );

	ReturnTarget();

	return hr;
}



HRESULT MyFlow2DRGB::LoadTextureOnScalar(LPCTSTR pfilename)
{

	HRESULT hr = S_OK;

	// Load Texture via DYNAMIC format, Use #0 Texture
	SAFE_RELEASE(m_ppPaintScalarTexture[0]);
	D3DFORMAT ThisFormat = D3DFMT_A8R8G8B8;
	V_RETURN(D3DXCreateTextureFromFileEx(m_pd3dDevice, pfilename, m_Nx ,m_Ny , D3DX_DEFAULT,
					D3DUSAGE_DYNAMIC,ThisFormat , D3DPOOL_DEFAULT,D3DX_DEFAULT, D3DX_FILTER_LINEAR, 0, NULL, NULL,&m_ppPaintScalarTexture[0]));

	TakeTarget();	
	for (int index = 0; index < 3 ; index++)  {
		m_pScalarId[index] = (m_pScalarId[index]+1)%2;
		m_pd3dDevice->SetRenderTarget(index,m_ppScalarSurface[m_pScalarId[index] + 2 * index]);
	}

	// Render On Three Textures
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) ) {

		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );

		//------ RENDERING ON THE TARGET TEXTURE -------
	
		V( m_pEffect->SetTechnique("Dispersion"));
		V( m_pEffect->SetTexture("NormalTexture", m_ppPaintScalarTexture[0]));

		UINT cPasses;
		V(m_pEffect->Begin(&cPasses, 0));
		for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
		
		}
		V(m_pEffect->End());
		
	}
	V( m_pd3dDevice->EndScene() );
	

	V(m_pd3dDevice->SetRenderTarget(1,NULL));
	V(m_pd3dDevice->SetRenderTarget(2,NULL));

	ReturnTarget();
	
	
	return hr;
}


HRESULT MyFlow2DRGB::MakeTextureOnScalar( const int index )
{
	if ( index >= g_NScalar ) return E_FAIL;
	
	m_pScalarId[index] = (m_pScalarId[index]+1)%2;

	HRESULT hr = S_OK;

	// Load Texture via DYNAMIC format 
	
	SAFE_RELEASE(m_ppPaintScalarTexture[index]);
	D3DFORMAT ThisFormat = D3DFMT_R32F;
	V_RETURN(m_pd3dDevice->CreateTexture(m_Nx ,m_Ny ,1,D3DUSAGE_DYNAMIC, ThisFormat, D3DPOOL_DEFAULT,&m_ppPaintScalarTexture[index],NULL));

	D3DLOCKED_RECT	Rect;
	RECT			RectRegion;
	
	RectRegion.top		= 0L;
	RectRegion.left		= 0L;
	RectRegion.bottom	= (long) m_Ny;
	RectRegion.right	= (long) m_Nx;
	
	float*		pTex;

	V(m_ppPaintScalarTexture[index]->LockRect(0,&Rect,&RectRegion, D3DLOCK_NO_DIRTY_UPDATE));

	if ( SUCCEEDED(hr)) {
		pTex = (float *)Rect.pBits;
	
		for( int j = 0; j <= m_N; j++ ) pTex[j] = 0.0f;
/*
		int CenterX = m_Nx/2;
		int CenterY = m_Ny/2;
		int SizeX = m_Nx/8;
		int SizeY = m_Nx/8;

		for ( int i = - SizeX/2; i <SizeX/2 ; i++){
			for ( int j = - SizeY/2; j < SizeY/2 ; j++){
				pTex[ (CenterX+i)+ m_Nx *( j+CenterY)] = 1.0f;
			}
		}
*/		
			V(m_ppPaintScalarTexture[index]->UnlockRect(0));	
		}
	

	TakeTarget();	
	m_pd3dDevice->SetRenderTarget(0,m_ppScalarSurface[m_pScalarId[index] + 2 * index]);
	
	// Render the Effected Texture
  
  if( SUCCEEDED( m_pd3dDevice->BeginScene() ) ) {
		// 
		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );

		//------ RENDERING ON THE TARGET TEXTURE -------
	
		V( m_pEffect->SetTechnique("Normal"));
		V( m_pEffect->SetTexture("NormalTexture", m_ppPaintScalarTexture[index]));

		UINT cPasses;
		V(m_pEffect->Begin(&cPasses, 0));
		for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
		
		}
		V(m_pEffect->End());
	}
	V( m_pd3dDevice->EndScene() );
		
	ReturnTarget();
	
	
	return hr;
}

// Write on a Dynamic Texture  and Render it on a Target Textrure
HRESULT MyFlow2DRGB::MakeTextureOnVector( )
{
	const int index = 0;

	HRESULT hr = S_OK;

	//------------------------------------
	//---- Create a Dynamic Texture ------
	//------------------------------------
	D3DFORMAT ThisFormat = D3DFMT_G32R32F;

	// Release the Old Dynamic Texture Content and Create a New Dynamic Texture
	SAFE_RELEASE(m_ppPaintVectorTexture[index]); 
	V_RETURN(m_pd3dDevice->CreateTexture(m_Nx ,m_Ny ,1,D3DUSAGE_DYNAMIC, ThisFormat, D3DPOOL_DEFAULT,&m_ppPaintVectorTexture[index],NULL));


	D3DLOCKED_RECT	Rect;
	V(m_ppPaintVectorTexture[index]->LockRect(0,&Rect,NULL, D3DLOCK_NO_DIRTY_UPDATE));

	D3DXVECTOR2* pvTex;   // For D3DFMT_G32R32F Format
	
	if ( SUCCEEDED(hr)) {
		pvTex = (D3DXVECTOR2 *)Rect.pBits;
		
		float Unit = 1.0f/float(m_Nx);	// Unitize by length/texcel 
	
		for( int j = 0; j < m_N; j++ ){
			pvTex[j].x =  0.0f* Unit;
			pvTex[j].y =  0.0f * Unit;
		}
	
		/*
		int CenterX = m_Nx/2;
		int CenterY = m_Ny/2;
		int SizeX	= m_Nx/8;
		int SizeY	= m_Ny/8;

	
		for ( int i = - SizeX/2; i < SizeX/2 ; i++){
			for ( int j = - SizeY/2; j < SizeY/2 ; j++){
				pvTex[ (CenterX+i)+ m_Nx *( j+CenterY)].x =2.0f * Unit;
				pvTex[ (CenterX+i)+ m_Nx *( j+CenterY)].y =-1.0f * Unit;
			}
		}
	*/
		V(m_ppPaintVectorTexture[index]->UnlockRect(0));	 // Release pvTex Contents
	}

	//----------------------------------------
	//----- Render On a Target Texture -------
	//----------------------------------------
	TakeTarget();
	m_pVectorId[index] = (m_pVectorId[index]+1)%2; 	
	m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[index] + 2 * index]);
	
  
  if( SUCCEEDED( m_pd3dDevice->BeginScene() ) ) {
	
		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );
	
		V( m_pEffect->SetTechnique("VectorInit"));		
		V( m_pEffect->SetTexture("NormalTexture", m_ppPaintVectorTexture[index])); // Dynamic Texture 

		UINT cPasses;
		V(m_pEffect->Begin(&cPasses, 0));
		for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
		
		}
		V(m_pEffect->End());
		
	}
	V( m_pd3dDevice->EndScene() );
		
	ReturnTarget();
	SAFE_RELEASE(m_ppPaintVectorTexture[index]);
	return hr;
}



HRESULT MyFlow2DRGB::DrawMixedScalar( )
{
	
	//=== RENDERING ON THE FRONT BUFFER ========
	HRESULT hr = S_OK;
	
	V( m_pEffect->SetTexture("ScalarTexture", m_ppScalarTexture[m_pScalarId[0]+2* 0]));
	V( m_pEffect->SetTexture("ScalarTexture2", m_ppScalarTexture[m_pScalarId[1]+2* 1]));
	V( m_pEffect->SetTexture("ScalarTexture3", m_ppScalarTexture[m_pScalarId[2]+2* 2]));
	
	V( m_pEffect->SetTechnique("ScalarMix"));

    V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
	}

	V(m_pEffect->End());

	return hr;

}


HRESULT MyFlow2DRGB::DrawScalar(const int index )
{
	if ( index >= g_NScalar ) return E_FAIL;
	
	//=== RENDERING ON THE FRONT BUFFER
	HRESULT hr;
	
	V( m_pEffect->SetTexture("NormalTexture", m_ppScalarTexture[2* index + m_pScalarId[index]]));
	
	
	V( m_pEffect->SetTechnique("RenderScalar"));

    V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
	}

	V(m_pEffect->End());

	return hr;

}


HRESULT MyFlow2DRGB::DrawVector()
{
	const int index = 0;
	
	
	//=== RENDERING ON THE FRONT BUFFER
	HRESULT hr;

	V( m_pEffect->SetTexture("NormalTexture", m_ppVectorTexture[m_pVectorId[index]+2* index]));
	V( m_pEffect->SetTechnique("RenderVector"));

	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
	}

	V(m_pEffect->End());

	return hr;

}


HRESULT MyFlow2DRGB::Display()
{
	const int vindex = 0;
	//=== RENDERING ON THE FRONT BUFFER
	HRESULT hr;

	V( m_pEffect->SetTexture("NormalTexture", m_pTmpScalarTexture));
	V( m_pEffect->SetTexture("VectorTexture", m_ppVectorTexture[m_pVectorId[vindex]+2* vindex]));

	V( m_pEffect->SetTexture("ScalarTexture", m_ppScalarTexture[m_pScalarId[0]+2* 0]));
	V( m_pEffect->SetTexture("ScalarTexture2", m_ppScalarTexture[m_pScalarId[1]+2* 1]));
	V( m_pEffect->SetTexture("ScalarTexture3", m_ppScalarTexture[m_pScalarId[2]+2* 2]));
	
	V( m_pEffect->SetTechnique("Display"));


	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
	}

	V(m_pEffect->End());


	return hr;

}


HRESULT MyFlow2DRGB::DiffuseVector()
{

	const int index = 0;
	
	HRESULT hr = S_OK;

	V( m_pEffect->SetTechnique("Diffuse2"));
	V( m_pEffect->SetTexture("NormalTexture", m_ppVectorTexture[m_pVectorId[index] + 2 * index]));
	V( m_pd3dDevice->SetTexture(0,m_ppVectorTexture[m_pVectorId[index] +2 * index]));
	
	SetTextureCondition(0,m_BC,FILTER_POINT);

	TakeTarget();

	m_pVectorId[index] = (m_pVectorId[index]+1)%2;
	m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[index] + 2 * index]);

	// 
	//	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );
	//------ RENDERING ON THE TARGET TEXTURE -------
	
	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());
			
	ReturnTarget();
	
	return hr;


}

HRESULT MyFlow2DRGB::MakeClosed()
{
	
	const int index = 0;

	HRESULT hr;
	
	TakeTarget();	
	int i;
	for (i = 0 ; i< 2; i++ ) {
		V( m_pd3dDevice->CreateTexture(m_Nx, m_Ny,1,D3DUSAGE_RENDERTARGET,ScalarFormat,D3DPOOL_DEFAULT,&(m_ppTmpScalarTexture[i]), NULL));	
		V( m_ppTmpScalarTexture[i]->GetSurfaceLevel(0, &(m_ppTmpScalarSurface[i])));
		V( m_pd3dDevice->SetRenderTarget(i, m_ppTmpScalarSurface[i] ));
		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 255, 0,0), 1.0f, 0) );
	}
	

	SetTextureCondition(0, m_BC,FILTER_POINT);
	SetTextureCondition(1, m_BC,FILTER_POINT);

	UINT cPasses;

			
	//------ Caculating div (Vector)  -------			
	V( m_pEffect->SetTexture("VectorTexture",m_ppVectorTexture[m_pVectorId[index] +2 * index]));
	V( m_pEffect->SetTechnique("Divergence"));
	

	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
	}
	V(m_pEffect->End());

	//	V(m_pd3dDevice->SetRenderTarget(0,NULL));  // ERROR!
	V(m_pd3dDevice->SetRenderTarget(1,NULL));
	V( m_pd3dDevice->SetTexture(0,NULL)); // Release Texture Sampler Stage
	V( m_pd3dDevice->SetTexture(1,NULL)); 

	
	//----- Jacobi Method  -------
	V( m_pEffect->SetTechnique("JacobiMethod"));
	
	m_ItrJacobiMethod = 40;
	for (  i = 0 ; i < m_ItrJacobiMethod ; i++ ) {
			
		V( m_pd3dDevice->SetRenderTarget(0,m_ppTmpScalarSurface[(i+1)%2]));
		V( m_pEffect->SetTexture("OldTexture", m_ppTmpScalarTexture[i%2]));
			
		V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );
			
		V(m_pEffect->Begin(&cPasses, 0));
		for (UINT iPass = 0; iPass < cPasses; iPass++) {
			V(m_pEffect->BeginPass(iPass));
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
			V(m_pEffect->EndPass());
		}
		V(m_pEffect->End());
	}

	//-----Make  Diverget Free, Note that we have set Vector Texture already.
		
	m_pVectorId[index] = (m_pVectorId[index]+1)%2;	
	V( m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[index] +2 * index]));
	V( m_pEffect->SetTexture("OldTexture", m_ppTmpScalarTexture[m_ItrJacobiMethod%2]));
	
	V( m_pEffect->SetTechnique("DivergentFree"));
	
	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );

	V( m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End())


	for ( int i= 0; i< 2 ; i++ ){
		SAFE_RELEASE(m_ppTmpScalarTexture[i]);
		SAFE_RELEASE(m_ppTmpScalarSurface[i]);
	}
	

	ReturnTarget();

	return hr;
}

HRESULT MyFlow2DRGB::AdvectScalar()
{
	HRESULT hr;
	
	TakeTarget();

	//Set Technique 
	V( m_pEffect->SetTechnique("AdvectScalar"));
	
	//
	V( m_pd3dDevice->SetTexture(0,m_ppVectorTexture[m_pVectorId[0] ]));
	for ( int sindex = 0; sindex < g_NScalar; sindex++) 
		V( m_pd3dDevice->SetTexture(1+sindex,m_ppScalarTexture[m_pScalarId[sindex] +2 * sindex]));

	V( m_pEffect->SetTexture("VectorTexture",m_ppVectorTexture[m_pVectorId[0]]));
	V( m_pEffect->SetTexture("ScalarTexture",m_ppScalarTexture[m_pScalarId[0] +2 * 0]));
	V( m_pEffect->SetTexture("ScalarTexture2",m_ppScalarTexture[m_pScalarId[1] +2 * 1]));
	V( m_pEffect->SetTexture("ScalarTexture3",m_ppScalarTexture[m_pScalarId[2] +2 * 2]));
	
	for ( int i = 0; i <= 4 ; i++) SetTextureCondition(i, m_BC,FILTER_POINT);


	for ( int sindex = 0; sindex < g_NScalar; sindex++) {
		m_pScalarId[sindex] = (m_pScalarId[sindex]+1)%2;
		V( m_pd3dDevice->SetRenderTarget(sindex ,m_ppScalarSurface[m_pScalarId[sindex] +2 * sindex ]));
	}

	
	
	//V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );
	
	UINT cPasses;	
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());


	V(m_pd3dDevice->SetRenderTarget(1,NULL));
	V(m_pd3dDevice->SetRenderTarget(2,NULL));


	V( m_pd3dDevice->SetTexture(0,NULL)); // Release Texture Sampler Stage
	V( m_pd3dDevice->SetTexture(1,NULL)); 
	V( m_pd3dDevice->SetTexture(2,NULL));
	V( m_pd3dDevice->SetTexture(3,NULL));
	ReturnTarget();
	return hr;
}


HRESULT MyFlow2DRGB::AdvectVector()
{
	HRESULT hr;
	
	TakeTarget();

	//Set Technique 
	V( m_pEffect->SetTechnique("AdvectVector"));
	
	//
	V( m_pd3dDevice->SetTexture(0,m_ppVectorTexture[m_pVectorId[0] ]));
	
	V( m_pEffect->SetTexture("VectorTexture",m_ppVectorTexture[m_pVectorId[0]]));
	
	for ( int i = 0; i <= 4 ; i++) SetTextureCondition(i, m_BC,FILTER_POINT);


	// Target Texture 
	m_pVectorId[0] = (m_pVectorId[0]+1)%2;
	V( m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[0]]));
	
	
	//V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );
	
	UINT cPasses;	
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
			
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());


	V( m_pd3dDevice->SetTexture(0,NULL)); // Release Texture Sampler Stage

	ReturnTarget();
	return hr;
}


HRESULT MyFlow2DRGB::AddVortex()
{
	const int index = 0;
	
	HRESULT hr = S_OK;

	V( m_pEffect->SetTechnique("AddVortex"));
	V( m_pEffect->SetTexture("VectorTexture", m_ppVectorTexture[m_pVectorId[index] + 2 * index]));
	V( m_pd3dDevice->SetTexture(0,m_ppVectorTexture[m_pVectorId[index] +2 * index]));
	
	SetTextureCondition(0,m_BC,FILTER_POINT);

	
	TakeTarget();	
		
	m_pVectorId[index] = (m_pVectorId[index]+1)%2;
	V( m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[index] + 2 * index]));

	
	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );

	//------ RENDERING ON THE TARGET TEXTURE -------
	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
		
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());
	
	ReturnTarget();

	return hr;

}


HRESULT MyFlow2DRGB::SaveVector()
{
	const int index = 0;
	HRESULT hr =S_OK;


	int indexV = m_pVectorId[index] + index; 
	V_RETURN(hr = D3DXSaveSurfaceToFile(L"SavedVector.dds", D3DXIFF_DDS,m_ppVectorSurface[indexV] ,NULL,NULL));

	return hr;

}


HRESULT MyFlow2DRGB::SaveScalar(const int index)
{
	HRESULT hr =S_OK;

	V_RETURN(hr = D3DXSaveSurfaceToFile(L"SavedScalar.dds", D3DXIFF_DDS,m_ppScalarSurface[m_pScalarId[index] + 2 * index] ,NULL,NULL));

	return hr;

}



HRESULT	MyFlow2DRGB::AddScalar(POINT StartPoint, const int index)
{
	HRESULT hr =E_FAIL;
	
	if (0<= StartPoint.x &&  StartPoint.x < m_Nx && 0 <= StartPoint.y && StartPoint.y < m_Ny ) {

		
		POINT Point;
		D3DLOCKED_RECT	Rect;
	
		D3DXVECTOR2* pTex;
		
		V(m_ppPaintScalarTexture[index]->LockRect(0,&Rect, 0, D3DLOCK_DISCARD));

		if ( SUCCEEDED(hr)) {
			pTex = (D3DXVECTOR2 *)Rect.pBits;
			
			Point = StartPoint;

			for ( int i = 0; i < m_N ; i++ ) {
				pTex[i].x = 0.0f;
				pTex[i].y = 0.0f;
			}
		
			
			int PaintSize = 70;
			D3DXVECTOR2 vPoint, vSigma;
			vSigma.x = float( PaintSize)/float( 16.0f);
			vSigma.y = vSigma.x;
			int pindex;
			for ( int i = -PaintSize;  i <=PaintSize; i++ ) {
				for ( int j = -PaintSize; j <= PaintSize; j++ ) {
					vPoint.x = (float)i;
					vPoint.y = (float)j;
					
					pindex = abs((Point.x+i)%m_Nx) + m_Nx *abs( (Point.y +j)%m_Ny);
					pTex[pindex ].x =100.0f *  Gaussian2D(vPoint, vSigma);
					
				}
			}
			V(m_ppPaintScalarTexture[index]->UnlockRect(0));
		}
	
	}

	V( m_pEffect->SetTechnique("ScalarMerge"));
	V( m_pEffect->SetTexture("ScalarTexture", m_ppScalarTexture[m_pScalarId[index] + 2 * index]));
	V( m_pEffect->SetTexture("ScalarTexture2", m_ppPaintScalarTexture[index]));

	V( m_pd3dDevice->SetTexture(0,m_ppScalarTexture[m_pScalarId[index] +2 * index]));
	V( m_pd3dDevice->SetTexture(1,m_ppPaintScalarTexture[index]));
	SetTextureCondition(0,m_BC, FILTER_POINT);
	SetTextureCondition(1,m_BC, FILTER_LINEAR);

	
	TakeTarget();	
		
	m_pScalarId[index] = (m_pScalarId[index]+1)%2;
	V( m_pd3dDevice->SetRenderTarget(0,m_ppScalarSurface[m_pScalarId[index] + 2 * index]));

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
		
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());
	
	ReturnTarget();

	return hr;
}


HRESULT	MyFlow2DRGB::AddVector(POINT Point, D3DXVECTOR2 VectorVal)
{
	const int index = 0;
	HRESULT hr =E_FAIL;
	
	if (0<= Point.x &&  Point.x < m_Nx && 0 <= Point.y && Point.y < m_Ny ) {
	
		D3DLOCKED_RECT Rect;
		
		D3DXVECTOR2	*pTex;
	
		V(m_ppPaintVectorTexture[index]->LockRect(0,&Rect,NULL, D3DLOCK_NO_DIRTY_UPDATE));

		if ( SUCCEEDED(hr)) {

			pTex = (D3DXVECTOR2 *)Rect.pBits;
			
			for ( int i = 0; i < m_N ; i++ ) {
				pTex[i].x = 0.0f;
				pTex[i].y = 0.0f;
			}

			VectorVal.x /= 700.0f;
			VectorVal.y /= 700.0f;
			int Length =(int) Norm(D3DXVECTOR2( VectorVal.x * (float)m_Nx, VectorVal.y * (float)m_Ny));
			Length /= 8;
			int Width = Length/5;

			float Sigma = (float)Width/32.0f;
			int px = 0;
			int py = 0;
			
			D3DXVECTOR2 nVec;
			if ( Norm(VectorVal) != 0.0f )	 nVec = VectorVal/ Norm(VectorVal); 
			else nVec = VectorVal;
			int pindex;
			for ( int i = 0;  i <= Length; i++ ) {
				for ( int j = -Width; j <= Width ; j++ )  {
				
					px = Point.x + int( nVec.x * (float)i  - nVec.y * (float)j);    // Tangent + Normal
					py = Point.y + int( nVec.y * (float)i  + nVec.x * (float)j);  // Tangent + Normal
				
					pindex =  abs(px%m_Nx) + m_Nx * abs(py%m_Ny);
				
					pTex[pindex].x =  VectorVal.x * Gaussian1D( float(j), Sigma ); 
					pTex[pindex].y =  VectorVal.y * Gaussian1D( float(j), Sigma );

					pTex[ pindex] *=  DistortedGaussian1D(float(Length/8-i),(float) Length/2, 2.0f);
							
				}
			}

			V(m_ppPaintVectorTexture[index]->UnlockRect(0));
		
		}
	}


	V( m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0,0), 1.0f, 0) );

	V( m_pEffect->SetTechnique("VectorMerge"));
	V( m_pEffect->SetTexture("VectorTexture", m_ppVectorTexture[m_pVectorId[index] + 2 * index]));
	V( m_pEffect->SetTexture("VectorTexture2", m_ppPaintVectorTexture[index]));


	V( m_pd3dDevice->SetTexture(0,m_ppVectorTexture[m_pVectorId[index] +2 * index]));
	V( m_pd3dDevice->SetTexture(1,m_ppPaintVectorTexture[index]));
	SetTextureCondition(0,m_BC, FILTER_POINT);
	SetTextureCondition(1,m_BC, FILTER_LINEAR);
	

	TakeTarget();	
		
	m_pVectorId[index] = (m_pVectorId[index]+1)%2;
	V( m_pd3dDevice->SetRenderTarget(0,m_ppVectorSurface[m_pVectorId[index] + 2 * index]));

	UINT cPasses;
	V(m_pEffect->Begin(&cPasses, 0));
	for (UINT iPass = 0; iPass < cPasses; iPass++) {
		
		V(m_pEffect->BeginPass(iPass));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Frame, sizeof(RHW_TEX1_VERTEX));
		V(m_pEffect->EndPass());
		
	}
	V(m_pEffect->End());
	
	ReturnTarget();


	return hr;
}

