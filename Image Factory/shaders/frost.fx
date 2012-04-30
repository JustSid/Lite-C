/*********************************************************************NVMH3****
File: $Id: //sw/devtools/ShaderLibrary/main/HLSL/post_frost_fast.fx#1 $

Copyright NVIDIA Corporation 2007
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY
LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF
NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

% Very similar to "post_frost" but faster at the expense of some sampling choices.

******************************************************************************/

/************* TWEAKABLES **************/

float4 vecSkill1;


texture TargetMap;

sampler postTex = sampler_state
{
	texture = <TargetMap>;
	AddressU  = Clamp;        
	AddressV  = Clamp;
	AddressW  = Clamp;
	MinFilter = Linear;
	MipFilter = Linear;
	MagFilter = Linear;
};


texture mtlSkin1;

sampler Noise2DSamp = sampler_state 
{
    texture = <mtlSkin1>;
    AddressU  = Wrap;        
    AddressV  = Wrap;
    AddressW  = Wrap;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
};

float4 frostFastPS(float2 Tex : TEXCOORD0) : COLOR
{
    float2 ox = float2(vecSkill1.x,0.0);
    float2 oy = float2(0.0,vecSkill1.y);
    float4 C00 = tex2D(postTex,Tex - oy - ox);
    float4 C22 = tex2D(postTex,Tex + oy + ox);
    float n = tex2D(Noise2DSamp,Tex*vecSkill1.z);
    n = fmod(n, 0.111111)/0.111111;
    float4 result = lerp(C00,C22,n);
    return result;
}

technique Main
{
	pass PostP0
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 frostFastPS();
	}
}

technique fallback { pass one { } }