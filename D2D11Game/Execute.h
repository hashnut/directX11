#pragma once

struct VertexColor
{
	D3DXVECTOR3 position; 
	D3DXCOLOR clear_color;

};

struct VertexTexture
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 uv; // Texture Coordinate
};


struct TRANSFORM_DATA
{
	// D3DXMATRIX : 4X4 float ���
	// ����� �׻� ������ķ� �ʱ�ȭ�ؾ� �Ѵ�.
	D3DXMATRIX world; // 4X4
	D3DXMATRIX view;
	D3DXMATRIX projection;
};

class Execute final
{
public:
	Execute();
	~Execute();

	void Update();
	void Render();

private:
	class Graphics* graphics = nullptr;

	// VertexColor�� �츮�� ���� ������... Ram�� ��.
	
	VertexTexture* vertices = nullptr;

	// CPU �ڿ��� GPU�� �Է��ϱ� ���� ��ȯ�ؾ� �Ѵ�!
	ID3D11Buffer* vertex_buffer = nullptr;

	uint* indices = nullptr;
	ID3D11Buffer* index_buffer = nullptr;


	// 0, 0, 0, 0, 0, 0, 0 �̷� ������ ��ǥ 3���� ���� ������ 4���� ��������...?
	// ��ǻ�Ͱ� ��� �а� ��ȹ�� ������� �ϴ��� -> Layout���� ����!
	// Layout �����ʹ� ȥ�� ������ ���� ����. �ܰ� ���� ��ġ�ؾ� �Ѵٴ� ��.
	ID3D11InputLayout* input_layout = nullptr;

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3DBlob* vs_blob = nullptr; // Blob : Binary Large Object

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* ps_blob = nullptr;

	// D3DXMATRIX : 4X4 float ���
	// ����� �׻� ������ķ� �ʱ�ȭ�ؾ� �Ѵ�.
	D3DXMATRIX world; // 4X4
	D3DXMATRIX view;
	D3DXMATRIX projection;

	/*
	1 0 0 0	// x
	0 1 0 0	// y
	0 0 1 0	// z
	0 0 0 1	// ������ 1�̸� ��ġ�� �ǹ� (��ġ�� ����Ǹ� 0 0 0 �κ��� �ٲ��.)

	y    z
	^   /
	|  /
	| /
	|------> x
	(0, 0, 0)
	: x, y, z ������ 1��ŭ ����, ��ġ�� (0, 0, 0)�� ���⺤��

	OpenGL - RH (Right Hand)��ǥ��
	DirectX - LH (Left Hand)��ǥ��

	*/

	TRANSFORM_DATA cpu_buffer;
	ID3D11Buffer* gpu_buffer = nullptr; // Constant Buffer (��� ����)

	ID3D11RasterizerState* rasterizer_state = nullptr;

	ID3D11ShaderResourceView* shader_resource = nullptr;

	// Texture
	// ID3D11Texture3D
	// RTV : OM
	// DSV : OM
	// SRV : ID3D11ShaderResourceView : �ؽ��� �����͸� �б⸸ �� �� �ִ� �ڿ� ��
	// UAV : 

};

// Rendering Pipeline : �������� �ϱ� ���� ����Ǿ�� �ϴ� �ܰ��
// - IA - VS - RS - PS - OM (IA, RS, OM �ܰ�� �ڵ��� �Ұ����ϴ�! VS, PS �ܰ�� �ڵ��� �����ϴ�!)

// IA : Input Assembler Stage : �⺻ �����Ͱ� ���� �Է� ������ �ܰ�
// - Vertex : DirectX�� �⺻ �����ʹ� �����̴�! - ����ڸ��� ������ �پ��ϱ⿡ �⺻ �ڷ����� �������� �ʴ´�! -> Ŀ���͸���¡ �ʿ�

// VS : Vertex Shader. Shader : GPU�� ���۽�Ű�� �Լ����� ����
// - Shader : HLSL(High Level Shader Language), GLSL
// - Transform(VS �ܰ迡���� �������� ���� ��ȯ�� ���ش�!)
// Local : ���赵 ���� ����
// World : ���� ����
// View : �������� ���� (�þ� ����)
// Projection : ���� ���� - ������ ���ߴ� ����. 3D �����͸� 2D �����ͷ� ��ȯ��Ű�� ��
// Clip : ���� �������� ����ȭ��Ų��.(0~1) �� �״�� ������ ����. - Homogeneous space : ���� (1, 0, 0, 0) ������ ���� w��� �ϴµ�...
// w - ������ 1�̸� ��ġ, 0�̸� ������ ǥ��
// DirectX������ Projection�� Clip�� ���յǾ� �ִ�.

// RS : Rasterizer : 3D �����͸� 2D �����ͷ� �ٲ��ִ� �� 
// - VS �ܰ迡�� ����ȭ�� ������ RS���� �ٽ� �÷��ش�!
// - NDC : Normalized Device Coordinate : ȭ�鿡 �°� �÷���
// NDC - Euclidean space 
// Viewport : ����ȭ�� ũ�⸦ �ٽ� Rasterized�� ũ��� ��ȯ�� ��
// Clipping
// Back-face Culling - ������ �ʴ� ���� �����ϴ� ��
// Viewport Transform - Viewport ������ ����
// Scan Transform - Pixel Shader�� �׷��� ������ �������ش�


