//--------------------------------------------------------------------------------------
// File: MyFlow2DRGB.fx
//
// The effect file for MyFlow2DRGB
// 
// Copyright (c)  N.Y. All rights reserved, All wrongs reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float4	gDiff = float4( 1.0f, 1.0f, 1.0f, 1.0f);

float	g_du,g_dv;
float	g_du2, g_dv2;
float	g_SizeX, g_SizeY;
float	g_dt;
float	g_da;

	
int		g_PoissonItr;
//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------

// For Plain Rendering,  a texture from a file to target texture, back buffer rendering 
texture NormalTexture;
sampler NormalSampler =
sampler_state
{
	Texture = <NormalTexture>;
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
	
	AddressU = WRAP;
	AddressV = WRAP;
};

// For Rendering to target Texture, or no need to interpolate 
texture TargetTexture;
sampler TargetSampler =
sampler_state
{
	Texture = <TargetTexture>;
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
	
	AddressU = WRAP;
	AddressV = WRAP;
	
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};



//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUT VS_Normal( VS_OUT In )
{
    VS_OUT Out = (VS_OUT)0;
  
	Out.Position = In.Position;
	Out.TextureUV = In.TextureUV; 
    
    return Out;  
};

//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUT
{
    float4 Color : COLOR0;  // Pixel color    
	float4 ColorN: COLOR1;
};

struct PS_OUT2
{
	float4 Vector: COLOR0;
	float4 Scalar: COLOR1;
};

struct PS_OUT3
{
	float4 Color0: COLOR0;
	float4 Color1: COLOR1;
	float4 Color2: COLOR2;
};


texture VectorTexture;
sampler VectorSampler =
sampler_state
{
	Texture = <VectorTexture>;

};


texture VectorTexture2;
sampler VectorSampler2 =
sampler_state
{
	Texture = <VectorTexture2>;
};


texture ScalarTexture;
sampler ScalarSampler =
sampler_state
{
	Texture = <ScalarTexture>;
	
};

texture ScalarTexture2;
sampler ScalarSampler2 =
sampler_state
{
	Texture = <ScalarTexture2>;
};

texture ScalarTexture3;
sampler ScalarSampler3 = 
sampler_state
{
	Texture = <ScalarTexture3>;
};

texture OldTexture;
sampler OldSampler =
sampler_state 
{
	Texture = <OldTexture>;

};


PS_OUT PS_Normal(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.Color =  tex2D(NormalSampler, In.TextureUV);
	return Out;
};


//==== Rendering Scalar Field to Back Buffer
PS_OUT PS_RenderScalar( VS_OUT In ) 
{ 
    PS_OUT Out = (PS_OUT)0;
	Out.Color =tex2D(NormalSampler, In.TextureUV).r;// * 10.0f;
    return Out;
};


//===== Rendering Vector Field to Back Buffer ========
PS_OUT PS_RenderVector(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float4 TmpColor = tex2D(NormalSampler, In.TextureUV);
	
	if ( TmpColor.x  < 0.0f) TmpColor.x *= -1.0f;
	if ( TmpColor.y  < 0.0f) TmpColor.y *= -1.0f;  
	
	Out.Color.x = TmpColor.x *( 500.0f);
	Out.Color.y = TmpColor.y * (500.0f);
	Out.Color.b = 0.0f;
	return Out;

};

PS_OUT PS_Diffuse(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;

//	float4 Field  = tex2D(NormalSampler, In.TextureUV);
	float4 FieldL = tex2D(NormalSampler, In.TextureUV + float2(g_du,0));
	float4 FieldR = tex2D(NormalSampler, In.TextureUV - float2(g_du,0));
	float4 FieldU = tex2D(NormalSampler, In.TextureUV + float2(0,g_dv));
	float4 FieldD = tex2D(NormalSampler, In.TextureUV - float2(0,g_dv));
	float coef = 0.001f;

	Out.Color.xy =  ( FieldL + FieldR + FieldU + FieldD )/4.0f;
	
//	Out.Color = Field;
	
	return Out;

};

