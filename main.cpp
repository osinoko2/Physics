#include <Novice.h>
#include <cmath>
#include <cassert>
#include <imgui.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdint>
#include <algorithm>

const char kWindowTitle[] = "LE2B_12_サクライショウセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center;
	float radius;
	unsigned int color;
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

struct Pendulum
{
	Vector3 anchor; // アンカーポイント。
	float length; // 紐の長さ
	float angle; // 現在の角度
	float angularVelocity; // 角速度
	float angularAcceleration; // 角加速度
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

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 Inverse(const Matrix4x4& m);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawLine(Vector3 v1, Vector3 v2, Matrix4x4 viewProjectionMatrix, Matrix4x4 viewportMatrix, unsigned int color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float deltaTime = 1.0f / 60.0f;

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };

	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Vector3 p = {};

	Pendulum pendulum;
	pendulum.anchor = { 0.0f, 1.0f, 0.0f };
	pendulum.length = 0.8f;
	pendulum.angle = 0.7f;
	pendulum.angularVelocity = 0.0f;
	pendulum.angularAcceleration = 0.0f;

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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f,1.0f }, cameraRotate, cameraTranslate);

		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.f);

		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

		pendulum.angularAcceleration = -(9.8f / pendulum.length) * std::sin(pendulum.angle);
		pendulum.angularVelocity += pendulum.angularAcceleration * deltaTime;
		pendulum.angle += pendulum.angularVelocity * deltaTime;

		// pは振り子の先端の位置。取り付けたいものを取り付ければ良い
		p.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
		p.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
		p.z = pendulum.anchor.z;

		Sphere sphere =
		{
			p,
			0.05f,
			WHITE
		};

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

		DrawLine(pendulum.anchor, p, viewProjectionMatrix, viewportMatrix, WHITE);

		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, sphere.color);

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

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 a;
	a.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	a.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	a.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	a.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	a.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	a.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	a.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	a.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	a.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	a.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	a.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	a.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	a.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	a.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	a.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	a.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return a;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = 1;
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = std::cos(radian);
	a.m[1][2] = std::sin(radian);
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = -std::sin(radian);
	a.m[2][2] = std::cos(radian);
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = std::cos(radian);
	a.m[0][1] = 0;
	a.m[0][2] = -std::sin(radian);
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = 1;
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = std::sin(radian);
	a.m[2][1] = 0;
	a.m[2][2] = std::cos(radian);
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 a;
	a.m[0][0] = std::cos(radian);
	a.m[0][1] = std::sin(radian);
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = -std::sin(radian);
	a.m[1][1] = std::cos(radian);
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = 1;
	a.m[2][3] = 0;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = 0;
	a.m[3][3] = 1;
	return a;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 a;
	Matrix4x4 b;

	b = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	a.m[0][0] = scale.x * b.m[0][0];
	a.m[0][1] = scale.x * b.m[0][1];
	a.m[0][2] = scale.x * b.m[0][2];
	a.m[0][3] = 0;
	a.m[1][0] = scale.y * b.m[1][0];
	a.m[1][1] = scale.y * b.m[1][1];
	a.m[1][2] = scale.y * b.m[1][2];
	a.m[1][3] = 0;
	a.m[2][0] = scale.z * b.m[2][0];
	a.m[2][1] = scale.z * b.m[2][1];
	a.m[2][2] = scale.z * b.m[2][2];
	a.m[2][3] = 0;
	a.m[3][0] = translate.x;
	a.m[3][1] = translate.y;
	a.m[3][2] = translate.z;
	a.m[3][3] = 1;

	return a;
}

