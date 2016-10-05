static const float strength = 5.0;
static const float2 offset  = float2( 1280.0f, 800.0f );
static const float falloff  = 0.0002;
static const float rad = 0.008;

float4 viewRay;
float4x4 inverseView;
#define NUM_SAMPLES	 64
static const float invSamples = 1.0 / (float)NUM_SAMPLES;

// AO sampling directions 
/*static const float3 AO_SAMPLES[ NUM_SAMPLES ] = 
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
};*/

float4 kernelOffsets[64]; 
// Textures.
//
texture tex0;    // Normal.xyz
texture tex1;    //DEPTH 32bit z / w (NON-LINEAR)
texture tex2;    // Random noise texture
sampler defss0 = sampler_state
{
    Texture = <tex0>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler defss1 = sampler_state
{
    Texture = <tex1>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler defss2 = sampler_state
{
    Texture = <tex2>;
    AddressU = WRAP;
    AddressV = WRAP;
    MagFilter = POINT;
    MinFilter = POINT;
    MipFilter = NONE;
};



// Vertex Shader.
//
struct VSInput
{
    float3 Position    :POSITION0;
    float2 Texture0    :TEXCOORD0;
};

struct VSOutput
{
    float4 Position    :POSITION0;
    float2 Texture0    :TEXCOORD0;
};

VSOutput mainVS( VSInput input )
{
    VSOutput output;

    output.Position = float4(input.Position, 1);
    output.Texture0 = input.Texture0;

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
    float3 fres = normalize( tex2D( defss2, input.Texture0).xyz * 2.0 - 1.0 );

    
    float3 normal = tex2D( defss0, input.Texture0 ).xyz * 2.0 - 1.0;
    normal = mul(normal, inverseView);
    float  depth  = LinearizeDepth(tex2D( defss1, input.Texture0 ).r);
    

    float3 ep = float3( input.Texture0, depth );
    float bl = 0.0;
    float radD = rad / depth;

    float3 ray;
    float4 occFrag;
    float  depthDiff;

    for( int i = 0; i < 64; ++i )
    {
        ray = radD * reflect( kernelOffsets[i], fres );//projected ray for sampling in screen space away from current pixel
        //occFrag.a = LinearizeDepth(tex2D(defss1, ep.xy + sign(dot(ray, normal)) * ray.xy));//depth of sampled pixel
	//occFrag.xyz = tex2D(defss0, ep.xy + sign(dot(ray, normal)) * ray.xy);//normal of sampled pixel

	occFrag.a = LinearizeDepth(tex2D(defss1, ep.xy + ray.xy));//depth of sampled pixel
	occFrag.xyz = tex2D(defss0, ep.xy + ray.xy);//normal of sampled pixel


        depthDiff = depth - occFrag.a;

        //bl += step( falloff, depthDiff ) * (1.0 - dot( occFrag.xyz, normal )) * (1.0 - smoothstep( falloff, strength, depthDiff ));
	bl += clamp(step( falloff, depthDiff ) * (1.0 - smoothstep( falloff, strength, depthDiff )) - 0.5f, 0.0f, 1.0f);
	
    }

    float ao = 1.0 - bl * invSamples;
    //return float4(1.0, 1.0, 1.0, 1.0);
    return float4( ao, ao, ao, 1.0 );
}

technique technique0
{
    pass p0
    {
	Cullmode = CW;
        ZEnable = false;
	ZWriteEnable = false;
	//AlphaBlendEnable = false;
	VertexShader = compile vs_3_0 mainVS();
	PixelShader = compile ps_3_0 mainPS();
    }
}