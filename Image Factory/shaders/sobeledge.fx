float4 vecSkill1;

#define NUM 9
const float2 c[NUM]
=	{
		float2(-0.0078125, 0.0078125), 
		float2( 0.00 ,     0.0078125),
		float2( 0.0078125, 0.0078125),
		float2(-0.0078125, 0.00 ),
		float2( 0.0,       0.0),
		float2( 0.0078125, 0.007 ),
		float2(-0.0078125,-0.0078125),
		float2( 0.00 ,    -0.0078125),
		float2( 0.0078125,-0.0078125)
	};

texture entSkin1;

sampler2D postTex = sampler_state
{
   texture = (entSkin1);
   MinFilter = linear;
   MagFilter = linear;
   MipFilter = linear;
   AddressU = Clamp;
   AddressV = Clamp;
};

float4 SobelEdge_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color;

	float3 col[NUM];

	for (int i=0; i < NUM; i++)
	{
		col[i] = tex2D(postTex, Tex.xy + c[i]);
	}

	float3 rgb2lum = float3(0.30, 0.59, 0.11);

	float lum[NUM];
	for (i=0; i < NUM; i++)
	{
		lum[i] = dot(col[i].xyz, rgb2lum);
	}

	float x = lum[2]+  lum[8]+2*lum[5]-lum[0]-2*lum[3]-lum[6];
	float y = lum[6]+2*lum[7]+  lum[8]-lum[0]-2*lum[1]-lum[2];

	float edge = (x*x + y*y < vecSkill1[1])? 1.0 : 0.0;

	Color.rgb = (vecSkill1[0] * col[5].xyz * edge.xxx);
	Color.a = 1.0;

	return Color;
}


technique tech_00
{
	pass pass_00
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 SobelEdge_PS();
	}
}

technique tech_01
{
	pass pass_00
	{
		Texture[0] = <entSkin1>;
	}
}