PS_OUT PS_Diffuse2(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 Field  = tex2D(NormalSampler, In.TextureUV);
	float4 FieldL = tex2D(NormalSampler, In.TextureUV + float2(g_du,0));
	float4 FieldR = tex2D(NormalSampler, In.TextureUV - float2(g_du,0));
	float4 FieldU = tex2D(NormalSampler, In.TextureUV + float2(0,g_dv));
	float4 FieldD = tex2D(NormalSampler, In.TextureUV - float2(0,g_dv));

//	Out.Color.xy =  ( FieldL+FieldR + FieldU+FieldD +6.0* Field)/10.0f;
	Out.Color.xy =  ( FieldL+FieldR + FieldU+FieldD +12.0* Field)/16.0f;

	return Out;


};

PS_OUT PS_VectorInit(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	Out.Color = tex2D( NormalSampler, In.TextureUV);
	Out.Color = Out.Color;
	return Out;
}

// Multi Texture Display
PS_OUT PS_Display(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	//float2 TexCo = frac(In.TextureUV.xy);
	float2 TexCo = In.TextureUV.xy;
	
	if ( TexCo.x < 0.5f && TexCo.y < 0.5f ) {
	
		Out.Color.r = tex2D( ScalarSampler, 2.0f * In.TextureUV).r * 10.0f ;
		
	} else if ( TexCo.x >= 0.5f && TexCo.y < 0.5f ) {
	
		Out.Color.g =tex2D(ScalarSampler2, 2.0f * (In.TextureUV- float2( 0.0f , 0.5f) ) ).r * 10.0f;
	
	} else if ( TexCo.x < 0.5f && TexCo.y >= 0.5f ) {
	
		Out.Color.b = tex2D( ScalarSampler3, 2.0f *( In.TextureUV - float2(0.5f,0.0f) )).r * 10.0f ;
			
	} else {
		
		float4 TmpColor = tex2D(VectorSampler, 2.0f * ( In.TextureUV - float2(0.5f,0.5f) ) );
	
		if ( TmpColor.x  < 0.0f) TmpColor.x *= -1.0f;
		if ( TmpColor.y  < 0.0f) TmpColor.y *= -1.0f;  
	
		Out.Color.x = TmpColor.x *( 1000.0f) ;
		Out.Color.y = TmpColor.y * (1000.0f) ;
		Out.Color.b = 0.0f;	
	}
    return Out;

}


//====== Make Vector Field Closed ========

PS_OUT PS_Divergence(VS_OUT In)
{
	PS_OUT Out  = (PS_OUT2)0;
	
//	float4 Vec0 = tex2D(VectorSampler, In.TextureUV);
	float4 VecL = tex2D(VectorSampler, In.TextureUV + float2(g_du,0));
	float4 VecR = tex2D(VectorSampler, In.TextureUV - float2(g_du,0));
	float4 VecU = tex2D(VectorSampler, In.TextureUV + float2(0,g_dv));
	float4 VecD = tex2D(VectorSampler, In.TextureUV - float2(0,g_dv));

	// Notice; No need to discretize
	Out.Color.g = (VecL.x -VecR.x)/g_du +( VecU.y -VecD.y)/g_dv;
	Out.Color.g /= 2.0f;
	Out.ColorN.g = Out.Color.g;
	return Out;
};

PS_OUT PS_JacobiMethod(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float divV = tex2D(OldSampler, In.TextureUV).g;
	
	float Old0 = tex2D(OldSampler, In.TextureUV ).r;
	float OldL = tex2D(OldSampler, In.TextureUV + float2( g_du,0)).r;
	float OldR = tex2D(OldSampler, In.TextureUV - float2( g_du,0)).r;
	float OldU = tex2D(OldSampler, In.TextureUV + float2(0, g_dv)).r;
	float OldD = tex2D(OldSampler, In.TextureUV - float2(0, g_dv)).r;

	//Out.Color.r =g_da* ((OldR + OldL-2.0f * Old0 )/(4.0f * g_du2)  + (OldU + OldD-2.0f* Old0 )/(4.0f * g_dv2)  - divV) + Old0 ;
	Out.Color.r = (OldR + OldL+OldU + OldD-g_dv2 *  divV)/4.0f ;
	return Out;
	
}

