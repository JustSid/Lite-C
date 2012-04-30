//Complex blur in each direction, change the strength with vecSkill1[0]

float4 vecSkill1;

float2 samples[12]
=	{
		-0.326212, -0.405805,
		-0.840144, -0.073580,
		-0.695914,  0.457137,
		-0.203345,  0.620716,
		 0.962340, -0.194983,
		 0.473434, -0.480026,
		 0.519456,  0.767022,
		 0.185461, -0.893124,
		 0.507431,  0.064425,
		 0.896420,  0.412458,
		-0.321940, -0.932615,
		-0.791559, -0.597705
	};
	
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

float4 Blur_PS(float2 texCoord: TEXCOORD0) : COLOR
{
	float4 color;

	color = tex2D(postTex, texCoord);

	for (int i = 0; i < 12; i++)
	{
		color += tex2D(postTex, texCoord + vecSkill1[0] * samples[i]);
	}
	color /= 13;
	//if(color.r > 0.1)
	return color;
	//else
	//return float4(color.r, color.g, color.b, 0);
}

float4 Selective_PS(float2 texCoord: TEXCOORD0) : COLOR
{
	float4 color;

	color = tex2D(postTex, texCoord);

	color.a = 1;

	return color;
}

technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 Blur_PS();
	}
}

technique fallback { pass one { } }