#pragma once
#include <crtdbg.h>//_ASSERT_EXPR()で必要.

//===========================================================
//	マクロ.
//===========================================================

//解放.
#define SAFE_RELEASE(p)	if(p!=nullptr){(p)->Release();(p)=nullptr;}
//破棄.
#define SAFE_DELETE(p) if(p!=nullptr){delete (p);(p)=nullptr;}
#define SAFE_DELETE_ARRAY(p)	\
{								\
	if(p!=nullptr){				\
		delete[] (p);			\
		(p) = nullptr;			\
	}							\
}

//Imgui：日本語対応マクロ.
//u8##str：プリプロセッサのトークン連結.
//		　例)「JAPANESE("こんにちは")->u8"こんにちは"」 となる.
//reinterpret_cast<const char*>(...)：u8"文字列"は型がconst char8_t*(C++20以降).
//									　でも、ImGui::Text()などは引数に「const char* text」.
//									　のようになるので、const char*が必要.
#define JAPANESE(str) reinterpret_cast<const char*>(u8##str)