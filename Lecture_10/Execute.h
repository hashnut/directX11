#pragma once

struct VertexColor
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
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
	
	VertexColor* vertices = nullptr;
	ID3D11Buffer* vertex_buffer = nullptr;

	uint* indices = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	ID3D11InputLayout* input_layout = nullptr;

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3DBlob* vs_blob = nullptr;

	ID3D11PixelShader* pixel_shader = nullptr;
	ID3DBlob* ps_blob = nullptr;

	D3DXMATRIX world; //4x4
	D3DXMATRIX view;
	D3DXMATRIX projection;
	/*
		1 0 0 0
		0 1 0 0
		0 0 1 0
		0 0 0 1


        y    z
		^   /
		|  /
		| /
		|-------> x
		(0, 0, 0)
       
       OpenGL  - RH
	   DirecrX - LH

	*/

};

//Rendering Pipeline
//- IA - VS - RS - PS - OM

//IA : Input Assembler Stage
//- Vertex

//VS : Vertex Shader -> shader : HLSL , GLSL
//- Transform
//Local
//World
//View
//Projection
//Clip	- Homogeneous space - ����

//RS : Rasterizer
//- NDC
//NDC	- Euclidean space
//Viewport






//(1, 0, 0, 1)

//w -> 0 -> ����
//w -> 1 -> ��ġ


/*

|
|---------------------0------ 4
|
|--------0------------------- 2
|



*/