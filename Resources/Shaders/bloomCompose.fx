sampler scene : register(s0);
sampler bloom : register(s1);
float intensity;
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
	return interpolated;
}
float4 mainPS(VSO input) : COLOR0
{
	float2 texSize = float2(560.0f, 270.0f);
	
	float4 color = tex2D(scene, input.uv);
	float4 bloomColor = manualSample(bloom, input.uv, texSize);
	float4 newColor = saturate(color + (bloomColor*intensity));
	
	
	return newColor;
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
