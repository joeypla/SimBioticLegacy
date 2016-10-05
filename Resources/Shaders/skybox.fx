
float4x4 world;
float4x4 view;
float4x4 projection;
//old is for last transform - velocity map
float4x4 oldWorldViewProjection;


float4x4 worldViewIT;

sampler colorSampler : register(s0);
sampler normalSampler : register(s1);
sampler specularSampler : register(s2);

bool wallhacked;
float3 wallhackedColor;
struct VSI
{
	float4 pos : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float3 tangent : TANGENT0;
	float3 biTangent : BINORMAL0;
};
struct VSO
{
	float4 pos : POSITION;
	
	float2 uv : TEXCOORD0;
	float2 depth : TEXCOORD1;
	float3x3 tbn : TEXCOORD2;
	float4 newPos : TEXCOORD5;
	float4 oldPos : TEXCOORD6;
};

VSO mainVS(VSI input)
{
	VSO output;
	//transform
	float4 worldPos = mul(input.pos, world);
	float4 viewPos = mul(worldPos, view);
	output.pos = mul(viewPos, projection);
	output.newPos = output.pos;
	//old transform

	output.oldPos = mul(input.pos, oldWorldViewProjection);

	//Depth
	output.depth.x = output.pos.z;
	output.depth.y = output.pos.w;
	
	
	//normal matrix
	output.tbn[0] = normalize(mul(input.tangent, (float3x3)worldViewIT));
	output.tbn[1] = normalize(mul(input.biTangent, (float3x3)worldViewIT));
	output.tbn[2] = normalize(mul(input.normal, (float3x3)worldViewIT));
	
	//UV
	output.uv = input.uv;
	return output;
}

//Pixel Shader
struct PSO
{
	float4 color : COLOR0;
	float4 normal : COLOR1;
	float4 depth : COLOR2;
	float4 velocity : COLOR3;
};

half3 encode(half3 n)
{
	n = normalize(n);
	n.xyz = 0.5f * ( n.xyz + 1.0f);
	return n;
}
half3 decode(float4 enc)
{
	return (2.0f*enc.xyz - 1.0f);
};


PSO mainPS(VSO input){
	PSO output;
	//color buffer holds RGB and specular values - w = luminance
	output.color = tex2D(colorSampler, input.uv);
	output.color.w = tex2D(specularSampler, input.uv).z;
	if (wallhacked)
	{
		output.color.xyz = wallhackedColor;
		output.color.w = 1.0f;
	}
	//normal buffer holds normalXYZ
	half3 normal = tex2D(normalSampler, input.uv).xyz * 2.0f - 1.0f;
	normal = mul(normal, input.tbn);
	output.normal.xyz = encode(normal);
	output.normal.w = tex2D(specularSampler, input.uv).y;
	//output.normal = tex2D(normalSampler, input.uv);
	//output.normal.xyz = encode(normalize(input.tbn[2]));
	
	//depth - 32 bit floating point non-linear depth
	float depth = input.depth.x /input.depth.y;//Normal depth
	//float depth = input.depth.x/120.0f;//Linear depth
	output.depth = float4(depth, depth, depth, depth);
	
	//velocity - x y = vel, z w = specular components
	float2 a = (input.newPos.xy / input.newPos.w);// * 0.5f + 1.0f;
	float2 b = (input.oldPos.xy / input.oldPos.w);// * 0.5f + 1.0f;
	float2 velocity = (a - b)*0.5f;// * 0.5f + 1.0f;
	output.velocity.xy = velocity*0.5f + 0.5f;//pow(velocity, 3.0f);
	output.velocity.z = tex2D(specularSampler, input.uv).x;
	output.velocity.w = tex2D(specularSampler, input.uv).y;
	return output;
	
}

technique technique0 {
	pass p0 {
		ZWriteEnable = true;
		//ZEnable = true; Will be set through the fixed function pipeline
		AlphaBlendEnable = false;
		CullMode = CCW;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}