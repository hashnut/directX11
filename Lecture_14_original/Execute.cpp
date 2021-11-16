#include "stdafx.h"
#include "Execute.h"
#include "Graphics.h"

Execute::Execute()
{
	graphics = new Graphics();
	graphics->Initialize();
	graphics->CreateBackBuffer(static_cast<uint>(Settings::Get().GetWidth()), static_cast<uint>(Settings::Get().GetHeight()));

	/*
		1	  |      3
			  |
		--------------
			  |
		0	  |      2
	*/
	 
	//Vertex Data
	{
		vertices = new VertexTexture[4];
		vertices[0].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f); //0
		vertices[0].uv = D3DXVECTOR2(0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(-0.5f, +0.5f, 0.0f); //1
		vertices[1].uv = D3DXVECTOR2(0.0f, 0.0f);

		vertices[2].position = D3DXVECTOR3(+0.5f, -0.5f, 0.0f); //2
		vertices[2].uv = D3DXVECTOR2(2.0f, 1.0f);

		vertices[3].position = D3DXVECTOR3(+0.5f, +0.5f, 0.0f); //3
		vertices[3].uv = D3DXVECTOR2(2.0f, 0.0f);
	}
	
	//Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = sizeof(VertexTexture) * 4;

		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = vertices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &vertex_buffer);
		assert(SUCCEEDED(hr));
	}

	//Index Data
	{
		indices = new uint[6]{ 0, 1, 2, 2, 1, 3 };
	}

	//Index Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(uint) * 6;

		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = indices;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &index_buffer);
		assert(SUCCEEDED(hr));
	}

	//Vertex Shader
	{
		auto hr = D3DX11CompileFromFileA
		(
			"Texture.hlsl",
			nullptr,
			nullptr,
			"VS",
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

	//Input Layout
	{
		//D3D11_INPUT_ELEMENT_DESC layout_desc[]
		//{
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//};

		D3D11_INPUT_ELEMENT_DESC layout_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto hr = graphics->GetDevice()->CreateInputLayout(layout_desc, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
		assert(SUCCEEDED(hr));
	}

	//Pixel Shader
	{
		auto hr = D3DX11CompileFromFileA
		(
			"Texture.hlsl",
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

	//Create World View Projection
	{
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);

		D3DXVECTOR3 A = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 B = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 C = D3DXVECTOR3(0, 0, 0);


		D3DXMatrixLookAtLH(&view, &A, &B, &C);
		D3DXMatrixOrthoLH(&projection, Settings::Get().GetWidth(), Settings::Get().GetHeight(), 0, 1);
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(), 0, Settings::Get().GetHeight(), 0, 1);

		/*
		0,0
		              
		        0,0 
				
			
		0,0
		*/



		//Perspective
		//Orthographic

		std::cout << "View Matrix" << std::endl;
		std::cout << view._11 << " " << view._12 << " " << view._13 << " " << view._14 << std::endl;
		std::cout << view._21 << " " << view._22 << " " << view._23 << " " << view._24 << std::endl;
		std::cout << view._31 << " " << view._32 << " " << view._33 << " " << view._34 << std::endl;
		std::cout << view._41 << " " << view._42 << " " << view._43 << " " << view._44 << std::endl;

		std::cout << std::endl;

		std::cout << "Projection Matrix" << std::endl;
		std::cout << projection._11 << " " << projection._12 << " " << projection._13 << " " << projection._14 << std::endl;
		std::cout << projection._21 << " " << projection._22 << " " << projection._23 << " " << projection._24 << std::endl;
		std::cout << projection._31 << " " << projection._32 << " " << projection._33 << " " << projection._34 << std::endl;
		std::cout << projection._41 << " " << projection._42 << " " << projection._43 << " " << projection._44 << std::endl;

		std::cout << std::endl;

		//World
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 500, 500, 1);

		std::cout << "Scale Matrix" << std::endl;
		std::cout << S._11 << " " << S._12 << " " << S._13 << " " << S._14 << std::endl;
		std::cout << S._21 << " " << S._22 << " " << S._23 << " " << S._24 << std::endl;
		std::cout << S._31 << " " << S._32 << " " << S._33 << " " << S._34 << std::endl;
		std::cout << S._41 << " " << S._42 << " " << S._43 << " " << S._44 << std::endl;

		std::cout << std::endl;

		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, 0, 0, 0);

		std::cout << "Translation Matrix" << std::endl;
		std::cout << T._11 << " " << T._12 << " " << T._13 << " " << T._14 << std::endl;
		std::cout << T._21 << " " << T._22 << " " << T._23 << " " << T._24 << std::endl;
		std::cout << T._31 << " " << T._32 << " " << T._33 << " " << T._34 << std::endl;
		std::cout << T._41 << " " << T._42 << " " << T._43 << " " << T._44 << std::endl;

		std::cout << std::endl;

		D3DXMATRIX R;
		D3DXMatrixRotationZ(&R, static_cast<float>(D3DXToRadian(45)));

		std::cout << "Rotation Matrix" << std::endl;
		std::cout << R._11 << " " << R._12 << " " << R._13 << " " << R._14 << std::endl;
		std::cout << R._21 << " " << R._22 << " " << R._23 << " " << R._24 << std::endl;
		std::cout << R._31 << " " << R._32 << " " << R._33 << " " << R._34 << std::endl;
		std::cout << R._41 << " " << R._42 << " " << R._43 << " " << R._44 << std::endl;

		//�� * �� * ��  * �� * ��
		//S * R * T * R * P  

		world = S * /*R **/ T; //Ctrl + K + C
	}

	//Create Constant Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC; //CPU - Write //GPU - Read
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TRANASFROM_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &gpu_buffer);
		assert(SUCCEEDED(hr));
	}

	//Create Raterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;

		//Clipping
		//Culling

		auto hr = graphics->GetDevice()->CreateRasterizerState(&desc, &rasterizer_state);
		assert(SUCCEEDED(hr));
	}

	//Create Shader Resource View
	{
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			graphics->GetDevice(),
			"Tree.png",
			nullptr,
			nullptr,
			&shader_resource,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}
}

