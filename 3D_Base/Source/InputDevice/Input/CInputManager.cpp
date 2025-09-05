#include "CInputManager.h"
#include <cmath>

CInputManager::CInputManager(DWORD ID)
    : m_XInput(nullptr)
    , m_KeyInput(nullptr)
    , m_UseKeyInput(false)
{
    m_XInput = new CXInput(ID);

    if (ID == 0) // 0番だけキーボード操作をできるようにする
    {
        m_UseKeyInput = true;
        m_KeyInput = std::make_unique<CKeyInput>();
    }
}

CInputManager::~CInputManager()
{
    SAFE_DELETE(m_XInput);
}

void CInputManager::Update()
{
    // スティックの入力と方向を取得し内部に保持
    m_LeftStickDir = GetLeftStickDirection(m_LeftStickVec);     // 左スティック
    m_RightStickDir = GetRightStickDirection(m_RightStickVec);  // 右スティック
    
    if (m_XInput != nullptr)
    {
        m_XInput->Update();
    }

    if (m_UseKeyInput)
    {
        m_KeyInput->Update();
    }
}

// 左スティックの入力検知
CInputManager::Direction CInputManager::GetLeftStickDirection(Vector2& Vec)
{
    // スティックの軸データを取得
    SHORT LeftX = m_XInput->GetLThumbX();
    SHORT LeftY = m_XInput->GetLThumbY();

    // floatに変換して、-1.0f～1.0に正規化
    float fx = static_cast<float>(LeftX) / MAX_THUMB_VALUE;
    float fy = static_cast<float>(LeftY) / MAX_THUMB_VALUE;

    // デッドゾーン処理
    // スティックが閾値より小さいときは無操作判定
    if (std::fabs(fx) < THRESHOLD && std::fabs(fy) < THRESHOLD)
    {
        // ベクトルを0に設定(移動なし)
        Vec = { 0.0f, 0.0f };

        // 方向なしを返す
        return Direction::None;
    }

    // スティックの入力格納
    Vec = { fx, fy };

    // ベクトルの方向に応じた方向列挙を返す
    return GetDirectionFromXY(fx, fy, THRESHOLD);
}

// 右スティックの入力検知
CInputManager::Direction CInputManager::GetRightStickDirection(Vector2& Vec)
{
    // スティックの軸データを取得
    SHORT LeftX = m_XInput->GetRThumbX();
    SHORT LeftY = m_XInput->GetRThumbY();

    // floatに変換して、-1.0f～1.0に正規化
    float fx = static_cast<float>(LeftX) / MAX_THUMB_VALUE;
    float fy = static_cast<float>(LeftY) / MAX_THUMB_VALUE;

    // デッドゾーン処理
    // スティックが閾値より小さいときは無操作判定
    if (std::fabs(fx) < THRESHOLD && std::fabs(fy) < THRESHOLD)
    {
        // ベクトルを0に設定(移動なし)
        Vec = { 0.0f, 0.0f };

        // 方向なしを返す
        return Direction::None;
    }

    // スティックの入力格納
    Vec = { fx, fy };

    // ベクトルの方向に応じた方向列挙を返す
    return GetDirectionFromXY(fx, fy, THRESHOLD);
}

// 左トリガーの入力検知
CInputManager::Trigger CInputManager::GetLeftTrigger()
{
    BYTE PushTrigger = m_XInput->GetLTrigger();

    // トリガーの押し込み
    if (PushTrigger > TriggerTHRESHOLD)
    {
        return Trigger::LeftTrigger; // 左トリガーを返す
    }

    return Trigger::None; // 無設定を返す
}

// 右トリガーの入力検知
CInputManager::Trigger CInputManager::GetRightTrigger()
{
    BYTE PushTrigger = m_XInput->GetRTrigger();

    // トリガーの押し込み
    if (PushTrigger > TriggerTHRESHOLD)
    {
        return Trigger::RightTrigger; // 右トリガーを返す
    }

    return Trigger::None; // 無設定を返す
}

