/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/
float4x4 world;
float4x4 view;
float4x4 projection;
float4x4 inverseViewProjection;
float4x4 inverseView;
float3 cameraPos;
float3 lightPos;
float4 lightColor;
float ambientFactor;
float radius;

float2 GBufferTextureSize;

sampler GBuffer0 : register(s0);
sampler GBuffer1 : register(s1);
sampler GBuffer2 : register(s2);
sampler GBuffer3 : register(s3);

struct VSI
{
	float4 pos : POSITION0;
};
struct VSO{
	float4 pos : POSITION0;
	float4 screenPosition : TEXCOORD0;
};
VSO mainVS(VSI input){
	VSO output;
	float4 worldPos = mul(input.pos, world);
	float4 viewPos = mul(worldPos, view);
	output.pos = mul(viewPos, projection);
	output.screenPosition = output.pos;
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
	float3 L = lightPos.xyz - pos.xyz;
	float d = length(L);
	float attenuation = saturate( 1.0f - max(0.1f, length(L))/(radius/2));
	//float attenuation = (d < radius) ? 1.0f / (lightAtt[0] + lightAtt[1]*d + lightAtt[2]*d*d) : 0.0f;
	L = normalize(L);
	float3 R = normalize(reflect(-L, N));
	float3 E = normalize(cameraPos - pos.xyz);
	float NL = saturate(dot(N, L));
	float3 Diffuse = NL * lightColor.xyz + ((1.0f - NL)*ambientFactor*lightColor.xyz);
	float specular = specularIntensity * pow(saturate(dot(R,E)), specularPower);
	float3 specularColor = lightColor.xyz;
	//return attenuation * float4(Diffuse.rgb, 0.0f) + (specular*float4(specularColor, 0.0)/2);
	return attenuation * float4(Diffuse.rgb, specular);
}
float3 decode(float3 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}
float4 mainPS(VSO input) : COLOR0{
	input.screenPosition.xy /= input.screenPosition.w;
	float2 UV = 0.5f * (float2(input.screenPosition.x, -input.screenPosition.y) + 1) - 0.5f*float2(1.0f/GBufferTextureSize.xy);
	float4 encodedNormal = manualSample(GBuffer1, UV, GBufferTextureSize);
	float3 normal = mul(decode(encodedNormal.xyz), inverseView);
	
	float specularIntensity = tex2D(GBuffer3, UV).z;
	float specularPower = tex2D(GBuffer3, UV).w*255.0f;
	
	
	
	float4 position = 1.0f;
	position.xy = input.screenPosition.xy;
	float Depth = manualSample(GBuffer2, UV, GBufferTextureSize);
	position.z = Depth;
	position = mul(position, inverseViewProjection);
	position /= position.w;
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	//return float4(position.xyz, 1.0f);
	
	return phong(position.xyz, normal, specularIntensity, specularPower);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

technique technique0 {
	pass p0 {
		//ZEnable = false;
		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}