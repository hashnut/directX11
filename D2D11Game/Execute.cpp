#include "stdafx.h"
#include "Execute.h"

#include "Graphics.h"

Execute::Execute()
{
	// �׳� ���� �ҿ����� ������ ����� �� ���ٰ� ����. ���� �ش� �ڷ����� ��������� include ���־�� �Ѵ�.
	graphics = new Graphics();
	graphics->Initialize();
	graphics->CreateBackBuffer(static_cast<uint>(Settings::Get().GetWidth()), static_cast<uint>(Settings::Get().GetHeight()));

	// �ð�������� ȸ���ϴ� �����ﰢ�� �����
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
		
		// Usage��? : GPU�� CPU �� ���� �����͸� �аų� �� ������ �����ϴ� �÷���
		// D3D11_USAGE_DEFAULT : GPU�� �����͸� �а� �� �� �ִ�.
		// D3D11_USAGE_IMMUTABLE : GPU�� �����͸� ���� �� �ִ�. (����� �Ұ�)
		desc.Usage = D3D11_USAGE_DEFAULT; 
		
		// Buffer�� �����͸� ��� ������������ ����� ���� �������ִ� �÷��״�.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// Vertex ���۸� ��� ���� ũ��� ������ ������?
		desc.ByteWidth = sizeof(VertexColor) * 4;

		// �ΰ����� ������.. CPU�����͸� GPU�� ������ �� �ִ�.
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
		// D3DX11�� �����ϴ� �Լ����� �ֽ� DirectX SDK���� �������� �ʴ´�.
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
	// stride : ���� �ϳ��� ũ�⿡ ���� ����
	uint stride = sizeof(VertexColor);

	// offset : ���� ���� �߿� � �������� ����� �������� ���� ����
	uint offset = 0;

	graphics->Begin();

	// Rendering Part
	// �ڿ��� ���鶧�� device, GPU�� ������ ���� device_context
	{
		// IA
		graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		graphics->GetDeviceContext()->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
		graphics->GetDeviceContext()->IASetInputLayout(input_layout);

		// ������ ��� ������ ���ΰ��� ���� ���Ἲ ����. ���⼭�� �ﰢ������ �̾��...
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)	;


		// VS : ������ ������ŭ ȣ��ȴ�.
		graphics->GetDeviceContext()->VSSetShader(vertex_shader, nullptr, 0);

		// PS
		graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);

		// Rendering Pipeline�� ������ �����ٸ� �׻� Draw �Լ��� ȣ��ž� �׷�����.
		graphics->GetDeviceContext()->DrawIndexed(10, 0, 0);

	}
	graphics->End();

}

// 
