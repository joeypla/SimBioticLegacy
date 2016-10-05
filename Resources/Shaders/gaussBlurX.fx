sampler source : register(s0);
float2 texSize;
float inverseFactor = 640.0f;
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

float4 manualSample(sampler Sampler, float2 UV, float2 textureSize)
{
	float2 texelPos = textureSize * UV;
	float2 lerps = frac(texelPos);
	float texelSize = 1.0 / textureSize;
	
	float4 sourceVals[4];
	sourceVals[0] = tex2D(Sampler, UV);
	sourceVals[1] = tex2D(Sampler, UV + float2(texelSize, 0));
	sourceVals[2] = tex2D(Sampler, UV + float2(0, texelSize));
	sourceVals[3] = tex2D(Sampler, UV + float2(texelSize, texelSize));
	
	float4 interpolated = lerp(lerp(sourceVals[0], sourceVals[1], lerps.x), lerp(sourceVals[2], sourceVals[3], lerps.x), lerps.y);
	//interpolated.xyz = interpolated.xyz * interpolated.w;
	return interpolated;
}
float4 mainPS(VSO input) : COLOR0
{
	
	float blurSize = 1.0f/inverseFactor;
	
	float4 color = 0.0f;
	color+=manualSample(source, float2(input.uv.x - blurSize*5, input.uv.y), texSize)*0.04f;
	color+=manualSample(source, float2(input.uv.x - blurSize*4, input.uv.y), texSize)*0.04f;
	color+=manualSample(source, float2(input.uv.x - blurSize*3, input.uv.y), texSize)*0.08f;
	color+=manualSample(source, float2(input.uv.x - blurSize*2, input.uv.y), texSize)*0.11f;
	color+=manualSample(source, float2(input.uv.x - blurSize, input.uv.y), texSize)*0.14f;
	color+=manualSample(source, float2(input.uv.x, input.uv.y), texSize)*0.16f;
	color+=manualSample(source, float2(input.uv.x + blurSize, input.uv.y), texSize)*0.14f;
	color+=manualSample(source, float2(input.uv.x + blurSize*2, input.uv.y), texSize)*0.11f;
	color+=manualSample(source, float2(input.uv.x + blurSize*3, input.uv.y), texSize)*0.08f;
	color+=manualSample(source, float2(input.uv.x + blurSize*4, input.uv.y), texSize)*0.04f;
	color+=manualSample(source, float2(input.uv.x + blurSize*5, input.uv.y), texSize)*0.04f;
	//return float4
	return color;
	
}

technique technique0 {
	pass p0 {
		Cullmode = CW;
		AlphaBlendEnable = false;
		
		AlphaTestEnable = false;
		ZWriteEnable = false;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}