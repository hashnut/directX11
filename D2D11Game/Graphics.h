#pragma once

// final : 더는 상속될 수 없다
class Graphics final
{
public:
	Graphics();
	~Graphics();

	void Initialize();
	void CreateBackBuffer(const uint& width, const uint& height);

	// -> type : 부분은 생략해도 괜찮다. auto를 쓰는 거는 정렬을 위해 사용...
	auto GetDevice() -> ID3D11Device* { return device; }
	auto GetDeviceContext() -> ID3D11DeviceContext* { return device_context; }

	void Begin();
	void End();


private:
	// 멤버 변수 선언과 동시에 초기화가 가능 : c++ 11 이상 버전부터!
	// I가 붙은 녀석들 - COM interface(Component Object Model), 내가 만든 게 아니라 가져다 쓰는 것만 가능 (4개) 
	// (동적할당 불가능, Release 함수로 메모리 해제해야함)
	// COM Interface는 Description을 가진다!
	// device : 자원 생성 -> RenderTargetView를 쓰려면 필요...
	ID3D11Device* device = nullptr;

	// 장치 내용
	// Rendering Pipeline에 세팅을 한다!
	ID3D11DeviceContext* device_context = nullptr;

	// DXGI : DirectX Graphics Infrastructure : 하부구조, 제조사마다 GPU 구조가 다른데, 프로그래머가 편하게 기능을 사용할 수 있게 해 준다.
	// SwapChain : 여러 DXGI 객체 중 하나. 두 개의 버퍼를 번갈아 가며 렌더링하여 깜빡임, 찢어짐 현상 방지! (더블 버퍼링) -> 이게 사슬처럼 생김
	// 백 버퍼를 관리한다고 보면 편함
	// 지지대, 판자 역할. 판자에 끼워지는 녀석이 RenderTargetView
	IDXGISwapChain* swap_chain = nullptr;

	// RenderTarget 이 도화지라고 보면 된다. View는 ResourceView라는 의미
	ID3D11RenderTargetView* render_target_view = nullptr;

	// viewport : 어떤 영역에 그릴 것인가. 이미지의 방향이나 위치 등
	D3D11_VIEWPORT viewport = { 0 };

	// {r, g, b, a} 의 구조체. a는 투명도 정보 (각 타입은 float임 -> 그래서 정규화라는 과정을 거친다.)
	// 뭔가 지울 만한 것이 생겼다? -> 같은 색깔로 칠하는 것이 더 빠르다. 그래서 clear라고 이해하자.
	// 내부에서 16진수를 처리하는 녀석이 있다. unsigned int 형식임!
	// 직관적으로 4바이트임을 잘 알 수 있다.
	D3DXCOLOR clear_color = 0xff555566; 
	

	
};

// Rendering Pipeline : 렌더링을 하기 위해 수행되어야 하는 단계들
// IA - VS - RS - PS - OM

// DirectX - API들의 집합, 그런데 렌더링에 관여를 함
// OpenGL

// GPU

