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

	// Vertex Data
	{
		vertices = new VertexTexture[4];

		vertices[0].position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);			//0
		vertices[0].uv = D3DXVECTOR2(0.0f, 1.0f);

		vertices[1].position = D3DXVECTOR3(-0.5f, +0.5f, 0.0f);			// 1
		vertices[1].uv = D3DXVECTOR2(0.0f, 0.0f);

		vertices[2].position = D3DXVECTOR3(+0.5f, -0.5f, 0.0f);			// 2
		vertices[2].uv = D3DXVECTOR2(1.0f, 1.0f);

		vertices[3].position = D3DXVECTOR3(+0.5f, +0.5f, 0.0f);			// 3
		vertices[3].uv = D3DXVECTOR2(1.0f, 0.0f);

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
		desc.ByteWidth = sizeof(VertexTexture) * 4;

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
			"Texture.hlsl",
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
	// ������ ������ �ٲ�� Input Layout�� �ٲ��� �Ѵ�!
	{

		//D3D11_INPUT_ELEMENT_DESC layout_desc[]
		//{  
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//};

		D3D11_INPUT_ELEMENT_DESC layout_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto hr = graphics->GetDevice()->CreateInputLayout(layout_desc, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
 		assert(SUCCEEDED(hr));
	}

	// Pixel Shader
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

	// Create World View Projection
	{
		// �Լ��� Ȱ���ؼ� ��� ����� ������ķ� ������ֱ�
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);

		D3DXVECTOR3 LHeye(0, 0, 0);
		D3DXVECTOR3 LHat(0, 0, 1);
		D3DXVECTOR3 LHup(0, 1, 0);

		D3DXMatrixLookAtLH(&view, &LHeye, &LHat, &LHup);

		// ���� ��� :
		// Perspective : ���ٰ��� �������� ����, ����ü�� ǥ���ȴ�.
		// Orthographic : ���� ����, ���ٰ��� �������� ����. ���������� �þ߰����� ������ü�� ǥ���ȴ�.
		D3DXMatrixOrthoLH(&projection, Settings::Get().GetWidth(), Settings::Get().GetHeight(), 0, 1);
		 
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(), Settings::Get().GetHeight(), 0, 0, 1);

		std::cout << "View Matrix" << std::endl;
		// D3DMATRIX ���� ����
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


		// World : Scaling�� ������ ũ�⸦ �ٲ� �� �ִ�.
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 500, 500, 1);

		std::cout << "Scale Matrix" << std::endl;
		std::cout << S._11 << " " << S._12 << " " << S._13 << " " << S._14 << std::endl;
		std::cout << S._21 << " " << S._22 << " " << S._23 << " " << S._24 << std::endl;
		std::cout << S._31 << " " << S._32 << " " << S._33 << " " << S._34 << std::endl;
		std::cout << S._41 << " " << S._42 << " " << S._43 << " " << S._44 << std::endl;

		std::cout << std::endl;
		
		// Translation : ������ �̵�������. ���� ������ ������ ������� ����
		D3DXMATRIX T;
		float t_x = 100, t_y = 100;
		t_x += 0.01; t_y += 0.01;
		D3DXMatrixTranslation(&T, 0, 0, 0);

		std::cout << "Translation Matrix" << std::endl;
		std::cout << T._11 << " " << T._12 << " " << T._13 << " " << T._14 << std::endl;
		std::cout << T._21 << " " << T._22 << " " << T._23 << " " << T._24 << std::endl;
		std::cout << T._31 << " " << T._32 << " " << T._33 << " " << T._34 << std::endl;
		std::cout << T._41 << " " << T._42 << " " << T._43 << " " << T._44 << std::endl;

		std::cout << std::endl;

		D3DXMATRIX R;
		// ȸ���� �߽��� �Ǵ� ���� �����̴�!
		D3DXMatrixRotationZ(&R, static_cast<float>D3DXToRadian(45));

		std::cout << "Rotation Matrix" << std::endl;
		std::cout << R._11 << " " << R._12 << " " << R._13 << " " << R._14 << std::endl;
		std::cout << R._21 << " " << R._22 << " " << R._23 << " " << R._24 << std::endl;
		std::cout << R._31 << " " << R._32 << " " << R._33 << " " << R._34 << std::endl;
		std::cout << R._41 << " " << R._42 << " " << R._43 << " " << R._44 << std::endl;

		std::cout << std::endl;

		// �� * �� * �� * �� * ��
		// S * R * T * R * P
		// ������ * ���� * �̵� * ���� * �θ�
		world = S * /*R **/ T; // T * S�ʹ� �ٸ���! ����� ��ȯ��Ģ�� �������� ����
	}


	// Create Constant Buffer
	// COM Interface ��ü�� ���� ���� Description�� ���� �����Ǿ�� �Ѵ�.
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC; // CPU - Write // GPU - READ
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TRANSFORM_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // USAGE�� DYNAMIC�̱� ������ CPU�� ���� �����ϰ� ��������.

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &gpu_buffer);
		assert(SUCCEEDED(hr));

	}

	// Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID; // FillMode ��� ä������ ����
		desc.CullMode = D3D11_CULL_BACK; // � ���� ������ �ʰ� �� �������� ����
		desc.FrontCounterClockwise = false; // ������ �׷����� ������� �ո�� �޸��� �����ִ� �� (DirectX�� ��������� �ð������ �ո����� ���)

		// Clipping : �׸� �Ŀ� �����͸� �߶󳻴� ��
		// Culling : �׸��� ���� �����͸� �߶󳻴� ��


		auto hr = graphics->GetDevice()->CreateRasterizerState(&desc, &rasterizer_state);
		assert(SUCCEEDED(hr));
	}


	// Create Shader Resource View
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

	//// world ũ�� ����
	//world._11 = 50;
	//world._22 = 50;

	//// world ��ġ ����
	//world._41 = 100;
	//world._42 = 100;


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

	SAFE_DELETE_ARRAY(indices);
	SAFE_RELEASE(index_buffer);

	SAFE_RELEASE(vertex_buffer);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(graphics);
}

