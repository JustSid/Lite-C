texture TargetMap;

float4 vecSkill1;

sampler postTex = sampler_state
{
   texture = (TargetMap);
   MinFilter = linear;
   MagFilter = linear;
   MipFilter = linear;
   AddressU = Clamp;
   AddressV = Clamp;
};

float4 Sharpen_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color;

	Color = tex2D( postTex, Tex.xy);
	Color -= tex2D( postTex, Tex.xy+0.0001)*vecSkill1[0];
	Color += tex2D( postTex, Tex.xy-0.0001)*vecSkill1[0];
	
	Color.a = 1.0;

	return Color;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 Sharpen_PS();
	}
}

technique fallback { pass one { } }