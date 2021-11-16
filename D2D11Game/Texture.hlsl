struct VertexInput
{
	// float4�� vector4�� ����
	// Semantic : �������� ��ó�� ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����
	float4 position : POSITION0; // POSITION�� Semantic, 0�� Semantic Index
	float2 uv : TEXCORD0;
};

struct PixelInput
{
	// SV : System Value - �߿��� ��... 
	float4 position : SV_POSITION0;
	float2 uv : TEXCORD0;
};	

// cbuffer : Constant buffer // register(b0)���� b�� �ǹ��ϴ� �� buffer!
cbuffer TransformBuffer : register(b0) // -> 0 ~ 13 (���� ��ȣ) // (4096 *) 16 byte ������ �����ؾ� �Ѵ�.
{
	matrix world;
	matrix view;
	matrix proj;
}


PixelInput VS(VertexInput input)
{
	// 1 X 4			4 X 4	----> 1 X 4
	// 1, 2, 3, 1	*	0000
	//					0000
	//					0000
	//					0000
	
	PixelInput output;

	// ��ġ ��ȯ�� �� ����� �����ָ� �ȴ�!
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);
	output.uv = input.uv;
	
	return output;
}


// t0 : �ؽ��� �ڿ��̶�� �ǹ��� t
Texture2D source_texture	: register(t0); // ������ �ȼ� �����Ͱ� �� ����
SamplerState samp			: register(s0);

// shader�� �������� �������� ������ �ϱ⿡ �ϳ��� ���Ͽ� PixelShaer, VertexShader ��� �ۼ��ص� ������

// ������ �Լ�
// Pixel Shader : Rasterizer�� ������ �ȼ� ������ŭ ȣ��
// Sampling : ������ �����͸� �����ϴ� �� 
float4 PS(PixelInput input) : SV_Target // SV_Target[n] �ø�ƽ���� ���� ���õ� ���� ������ Ÿ���� ����϶�� �˷��ش�.
{
	float4 color = source_texture.Sample(samp, input.uv);
	
	// HLSL�� clip() �Լ��� ���ڷ� �޴� �����Ͱ� 0���� ������ �ش� �����͸� ����Ѵ�
	// clip(color.a - 0.9f);
	
	if (color.a < 0.1f)
		discard;
	
	
	return color;
}
