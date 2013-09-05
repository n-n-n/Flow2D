//--------------------------------------------------------------------------------------
// File: MyFlow.fx
//
// The effect file for the MyFlow sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
texture g_pTexture;
sampler TextureSampler = 
sampler_state
{
    Texture = <g_pTexture>;
	MipFilter = POINT;
	MinFilter = POINT;
    MagFilter = POINT;
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
    
    // Transform the position from object space to homogeneous projection space
    Output.Position =vPos;
  
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
// color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output =(PS_OUTPUT)0;

	float4	Diff;
	Diff.r = 1.f;
    Diff.g = 1.f;
    Diff.b = 1.f;
    Diff.a = 1.0f; 
    // Lookup mesh texture and modulate it with diffuse
 //   Output.RGBColor = Diff- tex2D(TextureSampler, In.TextureUV);

	Output.RGBColor.b = tex2D(TextureSampler, In.TextureUV).r;
	
	Output.RGBColor.a = 1.0f;
    return Output;
}


//--------------------------------------------------------------------------------------
// Renders scene 
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader  = compile ps_2_0 RenderScenePS(); 
    }
}
