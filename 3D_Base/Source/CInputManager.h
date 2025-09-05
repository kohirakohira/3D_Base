#pragma once
#include "CXInput.h"     // コントローラークラス
#include "CKeyInput.h"   // キーボード入力
#include <memory>

//==========================================
// 入力統合マネージャークラス
// 担当：濵口 聖弥
//==========================================

class CInputManager
{
    //-----------------------
    // 定数宣言
    //-----------------------
public: // 変数
    static constexpr float      MAX_THUMB_VALUE = 32767.0f;   // SHORT型の正規化用の定数
    static constexpr float      THRESHOLD = 0.2f;             // 傾きの閾値(0f~1f)
    static constexpr BYTE       TriggerTHRESHOLD = 30;        // トリガーの閾値

public: // 構造体&列挙型
    // キーによる移動ベクトル.
    struct Vector2
    {
        float x;
        float z;
    };

    // スティックの入力方向
    enum class Direction
    {
        Up,         // 上
        Down,       // 下
        Left,       // 左
        Right,      // 右

        UpLeft,     // 左上
        UpRight,    // 右上
        DownLeft,   // 左下
        DownRight,  // 右下

        None = -1   // 無設定
    };

    // トリガー入力
    enum class Trigger
    {
        LeftTrigger,    // 左トリガー
        RightTrigger,   // 右トリガー
       
        None = -1       // 無設定          
    };

public:
    CInputManager(DWORD ID);    // コンストラクタ
    ~CInputManager();           // デストラクタ

    // 入力を毎フレーム実行
    void Update();

    // 入力処理の共通化
    Direction GetLeftStickDirection(Vector2& Vec);
    Direction GetRightStickDirection(Vector2& Vec);

    // トリガー入力
    Trigger GetLeftTrigger();   // 左トリガーの入力検知
    Trigger GetRightTrigger();  // 右トリガーの入力検知

    // キーボード入力方向取得（WASD or 矢印キー）
    Direction GetWASDKeyDirection();
    Direction GetArrowKeyDirection();

    // キーボード用
    bool IsKeyboardDown(int Key);	// キーを押した瞬間(トリガー)
    bool IsKeyboardRepeat(int Key);	// キーが押され続けている
    bool IsKeyboardUp(int Key);		// キーが離された(リリース)

    // コントローラー用
    bool IsControllerDown(CXInput::KEY key);	// ボタンを押した瞬間(トリガー)
    bool IsControllerRepeat(CXInput::KEY key);	// ボタンが押され続けている
    bool IsControllerUp(CXInput::KEY key);		// ボタンが離された(リリース)

    // 左スティックのベクトルを返す
    const Vector2& GetLeftStickVector() const { return m_LeftStickVec; }
    // 左スティックの現在の方向を返す
    const Direction GetLeftStickDirection() const { return m_LeftStickDir; }

    // 右スティックのベクトルを返す
    const Vector2& GetRightStickVector() const { return m_RightStickVec; }
    // 右スティックの現在の方向を返す
    const Direction GetRightStickDirection() const { return m_RightStickDir; }

private:
    // XY座標から方向判定する共通関数
    Direction GetDirectionFromXY(float x, float y, float threshold);

private:
    CXInput*                    m_XInput;       // コントローラー入力クラス
    std::unique_ptr<CKeyInput>  m_KeyInput;     // キー入力クラス

    bool                        m_UseKeyInput;  // キーボード操作するか

    // スティックのベクトル
    Vector2 m_LeftStickVec = { 0.0f, 0.0f };
    Vector2 m_RightStickVec = { 0.0f, 0.0f };

    // 入力方向をスティックに設定
    Direction m_LeftStickDir = Direction::None;
    Direction m_RightStickDir = Direction::None;
};
