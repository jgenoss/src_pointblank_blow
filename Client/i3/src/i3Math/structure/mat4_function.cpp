#include "i3MathType.h"
#include "structure/mat4_function.h"
#include "structure/mat4.h"

namespace i3
{
	namespace detail
	{
		bool mat4_inv_impl(mat4& matResult, const mat4& matInput, float* pfDet)
		{
			float t[3], v[16];
			
			t[0] = matInput.m[2][2] * matInput.m[3][3] - matInput.m[2][3] * matInput.m[3][2];
			t[1] = matInput.m[1][2] * matInput.m[3][3] - matInput.m[1][3] * matInput.m[3][2];
			t[2] = matInput.m[1][2] * matInput.m[2][3] - matInput.m[1][3] * matInput.m[2][2];
			v[0] = matInput.m[1][1] * t[0] - matInput.m[2][1] * t[1] + matInput.m[3][1] * t[2];
			v[4] = -matInput.m[1][0] * t[0] + matInput.m[2][0] * t[1] - matInput.m[3][0] * t[2];

			t[0] = matInput.m[1][0] * matInput.m[2][1] - matInput.m[2][0] * matInput.m[1][1];
			t[1] = matInput.m[1][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[1][1];
			t[2] = matInput.m[2][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[2][1];
			v[8] = matInput.m[3][3] * t[0] - matInput.m[2][3] * t[1] + matInput.m[1][3] * t[2];
			v[12] = -matInput.m[3][2] * t[0] + matInput.m[2][2] * t[1] - matInput.m[1][2] * t[2];

			float det = matInput.m[0][0] * v[0] + matInput.m[0][1] * v[4] +
				matInput.m[0][2] * v[8] + matInput.m[0][3] * v[12];

			if (det == 0.0f)
				return false;

			if (pfDet)
				*pfDet = det;

			t[0] = matInput.m[2][2] * matInput.m[3][3] - matInput.m[2][3] * matInput.m[3][2];
			t[1] = matInput.m[0][2] * matInput.m[3][3] - matInput.m[0][3] * matInput.m[3][2];
			t[2] = matInput.m[0][2] * matInput.m[2][3] - matInput.m[0][3] * matInput.m[2][2];
			v[1] = -matInput.m[0][1] * t[0] + matInput.m[2][1] * t[1] - matInput.m[3][1] * t[2];
			v[5] = matInput.m[0][0] * t[0] - matInput.m[2][0] * t[1] + matInput.m[3][0] * t[2];

			t[0] = matInput.m[0][0] * matInput.m[2][1] - matInput.m[2][0] * matInput.m[0][1];
			t[1] = matInput.m[3][0] * matInput.m[0][1] - matInput.m[0][0] * matInput.m[3][1];
			t[2] = matInput.m[2][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[2][1];
			v[9] = -matInput.m[3][3] * t[0] - matInput.m[2][3] * t[1] - matInput.m[0][3] * t[2];
			v[13] = matInput.m[3][2] * t[0] + matInput.m[2][2] * t[1] + matInput.m[0][2] * t[2];

			t[0] = matInput.m[1][2] * matInput.m[3][3] - matInput.m[1][3] * matInput.m[3][2];
			t[1] = matInput.m[0][2] * matInput.m[3][3] - matInput.m[0][3] * matInput.m[3][2];
			t[2] = matInput.m[0][2] * matInput.m[1][3] - matInput.m[0][3] * matInput.m[1][2];
			v[2] = matInput.m[0][1] * t[0] - matInput.m[1][1] * t[1] + matInput.m[3][1] * t[2];
			v[6] = -matInput.m[0][0] * t[0] + matInput.m[1][0] * t[1] - matInput.m[3][0] * t[2];

			t[0] = matInput.m[0][0] * matInput.m[1][1] - matInput.m[1][0] * matInput.m[0][1];
			t[1] = matInput.m[3][0] * matInput.m[0][1] - matInput.m[0][0] * matInput.m[3][1];
			t[2] = matInput.m[1][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[1][1];
			v[10] = matInput.m[3][3] * t[0] + matInput.m[1][3] * t[1] + matInput.m[0][3] * t[2];
			v[14] = -matInput.m[3][2] * t[0] - matInput.m[1][2] * t[1] - matInput.m[0][2] * t[2];

			t[0] = matInput.m[1][2] * matInput.m[2][3] - matInput.m[1][3] * matInput.m[2][2];
			t[1] = matInput.m[0][2] * matInput.m[2][3] - matInput.m[0][3] * matInput.m[2][2];
			t[2] = matInput.m[0][2] * matInput.m[1][3] - matInput.m[0][3] * matInput.m[1][2];
			v[3] = -matInput.m[0][1] * t[0] + matInput.m[1][1] * t[1] - matInput.m[2][1] * t[2];
			v[7] = matInput.m[0][0] * t[0] - matInput.m[1][0] * t[1] + matInput.m[2][0] * t[2];

			v[11] = -matInput.m[0][0] * (matInput.m[1][1] * matInput.m[2][3] - matInput.m[1][3] * matInput.m[2][1]) +
				matInput.m[1][0] * (matInput.m[0][1] * matInput.m[2][3] - matInput.m[0][3] * matInput.m[2][1]) -
				matInput.m[2][0] * (matInput.m[0][1] * matInput.m[1][3] - matInput.m[0][3] * matInput.m[1][1]);

			v[15] = matInput.m[0][0] * (matInput.m[1][1] * matInput.m[2][2] - matInput.m[1][2] * matInput.m[2][1]) -
				matInput.m[1][0] * (matInput.m[0][1] * matInput.m[2][2] - matInput.m[0][2] * matInput.m[2][1]) +
				matInput.m[2][0] * (matInput.m[0][1] * matInput.m[1][2] - matInput.m[0][2] * matInput.m[1][1]);

			det = 1.0f / det;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					matResult.m[i][j] = v[4 * i + j] * det;
				}
			}
			return true;
		}
	}

	bool mat4_inv(mat4& matResult, const mat4& matInput)
	{
		return detail::mat4_inv_impl(matResult, matInput, nullptr);
	}

	bool mat4_inv(mat4& matResult, const mat4& matInput, float& outDeterminant)
	{
		return detail::mat4_inv_impl(matResult, matInput, &outDeterminant);
	}
	
	float mat4_determinant(const mat4& matInput)
	{
		float t[3], v[4];

		t[0] = matInput.m[2][2] * matInput.m[3][3] - matInput.m[2][3] * matInput.m[3][2];
		t[1] = matInput.m[1][2] * matInput.m[3][3] - matInput.m[1][3] * matInput.m[3][2];
		t[2] = matInput.m[1][2] * matInput.m[2][3] - matInput.m[1][3] * matInput.m[2][2];
		v[0] = matInput.m[1][1] * t[0] - matInput.m[2][1] * t[1] + matInput.m[3][1] * t[2];
		v[1] = -matInput.m[1][0] * t[0] + matInput.m[2][0] * t[1] - matInput.m[3][0] * t[2];

		t[0] = matInput.m[1][0] * matInput.m[2][1] - matInput.m[2][0] * matInput.m[1][1];
		t[1] = matInput.m[1][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[1][1];
		t[2] = matInput.m[2][0] * matInput.m[3][1] - matInput.m[3][0] * matInput.m[2][1];
		v[2] = matInput.m[3][3] * t[0] - matInput.m[2][3] * t[1] + matInput.m[1][3] * t[2];
		v[3] = -matInput.m[3][2] * t[0] + matInput.m[2][2] * t[1] - matInput.m[1][2] * t[2];

		return matInput.m[0][0] * v[0] + matInput.m[0][1] * v[1] +
			matInput.m[0][2] * v[2] + matInput.m[0][3] * v[3];
	}


	// 정규직교행렬 역행렬함수(기존 역행렬함수보다 계산량이 적다)
	void mat4_inv_ortho_normal(mat4& matResult, const mat4& matInput)
	{
		const mat4* pmatRef = &matInput;
		mat4		matTemp;

		if (pmatRef == &matResult)		// 입/출력 값이 같은 주소인 경우..
		{
			matTemp = matInput;
			pmatRef = &matTemp;
		}

		const mat4& matInputRef = *pmatRef;

		matResult._11 = matInputRef._11;
		matResult._21 = matInputRef._12;
		matResult._31 = matInputRef._13;

		matResult._41 = -(matInputRef._41 *matInputRef._11 + matInputRef._42*matInputRef._12 + matInputRef._43*matInputRef._13);	// - P * XAxis

		matResult._12 = matInputRef._21;
		matResult._22 = matInputRef._22;
		matResult._32 = matInputRef._23;
		matResult._42 = -(matInputRef._41*matInputRef._21 + matInputRef._42*matInputRef._22 + matInputRef._43*matInputRef._23);	// - P * YAxis

		matResult._13 = matInputRef._31;
		matResult._23 = matInputRef._32;
		matResult._33 = matInputRef._33;

		matResult._43 = -(matInputRef._41*matInputRef._31 + matInputRef._42*matInputRef._32 + matInputRef._43*matInputRef._33);	// - P * ZAxis

		matResult._14 = 0.0f;	matResult._24 = 0.0f;	matResult._34 = 0.0f;	matResult._44 = 1.0f;
	}

	// 직교행렬 역행렬함수(기존 역행렬함수보다 계산량이 적다)
	void mat4_inv_ortho(mat4& matResult, const mat4& matInput)
	{
		float fScale_InvSq_Row1 = 1.f / (matInput._11 * matInput._11 + matInput._12 * matInput._12 + matInput._13 * matInput._13);
		float fScale_InvSq_Row2 = 1.f / (matInput._21 * matInput._21 + matInput._22 * matInput._22 + matInput._23 * matInput._23);
		float fScale_InvSq_Row3 = 1.f / (matInput._31 * matInput._31 + matInput._32 * matInput._32 + matInput._33 * matInput._33);

		mat4 matTemp = matInput;

		matTemp._11 *= fScale_InvSq_Row1;		matTemp._12 *= fScale_InvSq_Row1;		matTemp._13 *= fScale_InvSq_Row1;
		matTemp._21 *= fScale_InvSq_Row2;		matTemp._22 *= fScale_InvSq_Row2;		matTemp._23 *= fScale_InvSq_Row2;
		matTemp._31 *= fScale_InvSq_Row3;		matTemp._32 *= fScale_InvSq_Row3;		matTemp._33 *= fScale_InvSq_Row3;


		matResult._11 = matTemp._11;
		matResult._21 = matTemp._12;
		matResult._31 = matTemp._13;

		matResult._41 = -(matTemp._41 *matTemp._11 + matTemp._42*matTemp._12 + matTemp._43*matTemp._13);	// - P * XAxis

		matResult._12 = matTemp._21;
		matResult._22 = matTemp._22;
		matResult._32 = matTemp._23;
		matResult._42 = -(matTemp._41*matTemp._21 + matTemp._42*matTemp._22 + matTemp._43*matTemp._23);	// - P * YAxis

		matResult._13 = matTemp._31;
		matResult._23 = matTemp._32;
		matResult._33 = matTemp._33;

		matResult._43 = -(matTemp._41*matTemp._31 + matTemp._42*matTemp._32 + matTemp._43*matTemp._33);	// - P * ZAxis

		matResult._14 = 0.0f;	matResult._24 = 0.0f;	matResult._34 = 0.0f;	matResult._44 = 1.0f;

	}

	// 정규직교행렬 역행렬*전치 함수(평면변환시에 필수로 쓰인다.)
	void mat4_inv_transpose_ortho_normal(mat4& matResult, const mat4& matInput)
	{
		const mat4* pmatRef = &matInput;
		mat4		matTemp;

		if (pmatRef == &matResult)		// 입/출력 값이 같은 주소인 경우..
		{
			matTemp = matInput;
			pmatRef = &matTemp;
		}

		const mat4& matInputRef = *pmatRef;
		
		matResult._11 = matInputRef._11;
		matResult._12 = matInputRef._12;
		matResult._13 = matInputRef._13;
		matResult._14 = -(matInputRef._41 *matInputRef._11 + matInputRef._42*matInputRef._12 + matInputRef._43*matInputRef._13);	// - P * XAxis

		matResult._21 = matInputRef._21;
		matResult._22 = matInputRef._22;
		matResult._23 = matInputRef._23;
		matResult._24 = -(matInputRef._41*matInputRef._21 + matInputRef._42*matInputRef._22 + matInputRef._43*matInputRef._23);	// - P * YAxis

		matResult._31 = matInputRef._31;
		matResult._32 = matInputRef._32;
		matResult._33 = matInputRef._33;
		matResult._34 = -(matInputRef._41*matInputRef._31 + matInputRef._42*matInputRef._32 + matInputRef._43*matInputRef._33);	// - P * ZAxis

		matResult._41 = 0.0f;	matResult._42 = 0.0f;	matResult._43 = 0.0f;	matResult._44 = 1.0f;
	}
	// 직교행렬 역행렬*전치 함수(평면변환시에 필수로 쓰인다.)
	void mat4_inv_transpose_ortho(mat4& matResult, const mat4& matInput)
	{
		float fScale_InvSq_Row1 = 1.f / (matInput._11 * matInput._11 + matInput._12 * matInput._12 + matInput._13 * matInput._13);
		float fScale_InvSq_Row2 = 1.f / (matInput._21 * matInput._21 + matInput._22 * matInput._22 + matInput._23 * matInput._23);
		float fScale_InvSq_Row3 = 1.f / (matInput._31 * matInput._31 + matInput._32 * matInput._32 + matInput._33 * matInput._33);

		mat4 matTemp = matInput;
		//
		matTemp._11 *= fScale_InvSq_Row1;		matTemp._12 *= fScale_InvSq_Row1;		matTemp._13 *= fScale_InvSq_Row1;
		matTemp._21 *= fScale_InvSq_Row2;		matTemp._22 *= fScale_InvSq_Row2;		matTemp._23 *= fScale_InvSq_Row2;
		matTemp._31 *= fScale_InvSq_Row3;		matTemp._32 *= fScale_InvSq_Row3;		matTemp._33 *= fScale_InvSq_Row3;
		//
		matResult._11 = matTemp._11;
		matResult._12 = matTemp._12;
		matResult._13 = matTemp._13;

		matResult._14 = -(matTemp._41 *matTemp._11 + matTemp._42*matTemp._12 + matTemp._43*matTemp._13); // - P * XAxis

		matResult._21 = matTemp._21;
		matResult._22 = matTemp._22;
		matResult._23 = matTemp._23;
		matResult._24 = -(matTemp._41*matTemp._21 + matTemp._42*matTemp._22 + matTemp._43*matTemp._23);	// - P * YAxis

		matResult._31 = matTemp._31;
		matResult._32 = matTemp._32;
		matResult._33 = matTemp._33;
		matResult._34 = -(matTemp._41*matTemp._31 + matTemp._42*matTemp._32 + matTemp._43*matTemp._33);	// - P * ZAxis

		matResult._41 = 0.0f;	matResult._42 = 0.0f;	matResult._43 = 0.0f;	matResult._44 = 1.0f;
	}

	void mat4_transpose(mat4& matResult, const mat4& matInput)
	{
		const mat4* pmatRef = &matInput;
		mat4		matTemp;

		if (pmatRef == &matResult)		// 입/출력 값이 같은 주소인 경우..
		{
			matTemp = matInput;
			pmatRef = &matTemp;
		}

		const mat4& matInputRef = *pmatRef;
		
		matResult._11 = matInputRef._11;			matResult._12 = matInputRef._21;
		matResult._13 = matInputRef._31;			matResult._14 = matInputRef._41;

		matResult._21 = matInputRef._12;			matResult._22 = matInputRef._22;
		matResult._23 = matInputRef._32;			matResult._24 = matInputRef._42;

		matResult._31 = matInputRef._13;			matResult._32 = matInputRef._23;
		matResult._33 = matInputRef._33;			matResult._34 = matInputRef._43;

		matResult._41 = matInputRef._14;			matResult._42 = matInputRef._24;
		matResult._43 = matInputRef._34;			matResult._44 = matInputRef._44;
	}

}