#pragma once

struct VertexColor
{
	D3DXVECTOR3 position; 
	D3DXCOLOR clear_color;

};

class Execute final
{
public:
	Execute();
	~Execute();

	void Update();
	void Render();

private:
	class Graphics* graphics = nullptr;

	// VertexColor는 우리가 만든 데이터... Ram에 들어감.
	
	VertexColor* vertices = nullptr;

	// CPU 자원을 GPU에 입력하기 위해 변환해야 한다!
	ID3D11Buffer* vertex_buffer = nullptr;

	uint* indices = nullptr;
	ID3D11Buffer* index_buffer = nullptr;


	// 0, 0, 0, 0, 0, 0, 0 이런 식으로 좌표 3개와 색깔 데이터 4개가 들어왔을때...?
	// 컴퓨터가 어떻게 읽고 구획을 나누어야 하는지 -> Layout으로 설정!
	// Layout 데이터는 혼자 생성될 수가 없다. 단계 별로 일치해야 한다는 뜻.
	ID3D11InputLayout* input_layout = nullptr;

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3DBlob* vs_blob = nullptr; // Blob : Binary Large Object

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* ps_blob = nullptr;
};

// Rendering Pipeline : 렌더링을 하기 위해 수행되어야 하는 단계들
// - IA - VS - RS - PS - OM (IA, RS, OM 단계는 코딩이 불가능하다! VS, PS 단계는 코딩이 가능하다!)

// IA : Input Assembler Stage : 기본 데이터가 들어가는 입력 조립기 단계
// - Vertex : DirectX의 기본 데이터는 정점이다! - 사용자마다 정보가 다양하기에 기본 자료형을 제공하지 않는다! -> 커스터마이징 필요

// VS : Vertex Shader. Shader : GPU를 동작시키는 함수들의 집합
// - Shader : HLSL(High Level Shader Language), GLSL
// - Transform(VS 단계에서는 정점들의 공간 변환을 해준다!)
// Local : 설계도 내의 공간
// World : 세계 공간
// View : 보여지는 영역 (시야 공간)
// Projection : 투영 공간 - 차원을 낮추는 개념. 3D 데이터를 2D 데이터로 변환시키는 것
// Clip : 위의 공간들을 정규화시킨다.(0~1) 말 그대로 오리는 느낌. - Homogeneous space : 동차 (1, 0, 0, 0) 마지막 값을 w라고 하는데...
// w - 동차가 1이면 위치, 0이면 방향을 표현
// DirectX에서는 Projection과 Clip이 결합되어 있다.

// RS : Rasterizer : 3D 데이터를 2D 데이터로 바꿔주는 것 
// - VS 단계에서 정규화한 공간을 RS에서 다시 늘려준다!
// - NDC : Normalized Device Coordinate : 화면에 맞게 늘려짐
// NDC - Euclidean space 
// Viewport : 정규화된 크기를 다시 Rasterized된 크기로 변환해 줌



