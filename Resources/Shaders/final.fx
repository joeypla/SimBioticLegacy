float2 GBufferTextureSize;
float3 ambientLight;
//float nearPlane;
//float farPlane;
float3 fogColor;
float fogStart;
float fogEnd;
sampler Albedo : register(s0);
sampler LightMap : register(s1);
sampler BloomBuffer: register(s2);
sampler depthMap : register(s3);
sampler ssao : register(s4);
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

float LinearizeDepth(float z)
{
	float nearPlane = 0.1f;
	float farPlane = 120.0f;
	return (2 * nearPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

float4 mainPS(VSO input) : COLOR0
{
	float4 color = pow(tex2D(Albedo, input.UV), 1.0f/2.2f);
	float4 lighting = tex2D(LightMap, input.UV);
	float3 bloom = tex2D(BloomBuffer, input.UV).xyz;
	float depth = tex2D(depthMap, input.UV).r;
	float3 ssaoAmount = tex2D(ssao, input.UV).xyz;
	float fogAmount = smoothstep(fogStart, fogEnd , LinearizeDepth(depth)*120.0f);
	fogAmount = 0.0f;
	//create bloom highlights. This means if a component is high enough, it maps to white.
	float redPass = saturate(bloom.x - 0.7f);
	float bluePass = saturate(bloom.y - 0.7f);
	float greenPass = saturate(bloom.z - 0.7f);
	bloom.xyz += redPass*3.0f;
	bloom.xyz += bluePass*3.0f;
	bloom.xyz += greenPass*3.0f;

	//if (bloom.x > 0.7f || bloom.y > 0.7f || bloom.z > 0.7f) bloom.xyz = float3(1.0f, 1.0f, 1.0f);
	lighting.xyz = lighting.xyz + ambientLight + color.xyz*color.w;
	float4 output = float4((color.xyz * lighting.xyz * ssaoAmount + (lighting.w*lighting.xyz*1.0f) + bloom*2.0f)*(1.0f - fogAmount) + fogColor*fogAmount, 1);
	float4 correctedOutput = pow(output, 3.0f/2.2f);
	return correctedOutput;
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
