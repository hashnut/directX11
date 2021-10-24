#pragma once

// Rendering Pipeline : 렌더링을 하기 위해 수행되어야 하는 단계들
// IA - VS - RS - PS - OM

// IA : Input Assembler Stage : 기본 데이터가 들어가는 입력 조립기 단계
// - Vertex : DirectX의 기본 데이터는 정점이다! - 사용자마다 정보가 다양하기에 기본 자료형을 제공하지 않는다! -> 커스터마이징 필요

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

	// 0, 0, 0, 0, 0, 0, 0 이런 식으로 좌표 3개와 색깔 데이터 4개가 들어왔을때...?
	// 컴퓨터가 어떻게 읽고 구획을 나누어야 하는지 -> Layout으로 설정!
	// Layout 데이터는 혼자 생성될 수가 없다. 단계 별로 일치해야 한다는 뜻.
	ID3D11InputLayout* input_layout = nullptr;

};