PS_OUT PS_DivergentFree(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	//float Old0 = tex2D(OldSampler, In.TextureUV ).r;
	float OldL = tex2D(OldSampler, In.TextureUV + float2(g_du,0)).r;
	float OldR = tex2D(OldSampler, In.TextureUV - float2(g_du,0)).r;
	float OldU = tex2D(OldSampler, In.TextureUV + float2(0,g_dv)).r;
	float OldD = tex2D(OldSampler, In.TextureUV - float2(0,g_dv)).r;

	float4 Vec = tex2D(VectorSampler, In.TextureUV);
	
	float2 Grad = (float2)0;
	
	Grad.x = (OldL - OldR)/g_du;
	Grad.y = (OldU - OldD)/g_dv; 
	Grad /= 2.0f;
	
//	Grad.x = g_du * round( Grad.x * g_SizeX);
//	Grad.y = g_dv * round( Grad.x * g_SizeY);

	
	Out.Color.xy = Vec.xy -Grad.xy;
	
	return Out; 
}


PS_OUT3 PS_AdvectScalar(VS_OUT In)
{
	PS_OUT3 Out= (PS_OUT3)0;

	// CAUTION : This works as a boundary condition
	float2 OldUV	= In.TextureUV - tex2D(VectorSampler, In.TextureUV).xy;
	
	float2 Weight;
/*	
	if ( OldUV.x < 0 ) { 
		Weight.x = 1.0f -frac(OldUV.x *g_SizeX); }
	else {
		Weight.x = frac(OldUV.x * g_SizeX);
	}
	
	if ( OldUV.y < 0 ) { 
		Weight.y = 1.0f -frac(OldUV.y *g_SizeY); }
	else {
		Weight.y = frac(OldUV.y * g_SizeY);
	}
*/	
	Weight.x = frac(OldUV.x * g_SizeX);
	Weight.y = frac(OldUV.y * g_SizeY);

	float2	Vector[4];
	float4 Scalar;
	
	float2 du = float2(g_du,0);
	float2 dv = float2(0,g_dv);
	float2 duv= float2(g_du,g_dv);

	
	Scalar.x = tex2D(ScalarSampler, OldUV).r;
	Scalar.y = tex2D(ScalarSampler, OldUV + du).r;
	Scalar.z = tex2D(ScalarSampler, OldUV + dv).r;
	Scalar.w = tex2D(ScalarSampler, OldUV + duv).r;
	
	Out.Color0.r = lerp( lerp( Scalar.x, Scalar.y, Weight.x), lerp( Scalar.z, Scalar.w, Weight.x), Weight.y);
	
	Scalar.x = tex2D(ScalarSampler2, OldUV).r;
	Scalar.y = tex2D(ScalarSampler2, OldUV + du).r;
	Scalar.z = tex2D(ScalarSampler2, OldUV + dv).r;
	Scalar.w = tex2D(ScalarSampler2, OldUV + duv).r;
	
	Out.Color1.r = lerp( lerp( Scalar.x, Scalar.y, Weight.x), lerp( Scalar.z, Scalar.w, Weight.x), Weight.y);

	Scalar.x = tex2D(ScalarSampler3, OldUV).r;
	Scalar.y = tex2D(ScalarSampler3, OldUV + du).r;
	Scalar.z = tex2D(ScalarSampler3, OldUV + dv).r;
	Scalar.w = tex2D(ScalarSampler3, OldUV + duv).r;
	
	Out.Color2.r = lerp( lerp( Scalar.x, Scalar.y, Weight.x), lerp( Scalar.z, Scalar.w, Weight.x), Weight.y);
	
	
	return Out;
	
}



