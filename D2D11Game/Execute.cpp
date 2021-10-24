#include "stdafx.h"
#include "Execute.h"

#include "Graphics.h"

Execute::Execute()
{
	// �׳� ���� �ҿ����� ������ ����� �� ���ٰ� ����. ���� �ش� �ڷ����� ��������� include ���־�� �Ѵ�.
	graphics = new Graphics();
	graphics->Initialize();
	graphics->CreateBackBuffer(static_cast<uint>(Settings::Get().GetWidth()), static_cast<uint>(Settings::Get().GetHeight()));
}

Execute::~Execute()
{
	SAFE_DELETE(graphics);
}

void Execute::Update()
{
}

void Execute::Render()
{
	graphics->Begin();

	// Rendering Part
	{


	}

	graphics->End();

}
