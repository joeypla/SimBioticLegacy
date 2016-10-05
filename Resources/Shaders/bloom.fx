sampler scene : register(s0);
float threshold;
struct VSI
{
	float3 pos : POSITION0;
	float2 uv: TEXCOORD0;
	
};
struct VSO
{
	float4 pos : POSITION0;
	float2 uv : TEXCOORD0;
	
};

VSO mainVS(VSI input)
{
	VSO output;
	output.pos = float4(input.pos, 1);
	output.uv = input.uv;// - float2(1.0f/GBufferTextureSize.xy);
	
	return output;
}

//Pixel Shader


float4 mainPS(VSO input) : COLOR0
{
	
	//float3 color = pow(tex2D(scene, input.uv).xyz, 2);
	float4 color = float4(tex2D(scene, input.uv).xyz, 1);
	
	
	
	return (color - threshold)/(1 - threshold);
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
