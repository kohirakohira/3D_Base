#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

//DirectX9.
#include <d3dx9.h>

//ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
//DirectX9.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )


/**************************************************
*	DirectX9 セットアップ.
**/
class CDirectX9
{
public:
	//インスタンス取得(唯一のアクセス経路).
	//※関数の前にstaticを付けることでインスタンス生成しなくても使用できる.
	static CDirectX9& GetInstance()
	{
		//唯一のインスタンスを作成する.
		//※staticで作成されたので2回目以降は、下の1行は無視される.
		static CDirectX9 s_Instance;	//s_:staticの意味.
		return s_Instance;
	}

	~CDirectX9();

	//DirectX9構築.
	HRESULT Create( HWND hWnd );
	//DirectX9解放.
	void Release();

	//デバイスオブジェクトを取得.
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }


private://外部からアクセス不可能.
	//外部からコンストラクタへのアクセスを禁止する.
	CDirectX9();
	//コピーコンストラクタによるコピーを禁止する.
	//「=delete」で関数の定義を削除できる.
	CDirectX9(const CDirectX9& rhs) = delete;
	//代入演算子によるコピーを禁止する.
	//operator(オペレータ):演算子のオーバーロードで、演算の中身を拡張できる.
	CDirectX9& operator = (const CDirectX9& rhs) = delete;

private:
	LPDIRECT3DDEVICE9	m_pDevice9;	//デバイスオブジェクト.
};