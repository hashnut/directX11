#include "stdafx.h"
#include "Execute.h"

#include "Graphics.h"

Execute::Execute()
{
	// 그냥 쓰면 불완전한 형식은 사용할 수 없다고 나옴. 따라서 해당 자료형의 헤더파일을 include 해주어야 한다.
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
