#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>

const char kWindowTitle[] = "LE2B_12_サクライショウセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct ConicalPendulum
{
	Vector3 anchor; // アンカーポイント。固定された端の位置
	float length; // 紐の長さ
	float halfApexAngle; // 円錐の直角の半分
	float angle; // 現在の角度
	float angularVelocity; // 角速度
};

struct Ball {
	Vector3 position; // ボールの位置
	Vector3 velocity; // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass; // ボールの質量
	float radius; // ボールの半径
	unsigned int color; // ボールの色
};

struct Sphere {
	Vector3 center;
	float radius;
};

Vector3 ProJect(const Vector3& v1, const Vector3& v2);
Vector3 Reflect(const Vector3& input, const Vector3& normal);
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	Ball ball;
	ConicalPendulum conicalPendulum;
	conicalPendulum.anchor = { 0.0f, 1.0f, 0.0f };
	conicalPendulum.length = 0.8f;
	conicalPendulum.halfApexAngle = 0.7f;
	conicalPendulum.angle = 0.0f;
	conicalPendulum.angularVelocity = 0.0f;
	float deltaTime = 1.0f / 60.0f;
	float radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	float height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	ball.acceleration = { 0.0f, -9.8f, 0.0f };
	ball.position.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
	ball.position.y = conicalPendulum.anchor.y - height;
	ball.position.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
		conicalPendulum.angle += conicalPendulum.angularVelocity * deltaTime;
		ball.velocity.x += ball.acceleration.x * deltaTime;
		ball.velocity.y += ball.acceleration.y * deltaTime;
		ball.velocity.z += ball.acceleration.z * deltaTime;
		ball.position.x += ball.velocity.x * deltaTime;
		ball.position.y += ball.velocity.y * deltaTime;
		ball.position.z += ball.velocity.z * deltaTime;
		if (IsCollision(Sphere{ball.position, ball.radius}, plane))
		{
			Vector3 reflected = Reflect(ball.velocity, plane.normal);
			Vector3 projectToNormal = ProJect(reflected, plane.normal);
			Vector3 movingDirection = reflected - projectToNormal;
			ball.velocity = projectToNormal * e + movingDirection;
		}

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

const float Dot(const Vector3& v1, const Vector3& v2)
{
	float b;
	b = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
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

Vector3 ProJect(const Vector3& v1, const Vector3& v2)
{
	Vector3 a;
	Vector3 b = Normalize(v2);
	a.x = Dot(v1, Normalize(v2)) * b.x;
	a.y = Dot(v1, Normalize(v2)) * b.y;
	a.z = Dot(v1, Normalize(v2)) * b.z;

	return a;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal)
{
	input - (2.0f * Dot(input, normal) * normal);
	return Vector3();
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