PS_OUT PS_AdvectVector(VS_OUT In)
{
	PS_OUT Out= (PS_OUT)0;
	
	float4 Velocity	= tex2D(VectorSampler, In.TextureUV);
	
	// CAUTION : This works as a boundary condition
	float2 OldUV	= In.TextureUV - Velocity.xy;
	
	float2 Weight;
/*	
	if ( OldUV.x < 0 ) { 
		Weight.x = 1.0f -frac(OldUV.x * g_SizeX); }
	else {
		Weight.x = frac(OldUV.x * g_SizeX);
	}
	
	if ( OldUV.y < 0 ) { 
		Weight.y = 1.0f -frac(OldUV.y * g_SizeY); }
	else {
		Weight.y = frac(OldUV.y * g_SizeY);
	}
*/
	Weight.x = frac(OldUV.x * g_SizeX);
	Weight.y = frac(OldUV.y * g_SizeY);
	
		
	float2 du = float2(g_du,0);
	float2 dv = float2(0,g_dv);
	float2 duv= float2(g_du,g_dv);

	float2	Vector[4];
	Vector[0] = tex2D(VectorSampler, OldUV).xy;
	Vector[1] = tex2D(VectorSampler, OldUV + du).xy;
	Vector[2] = tex2D(VectorSampler, OldUV + dv).xy;
	Vector[3] = tex2D(VectorSampler, OldUV + duv).xy;
	
	Out.Color.xy = lerp( lerp( Vector[0], Vector[1], Weight.x), lerp( Vector[2], Vector[3], Weight.x), Weight.y);
	
	return Out;
	
}




PS_OUT	PS_ScalarMerge(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float ScalarVal[2];
	
	ScalarVal[0] = tex2D(ScalarSampler, In.TextureUV).x;
	ScalarVal[1] = tex2D(ScalarSampler2, In.TextureUV).x;
	
	Out.Color.x = ScalarVal[0] + ScalarVal[1];

	return Out;	
}

PS_OUT	PS_VectorMerge(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float2 VectorVal[2];
	
	VectorVal[0] = tex2D(VectorSampler, In.TextureUV).xy;
	VectorVal[1] = tex2D(VectorSampler2, In.TextureUV).xy;
	
	Out.Color.xy = VectorVal[0] + VectorVal[1];
	
	return Out;
}

PS_OUT PS_AddVortex(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float4 Vector0,VectorR,VectorL,VectorRR, VectorLL, VectorU,VectorD,VectorUU, VectorDD, VectorLU, VectorRU,VectorLD, VectorRD;
	
	float2 du = float2(g_du, 0.0f);
	float2 dv = float2(0.0f, g_dv);
	
	Vector0		= tex2D(VectorSampler, In.TextureUV);
	VectorL		= tex2D(VectorSampler, In.TextureUV + du);
	VectorR		= tex2D(VectorSampler, In.TextureUV - du);
	VectorU		= tex2D(VectorSampler, In.TextureUV + dv);
	VectorD		= tex2D(VectorSampler, In.TextureUV - dv);
	VectorLL	= tex2D(VectorSampler, In.TextureUV + 2.0f * du );
	VectorRR	= tex2D(VectorSampler, In.TextureUV - 2.0f * du );
	VectorUU	= tex2D(VectorSampler, In.TextureUV + 2.0f * dv );
	VectorDD	= tex2D(VectorSampler, In.TextureUV - 2.0f * dv );
	VectorLU	= tex2D(VectorSampler, In.TextureUV + du + dv);
	VectorRU	= tex2D(VectorSampler, In.TextureUV - du + dv);
	VectorLD	= tex2D(VectorSampler, In.TextureUV + du - dv);
	VectorRD	= tex2D(VectorSampler, In.TextureUV - du - dv);
	
	float	Vortex0, abVortexR,abVortexL, abVortexU, abVortexD;
	int		VSign;
	float2	GradVortex;
	
	Vortex0		= (VectorL.y - VectorR.y )/(2.0f * g_du) - ( VectorU.x - VectorD.x	)/(2.0f * g_dv);
	
	abVortexR		= abs(	(Vector0.y -	VectorRR.y	)/(2.0f * g_du) - (VectorRU.x	- VectorRD.x)/(2.0f * g_dv)	);
	abVortexL		= abs(	(VectorLL.y -	Vector0.y	)/(2.0f * g_du) - (VectorLU.x	- VectorLD.x)/(2.0f * g_dv)	);
	abVortexU		= abs(	(VectorLU.y -	VectorRU.y	)/(2.0f * g_du) - (VectorUU.x	- Vector0.x	)/(2.0f * g_dv)	);
	abVortexD		= abs(	(VectorLD.y -	VectorRD.y  )/(2.0f * g_du) - (Vector0.x	- VectorDD.x)/(2.0f * g_dv)	);
	
	
	GradVortex.x	=  ( abVortexL  -abVortexR )/(2.0f * g_du);
	GradVortex.y	=  ( abVortexU  - abVortexD )/(2.0f * g_dv);
	float2 nGradVortex = normalize(GradVortex);
	
	float g_alpha = 0.0001f; // 0.0005; Divergent
	Out.Color.xy =Vector0.xy+g_alpha *  float2( nGradVortex.y, - nGradVortex.x) * Vortex0;
	
	return Out;
}


