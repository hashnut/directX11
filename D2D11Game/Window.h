#pragma once
#include "stdafx.h"

namespace Window
{
	static HINSTANCE global_instance;
	static HWND global_handle;

	// CALLBACK 함수는 다른 코드의 인수로서 넘겨줄 수 있는 실행 가능한 코드를 의미한다
	inline LRESULT CALLBACK WndProc
	(
		HWND handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		switch (message) {
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;


		default:
			return DefWindowProc(handle, message, wParam, lParam);
		}

		return 0;
			
	}

	inline void Create(HINSTANCE hInstance, const UINT& width, const UINT& height)
	{
		WNDCLASSEX wnd_class;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW); // 커서 타입
		wnd_class.hIcon = LoadIcon(nullptr, IDI_ERROR); // 아이콘 타입
		wnd_class.hIconSm = LoadIcon(nullptr, IDI_ERROR);
		wnd_class.hInstance = hInstance;
		wnd_class.lpfnWndProc = WndProc; // CALLBACK 함수가 아니면 에러가 뜬다.
		wnd_class.lpszClassName = L"D2D11Game";
		wnd_class.lpszMenuName = nullptr;
		wnd_class.style = CS_HREDRAW | CS_VREDRAW; // HORIZONTAL REDRAW, VERTICAL REDRAW
		wnd_class.cbSize = sizeof(WNDCLASSEX);

		auto check = RegisterClassEx(&wnd_class);
		assert(check != 0);

		global_handle = CreateWindowExW
		(
			WS_EX_APPWINDOW,
			L"D2D11Game",
			L"D2D11Game",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<int>(width),
			static_cast<int>(height),
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);
		assert(global_handle != nullptr);
	}

	inline void Show()
	{
		SetForegroundWindow(global_handle); 
		SetFocus(global_handle);
		ShowCursor(TRUE);
		ShowWindow(global_handle, SW_NORMAL); // 윈도우 실제로 보여줌
		UpdateWindow(global_handle);
	}


	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	inline void Destroy()
	{
		DestroyWindow(global_handle);
		UnregisterClass(L"D2D11Game", global_instance);
	}

	inline const UINT GetWidth()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}

	inline const UINT GetHeight()
	{
		RECT rect;
		GetClientRect(global_handle, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}
}