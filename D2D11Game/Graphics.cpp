#include "stdafx.h"
#include "Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
	// 먼저 할당된 포인터를 제일 나중에 해제해 주자! (dangling pointer 상황 방지)
	SAFE_RELEASE(render_target_view);
	SAFE_RELEASE(device);
	SAFE_RELEASE(device_context);
	SAFE_RELEASE(swap_chain);

}

void Graphics::Initialize()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferDesc.Width = 0; // 백버퍼 크기를 잡아줌
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	// R8.. 하나의 채널에 몇비트 할당할 것인지. RED : 8비트! (0~255)
	// UNORM : 정규화된 ... (0~255의 값을 0~1로 오도록 정규화한 것)
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 한 라인을 Scanline... Unpecified : 미확인
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 화면이 확대될 때 어떤 효과를 줄 것인지?
	desc.BufferCount = 1;

	// Aliasing : 계단 현상. Anti-Aliasing을 하는 방법에는 보통 SSA와 MSAA가 있다.
	// SSAA : 이미지 하나를 k배로 늘리고 줄인다 + 보정 -> 비용이 커서 잘 안쓰임
	// MSAA : 특정 부분(예 : 외각선)만 처리해서 상대적으로 저렴함.
	// MSAA 에서 샘플링을 어떻게 할 것인가, 변수를 결정.
	desc.SampleDesc.Count = 1;
	
	// 위에 있는 버퍼를 어떻게 사용할 것인가?
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.SampleDesc.Quality = 0;

	desc.OutputWindow = Settings::Get().GetWindowHandle();
	desc.Windowed = TRUE;
	// 더블 버퍼링할 때 잉여 버퍼를 어떻게 할지..? DISCARD : 폐기!
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::vector<D3D_FEATURE_LEVEL> feature_levels
	{
		//D3D_FEATURE_LEVEL_11_1, // Undefined... Maybe deprecated?
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	// Device, Device Context, SwapChain까지 한꺼번에 만들 수 있다.
	// return type이 HRESULT임. HRESULT는 함수의 결과가 성공했는지 실패했는지의 정보를 담고 있다.
	auto hr = D3D11CreateDeviceAndSwapChain		
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, // 하드웨어 가속을 사용할 수 있게 해준다.
		nullptr,
		0, // 어떤 API를 사용할 것인지
		feature_levels.data(), // &feature_levels[0] 와 같다. 첫 시작 지점의 포인터를 반환
		feature_levels.size(),
		D3D11_SDK_VERSION,
		&desc,
		&swap_chain,
		&device,
		nullptr,
		&device_context
	);

	// 함수의 결과가 성공했는지 실패했는지의 정보를 받아 assert
	assert(SUCCEEDED(hr));

}

void Graphics::CreateBackBuffer(const uint& width, const uint& height)
{
	auto hr = swap_chain->ResizeBuffers
	(
		0, // ResizeBuffers 함수에는 초기값다 다른, 변형할 값만 넣어주면 된다.
		width,
		height,
		DXGI_FORMAT_UNKNOWN, // 포맷을 모르겠다... (위에 정의된 기본 값 사용)
		0
	);

	assert(SUCCEEDED(hr));

	// ID3D11SwapChain을 통해서 BackBuffer 정보를 얻어낼 수 있다
	ID3D11Texture2D* back_buffer = nullptr;
	hr = swap_chain->GetBuffer
	(
		0,
		// UUID : Universally Unique ID		GUID : Globally Unique ID
		// ID3D11Texture2D의 정의로 가보면, MIDL_INTERFACE("6f15aaf2-d208-4e89-9ab4-489535d34f9c") 가 UID에 해당한다!
		// UUID, GUID 는 중복될 경우가 없기에 고유 ID라고도 부른다 (MS에서 GUID 사용, 전역적으로는 UUID 사용)
		__uuidof(ID3D11Texture2D), //IID_ID3D11Texture2D, // IID : Interface Identifier
		reinterpret_cast<void**>(&back_buffer)
	);

	assert(SUCCEEDED(hr));

	hr = device->CreateRenderTargetView
	(
		back_buffer,
		nullptr,
		&render_target_view
	);

	assert(SUCCEEDED(hr));

	// 시작점
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width =  static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	
	// delete말고, Release()함수를 이용해서 메모리 해제를 해 주어야 한다!
	SAFE_RELEASE(back_buffer);
}

// 그리기 시작 : 버퍼 세팅과 초기화 작업
void Graphics::Begin()
{
	// OM : Output Merger Stage
	// Output Merger Stage에 세팅될 수 있는 RenderTarget의 개수는 총 8장이다.
	device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	device_context->RSSetViewports(1, &viewport);
	device_context->ClearRenderTargetView(render_target_view, clear_color);

}

// 그리기 끝
void Graphics::End()
{
	// 후면 버퍼를 전면 버퍼로 출력해 준다.
	auto hr = swap_chain->Present(1, 0);
	assert(SUCCEEDED(hr));


}


// ID3D11Resource
// 모든 ID3D11은 이 Resource 녀석을 상속 받는다. (Buffer와 Texture로 나뉨)

// ID3D11Buffer : 구조체를 만들어 넘기는 자원(Buffer)		ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D : 이미지화된 자원(Texture)

// Resource View : Texture 자원의 사용 용도를 명확히 해주는 개념
// - ID3D11RenderTargetView(화면 그리기), ID3D11ShaderResourceView(이미 만들어진 이미지 넘기기), ID3D11DepthStencilView(깊이 관련...), ID3D11UnorderedAccessView(읽고 쓰기가 다 된다)