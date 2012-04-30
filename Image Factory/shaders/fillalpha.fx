float4 vecSkill1;

float4 fillColor(float2 Tex: TEXCOORD0) : COLOR
{
	float4 color;
	color.rgb = vecSkill1.xyz;
	color.a = vecSkill1.w;
	
	return color;
}

technique fill
{
	pass p1
	{
		AlphaBlendEnable = false;
		PixelShader = compile ps_2_0 fillColor();
	}
}
