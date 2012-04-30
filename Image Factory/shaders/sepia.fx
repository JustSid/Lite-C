float4 vecSkill1;	//Desaturation and Toning

//float Desat = 0.5f;	//Desaturation
//float Toned = 1.0f;	//Toning
float3 LightColor = {1,0.7,0.5};	//Paper Tone
float3 DarkColor = {0.2,0.05,0};	//Stain Tone


texture TargetMap;
sampler postTex = sampler_state
{
   texture = (TargetMap);
   MinFilter = linear;
   MagFilter = linear;
   MipFilter = linear;
   AddressU = clamp;
   AddressV = clamp;
};


float4 Sepia_PS(float2 TexCoord : TEXCOORD0)	:	COLOR
{
	float3 scnColor = LightColor * tex2D(postTex, TexCoord).xyz;
	float3 grayXfer = float3(0.3,0.59,0.11);
	float gray = dot(grayXfer,scnColor);
	float3 muted = lerp(scnColor,gray.xxx,vecSkill1.x);
	float3 sepia = lerp(DarkColor,LightColor,gray);
	float3 result = lerp(muted,sepia,vecSkill1.y);
	return float4(result,1);
}

technique Main
{
	pass p001
	{
		AlphaBlendEnable = false;
		VertexShader = null;
		PixelShader = compile ps_2_0 Sepia_PS();
	}
}

technique fallback { pass one { } }