#pragma once

// final : ���� ��ӵ� �� ����
class Graphics final
{
public:
	Graphics();
	~Graphics();

	void Initialize();
	void CreateBackBuffer(const uint& width, const uint& height);

	// -> type : �κ��� �����ص� ������. auto�� ���� �Ŵ� ������ ���� ���...
	auto GetDevice() -> ID3D11Device* { return device; }
	auto GetDeviceContext() -> ID3D11DeviceContext* { return device_context; }

	void Begin();
	void End();


private:
	// ��� ���� ����� ���ÿ� �ʱ�ȭ�� ���� : c++ 11 �̻� ��������!
	// I�� ���� �༮�� - COM interface(Component Object Model), ���� ���� �� �ƴ϶� ������ ���� �͸� ���� (4��) 
	// (�����Ҵ� �Ұ���, Release �Լ��� �޸� �����ؾ���)
	// COM Interface�� Description�� ������!
	// device : �ڿ� ���� -> RenderTargetView�� ������ �ʿ�...
	ID3D11Device* device = nullptr;

	// ��ġ ����
	// Rendering Pipeline�� ������ �Ѵ�!
	ID3D11DeviceContext* device_context = nullptr;

	// DXGI : DirectX Graphics Infrastructure : �Ϻα���, �����縶�� GPU ������ �ٸ���, ���α׷��Ӱ� ���ϰ� ����� ����� �� �ְ� �� �ش�.
	// SwapChain : ���� DXGI ��ü �� �ϳ�. �� ���� ���۸� ������ ���� �������Ͽ� ������, ������ ���� ����! (���� ���۸�) -> �̰� �罽ó�� ����
	// �� ���۸� �����Ѵٰ� ���� ����
	// ������, ���� ����. ���ڿ� �������� �༮�� RenderTargetView
	IDXGISwapChain* swap_chain = nullptr;

	// RenderTarget �� ��ȭ����� ���� �ȴ�. View�� ResourceView��� �ǹ�
	ID3D11RenderTargetView* render_target_view = nullptr;

	// viewport : � ������ �׸� ���ΰ�. �̹����� �����̳� ��ġ ��
	D3D11_VIEWPORT viewport = { 0 };

	// {r, g, b, a} �� ����ü. a�� ���� ���� (�� Ÿ���� float�� -> �׷��� ����ȭ��� ������ ��ģ��.)
	// ���� ���� ���� ���� �����? -> ���� ����� ĥ�ϴ� ���� �� ������. �׷��� clear��� ��������.
	// ���ο��� 16������ ó���ϴ� �༮�� �ִ�. unsigned int ������!
	// ���������� 4����Ʈ���� �� �� �� �ִ�.
	D3DXCOLOR clear_color = 0xff555566; 
	

	
};

// Rendering Pipeline : �������� �ϱ� ���� ����Ǿ�� �ϴ� �ܰ��
// IA - VS - RS - PS - OM

// DirectX - API���� ����, �׷��� �������� ������ ��
// OpenGL

// GPU

