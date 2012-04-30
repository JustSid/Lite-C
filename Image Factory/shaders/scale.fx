float4 vecSkill1;
float4 vecViewPort;

texture TargetMap;
sampler smpSource = sampler_state { texture = <TargetMap>; AddressU = wrap;	AddressV = wrap; };

float4 ColorToAlpha_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color;

	Tex.x *= vecViewPort.x/vecSkill1.x;
	Tex.y *= vecViewPort.y/vecSkill1.y;

	Color = tex2D(smpSource, Tex.xy);
	
	return Color;
}

technique tech_00
{
	pass pass_00
	{
		AlphaBlendEnable = false; 
		VertexShader = null;
		PixelShader = compile ps_2_0 ColorToAlpha_PS();
	}
}

technique fallback { pass one { } }