PS_OUT PS_RenderVortex(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	float4 VectorR,VectorL,VectorU,VectorD;
	
	float2 du = float2(g_du, 0.0f);
	float2 dv = float2(0.0f, g_dv);
	
	
	VectorL		= tex2D(VectorSampler, In.TextureUV + du);
	VectorR		= tex2D(VectorSampler, In.TextureUV -du);
	VectorU		= tex2D(VectorSampler, In.TextureUV + dv);
	VectorD		= tex2D(VectorSampler, In.TextureUV - dv);
	
	float Vortex0;

	Vortex0	= (VectorL.y -   VectorR.y	)/(2.0f * g_du) - (VectorU.x	- VectorD.x	)/(2.0f * g_dv);
	Vortex0 *= 10.0f;
	
	if ( Vortex0 > 0) {
		Out.Color.r = Vortex0;
	} else {
		Out.Color.b =-Vortex0;
	} 
	
	return Out;
}




PS_OUT PS_ScalarMix(VS_OUT In)
{
	PS_OUT Out = (PS_OUT)0;
	
	Out.Color.r = tex2D(ScalarSampler,	In.TextureUV).r;
	Out.Color.g = tex2D(ScalarSampler2, In.TextureUV).r;
	Out.Color.b = tex2D(ScalarSampler3, In.TextureUV).r;
	
	return Out;
}


PS_OUT3 PS_Dispersion(VS_OUT In)
{
	PS_OUT3 Out = (PS_OUT3)0;
	
	Out.Color0.r = tex2D(NormalSampler,	In.TextureUV).r;
	Out.Color1.r = tex2D(NormalSampler, In.TextureUV).g;
	Out.Color2.r = tex2D(NormalSampler, In.TextureUV).b;
	
	return Out;
}



//--------------------------------------------------
// Renders scene 
//--------------------------------------------------

technique Normal
{
    pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_Normal(); 
    }
}

technique VectorInit
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_VectorInit(); 
	}
}

technique RenderScalar
{
    pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_RenderScalar(); 
    }
}


technique RenderVector
{
    pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_RenderVector(); 
    }
}

technique Diffuse
{
    pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_Diffuse(); 
    }
}


technique Diffuse2
{
    pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_Diffuse2(); 
    }
}


technique Divergence
{
	pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_Divergence(); 
    }
}

technique JacobiMethod
{
	pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_JacobiMethod(); 
    }
}

technique DivergentFree
{
	pass P0
    {          
        VertexShader = compile vs_2_0 VS_Normal();
        PixelShader  = compile ps_2_0 PS_DivergentFree(); 
    }
}

technique AdvectScalar
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_AdvectScalar();
	}
}


technique AdvectVector
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_AdvectVector();
	}

}


technique AddVortex
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_AddVortex();
	}

}

technique ScalarMerge
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_ScalarMerge();
	}
}

technique	VectorMerge
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader  = compile ps_2_0 PS_VectorMerge();
	}

}


technique Display
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_Display();
	}

}

technique ScalarMix
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_ScalarMix();
	}

}

technique Dispersion
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_Dispersion();
	}

}


technique RenderVortex
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Normal();
		PixelShader	 = compile ps_2_0 PS_RenderVortex();
	}

}
