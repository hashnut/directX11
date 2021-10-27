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
		vertices = new VertexColor[4];

		vertices[0].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);			//0
		vertices[0].clear_color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(-0.5f, +0.5f, 0.0f);			// 1
		vertices[1].clear_color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[2].position = D3DXVECTOR3(+0.5f, -0.5f, 0.0f);			// 2
		vertices[2].clear_color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		vertices[3].position = D3DXVECTOR3(+0.5f, +0.5f, 0.0f);			// 3
		vertices[3].clear_color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

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
		desc.ByteWidth = sizeof(VertexColor) * 4;

		// 부가적인 데이터.. CPU데이터를 GPU에 연결할 수 있다.
		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = vertices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &vertex_buffer);

		assert(SUCCEEDED(hr));
	}

	// Index data
	{
		indices = new uint[6]{0, 1, 2, 2, 1, 3};
		
	}

	// Index Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(uint) * 6;

		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = indices;

		auto hr = graphics->GetDevice()->CreateBuffer
		(
			&desc,
			&sub_data,
			&index_buffer
		);

		assert(SUCCEEDED(hr));
	}

	// Vertex Shader
	{
		// D3DX11로 시작하는 함수들은 최신 DirectX SDK에는 지원하지 않는다.
		auto hr = D3DX11CompileFromFileA
		(
			"Color.hlsl",
			nullptr,
			nullptr,
			"VS", // entry point
			"vs_5_0",
			0,
			0,
			nullptr,
			&vs_blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = graphics->GetDevice()->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);
		assert(SUCCEEDED(hr));

	}


	// Input Layout
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[]
		{  
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto hr = graphics->GetDevice()->CreateInputLayout(layout_desc, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
		assert(SUCCEEDED(hr));
	}

	// Pixel Shader
	{
		auto hr = D3DX11CompileFromFileA
		(
			"Color.hlsl",
			nullptr,
			nullptr,
			"PS",
			"ps_5_0",
			0,
			0,
			nullptr,
			&ps_blob,
			nullptr,
			nullptr
		);

		assert(SUCCEEDED(hr));

		hr = graphics->GetDevice()->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);

		assert(SUCCEEDED(hr));

	}
}

Execute::~Execute()
{


	SAFE_RELEASE(pixel_shader);
	SAFE_RELEASE(ps_blob); 

	SAFE_RELEASE(input_layout);

	SAFE_RELEASE(vertex_shader);
	SAFE_RELEASE(vs_blob);

	SAFE_DELETE_ARRAY(indices);
	SAFE_RELEASE(index_buffer);

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
		// IA
		graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		graphics->GetDeviceContext()->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
		graphics->GetDeviceContext()->IASetInputLayout(input_layout);

		// 정점을 어떻게 연결할 것인가에 대한 연결성 정보. 여기서는 삼각형으로 이어라...
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)	;


		// VS : 정점의 개수만큼 호출된다.
		graphics->GetDeviceContext()->VSSetShader(vertex_shader, nullptr, 0);

		// PS
		graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);

		// Rendering Pipeline에 세팅이 끝났다면 항상 Draw 함수가 호출돼야 그려진다.
		graphics->GetDeviceContext()->DrawIndexed(10, 0, 0);

	}
	graphics->End();

}

// 
