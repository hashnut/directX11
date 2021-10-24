#include "stdafx.h"
#include "Execute.h"

#include "Graphics.h"

Execute::Execute()
{
	// 그냥 쓰면 불완전한 형식은 사용할 수 없다고 나옴. 따라서 해당 자료형의 헤더파일을 include 해주어야 한다.
	graphics = new Graphics();
	graphics->Initialize();
	graphics->CreateBackBuffer(static_cast<uint>(Settings::Get().GetWidth()), static_cast<uint>(Settings::Get().GetHeight()));

	// 시계방향으로 회전하는 직각삼각형 만들기
	vertices = new VertexColor[3];

	// Vertex Data
	{

		vertices[0].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
		vertices[0].clear_color = D3DXCOLOR(1.01f, 0.0f, 0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(+0.5f, +0.5f, 0.0f);
		vertices[1].clear_color = D3DXCOLOR(1.01f, 0.0f, 0.0f, 1.0f);

		vertices[2].position = D3DXVECTOR3(+0.5f, -0.5f, 0.0f);
		vertices[2].clear_color = D3DXCOLOR(1.01f, 0.0f, 0.0f, 1.0f);

	}


	// Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		
		// Usage란? : GPU와 CPU 중 누가 데이터를 읽거나 쓸 것인지 결정하는 플래그
		// D3D11_USAGE_DEFAULT : GPU만 데이터를 읽고 쓸 수 있다.
		// D3D11_USAGE_IMMUTABLE : GPU만 데이터를 읽을 수 있다. (쓰기는 불가)
		desc.Usage = D3D11_USAGE_DEFAULT; 
		
		// Buffer형 데이터를 어느 스테이지에서 사용할 건지 지정해주는 플래그다.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// Vertex 버퍼를 어느 정도 크기로 생성할 것인지?
		desc.ByteWidth = sizeof(VertexColor) * 3;

		// 부가적인 데이터.. CPU데이터를 GPU에 연결할 수 있다.
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = vertices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &vertex_buffer);

		assert(SUCCEEDED(hr));
	}

}

Execute::~Execute()
{
	SAFE_RELEASE(vertex_buffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE(graphics);
}

void Execute::Update()
{
}

void Execute::Render()
{
	// stride : 정점 하나의 크기에 대한 정보
	uint stride = sizeof(VertexColor);

	// offset : 정점 정보 중에 어떤 정점부터 사용할 것인지에 대한 정보
	uint offset = 0;

	graphics->Begin();

	// Rendering Part
	// 자원을 만들때는 device, GPU에 연동할 때는 device_context
	{
		graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		
		// TODO : input layout
		
		// 정점을 어떻게 연결할 것인가에 대한 연결성 정보. 여기서는 삼각형으로 이어라...
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)	;

	}

	graphics->End();

}

// 
