struct VertexInput
{
	// float4는 vector4와 같음
	// Semantic : 데이터의 출처와 역할에 대한 분명한 의미를 부여하기 위한 키워드
	float4 position : POSITION0; // POSITION이 Semantic, 0가 Semantic Index
	float2 uv : TEXCORD0;
};

struct PixelInput
{
	// SV : System Value - 중요한 값... 
	float4 position : SV_POSITION0;
	float2 uv : TEXCORD0;
};	

// cbuffer : Constant buffer // register(b0)에서 b가 의미하는 건 buffer!
cbuffer TransformBuffer : register(b0) // -> 0 ~ 13 (슬롯 번호) // (4096 *) 16 byte 단위로 정렬해야 한다.
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

	// 위치 변환할 때 행렬을 곱해주면 된다!
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);
	output.uv = input.uv;
	
	return output;
}


// t0 : 텍스쳐 자원이라는 의미의 t
Texture2D source_texture	: register(t0); // 원본의 픽셀 데이터가 들어가 있음
SamplerState samp			: register(s0);

// shader는 진입점을 기준으로 컴파일 하기에 하나의 파일에 PixelShaer, VertexShader 모두 작성해도 괜찮음

// 진입점 함수
// Pixel Shader : Rasterizer가 지정한 픽셀 개수만큼 호출
// Sampling : 임의의 데이터를 추출하는 것 
float4 PS(PixelInput input) : SV_Target // SV_Target[n] 시멘틱으로 현재 세팅된 메인 렌더러 타겟을 사용하라고 알려준다.
{
	float4 color = source_texture.Sample(samp, input.uv);
	
	// HLSL에 clip() 함수는 인자로 받는 데이터가 0보다 작으면 해당 데이터를 폐기한다
	// clip(color.a - 0.9f);
	
	if (color.a < 0.1f)
		discard;
	
	
	return color;
}
