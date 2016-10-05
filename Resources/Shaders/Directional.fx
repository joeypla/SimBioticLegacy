/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/

float4x4 inverseViewProjection;
float4x4 inverseView;
float3 cameraPos;
float3 dir;
float4 lightColor;
float intensity;

float2 GBufferTextureSize;

sampler GBuffer0 : register(s0);
sampler GBuffer1 : register(s1);
sampler GBuffer2 : register(s2);

struct VSI
{
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VSO{
	float4 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
VSO mainVS(VSI input){
	VSO output;
	output.pos = float4(input.pos, 1);
	output.uv = input.uv;// - float2(1.0f / GBufferTextureSize.xy);
	return output;
}

float4 manualSample(sampler Sampler, float2 UV, float2 textureSize)
{
return tex2D(Sampler, UV);
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

float4 phong(float3 pos, float3 N, float specularIntensity, float specularPower)
{
	float3 R = normalize(reflect(dir, N));
	float3 E = normalize(cameraPos - pos.xyz);
	float NL = dot(N, -dir);
	float3 Diffuse = NL * lightColor.xyz + float3(0.1f, 0.1f, 0.1f);
	float specular = specularIntensity * pow(saturate(dot(R,E)), specularPower);
	//return intensity * float4(Diffuse.rgb, specular);
	return float4(Diffuse.rgb, 0);
}
float3 decode(float3 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}
float4 mainPS(VSO input) : COLOR0{
	
	float4 encodedNormal = tex2D(GBuffer1, input.uv);
	half3 normal = mul(decode(encodedNormal.xyz), inverseView);
	
	float specularIntensity = tex2D(GBuffer0, input.uv).w;
	
	float specularPower = encodedNormal.w * 255;
	
	float depth = manualSample(GBuffer2, input.uv, GBufferTextureSize).x;
	
	float4 position = 1.0f;
	position.x = input.uv.x * 2.0f - 1.0f;
	position.y = -(input.uv.x * 2.0f - 1.0f);
	position.z = depth;
	
	position = mul(position, inverseViewProjection);
	position /= position.w;
	return saturate(phong(position.xyz, normal, specularIntensity, specularPower));
}

technique technique0 {
	pass p0 {
		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		CullMode = None;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
