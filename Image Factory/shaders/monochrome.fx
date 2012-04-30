texture TargetMap;

sampler postTex = sampler_state
{
	texture 	= (TargetMap);
	MinFilter	= linear;
	MagFilter	= linear;
	MipFilter	= linear;
	AddressU		= Clamp;
	AddressV		= Clamp;
};

float4 Monochrom_PS(float2 tex : TEXCOORD0) : COLOR
{
	float4 Color = tex2D(postTex,tex);
	float luminosity = (0.2125 * Color.r) + (0.7154 * Color.g) + (0.0721 * Color.b);
	
	Color.rgb = luminosity;
	return Color;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 Monochrom_PS();
	}
}

technique fallback { pass one { } }