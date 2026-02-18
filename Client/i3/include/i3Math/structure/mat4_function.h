#pragma once

namespace i3
{
	struct mat4;

	
	float mat4_determinant(const mat4& matInput);

	// 역행렬을 구할수 없는 경우 리턴을 false로 잡아둔다..
	bool mat4_inv(mat4& matResult, const mat4& matInput);
	bool mat4_inv(mat4& matResult, const mat4& matInput, float& outDeterminant);

	// 정규직교행렬 역행렬함수(기존 역행렬함수보다 계산량이 적다)
	void mat4_inv_ortho_normal(mat4& matResult, const mat4& matInput);
	// 직교행렬 역행렬함수(기존 역행렬함수보다 계산량이 적다)
	void mat4_inv_ortho(mat4& matResult, const mat4& matInput);

	// 정규직교행렬 역행렬*전치 함수(평면변환시에 필수로 쓰인다.)
	void mat4_inv_transpose_ortho_normal(mat4& matResult, const mat4& matInput);
	// 직교행렬 역행렬*전치 함수(평면변환시에 필수로 쓰인다.)
	void mat4_inv_transpose_ortho(mat4& matResult, const mat4& matInput);

	void mat4_transpose(mat4& matResult, const mat4& matInput);
	
}
