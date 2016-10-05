float4x4 world;
float4x4 view;
float4x4 projection;

float3 lightPosition;

float depthPrecision;

struct VSI
{
	float4 position : POSITION0;
};

struct VSO
{
	float4 position : POSITION0;
	float4 worldPosition : POSITION1; 
	float2 depth : TEXCOORD0;
};

VSO VS(VSI input)
{
	VSO output;
	float4 worldPosition = mul(input.position, world);
	float4 viewPos = mul(worldPosition, view);
	output.position = mul(viewPos, projection);
	
	output.worldPosition = worldPosition;
	output.depth.x = viewPos.z;
	output.depth.y = viewPos.z;
	return output;
}

float4 PS(VSO input) : COLOR0
{

	//This stores non-linear depth
	//float depth = input.depth.x / input.depth.y;
	float depth = input.depth.x / depthPrecision;
	float depthSquared = depth * depth;
	return float4(depth, depthSquared, depth, depthSquared);

	//This stores linear depth
	//float depth = (input.depth.x - 0.1f)/(120.0f - 0.1f);
	//float depth = input.worldPosition.z;
	//return float4(depth, depth, depth, depth);
	
}

technique technique0
{
	pass p0
	{
		Cullmode = None;
		ZEnable = true;
		ZWriteEnable = true;
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
	}
}