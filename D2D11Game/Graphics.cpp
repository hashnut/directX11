#include "stdafx.h"
#include "Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
	// ���� �Ҵ�� �����͸� ���� ���߿� ������ ����! (dangling pointer ��Ȳ ����)
	SAFE_RELEASE(render_target_view);
	SAFE_RELEASE(device);
	SAFE_RELEASE(device_context);
	SAFE_RELEASE(swap_chain);

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
	
	// ���� �ִ� ���۸� ��� ����� ���ΰ�?
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.SampleDesc.Quality = 0;

	desc.OutputWindow = Settings::Get().GetWindowHandle();
	desc.Windowed = TRUE;
	// ���� ���۸��� �� �׿� ���۸� ��� ����..? DISCARD : ���!
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	std::vector<D3D_FEATURE_LEVEL> feature_levels
	{
		//D3D_FEATURE_LEVEL_11_1, // Undefined... Maybe deprecated?
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

	// �Լ��� ����� �����ߴ��� �����ߴ����� ������ �޾� assert
	assert(SUCCEEDED(hr));

}

void Graphics::CreateBackBuffer(const uint& width, const uint& height)
{
	auto hr = swap_chain->ResizeBuffers
	(
		0, // ResizeBuffers �Լ����� �ʱⰪ�� �ٸ�, ������ ���� �־��ָ� �ȴ�.
		width,
		height,
		DXGI_FORMAT_UNKNOWN, // ������ �𸣰ڴ�... (���� ���ǵ� �⺻ �� ���)
		0
	);

	assert(SUCCEEDED(hr));

	// ID3D11SwapChain�� ���ؼ� BackBuffer ������ �� �� �ִ�
	ID3D11Texture2D* back_buffer = nullptr;
	hr = swap_chain->GetBuffer
	(
		0,
		// UUID : Universally Unique ID		GUID : Globally Unique ID
		// ID3D11Texture2D�� ���Ƿ� ������, MIDL_INTERFACE("6f15aaf2-d208-4e89-9ab4-489535d34f9c") �� UID�� �ش��Ѵ�!
		// UUID, GUID �� �ߺ��� ��찡 ���⿡ ���� ID��� �θ��� (MS���� GUID ���, ���������δ� UUID ���)
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

	// ������
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width =  static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	
	// delete����, Release()�Լ��� �̿��ؼ� �޸� ������ �� �־�� �Ѵ�!
	SAFE_RELEASE(back_buffer);
}

// �׸��� ���� : ���� ���ð� �ʱ�ȭ �۾�
void Graphics::Begin()
{
	// OM : Output Merger Stage
	// Output Merger Stage�� ���õ� �� �ִ� RenderTarget�� ������ �� 8���̴�.
	device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	device_context->RSSetViewports(1, &viewport);
	device_context->ClearRenderTargetView(render_target_view, clear_color);

}

// �׸��� ��
void Graphics::End()
{
	// �ĸ� ���۸� ���� ���۷� ����� �ش�.
	auto hr = swap_chain->Present(1, 0);
	assert(SUCCEEDED(hr));


}


// ID3D11Resource
// ��� ID3D11�� �� Resource �༮�� ��� �޴´�. (Buffer�� Texture�� ����)

// ID3D11Buffer : ����ü�� ����� �ѱ�� �ڿ�(Buffer)		ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D : �̹���ȭ�� �ڿ�(Texture)

// Resource View : Texture �ڿ��� ��� �뵵�� ��Ȯ�� ���ִ� ����
// - ID3D11RenderTargetView(ȭ�� �׸���), ID3D11ShaderResourceView(�̹� ������� �̹��� �ѱ��), ID3D11DepthStencilView(���� ����...), ID3D11UnorderedAccessView(�а� ���Ⱑ �� �ȴ�)