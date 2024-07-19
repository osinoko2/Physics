#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LE2B_12_サクライショウセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct Spring {
	Vector3 anchor; // アンカー。固定された端の位置
	float naturalLength; // 自然長
	float stiffness; // 剛性。バネ定数k
	float dampingCoefficient; // 減衰係数
};

struct Ball {
	Vector3 position; // ボールの位置
	Vector3 velocity; // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass; // ボールの質量
	float radius; // ボールの半径
	unsigned int color; // ボールの色
};

float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v);
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Spring spring{};
	spring.anchor = { 0.0f, 0.0f, 0.0f };
	spring.naturalLength = 1.0f;
	spring.stiffness = 100.0f;
	spring.dampingCoefficient = 2.0f;

	Ball ball{};
	ball.position = { 1.2f, 0.0f, 0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;

	float deltaTime = 1.0f / 60.0f;
	Vector3 diff = ball.position - spring.anchor;
	float length = Length(diff);
	if (length != 0.0f){
		Vector3 direction = Normalize(diff);
		Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
		Vector3 displacement = length * (ball.position - restPosition);
		Vector3 restoringForce = -spring.stiffness * displacement;
		// 減衰抵抗を計算する
		Vector3 dampingForce = -spring.dampingCoefficient * ball.velocity;
		// 減衰抵抗も加味して、物体にかかる力を決定する
		Vector3 force = restoringForce + dampingForce;
		ball.acceleration = force / ball.mass;
	}

	// 加速度も速度もどちらも秒を基準とした値である
	// それが、1/60秒間(deltaTime)適用されたと考える
	ball.velocity.x += ball.acceleration.x * deltaTime;
	ball.velocity.y += ball.acceleration.y * deltaTime;
	ball.velocity.z += ball.acceleration.z * deltaTime;
	ball.position.x += ball.velocity.x * deltaTime;
	ball.position.y += ball.velocity.y * deltaTime;
	ball.position.z += ball.velocity.z * deltaTime;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		float angularVelocity = 3.14f;
		float angle = 0.0f;

		///
		/// ↓更新処理ここから
		///

		angle += angularVelocity * deltaTime;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

float Length(const Vector3& v)
{
	float b;
	b = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return b;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 a;
	float b;
	b = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	a.x = v.x / b;
	a.y = v.y / b;
	a.z = v.z / b;
	return a;
}

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	Vector3 a;
	a.x = v1.x + v2.x;
	a.y = v1.y + v2.y;
	a.z = v1.z + v2.z;
	return a;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	Vector3 a;
	a.x = v1.x - v2.x;
	a.y = v1.y - v2.y;
	a.z = v1.z - v2.z;
	return a;
}

Vector3 Multiply(float scalar, const Vector3& v)
{
	Vector3 a;
	a.x = scalar * v.x;
	a.y = scalar * v.y;
	a.z = scalar * v.z;
	return a;
}