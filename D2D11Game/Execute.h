#pragma once

// Rendering Pipeline : �������� �ϱ� ���� ����Ǿ�� �ϴ� �ܰ��
// IA - VS - RS - PS - OM

// IA : Input Assembler Stage : �⺻ �����Ͱ� ���� �Է� ������ �ܰ�
// - Vertex : DirectX�� �⺻ �����ʹ� �����̴�! - ����ڸ��� ������ �پ��ϱ⿡ �⺻ �ڷ����� �������� �ʴ´�! -> Ŀ���͸���¡ �ʿ�

struct VertexColor
{
	D3DXVECTOR3 position; 
	D3DXCOLOR clear_color;

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
	
	VertexColor* vertices = nullptr;

	// CPU �ڿ��� GPU�� �Է��ϱ� ���� ��ȯ�ؾ� �Ѵ�!
	ID3D11Buffer* vertex_buffer = nullptr;

	// 0, 0, 0, 0, 0, 0, 0 �̷� ������ ��ǥ 3���� ���� ������ 4���� ��������...?
	// ��ǻ�Ͱ� ��� �а� ��ȹ�� ������� �ϴ��� -> Layout���� ����!
	// Layout �����ʹ� ȥ�� ������ ���� ����. �ܰ� ���� ��ġ�ؾ� �Ѵٴ� ��.
	ID3D11InputLayout* input_layout = nullptr;

};

