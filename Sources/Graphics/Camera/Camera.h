// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Camera.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/24 13:05 : 作成
// 概要   : カメラクラスの定義
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <cstdint>
#include <d3d12.h>
#include <System/SimpleMath.h>

// ====== クラス定義 ======
class Camera
{
public: // 列挙体・構造体定義
	// イベントタイプ
	enum EventType
	{
		EventRotate = 0x1 << 0,		// 回転
		EventDolly = 0x1 << 1,		// ズーム
		EventMove = 0x1 << 2,		// 移動
		EventPanTilt = 0x1 << 3,	// パン・チルト
		EventReset = 0x1 << 4,		// リセット
	};

	// イベント構造体
	struct Event
	{
		uint32_t    Type = 0;       // イベントタイプ
		float       RotateH = 0.0f;	// 水平方向の回転角(rad)
		float       RotateV = 0.0f; // 垂直方向の回転角(rad)
		float       Pan = 0.0f;     // 左右の首振り角(rad)
		float       Tilt = 0.0f;    // 上下の首振り角(rad)
		float       Dolly = 0.0f;   // ズーム量
		float       MoveX = 0.0f;   // X軸方向への移動量(ビュー空間)
		float       MoveY = 0.0f;   // Y軸方向への移動量(ビュー空間)
		float       MoveZ = 0.0f;   // Z軸方向への移動量(ビュー空間)
	};

public:	//	パブリック関数
	// コンストラクタ
	Camera();
	// デストラクタ
	~Camera();

	/// <summary>
	/// 位置を設定
	/// </summary>
	/// <param name="position">位置</param>
	void SetPosition(const DirectX::SimpleMath::Vector3& position);

	/// <summary>
	/// 注視点を設定
	/// </summary>
	/// <param name="target">注視点</param>
	void SetTarget(const DirectX::SimpleMath::Vector3& target);

	/// <summary>
	/// イベントの更新処理
	/// </summary>
	/// <param name="event">イベント</param>
	void UpdateEvent(const Event& event);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 保存
	/// </summary>
	void Preserve();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

public: // アクセサ関数
	/// <summary>
	/// カメラの垂直方向回転角(rad)を取得
	/// </summary>
	/// <returns>垂直方向回転角(rad)</returns>
	const float& GetAngleV() const;

	/// <summary>
	/// カメラの水平方向回転角(rad)を取得
	/// </summary>
	/// <returns>水平方向回転角(rad)</returns>
	const float& GetAngleH() const;

	/// <summary>
	/// カメラから注視点までの距離を取得
	/// </summary>
	/// <returns>距離</returns>
	const float& GetDistance() const;

	/// <summary>
	/// カメラの位置を取得
	/// </summary>
	/// <returns>位置</returns>
	const DirectX::SimpleMath::Vector3& GetPosition() const;

	/// <summary>
	/// カメラの注視点を取得
	/// </summary>
	/// <returns>注視点</returns>
	const DirectX::SimpleMath::Vector3& GetTarget() const;

	/// <summary>
	/// カメラの上向きベクトルを取得
	/// </summary>
	/// <returns>上方向</returns>
	const DirectX::SimpleMath::Vector3& GetUpward() const;

	/// <summary>
	/// カメラの前方向を取得
	/// </summary>
	/// <returns>前方向</returns>
	const DirectX::SimpleMath::Vector3& GetForward() const;

	/// <summary>
	/// ビュー行列を取得
	/// </summary>
	/// <returns>ビュー行列</returns>
	const DirectX::SimpleMath::Matrix& GetView() const;

private:	// プライベート列挙体・構造体
	// フラグ
	enum DirtyFlag
	{
		DirtyNone = 0x0,			// 再計算なし
		DirtyPosition = 0x1 << 0,	// 位置を再計算
		DirtyTarget = 0x1 << 1,		// 注視点を再計算
		DirtyAngle = 0x1 << 2,		// 回転角を再計算
		DirtyMatrix = 0x1 << 3,		// 行列を再計算
	};

	// パラメータ構造体
	struct Param
	{
		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Vector3 Target;
		DirectX::SimpleMath::Vector3 Upward;
		DirectX::SimpleMath::Vector3 Forward;
		DirectX::XMFLOAT2 Angle;
		float Distance;
	};

private:	// プライベート関数
	// 回転
	void Rotate(float angleH, float angleV);
	// パン・チルト
	void Pantilt(float angleH, float angleV);
	// 移動
	void Move(float moveX, float moveY, float moveZ);
	// ズーム
	void Dolly(float value);
	// 位置の計算
	void ComputePosition();
	// 注視点の計算
	void ComputeTarget();
	// 回転角の計算
	void ComputeAngle();

private:	// メンバ変数
	Param m_Current = {};	// 現在のパラメータ
	Param m_Preserve = {};	// 保存用パラメータ
	DirectX::SimpleMath::Matrix m_View = DirectX::SimpleMath::Matrix::Identity;	// ビュー行列
	uint32_t m_DirtyFlag = 0;	// フラグ

};

class Projector
{
public:	// 列挙体定義
	enum Mode
	{
		Perspective,        //!< 透視投影.
		Orhographic,        //!< 正射影.
	};

public:	// パブリック関数
	// コンストラクタ
	Projector();
	// デストラクタ
	~Projector();

	/// <summary>
	/// 現在の射影パラメータを保存
	/// </summary>
	void Preserve();
	/// <summary>
	/// 射影パラメータをリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 透視投影パラメータを設定
	/// </summary>
	/// <param name="fov"></param>
	/// <param name="aspect"></param>
	/// <param name="nearClip"></param>
	/// <param name="farClip"></param>
	void SetPerspective(float fov, float aspect, float nearClip, float farClip);

	/// <summary>
	/// 正射影パラメータを設定
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <param name="top"></param>
	/// <param name="bottom"></param>
	/// <param name="nearClip"></param>
	/// <param name="farClip"></param>
	void SetOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

public:	// アクセサ関数
	/// <summary>
	/// 射影モードを取得
	/// </summary>
	/// <returns>射影モード</returns>
	const Mode& GetMode() const;

	/// <summary>
	/// 垂直画角を取得
	/// </summary>
	/// <returns>垂直画角</returns>
	const float& GetFieldOfView() const;

	/// <summary>
	/// アスペクト比を取得
	/// </summary>
	/// <returns>アスペクト比</returns>
	const float& GetAspect() const;

	/// <summary>
	/// 近クリップ平面までの距離を取得
	/// </summary>
	/// <returns>近クリップ平面までの距離</returns>
	const float& GetNearClip() const;

	/// <summary>
	/// 遠クリップ平面までの距離を取得
	/// </summary>
	/// <returns>遠クリップ平面までの距離</returns>
	const float& GetFarClip() const;

	/// <summary>
	/// 射影行列を取得
	/// </summary>
	/// <returns>射影行列</returns>
	const DirectX::SimpleMath::Matrix& GetMatrix() const;

private:	// プライベート列挙体・構造体
	struct Param
	{
		Mode Mode;          // 射影モード
		float Aspect;       // アスペクト比
		float FieldOfView;	// 垂直画角
		float Left;         // 左端
		float Right;        // 右端
		float Top;          // 上端
		float Bottom;       // 下端
		float NearClip;     // 近クリップ平面までの距離
		float FarClip;      // 遠クリップ平面までの距離
	};

private:	// メンバ変数
	Param m_Current;					// 現在のパラメータ
	Param m_Preserve;					// 保存用パラメータ
	DirectX::SimpleMath::Matrix m_Proj; // 射影行列
};