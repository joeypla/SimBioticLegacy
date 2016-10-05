struct VertexShaderInput
{
    float3 Position : POSITION0;
};
struct VertexShaderOutput
{
    float4 Position : POSITION0;
};
VertexShaderOutput VertexShaderFunction(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.Position = float4(input.Position,1);
    return output;
}
struct PixelShaderOutput
{
    float4 Color : COLOR0;
    float4 Normal : COLOR1;
    float4 Depth : COLOR2;
	float4 velocity : COLOR3;
};
PixelShaderOutput PixelShaderFunction(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //black color
    output.Color = 0.0f;
    output.Color.a = 0.0f;
    //when transforming 0.5f into [-1,1], we will get 0.0f
    output.Normal.rgb = 0.5f;
    //no specular power
    output.Normal.a = 0.0f;
    //max depth
    output.Depth = 1.0f;

output.velocity = 0.5f;
    return output;
}
technique Technique0
{
    pass Pass0
    {
	ZEnable = false;
	ZWriteEnable = false;
	Cullmode = CW;
        VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
}