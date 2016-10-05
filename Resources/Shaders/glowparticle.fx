/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/
float4x4 world;
float4x4 view;
float4x4 projection;
float4 color;



sampler particleTexture : register(s0);

struct VSI
{
	float4 pos : POSITION0;
	float2 UV  : TEXCOORD0;
};
struct VSO{
	float4 pos : POSITION0;
	float2 UV  : TEXCOORD0;
};
VSO mainVS(VSI input){
	VSO output;
	float4 worldPos = mul(input.pos, world);
	float4 viewPos = mul(worldPos, view);
	output.pos = mul(viewPos, projection);
	output.UV = input.UV;
	return output;
}

float4 mainPS(VSO input) : COLOR0{
	float4 textureColor = tex2D(particleTexture, input.UV);
	float4 finalColor = textureColor * color;
	return finalColor;
}

technique technique0 {
	pass p0 {
		ZEnable = true;
		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		Cullmode = CCW;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}