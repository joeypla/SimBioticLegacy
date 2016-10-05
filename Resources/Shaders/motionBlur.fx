float2 textureSize;
float velocityScale;
sampler velocityMap : register(s0);
sampler sourceMap : register(s1);
sampler depthMap : register(s2);
int MAX_SAMPLES = 16;
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

//Pixel Shader
//The ambient lighting is actually done here to save a fullscreen pass. I don't
//see the problem with it yet.

float4 mainPS(VSO input) : COLOR0
{
	float2 texelSize = 1.0f / textureSize;

	float2 velocity = 2.0f* (tex2D(velocityMap, input.UV).xy - 0.5f) * 4.0f;
	velocity = pow(velocity, 2.0f);
	//velocity = float2(0.0f, 0.0f);
	//perform blur
	//float speed = length(velocity / texelSize);
	//int nSamples = clamp(floor(speed), 1, 16);

	float4 result = 0.0f;
	float fi = 0.0f;

	float depth = tex2D(depthMap, input.UV).r;
	for (int i = 0; i < 8; i++)
	{
		
		float2 offset = velocity * (fi / 7.0 - 0.5f) * velocityScale;
		float4 sampleVelocity = tex2D(sourceMap, clamp(input.UV + offset, 0.0f, 1.0f));

		

		//if (abs(depth - tex2D(depthMap, clamp(input.UV + offset, 0.0f, 1.0f)).r) < 0.05f)
		//{
			result += sampleVelocity;
			fi += 1.0f;
		//}
	}
	result /= 8.0f;///= fi;//8.0f;
	return result;
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
