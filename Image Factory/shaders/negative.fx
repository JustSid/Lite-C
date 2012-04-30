texture TargetMap;

sampler postTex = sampler_state
{
	texture 		= (TargetMap);
	MinFilter	= linear;
	MagFilter	= linear;
	MipFilter	= linear;
	AddressU		= Clamp;
	AddressV		= Clamp;
};

float4 Negative_PS(float2 tex : TEXCOORD0) : COLOR
{
	float4 pixel = tex2D(postTex,tex);
	pixel.a = 0;
	
	return float4(1.0f,1.0f,1.0f,1.0f) - pixel;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 Negative_PS();
	}
}

technique fallback { pass one { } }