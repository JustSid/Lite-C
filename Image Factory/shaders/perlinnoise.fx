float4 vecSkill1;
float4 vecViewPort;

#define STEPS 16

/* 

  Random number generator (hash function)

  Sylvain Lefebvre 2004-07-03

*/

#define fmodp(x,n) ((n)*frac((x)/(n)))
#define M_PI 3.14159265358979323846

float mccool_rand(float2 ij)
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

float randomDepth(float2 pos, int step)
{
    if(step == 1)
    {
        /*pos.x += vecSkill1.x;
        pos.y += vecSkill1.x;
        pos.x = ((int)((pos.x*vecViewPort.x)/(vecViewPort.x/pow(2, step+1)))) * (vecViewPort.x/pow(2, step+1));
        pos.x = pos.x / vecViewPort.x;
        pos.y = ((int)((pos.y*vecViewPort.y)/(vecViewPort.y/pow(2, step+1)))) * (vecViewPort.y/pow(2, step+1));
        pos.y = pos.y / vecViewPort.y;
        float rcolor = mccool_rand(pos.xy);
        rcolor = rcolor-0.5;
        rcolor = rcolor * 0.5;
        rcolor = rcolor+0.5;
        return rcolor;*/
        float depth[4];
        depth[0] = mccool_rand(vecSkill1.xx);
        depth[1] = mccool_rand(float2(1,0)+vecSkill1.xx);
        depth[2] = mccool_rand(float2(0,1)+vecSkill1.xx);
        depth[3] = mccool_rand(float2(1,1)+vecSkill1.xx);
        float finalCol = 0.5 * ((depth[0]*pos.x)+(depth[1]*(1-pos.x))) + 0.5 * ((depth[2]*pos.y)+(depth[3]*(1-pos.y)));
        return finalCol;
    }
    else if(step != STEPS)
    {
        pos.x += vecSkill1.x;
        pos.y += vecSkill1.x;
        pos.x = ((int)((pos.x*vecViewPort.x)/(vecViewPort.x/pow(2, step+1)))) * (vecViewPort.x/pow(2, step+1));
        pos.x = pos.x / vecViewPort.x;
        pos.y = ((int)((pos.y*vecViewPort.y)/(vecViewPort.y/pow(2, step+1)))) * (vecViewPort.y/pow(2, step+1));
        pos.y = pos.y / vecViewPort.y;
        float rcolor = mccool_rand(pos.xy);
        rcolor = rcolor-0.5;
        rcolor = rcolor * (float)pow(0.75f, step+1);
        return rcolor;
    }
    else
    {
        pos.x += vecSkill1.x;
        pos.y += vecSkill1.x;
        float rcolor = mccool_rand(pos.xy);
        rcolor -= 0.5;
        rcolor = rcolor * (float)pow(0.75f, step+1);
        return rcolor;
    }
}

float4 Noise_PS(float2 Tex: TEXCOORD0) : COLOR
{
	float4 color = 0;
    float depth = 0;

    for(int i = 1; i<=STEPS; i++)
    {
        depth += randomDepth(Tex, i);
    }
	
    color.b = depth;

    //color.rgb = rcolor;
	color.a = 1;
	
	return color;
}


technique noise
{
	pass p1
	{
		AlphaBlendEnable = false;
		PixelShader = compile ps_3_0 Noise_PS();
	}
}
