float4 vecSkill1;
texture TargetMap;
sampler smpSource = sampler_state { texture = <TargetMap>; };

float4 BlackWhite_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color;

	Color.a = 1.0f;
	Color = tex2D( smpSource, Tex.xy);
	Color.rgb = (Color.r+Color.g+Color.b)/3.0f;

	if (Color.r < vecSkill1.x)
		Color.r = 0.0f;
	else
		Color.r = 1.0f;

	Color.gb = Color.r;
	
	return Color;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 BlackWhite_PS();
	}
}

technique fallback { pass one { } }