void Execute::Update()
{
	//srand(static_cast<unsigned int>(time(NULL)));
	//float radian = static_cast<float>(rand() % 7 + 3) / 100.0f;
	//
	//D3DXMATRIX P_R;
	//D3DXMatrixRotationZ(&P_R, radian);

	//// �θ� ����� �����ָ� �θ� ��Ŀ� ���ӵȴ�!
	//world = world * P_R;


	// D3DXMATRIX				- �� �켱 ���
	// GPU - Shader = matrix	- �� �켱 ���
	// Color.hlsl���� row_major �ٿ��ִ� ����� ������... 
	// Transpose�� �� ���ϴ�.


	D3DXMatrixTranspose(&cpu_buffer.world, &world); 
	D3DXMatrixTranspose(&cpu_buffer.view, &view); 
	D3DXMatrixTranspose(&cpu_buffer.projection, &projection); 

	// cpu_buffer�� gpu�� ������
	// ���������� �����Ϳ� ������ ���� DeviceContext�� ����Ѵ�.
	// D3D11_MAPPED_SUBRESOURCE : ��� ���� �ڿ��� ���Ž�ų �� �ִ�.
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	graphics->GetDeviceContext()->Map
	(
		gpu_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_subresource
	);

	memcpy(mapped_subresource.pData, &cpu_buffer, sizeof(TRANSFORM_DATA));

	graphics->GetDeviceContext()->Unmap(gpu_buffer, 0);
}

void Execute::Render()
{
	// stride : ���� �ϳ��� ũ�⿡ ���� ����
	uint stride = sizeof(VertexTexture);

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
		// ù ������ StartSlot�� HLSL �ڵ忡�� buffer�� ����� �� �Է��ߴ� ���� ��ȣ�� ��Ÿ����.
		graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &gpu_buffer);  

		// RS
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);

		// PS
		graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);
		graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &shader_resource);

		// Rendering Pipeline�� ������ �����ٸ� �׻� Draw �Լ��� ȣ��ž� �׷�����.
		graphics->GetDeviceContext()->DrawIndexed(10, 0, 0);

	}
	graphics->End();

}

// 
