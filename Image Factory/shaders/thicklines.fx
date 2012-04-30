//Complex blur in each direction, change the strength with vecSkill1[0]

float4 vecSkill1;
float4 vecViewPort;

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
	if(color.r > 0.01f)
	return float4(0.5098, 0.5608, 0.6784, 1);
	else
	return float4(color.r, color.g, color.b, 0);
}

float4 ThickLines_PS(float2 texCoord: TEXCOORD0) : COLOR
{
	float4 Color;

	Color = tex2D(postTex, texCoord);

	if(Color.r > 0.6f)
	{
		return Color;
	}
	else
	{
		float2 texPos;
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				texPos.x = texCoord.x + i*0.00078125f;
				texPos.y = texCoord.y + i*0.00138889f;
				Color = tex2D(postTex, texPos);
				if(Color.r > 0.6f)
				{
					return float4(0.5098f, 0.5608f, 0.6784f, 1.0f);
				}
			}
		}
		return float4(Color.rgb,0);
	}
}

technique tech_00
{
	pass pass_00
	{
		AlphaBlendEnable = false; 
		VertexShader = null;
		PixelShader = compile ps_2_0 ThickLines_PS();//Blur_PS();
	}
}

technique fallback { pass one { } }