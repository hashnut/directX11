struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
    float2 uv : TEXCOORD0;
};

cbuffer TransformBuffer : register(b0) // ->  0 ~ 13 // 4096 * 16byte
{
    matrix world;
    matrix view;
    matrix proj;
};

PixelInput VS(VertexInput input)
{
    // 1 x 4            4 x 4  -----> 1 x 4
    //1, 2, 3, 1   *    0000
    //                  0000
    //                  0000
    //                  0000

    PixelInput output;
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, proj);

    output.uv = input.uv;

    return output;
}

Texture2D source_texture : register(t0);
SamplerState samp        : register(s0);

float4 PS(PixelInput input) : SV_Target
{
    float4 color = source_texture.Sample(samp, input.uv);

    //clip(color.a - 0.9f);

    if(color.a < 0.1f)
        discard;

    return color;
}