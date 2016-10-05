float2 textureSize;

sampler sourceSampler : register(s0);

static const float gOffsets[4] = {-1.5f, -0.5f, 0.5f, 1.5f};
struct VSI
{
	float3 pos : POSITION0;
	float2 UV : TEXCOORD0;
	
};
struct VSO
{
	float4 pos : POSITION0;
	float2 UV : TEXCOORD0;
	
};

VSO mainVS(VSI input)
{
	VSO output;
	output.pos = float4(input.pos, 1);
	output.UV = input.UV;

	return output;
}



float4 mainPS(VSO input) : COLOR0
{
	float4 color = 0.0f;
	
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			float2 offset = float2(gOffsets[x], gOffsets[y]) / textureSize;
			color += tex2D(sourceSampler, input.UV + offset);
		}
	}

	color /= 16.0f;
	return color;
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
