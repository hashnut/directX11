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
		
		// Usage란? : GPU와 CPU 중 누가 데이터를 읽거나 쓸 것인지 결정하는 플래그
		// D3D11_USAGE_DEFAULT : GPU만 데이터를 읽고 쓸 수 있다.
		// D3D11_USAGE_IMMUTABLE : GPU만 데이터를 읽을 수 있다. (쓰기는 불가)
		desc.Usage = D3D11_USAGE_DEFAULT; 
		
		// Buffer형 데이터를 어느 스테이지에서 사용할 건지 지정해주는 플래그다.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// Vertex 버퍼를 어느 정도 크기로 생성할 것인지?
		desc.ByteWidth = sizeof(VertexTexture) * 4;

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
	// 정점의 정보가 바뀌면 Input Layout도 바뀌어야 한다!
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
		// 함수를 활용해서 모든 행렬을 단위행렬로 만들어주기
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);

		D3DXVECTOR3 LHeye(0, 0, 0);
		D3DXVECTOR3 LHat(0, 0, 1);
		D3DXVECTOR3 LHup(0, 1, 0);

		D3DXMatrixLookAtLH(&view, &LHeye, &LHat, &LHup);

		// 투영 방식 :
		// Perspective : 원근감이 느껴지는 투영, 절두체로 표현된다.
		// Orthographic : 직교 투영, 원근감이 느껴지지 않음. 직교투영의 시야공간은 직육면체로 표현된다.
		D3DXMatrixOrthoLH(&projection, Settings::Get().GetWidth(), Settings::Get().GetHeight(), 0, 1);
		 
		//D3DXMatrixOrthoOffCenterLH(&view, 0, Settings::Get().GetWidth(), Settings::Get().GetHeight(), 0, 0, 1);

		std::cout << "View Matrix" << std::endl;
		// D3DMATRIX 정의 참고
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


		// World : Scaling은 공간의 크기를 바꿀 수 있다.
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 500, 500, 1);

		std::cout << "Scale Matrix" << std::endl;
		std::cout << S._11 << " " << S._12 << " " << S._13 << " " << S._14 << std::endl;
		std::cout << S._21 << " " << S._22 << " " << S._23 << " " << S._24 << std::endl;
		std::cout << S._31 << " " << S._32 << " " << S._33 << " " << S._34 << std::endl;
		std::cout << S._41 << " " << S._42 << " " << S._43 << " " << S._44 << std::endl;

		std::cout << std::endl;
		
		// Translation : 공간을 이동시켜줌. 이전 공간의 정보를 계승하지 않음
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
		// 회전의 중심이 되는 축은 고정이다!
		D3DXMatrixRotationZ(&R, static_cast<float>D3DXToRadian(45));

		std::cout << "Rotation Matrix" << std::endl;
		std::cout << R._11 << " " << R._12 << " " << R._13 << " " << R._14 << std::endl;
		std::cout << R._21 << " " << R._22 << " " << R._23 << " " << R._24 << std::endl;
		std::cout << R._31 << " " << R._32 << " " << R._33 << " " << R._34 << std::endl;
		std::cout << R._41 << " " << R._42 << " " << R._43 << " " << R._44 << std::endl;

		std::cout << std::endl;

		// 스 * 자 * 이 * 공 * 부
		// S * R * T * R * P
		// 스케일 * 자전 * 이동 * 공전 * 부모
		world = S * /*R **/ T; // T * S와는 다르다! 행렬은 교환법칙이 성립하지 않음
	}


	// Create Constant Buffer
	// COM Interface 객체를 만들 때는 Description이 먼저 구현되어야 한다.
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC; // CPU - Write // GPU - READ
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(TRANSFORM_DATA);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // USAGE가 DYNAMIC이기 때문에 CPU가 접근 가능하게 설정하자.

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &gpu_buffer);
		assert(SUCCEEDED(hr));

	}

	// Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID; // FillMode 어떻게 채울지를 결정
		desc.CullMode = D3D11_CULL_BACK; // 어떤 면을 보이지 않게 할 것인지를 결정
		desc.FrontCounterClockwise = false; // 정점이 그려지는 순서대로 앞면과 뒷면을 정해주는 값 (DirectX는 통상적으로 시계방향을 앞면으로 사용)

		// Clipping : 그린 후에 데이터를 잘라내는 것
		// Culling : 그리기 전에 데이터를 잘라내는 것


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

	//// world 크기 조정
	//world._11 = 50;
	//world._22 = 50;

	//// world 위치 조정
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

	//// 부모 행렬을 곱해주면 부모 행렬에 종속된다!
	//world = world * P_R;


	// D3DXMATRIX				- 행 우선 행렬
	// GPU - Shader = matrix	- 열 우선 행렬
	// Color.hlsl에서 row_major 붙여주는 방법도 있지만... 
	// Transpose가 더 편하다.


	D3DXMatrixTranspose(&cpu_buffer.world, &world); 
	D3DXMatrixTranspose(&cpu_buffer.view, &view); 
	D3DXMatrixTranspose(&cpu_buffer.projection, &projection); 

	// cpu_buffer를 gpu로 보내기
	// 파이프라인 데이터에 접근할 때는 DeviceContext를 사용한다.
	// D3D11_MAPPED_SUBRESOURCE : 상수 버퍼 자원을 갱신시킬 수 있다.
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
	// stride : 정점 하나의 크기에 대한 정보
	uint stride = sizeof(VertexTexture);

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
		// 첫 인자인 StartSlot은 HLSL 코드에서 buffer를 등록할 때 입력했던 버퍼 번호를 나타낸다.
		graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &gpu_buffer);  

		// RS
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);

		// PS
		graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);
		graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &shader_resource);

		// Rendering Pipeline에 세팅이 끝났다면 항상 Draw 함수가 호출돼야 그려진다.
		graphics->GetDeviceContext()->DrawIndexed(10, 0, 0);

	}
	graphics->End();

}

// 
