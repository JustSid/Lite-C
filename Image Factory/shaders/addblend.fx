Texture TargetMap; // source bitmap 
sampler2D smpSrc = sampler_state { texture = <TargetMap>; };
Texture mtlSkin2; // source bitmap 
sampler2D smpSource = sampler_state { texture = <mtlSkin2>; };
float4 vecViewPort;			
float4 vecSkill1;			
float4 process_alpha(float2 Tex: TEXCOORD0): COLOR 
{												
	float2 Pos;									
	Pos.x = Tex.x;								
	Pos.y = Tex.y;								
	float4 Color = tex2D( smpSrc,Pos.xy);		
	Color.a *= vecSkill1.x;						
	return Color;								
}												
float4 process_combine(float2 Tex: TEXCOORD0): COLOR 
{												
	float4 Color = tex2D( smpSource,Tex.xy);		
	return Color;								
}												
												
technique alphaCombine {						
	pass p1 {									
		AlphaBlendEnable 	= true;				
		SrcBlend			= SrcAlpha;			
		DestBlend			= InvSrcAlpha;		
		AlphaFunc			= GREATEREQUAL;		
		BlendOp				= ADD;				
		PixelShader			= compile ps_2_0 process_combine();
	}
}   