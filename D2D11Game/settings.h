#pragma once
#include "stdafx.h"

// Singleton Pattern : 하나의 인스턴스만 생성
class Settings final
{
public:
	static Settings& Get()
	{
		static Settings instance;
		return instance;
	}

	auto GetWindowHandle() const -> HWND { return handle; }
	void SetWindowHandle(HWND handle) { this->handle = handle; }

	auto GetWidth() const -> const float& { return width; }
	void SetWidth(const float& width) { this->width = width; }

	auto GetHeight() const -> const float& { return height; }
	void SetHeight(const float& height) { this->height = height; }


private:
	// default 키워드는 = {} 로 생각하면 된다.
	Settings() = default;
	~Settings() = default;

private:
	HWND handle = nullptr;
	float width = 0.0f;
	float height = 0.0f;

};