// WASDキーの入力検知
CInputManager::Direction CInputManager::GetWASDKeyDirection()
{
    float x = 0.f;
    float y = 0.f;

    if (m_UseKeyInput)
    {
        if (m_KeyInput->IsRepeat('W')) y = 1.0f;
        if (m_KeyInput->IsRepeat('S')) y = -1.0f;
        if (m_KeyInput->IsRepeat('A')) x = -1.0f;
        if (m_KeyInput->IsRepeat('D')) x = 1.0f;
    }

    return GetDirectionFromXY(x, y, THRESHOLD);
}

// 矢印キーの入力検知
CInputManager::Direction CInputManager::GetArrowKeyDirection()
{
    float x = 0.f;
    float y = 0.f;

    if (m_UseKeyInput)
    {
        if (m_KeyInput->IsRepeat(VK_UP)) y = 1.0f;
        if (m_KeyInput->IsRepeat(VK_DOWN)) y = -1.0f;
        if (m_KeyInput->IsRepeat(VK_LEFT)) x = -1.0f;
        if (m_KeyInput->IsRepeat(VK_RIGHT)) x = 1.0f;
    }

    return GetDirectionFromXY(x, y, THRESHOLD);
}

// XY座標から方向列挙型に変換する共通関数
CInputManager::Direction CInputManager::GetDirectionFromXY(float x, float y, float threshold)
{
    // 上方向(Y軸がプラス)
    if (y > threshold) 
    {
        // 左方向(X軸がマイナス)
        if (x < -threshold)
        {
            // 左上方向を返す
            return Direction::UpLeft;
        }

        // 右方向(X軸がプラス)
        if (x > threshold)
        {
            // 右上方向を返す
            return Direction::UpRight;
        }

        // 左右入力がないときは上方向を返す
        return Direction::Up;
    }

    // 下方向(Y軸がマイナス)
    if (y < -threshold)
    {
        // 左方向(X軸がマイナス)
        if (x < -threshold)
        {
            // 左下方向を返す
            return Direction::DownLeft;
        }

        // 右方向(X軸がプラス)
        if (x > threshold)
        {
            // 右下方向を返す
            return Direction::DownRight;
        }

        // 左右入力がないときは下を返す
        return Direction::Down;
    }
    else // Y軸入力がないとき
    {
        // 右方向(X軸がプラス)
        if (x > threshold)
        {
            // 右方向を返す
            return Direction::Right;
        }

        // 左方向(X軸がマイナス)
        if (x < -threshold)
        {
            // 左方向を返す
            return Direction::Left;
        }
    }

    // 無入力を返す
    return Direction::None;
}

// キーボード用
// キーを押した瞬間(トリガー)
bool CInputManager::IsKeyboardDown(int key)
{
    if (!m_UseKeyInput || !m_KeyInput)
        return false;
    return m_KeyInput->IsDown(key);
}

// キーが押され続けている
bool CInputManager::IsKeyboardRepeat(int key)
{
    if (!m_UseKeyInput || !m_KeyInput)
        return false;
    return m_KeyInput->IsRepeat(key);
}

// キーが離された(リリース)
bool CInputManager::IsKeyboardUp(int key)
{
    if (!m_UseKeyInput || !m_KeyInput)
        return false;
    return m_KeyInput->IsUp(key);
}

// コントローラー用
// ボタンを押した瞬間(トリガー)
bool CInputManager::IsControllerDown(CXInput::KEY key)
{
    if (!m_XInput)
        return false;
    return m_XInput->IsDown(key);
}

// ボタンが押され続けている
bool CInputManager::IsControllerRepeat(CXInput::KEY key)
{
    if (!m_XInput)
        return false;
    return m_XInput->IsDown(key);
}

// ボタンが離された(リリース)
bool CInputManager::IsControllerUp(CXInput::KEY key)
{
    if (!m_XInput)
        return false;
    return m_XInput->IsDown(key);
}
