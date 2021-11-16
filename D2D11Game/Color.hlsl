struct VertexInput
{
	// float4�� vector4�� ����
	// Semantic : �������� ��ó�� ���ҿ� ���� �и��� �ǹ̸� �ο��ϱ� ���� Ű����
	float4 position : POSITION0; // POSITION�� Semantic, 0�� Semantic Index
	float4 color : COLOR0;
};

struct PixelInput
{
	// SV : System Value - �߿��� ��... 
	float4 position : SV_POSITION0;
	float4 color : COLOR0;
};

// cbuffer : Constant buffer
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
	output.color = input.color;
	
	return output;
}

// shader�� �������� �������� ������ �ϱ⿡ �ϳ��� ���Ͽ� PixelShaer, VertexShader ��� �ۼ��ص� ������

// ������ �Լ�
// Pixel Shader : Rasterizer�� ������ �ȼ� ������ŭ ȣ��
float4 PS(PixelInput input) : SV_Target // SV_Target[n] �ø�ƽ���� ���� ���õ� ���� ������ Ÿ���� ����϶�� �˷��ش�.
{
	return input.color;
	
}
