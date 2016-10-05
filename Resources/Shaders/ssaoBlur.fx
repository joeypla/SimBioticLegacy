sampler source : register(s0);
float2 inputTextureSize;
float inverseFactor = 600.0f;
static float blurSize = 4;
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
	interpolated.xyz = interpolated.xyz * interpolated.w;
	return interpolated;
}
float4 mainPS(VSO input) : COLOR0
{
	float2 texelSize = 1.0f / inputTextureSize;
	float4 result = 0.0f;
	float2 hlim = -blurSize * 0.5f + 0.5f;
	for (int i = 0; i < blurSize; i++)
	{
		for (int j = 0; j < blurSize; j++)
		{
			float2 offset = (hlim + float2(i, j)) * texelSize;
			result += tex2D(source, input.uv + offset);
		}
	}
	return result / float(blurSize*blurSize);
	
	
}

technique technique0 {
	pass p0 {
		Cullmode = CW;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ZWriteEnable = false;
		ZEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}