float4 vecSkill1;
texture TargetMap;
sampler smpSource = sampler_state { texture = <TargetMap>; };

float4 ColorToAlpha_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 AlphaColor;
	float4 Color;

	AlphaColor = tex2D( smpSource, Tex.xy);

	AlphaColor.r -= vecSkill1.x;
	AlphaColor.g -= vecSkill1.y;
	AlphaColor.b -= vecSkill1.z;
	AlphaColor.a = abs(AlphaColor.r)+abs(AlphaColor.g)+abs(AlphaColor.b);

	Color = tex2D( smpSource, Tex.xy);
	Color.a = AlphaColor.a;
	
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