Matrix4x4 Inverse(const Matrix4x4& m)
{
	Matrix4x4 a;
	float detA =
		m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -

		m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -

		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +

		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +

		m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -

		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -

		m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +

		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	a.m[0][0] = 1 / detA * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	a.m[0][1] = 1 / detA * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	a.m[0][2] = 1 / detA * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	a.m[0][3] = 1 / detA * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	a.m[1][0] = 1 / detA * -(m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][2]);
	a.m[1][1] = 1 / detA * -(-m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][2]);
	a.m[1][2] = 1 / detA * -(m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][2]);
	a.m[1][3] = 1 / detA * -(-m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][2]);

	a.m[2][0] = 1 / detA * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	a.m[2][1] = 1 / detA * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	a.m[2][2] = 1 / detA * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	a.m[2][3] = 1 / detA * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	a.m[3][0] = 1 / detA * -(m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][2] * m.m[3][1]);
	a.m[3][1] = 1 / detA * -(-m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][1] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][2] + m.m[0][0] * m.m[2][2] * m.m[3][1]);
	a.m[3][2] = 1 / detA * -(m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][1] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][2] - m.m[0][0] * m.m[1][2] * m.m[3][1]);
	a.m[3][3] = 1 / detA * -(-m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][1] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][2] + m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return a;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 a;
	a.m[0][0] = 1 / aspectRatio * 1 / std::tan(fovY / 2);
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = 1 / std::tan(fovY / 2);
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = farClip / (farClip - nearClip);
	a.m[2][3] = 1;
	a.m[3][0] = 0;
	a.m[3][1] = 0;
	a.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	a.m[3][3] = 0;
	return a;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 a;
	a.m[0][0] = width / 2;
	a.m[0][1] = 0;
	a.m[0][2] = 0;
	a.m[0][3] = 0;
	a.m[1][0] = 0;
	a.m[1][1] = -height / 2;
	a.m[1][2] = 0;
	a.m[1][3] = 0;
	a.m[2][0] = 0;
	a.m[2][1] = 0;
	a.m[2][2] = maxDepth - minDepth;
	a.m[2][3] = 0;
	a.m[3][0] = left + width / 2;
	a.m[3][1] = top + height / 2;
	a.m[3][2] = minDepth;
	a.m[3][3] = 1;
	return a;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 b;
	b.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
	b.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
	b.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);
	float w = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);
	assert(w != 0.0f);
	b.x /= w;
	b.y /= w;
	b.z /= w;
	return b;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);
	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,kGridHalfWidth });
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,-kGridHalfWidth });

		Matrix4x4 startWorldViewProjectionMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

		Vector3 ndcStartVertex = Transform({ -2,0,0 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = Transform({ -2,0,0 }, endWorldViewProjectionMatrix);

		Vector3 screenStartVertex = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = Transform(ndcEndVertex, viewportMatrix);
		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {

		Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { kGridHalfWidth,0.0f,(zIndex * kGridEvery) });
		Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { -kGridHalfWidth,0.0f,(zIndex * kGridEvery) });

		Matrix4x4 startWorldViewProjectionMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

		Vector3 ndcStartVertex = Transform({ 0,0,-2 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = Transform({ 0,0,-2 }, endWorldViewProjectionMatrix);

		Vector3 screenStartVertex = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = Transform(ndcEndVertex, viewportMatrix);

		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 12;
	const float kLonEvery = 2.0f * float(M_PI) / kSubdivision;
	const float kLatEvery = 2.0f * float(M_PI) / kSubdivision;
	float pi = float(M_PI);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			Vector3 a, b, c;

			a = { sphere.radius * cosf(lon) * cosf(lat) + sphere.center.x, sphere.radius * sinf(lon) + sphere.center.y, sphere.radius * cosf(lon) * sinf(lat) + sphere.center.z };

			b = { sphere.radius * cosf(lon + kLonEvery) * cosf(lat) + sphere.center.x, sphere.radius * sinf(lon + kLonEvery) + sphere.center.y, sphere.radius * cosf(lon + kLonEvery) * sinf(lat) + sphere.center.z };

			c = { sphere.radius * cosf(lon) * cosf(lat + kLatEvery) + sphere.center.x, sphere.radius * sinf(lon) + sphere.center.y, sphere.radius * cosf(lon) * sinf(lat + kLatEvery) + sphere.center.z };

			Matrix4x4 aWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, a);
			Matrix4x4 bWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, b);
			Matrix4x4 cWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, c);

			Matrix4x4 aWorldViewProjectionMatrix = Multiply(aWorldMatrix, viewProjectionMatrix);
			Matrix4x4 bWorldViewProjectionMatrix = Multiply(bWorldMatrix, viewProjectionMatrix);
			Matrix4x4 cWorldViewProjectionMatrix = Multiply(cWorldMatrix, viewProjectionMatrix);

			Vector3 ndcAVertex = Transform({ 0,0,0 }, aWorldViewProjectionMatrix);
			Vector3 ndcBVertex = Transform({ 0,0,0 }, bWorldViewProjectionMatrix);
			Vector3 ndcCVertex = Transform({ 0,0,0 }, cWorldViewProjectionMatrix);

			Vector3 screenAVertex = Transform(ndcAVertex, viewportMatrix);
			Vector3 screenBVertex = Transform(ndcBVertex, viewportMatrix);
			Vector3 screenCVertex = Transform(ndcCVertex, viewportMatrix);

			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenBVertex.x), int(screenBVertex.y), color);
			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenCVertex.x), int(screenCVertex.y), color);
		}
	}
}

void DrawLine(Vector3 v1, Vector3 v2, Matrix4x4 viewProjectionMatrix, Matrix4x4 viewportMatrix, unsigned int color)
{

	// ワールド座標系
	Matrix4x4 startWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, v1);
	Matrix4x4 endWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, v2);

	// 同時クリップ空間
	Matrix4x4 startWorldViewProjectionMatrix = Multiply(startWorldMatrix, viewProjectionMatrix);
	Matrix4x4 endWorldViewProjectionMatrix = Multiply(endWorldMatrix, viewProjectionMatrix);

	// 正規化デバイス座標系 -> スクリーン座標系
	Vector3 startScreen = Transform(Transform({ 0.0f,0.0f,0.0f }, startWorldViewProjectionMatrix), viewportMatrix);
	Vector3 endScreen = Transform(Transform({ 0.0f,0.0f,0.0f }, endWorldViewProjectionMatrix), viewportMatrix);

	// 描画
	Novice::DrawLine(static_cast<int>(startScreen.x), static_cast<int>(startScreen.y), static_cast<int>(endScreen.x), static_cast<int>(endScreen.y), color);
}