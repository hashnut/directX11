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

PixelInput VS(VertexInput input)
{
	PixelInput output;

	output.position = input.position;
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
