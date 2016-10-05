//-----------------------------------------------------------------------------
// File: SpotLight.fx
//
// Renders a spot light source to a lighting render target based on a G-buffer
//
// Copyright © Lucas Loreggia 2012
//-----------------------------------------------------------------------------

#include "Common.fx"

float phi = 45.0f;

float3 cameraPos; 

float3 lightColor;

float3 lightDirection;

bool DrawShadows = false;

float FallOff = 1.0f;

float2 HalfPixel;

float Intensity = 1.0f;

float4x4 InverseViewProjection; 

float4x4 LightViewProjection;

float3 Position;

float Radius;

float RadialFallOff = 1.0f;

float2 ViewportSize;

texture ColorMap;
sampler colorSampler = sampler_state
{
    Texture = <ColorMap>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = POINT;
};

texture NorSpecMap;
sampler norSpecSampler = sampler_state
{
    Texture = <NorSpecMap>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = POINT;
};

texture DepthMap;
sampler depthSampler = sampler_state
{
    Texture = <DepthMap>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = POINT;
};

texture ShadowMap;
sampler shadowSampler = sampler_state
{
    Texture = <ShadowMap>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = POINT;
};


float4 VS_Light(float3 position : POSITION)
    : POSITION
{
    return float4(position, 1);
}

float InverseLerp(float a, float b, float x)
{
    return (x - a) / (b - a);
}

float4 PS_Light(float2 vPos : VPOS)
    : COLOR0
{
    float2 texCoord = vPos / ViewportSize;

    float4 norSpec = tex2D(norSpecSampler, texCoord);

    // unpack normals
    float3 normal = DecodeNormal(norSpec.xy);

    // depth & position
    float depth = tex2D(depthSampler, texCoord).x;
    float4 surfacePosition = ScreenCoordsToWorld(texCoord, depth, InverseViewProjection);

    // Shadows
    float occlusionValue = 1;
        
    if(DrawShadows)
    {
        //transform to light screen-space
        float4 lPosition = mul(surfacePosition, LightViewProjection);
        lPosition /= lPosition.w;
        float2 lTexCoord = (lPosition.xy / 2.0f) + float2(0.5f, 0.5f);
        //lTexCoord.y = 1.0f - lTexCoord.y;

        //pcf shadow mapping (PS3.0)
        occlusionValue = ComputePCF2D(shadowSampler, lTexCoord, lPosition.z, HalfPixel);
    }

    //surface-to-light vector
    float3 surfaceToLight = Position - surfacePosition;

    //compute attenuation based on distance - linear attenuation
    float attenuation = pow(saturate(1.0f - length(surfaceToLight) / Radius), FallOff);

    //normalize light vector
    surfaceToLight = normalize(surfaceToLight);

    //SpotDotLight = cosine of the angle between spotdirection and surfaceToLight
    float SdL = dot(-surfaceToLight, Direction);
    float cosAngle = cos(Angle);

    if(SdL > cosAngle)
    {
        float spotIntensity = pow(abs(InverseLerp(cosAngle, 1, SdL)), RadialFallOff);

        //compute diffuse light
        float NdL = max(0, dot(normal, surfaceToLight));
        float3 diffuseLight = NdL * Color;

        float specularLight = 0.0f;
            
        if(DrawSpecularLight)
        {
            // Specular light
            float3 reflectionVector = normalize(reflect(-surfaceToLight, normal));
            float3 directionToCamera = normalize(CameraPosition - surfacePosition);
            specularLight = norSpec.z * pow(saturate(dot(reflectionVector, directionToCamera)), norSpec.w * 255);
        }

        return float4(diffuseLight, specularLight) * spotIntensity * attenuation * Intensity * occlusionValue;
    }
    else
    {
        return float4(0, 0, 0, 1);
    }
}

technique SpotLight
{
    pass Pass1
    {
        VertexShader = compile vs_3_0 VS_Light();
        PixelShader = compile ps_3_0 PS_Light();
    }
}