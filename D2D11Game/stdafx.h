#pragma once

// Debug mode에만 세팅됨
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") // 테스트용 콘솔창 띄우기
#endif

//Window
#include <Windows.h>
#include <assert.h>

//DirectX
#include <D3Dcompiler.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

// STL	
#include <vector>
#include <iostream>

// 라이브러리를 연결하는 전처리기 코드의 표현
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dcompiler.lib")


typedef unsigned int uint;

// MACRO Function
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p) = nullptr; } }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p) = nullptr; } }

// Framework
#include "Settings.h"