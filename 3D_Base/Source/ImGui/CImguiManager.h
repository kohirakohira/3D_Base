#pragma once
#include <iostream>



class CImguiManager
{
public:
	//Imgui‚ÌƒVƒ“ƒOƒ‹ƒgƒ“‰».
	static CImguiManager& GetInstance()
	{
		static CImguiManager instance;
		return instance;
	}

public:
	CImguiManager();
	~CImguiManager();

public:


private:

};