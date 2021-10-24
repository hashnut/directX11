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
	desc.BufferDesc.Width = 0; // ����� ũ�⸦ �����
	desc.BufferDesc.Height = 0; 
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	// R8.. �ϳ��� ä�ο� ���Ʈ �Ҵ��� ������. RED : 8��Ʈ! (0~255)
	// UNORM : ����ȭ�� ... (0~255�� ���� 0~1�� ������ ����ȭ�� ��)
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �� ������ Scanline... Unpecified : ��Ȯ��
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // ȭ���� Ȯ��� �� � ȿ���� �� ������?
	desc.BufferCount = 1;

	// Aliasing : ��� ����. Anti-Aliasing�� �ϴ� ������� ���� SSA�� MSAA�� �ִ�.
	// SSAA : �̹��� �ϳ��� k��� �ø��� ���δ� + ���� -> ����� Ŀ�� �� �Ⱦ���
	// MSAA : Ư�� �κ�(�� : �ܰ���)�� ó���ؼ� ��������� ������.
	// MSAA ���� ���ø��� ��� �� ���ΰ�, ������ ����.
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.OutputWindow = Window::global_handle;
	desc.Windowed = TRUE;
	// ���� ���۸��� �� �׿� ���۸� ��� ����..? DISCARD : ���!
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::vector<D3D_FEATURE_LEVEL> feature_levels
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	// Device, Device Context, SwapChain���� �Ѳ����� ���� �� �ִ�.
	// return type�� HRESULT��. HRESULT�� �Լ��� ����� �����ߴ��� �����ߴ����� ������ ��� �ִ�.
	auto hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, // �ϵ���� ������ ����� �� �ְ� ���ش�.
		nullptr,
		0, // � API�� ����� ������
		feature_levels.data(), // &feature_levels[0] �� ����. ù ���� ������ �����͸� ��ȯ
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
