texture TargetMap;

sampler postTex = sampler_state
{
   texture = (TargetMap);
   MinFilter = linear;
   MagFilter = linear;
   MipFilter = linear;
   AddressU = Clamp;
   AddressV = Clamp;
};

float4 Emboss_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color;
	
	Color.a = 1.0f;
	Color.rgb = 0.5f;
	
	Color -= tex2D( postTex, Tex.xy-0.001)*2.0f;
	Color += tex2D( postTex, Tex.xy+0.001)*2.0f;
	
	Color.rgb = (Color.r+Color.g+Color.b)/3.0f;

	return Color;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 Emboss_PS();
	}
}

technique fallback { pass one { } }