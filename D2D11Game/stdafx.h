#pragma once

// Debug mode���� ���õ�
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") // �׽�Ʈ�� �ܼ�â ����
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

// ���̺귯���� �����ϴ� ��ó���� �ڵ��� ǥ��
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