float4 vecSkill1;
float4 vecViewPort;

/* 

  Random number generator (hash function)

  Sylvain Lefebvre 2004-07-03

*/


#define M_PI 3.14159265358979323846

#define fmodp(x,n) ((n)*frac((x)/(n)))

//-----------------------------------------------------------------------------

float2 old_rand(float2 ij,float2 n)
{
  float2 xy0 = floor((ij+n)*2000.0)/256.0;
  float2 xym = frac(xy0/257.0)*257.0 + 1.0;
  float2 xym2= frac(xym*xym);
  float2 pxy = xym2.xy * xym.yx;
  float2 xy1 = xy0 + pxy.xy + pxy.yx;
  return (xy1);
}

  
//-----------------------------------------------------------------------------


float2 our_rand_cpu(float2 ij)
{
  float2 xy0 = ij/M_PI;
  float2 xym = fmodp(xy0,257.0) + 1.0;
  float2 xym2= frac(xym*xym);
  float2 pxy = xym2.xy * xym.yx;
  float2 xy1 = xy0 + pxy.xy + pxy.xx;
  return (frac(xy1)*256.0);
}


//-----------------------------------------------------------------------------


float2 our_rand(float2 ij)
{
  const float4 a=float4(M_PI * M_PI * M_PI * M_PI, exp(4.0),  1.0, 0.0);
  const float4 b=float4(pow(13.0, M_PI / 2.0), sqrt(1997.0),  0.0, 1.0);

  float2 xy0    = ij/M_PI;
  float2 xym    = fmodp(xy0.xy,257.0)+1.0;
  float2 xym2   = frac(xym*xym);
  float4 pxy    = float4(xym.yx * xym2 , frac(xy0));
  float2 xy1    = float2(dot(pxy,a) , dot(pxy,b));
  float2 result = frac(xy1);
  
  return (result*256.0);
}


//-----------------------------------------------------------------------------


float2 mccool_rand(float2 ij)
{
  const float4 a=float4(M_PI * M_PI * M_PI * M_PI, exp(4.0), pow(13.0, M_PI / 2.0), sqrt(1997.0));
  float4 result =float4(ij,ij);

  for(int i = 0; i < 3; i++) 
  {
      result.x = frac(dot(result, a));
      result.y = frac(dot(result, a));
      result.z = frac(dot(result, a));
      result.w = frac(dot(result, a));
  }

  return (result.xy);
}


//-----------------------------------------------------------------------------

#ifdef RND_NO_PERMUTATION_TABLE

float2 permut_rand(float2 ij)
{
  const float  invPI = 1.0/M_PI;
  const float  inv16 = 1.0/16.0;
  const float  s17   = sqrt(1997.0);

  float2 p=ij*inv16;
  float2 sigma;

  for (float i=0;i<2;i++) // 2 iteration not completly random, 3 ok
  {
    sigma = tex2D(S_PermutationTable,p).xy;
    p     = p*invPI + sigma*s17;
  }

  return (sigma.xy*256.0);

}

#endif

//-----------------------------------------------------------------------------

// select a rand function

float2 rand(float2 ij,float2 n)
{
//  return (our_rand_cpu(ij));
  return (our_rand(ij));
//  return (permut_rand(ij));
}


//-----------------------------------------------------------------------------

float4 Noise_PS(float2 Tex: TEXCOORD0) : COLOR
{
	float4 color;
	float rcolor = mccool_rand(Tex.xy + float2(vecSkill1.x, vecSkill1.x));
	color.rgb = rcolor;
	color.a = 1;
	
	return color;
}

technique noise
{
	pass p1
	{
		AlphaBlendEnable = false;
		PixelShader = compile ps_2_0 Noise_PS();
	}
}
