#include "stdafx.h"
#include "Graphics.h"
#include "Window.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
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
	desc.SampleDesc.Quality = 0;

	desc.OutputWindow = Window::global_handle;
	desc.Windowed = TRUE;
	// 더블 버퍼링할 때 잉여 버퍼를 어떻게 할지..? DISCARD : 폐기!
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::vector<D3D_FEATURE_LEVEL> feature_levels
	{
		D3D_FEATURE_LEVEL_11_1,
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

	assert(SUCCEEDED(hr));

}

void Graphics::CreateBackBuffer()
{
}
