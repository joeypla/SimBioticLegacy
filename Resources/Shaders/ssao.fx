static const float strength = 0.2;
static const float2 noiseScale  = float2( 2560.0 / 4.0, 1600.0 / 4.0 );
static const float falloff  = 0.0002;
static const float rad = 0.002;

float4 viewRay;
float4x4 inverseView;
float4x4 viewProjectionMatrix;
float4x4 projectionMatrix;
float4x4 inverseProjectionMatrix;
float4x4 inverseViewProjection;
float radius;
#define NUM_SAMPLES	 10
static const float invSamples = 1.0 / (float)NUM_SAMPLES;

// AO sampling directions 
static const float3 AO_SAMPLES[ NUM_SAMPLES ] = 
{
    float3(-0.010735935,  0.0164701800,  0.0062425877),
    float3(-0.065333690,  0.3647007000, -0.1374632100),
    float3(-0.653923500, -0.0167263880, -0.5300095700),
    float3( 0.409582850,  0.0052428036, -0.5591124000),
    float3(-0.146536600,  0.0989926700,  0.1557167900),
    float3(-0.441221120, -0.5458797000,  0.0491253200),
    float3( 0.037555660, -0.1096134500, -0.3304027300),
    float3( 0.019100213,  0.2965278300,  0.0662376660),
    float3( 0.876532300,  0.0112360040,  0.2826596200),
    float3( 0.292644350, -0.4079423800,  0.1596416700)
};

float4 kernelOffsets[64]; 
// Textures.
//
texture tex0;    // Normal.xyz
texture tex1;    //DEPTH 32bit z / w (NON-LINEAR)
texture tex2;    // Random noise texture
sampler normalSampler = sampler_state
{
    Texture = <tex0>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler depthSampler = sampler_state
{
    Texture = <tex1>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler randomNoiseSampler = sampler_state
{
    Texture = <tex2>;
    AddressU = WRAP;
    AddressV = WRAP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = POINT;
};



// Vertex Shader.
//
struct VSInput
{
    float3 Position    :POSITION0;
    float2 UV    :TEXCOORD0;
};

struct VSOutput
{
    float4 Position    :POSITION0;
    float2 UV    :TEXCOORD0;
};

VSOutput mainVS( VSInput input )
{
    VSOutput output;

    output.Position = float4(input.Position, 1);
    output.UV = input.UV;

    return output;
}

float LinearizeDepth(float z)
{
	float nearPlane = 0.1f;
	float farPlane = 120.0f;
	return (2 * nearPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

float4 mainPS( VSOutput input ): COLOR0
{
    float depth = tex2D(depthSampler, input.UV);
    float4 vpOrigin = 1.0f;
    vpOrigin.x = input.UV.x * 2.0f - 1.0f;
    vpOrigin.y = (input.UV.y) * 2.0f - 1.0f;
    vpOrigin.z = tex2D(depthSampler, input.UV).x;
    vpOrigin = mul(vpOrigin, inverseProjectionMatrix);
    vpOrigin.xyz /= vpOrigin.w;

    
    float3 normal = tex2D(normalSampler, input.UV).xyz *  2.0f - 1.0f;
    normal = normalize(normal);

    float3 randomVector = tex2D(randomNoiseSampler, input.UV * noiseScale).xyz * 2.0f - 1.0f;
    float3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 tbnMatrix = float3x3(tangent, bitangent, normal);

    float subjectDepth = depth;
    float occlusion = 0.0f;
    float numImportantSamples = 32.0f;
    for (int i = 0; i < 64; i++)
    {
	float3 samplePosition = kernelOffsets[i];
	//float3 samplePosition = mul(tbnMatrix, kernelOffsets[i]);
	samplePosition = samplePosition * radius + vpOrigin;
	
	float4 offset = float4(samplePosition, 1);
	offset = mul(offset, projectionMatrix);
	offset.xy /= offset.w;
	offset.xy = offset.xy * 0.5f + 0.5f;

	float sampleDepth = tex2D(depthSampler, offset.xy);
	
	float rangeCheck = abs(LinearizeDepth(subjectDepth) - LinearizeDepth(sampleDepth));
        if (rangeCheck <= 0.002f)
        {
		rangeCheck = 1.0f;
        } else
	{
		rangeCheck = 0.0f;
		numImportantSamples -= 1.0f;
	}
	occlusion += ((sampleDepth <= offset.z) ? 1.0f : 0.0f);
    }
    occlusion /= 64.0f;//numImportantSamples;
    
    return float4( 1.0f - (occlusion), 1.0f - (occlusion), 1.0f - (occlusion), 1.0f - (occlusion));
}

technique technique0
{
    pass p0
    {
	Cullmode = CW;
        ZEnable = false;
	ZWriteEnable = false;
	VertexShader = compile vs_3_0 mainVS();
	PixelShader = compile ps_3_0 mainPS();
    }
}