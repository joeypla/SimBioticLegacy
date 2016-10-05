

float focalWidth;
float focalDistance;

sampler scene : register(s0);
sampler blurredScene : register(s1);
sampler depth: register(s2);

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

float calculateBlurFactor(float pDepth)
{
	return smoothstep(0, focalWidth, abs(focalDistance - (pDepth * 120.0f)));
}
float4 mainPS(VSO input) : COLOR0
{
	float4 sceneColor = tex2D(scene, input.UV);
	float4 sceneBlurredColor = tex2D(blurredScene, input.UV);
	float sceneDepth = tex2D(depth, input.UV);
	float linearDepth = LinearizeDepth(sceneDepth);

	float blurFactor = calculateBlurFactor(linearDepth);
	
	return lerp(sceneColor, sceneBlurredColor, saturate(blurFactor) * 1.0f);
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
