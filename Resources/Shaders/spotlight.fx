 /*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/
float4x4 world;//
float4x4 view;//
float4x4 inverseView;//
float4x4 projection;//
float4x4 inverseViewProjection;//
float3 cameraPos;//

//per light info
float4x4 lightViewProjection;//
float shadowMapSize;//
//light properties
float3 lightPosition;//
float3 lightDirection;
float3 lightColor;//
float lightIntensity;//1.0f always for now
float ambientFactor;
float theta;
float phi;
float radius;

float3 fogColor;
float fogStart;
float fogEnd;

//other
float2 GBufferTextureSize;
bool shadows;
sampler shadowMap : register(s0);
sampler GBuffer1 : register(s1);
sampler GBuffer2 : register(s2);
sampler GBuffer3 : register(s3);

float depthPrecision;
const float depthBias = 0.00005f;
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
float LinearizeDepth(float z)
{
	float nearPlane = 0.1f;
	float farPlane = 120.0f;
	return (2 * nearPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}
float4 phong(float3 pos, float3 N, float radialAttenuation, float specularIntensity, float specularPower)
{
	float3 L = lightPosition.xyz - pos.xyz;

	
	float heightAttenuation = 1.0f - saturate(length(L) - (radius/2));
	
	float attenuation = 1.0f;//min(radialAttenuation, heightAttenuation);

	L = normalize(L);

	float SL = dot(-L, lightDirection);
	float4 shading = 0.0f;

	if (SL > cos(phi*0.5f))
	{
		float3 R = normalize(reflect(-L, N));
		float3 E = normalize(cameraPos - pos.xyz);
		float NL = dot(N, L);
		float spotEffect = smoothstep(cos(phi*0.5f), cos(theta*0.5f), SL);
		float3 diffuse = NL * lightColor.xyz;
		float specular = specularIntensity * pow(saturate(dot(R, E)), specularPower);
		
		shading = spotEffect*heightAttenuation*float4(diffuse, specular);
	}
	
	return shading;
}
float3 decode(float3 enc)
{
	return (2.0f * enc.xyz - 1.0f);
}
float linstep(float min, float max, float v)  
{  
    return clamp((v - min) / (max - min), 0, 1);  
};
float ChebyshevUpperBound(float2 Moments, float Mean)  
{  
    // One-tailed inequality valid if Mean > Moments.x
    float p = (Mean <= Moments.x);

    // Compute variance.
    float Variance = Moments.y - (Moments.x*Moments.x);
    Variance = max(Variance, 0.00002);

    //Compute probabilistic upper bound
    float d = Moments.x - Mean;
    float p_max = linstep(0.2f, 1.0f, Variance / (Variance + d * d)); 
    //return Mean * p;
    //return p_max;
    return max(p, p_max);
};
 
float ShadowContribution(float2 LightTexCoord, float DistanceToLight)  
{  
    // Read the moments from the variance shadow map.  
    float2 moments = tex2D(shadowMap, LightTexCoord).xy;
    // Compute the Chebyshev upper bound.  
    return ChebyshevUpperBound(moments, DistanceToLight);  
};



float4 mainPS(VSO input) : COLOR0{
	input.screenPosition.xy /= input.screenPosition.w;
	float2 UV = 0.5f * (float2(input.screenPosition.x, -input.screenPosition.y) + 1) -float2(1.0f/GBufferTextureSize.xy);
	float4 encodedNormal = tex2D(GBuffer1, UV);
	float3 normal = mul(decode(encodedNormal.xyz), inverseView);
	
	float specularIntensity = tex2D(GBuffer3, UV).z;
	float specularPower = tex2D(GBuffer3, UV).w*255.0f;
	
	
	
	float4 position = 1.0f;
	position.xy = input.screenPosition.xy;
	float Depth = manualSample(GBuffer2, UV, GBufferTextureSize);
	position.z = Depth;
	position = mul(position, inverseViewProjection);
	
	position /= position.w;
	
	float4 lightScreenPos = mul(position, lightViewProjection);
	lightScreenPos /= lightScreenPos.w;
	
	float2 LUV = 0.5f * (float2(lightScreenPos.x, -lightScreenPos.y) + 1);
	
	
	float lZ = tex2D(shadowMap, LUV);
	float attenuation = 1.0f;
	
	float shadowFactor = 1;
	float myLength = length(lightPosition - position) / radius;
	shadowFactor = ShadowContribution(LUV, myLength);

	

	float4 lighting = shadowFactor * phong(position.xyz, normal, attenuation, specularIntensity, specularPower);
	//float fogAmount = smoothstep(fogStart, fogEnd , LinearizeDepth(Depth)*120.0f);
	return lighting;// + lighting*fogAmount*float4(fogColor, 1.0);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	
}

technique technique0 {
	pass p0 {
		ZEnable = false;
		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}