Execute::~Execute()
{
	SAFE_RELEASE(shader_resource);

	SAFE_RELEASE(rasterizer_state);

	SAFE_RELEASE(gpu_buffer);

	SAFE_RELEASE(pixel_shader);
	SAFE_RELEASE(ps_blob);

	SAFE_RELEASE(input_layout);

	SAFE_RELEASE(vertex_shader);
	SAFE_RELEASE(vs_blob);

	SAFE_RELEASE(index_buffer);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertex_buffer);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(graphics);
}

void Execute::Update()
{
	//world._11 = 50;
	//world._22 = 50;

	//world._41 = 100;
	//world._42 = 100;	

	//D3DXMATRIX			- �� �켱 ���
	//GPU - shader - matrix - �� �켱 ���

	//1, 0, 0, 0
	//0
	//0
	//0
	
	//static float radian = 0.0f;
	//radian += 0.01f;
	//
	//D3DXMATRIX P;
	//D3DXMatrixRotationZ(&P, radian);
	//
	//world = world * P;


	//��ġ��� - Transpose

	D3DXMatrixTranspose(&cpu_buffer.world, &world);
	D3DXMatrixTranspose(&cpu_buffer.view, &view);
	D3DXMatrixTranspose(&cpu_buffer.projection, &projection);

	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	graphics->GetDeviceContext()->Map
	(
		gpu_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_subresource
	);

	memcpy(mapped_subresource.pData, &cpu_buffer, sizeof(TRANASFROM_DATA));

	graphics->GetDeviceContext()->Unmap(gpu_buffer, 0);
}

void Execute::Render()
{
	uint stride = sizeof(VertexTexture);
	uint offset = 0;

	graphics->Begin();
	{
		//IA
		graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		graphics->GetDeviceContext()->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
		graphics->GetDeviceContext()->IASetInputLayout(input_layout);
		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//VS
		graphics->GetDeviceContext()->VSSetShader(vertex_shader, nullptr, 0);
		graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &gpu_buffer);

		//RS
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);

		//PS
		graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);
		graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &shader_resource);

		graphics->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
	graphics->End();
}

//-0.5, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f

//       o

//0      0