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
		
		// Usage��? : GPU�� CPU �� ���� �����͸� �аų� �� ������ �����ϴ� �÷���
		// D3D11_USAGE_DEFAULT : GPU�� �����͸� �а� �� �� �ִ�.
		// D3D11_USAGE_IMMUTABLE : GPU�� �����͸� ���� �� �ִ�. (����� �Ұ�)
		desc.Usage = D3D11_USAGE_DEFAULT; 
		
		// Buffer�� �����͸� ��� ������������ ����� ���� �������ִ� �÷��״�.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// Vertex ���۸� ��� ���� ũ��� ������ ������?
		desc.ByteWidth = sizeof(VertexColor) * 3;

		// �ΰ����� ������.. CPU�����͸� GPU�� ������ �� �ִ�.
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
	// stride : ���� �ϳ��� ũ�⿡ ���� ����
	uint stride = sizeof(VertexColor);

	// offset : ���� ���� �߿� � �������� ����� �������� ���� ����
	uint offset = 0;

	graphics->Begin();

	// Rendering Part
	// �ڿ��� ���鶧�� device, GPU�� ������ ���� device_context
	{
		graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		
		// TODO : input layout
		
		// ������ ��� ������ ���ΰ��� ���� ���Ἲ ����. ���⼭�� �ﰢ������ �̾��...
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)	;

	}

	graphics->End();

}

// 
