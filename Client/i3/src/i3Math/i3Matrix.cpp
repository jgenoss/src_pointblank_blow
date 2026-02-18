#include "i3MathType.h"
#include "i3Matrix.h"
#include "i3Base/string/ext/safe_string_copy.h"

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
bool	i3Matrix::isValid(const char * pszName, MATRIX * pMat)
// bool	i3Matrix::isValid(i3Node* pNode, MATRIX * pMat)
{
	INT32 i, j;

	bool bValid = true;

	for( i = 0; i < 4; i++)
	{
		for( j = 0; j < 4; j++)
		{
			if( i3Math::isValid( pMat->m[i][j]) == false)
			{	
				//I3PRINTLOG(I3LOG_WARN, "!!!!!!!! i3Matrix::isValid %s \n", pszName);
//#ifdef FILL_ZERO_HEAP_AND_PHYSIX_DATA				
//				pMat->m[i][j] = 0.f;
//#else
//				int* iii = nullptr;
//				*iii = 10;
//#endif
				bValid = false;
				break;
			}
		}

		if( !bValid)
			break;
	}

	if( !bValid)
	{
		i3Matrix::Identity( pMat);
	}

	return bValid;
}
#endif // I3_CHECK_FLOAT_VALUE_FOR_PHYSICX

void	i3Matrix::SetToXML( i3XMLElement * pXML, const char * pszName, MATRIX * pMtx)
{
	char szName[256], szValue[256];

	sprintf( szName, "%s0", pszName);
	sprintf( szValue, "%f %f %f %f", pMtx->m00, pMtx->m01, pMtx->m02, pMtx->m03);
	pXML->SetAttribute( szName, szValue);

	sprintf( szName, "%s1", pszName);
	sprintf( szValue, "%f %f %f %f", pMtx->m10, pMtx->m11, pMtx->m12, pMtx->m13);
	pXML->SetAttribute( szName, szValue);

	sprintf( szName, "%s2", pszName);
	sprintf( szValue, "%f %f %f %f", pMtx->m20, pMtx->m21, pMtx->m22, pMtx->m23);
	pXML->SetAttribute( szName, szValue);

	sprintf( szName, "%s3", pszName);
	sprintf( szValue, "%f %f %f %f", pMtx->m30, pMtx->m31, pMtx->m32, pMtx->m33);
	pXML->SetAttribute( szName, szValue);
}

void	i3Matrix::GetFromXML( i3XMLElement * pXML, const char * pszName, MATRIX * pMtx)
{
	char szName[256], szValue[256], szToken[128];
	char sep;
	i3LineLexer lex;

	::memset(szToken, 0, sizeof(szToken));
	
	{
		sprintf( szName, "%s0", pszName);
		i3::safe_string_copy( szValue, pXML->Attribute( szName), 256);

		lex.SetMasterString( szValue);

		lex.GetNextToken( szToken, &sep, " ");			pMtx->m00 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m01 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m02 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m03 = (REAL32) atof( szToken);
	}

	{
		sprintf( szName, "%s1", pszName);
		i3::safe_string_copy( szValue, pXML->Attribute( szName), 256);

		lex.SetMasterString( szValue);

		lex.GetNextToken( szToken, &sep, " ");			pMtx->m10 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m11 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m12 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m13 = (REAL32) atof( szToken);
	}

	{
		sprintf( szName, "%s2", pszName);
		i3::safe_string_copy( szValue, pXML->Attribute( szName), 256);

		lex.SetMasterString( szValue);

		lex.GetNextToken( szToken, &sep, " ");			pMtx->m20 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m21 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m22 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m23 = (REAL32) atof( szToken);
	}

	{
		sprintf( szName, "%s3", pszName);
		i3::safe_string_copy( szValue, pXML->Attribute( szName), 256);

		lex.SetMasterString( szValue);

		lex.GetNextToken( szToken, &sep, " ");			pMtx->m30 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m31 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m32 = (REAL32) atof( szToken);
		lex.GetNextToken( szToken, &sep, " ");			pMtx->m33 = (REAL32) atof( szToken);
	}
}

void	i3Matrix::MakeWithUp( MATRIX * pOut, VEC3D * pUpDir, VEC3D * pAtDir)
{
	REAL32 dot;
	VEC3D * pRight, * pUp, * pAt;

	// At Vector!
	pRight = i3Matrix::GetRight( pOut); 
	pUp = i3Matrix::GetUp( pOut);
	pAt = i3Matrix::GetAt( pOut);

	i3Vector::Normalize( pUp, pUpDir); 

	// 문제!
	//
	// 각 Matrix 연산에서 VEC3D는 PSP에서 실제로는 16 bytes 길이다.
	// 이것을 Matrix에 적용할 경우, Shear 값으로 들어가는 결과를 유발할 수 있다.
	dot = i3Vector::Dot( pUp, pAtDir);
	if( i3Math::abs(dot) >= (1.0f - 0.00001f))
	{
		// 같은 방향의 Vector
		static REAL32 ALIGN16 s_Axis[3][4] =  
		{
			{	1.0f, 0.0f, 0.0f, 0.0f},
			{	0.0f, 1.0f, 0.0f, 0.0f},  
			{	0.0f, 0.0f, 1.0f, 0.0f}
		} ;
		INT32 i = 0;

		for( i = 0; (i < 3) && ( i3Math::abs( dot) >= (1.0f - 0.00001f)); i++)
		{
			dot = i3Vector::Dot( pUp, (VEC3D *) s_Axis[i]);

			if( i3Math::abs( dot) < (1.0f - 0.00001f))
			{
				i3Vector::Cross( pRight, (VEC3D *) s_Axis[i], pUp);
				i3Vector::Normalize( pRight, pRight);
				break;
			}
		}
	}
	else
	{
		i3Vector::Cross( pRight, pAtDir, pUp);
		i3Vector::Normalize( pRight, pRight);
	}

	i3Vector::Cross( pAt, pRight, pUp);
	i3Vector::Normalize( pAt, pAt);

	i3Matrix::SetPos( pOut, 0.0f, 0.0f, 0.0f, 1.0f);

	pOut->vec[0].w = pOut->vec[1].w = pOut->vec[2].w = 0.0f;
	//pRight->vec[3] = pUp->vec[3] = pAt->vec[3] = 0.0f;
}

void	i3Matrix::MakeWithAt( MATRIX * pOut, VEC3D * pAtDir, VEC3D * pUpDir)
{
	REAL32 dot;
	VEC3D * pRight, * pUp, * pAt;

	pRight = i3Matrix::GetRight( pOut); 
	pUp = i3Matrix::GetUp( pOut);
	pAt = i3Matrix::GetAt( pOut);

	// At Vector!
	i3Vector::Normalize( pAt, pAtDir);

	dot = i3Vector::Dot( pAt, pUpDir);
	if( i3Math::abs(dot) >= (1.0f - 0.00001f))
	{
		// 같은 방향의 Vector
		static REAL32 ALIGN16 s_Axis[3][4] = 
		{
			{	1.0f, 0.0f, 0.0f, 0.0f},
			{	0.0f, 1.0f, 0.0f, 0.0f},
			{	0.0f, 0.0f, 1.0f, 0.0f}
		};
		INT32 i = 0;

		for( i = 0; (i < 3) && ( i3Math::abs( dot) >= (1.0f - 0.00001f)); i++)
		{
			dot = i3Vector::Dot( pAt, (VEC3D *) s_Axis[i]);

			if( i3Math::abs( dot) < (1.0f - 0.00001f))
			{
				i3Vector::Cross( pRight, pAt, (VEC3D *) s_Axis[i]);
				i3Vector::Normalize( pRight, pRight);
				break;
			}
		}
	}
	else
	{
		i3Vector::Cross( pRight, pAt, pUpDir);
		i3Vector::Normalize( pRight, pRight);
	}

	i3Vector::Cross( pUp, pAt, pRight);
	i3Vector::Normalize( pUp, pUp);

	i3Matrix::SetPos( pOut, 0.0f, 0.0f, 0.0f, 1.0f);

	pOut->vec[0].w = pOut->vec[1].w = pOut->vec[2].w = 0.0f;
	//pRight->vec[3] = pUp->vec[3] = pAt->vec[3] = 0.0f;
}

#if defined( I3_PSP)
	MATRIX * i3Matrix::Inverse( MATRIX * pOut, REAL32 * pDeterminant, const MATRIX * pM)
	{
	#if 0
		__asm__ (
			".set			push\n"					// save assember option
			".set			noreorder\n"			// suppress reordering
			"lv.q			c100,  0 + %1\n"		// c100 = pm1->x
			"lv.q			c110, 16 + %1\n"		// c110 = pm1->y
			"lv.q			c120, 32 + %1\n"		// c120 = pm1->z
			"lv.q			c000, 48 + %1\n"		// c000 = pm1->w
			"vzero.t		c130\n"					// c130 = (0, 0, 0)
			"vtfm3.t		c010, e100, c000\n"		// c010 = [c100]tr * c000
			"sv.q			r100,  0 + %0\n"		// pm0->x = r100
			"sv.q			r101, 16 + %0\n"		// pm0->y = r101
			"vneg.t			c000, c010\n"			// c000 = (s010, s011, s012, s003)
			"sv.q			r102, 32 + %0\n"		// pm0->z = r102
			"sv.q			c000, 48 + %0\n"		// pm0->w = c000
			".set			pop\n"					// restore assember option
			: "=m"(*pOut)
			: "m"(*pM)
		);

	#else
		REAL32 tmp[12]; /* temp array for pairs */
		REAL32 src[16]; /* array of transpose source matrix */
		INT32 i, j;
		REAL32 det;

		/* transpose matrix */
		for ( i = 0; i < 4; i++) 
		{
			src[i]		= pM->m[i][0];
			src[i + 4]	= pM->m[i][1];
			src[i + 8]	= pM->m[i][2];
			src[i + 12] = pM->m[i][3];
		}

		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];

		/* calculate first 8 elements (cofactors) */
		pOut->m00 = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
		pOut->m00 -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
		pOut->m01 = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
		pOut->m01 -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
		pOut->m02 = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
		pOut->m02 -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
		pOut->m03 = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
		pOut->m03 -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
		pOut->m10 = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
		pOut->m10 -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
		pOut->m11 = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
		pOut->m11 -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
		pOut->m12 = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
		pOut->m12 -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
		pOut->m13 = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
		pOut->m13 -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2]*src[7];
		tmp[1] = src[3]*src[6];
		tmp[2] = src[1]*src[7];
		tmp[3] = src[3]*src[5];
		tmp[4] = src[1]*src[6];
		tmp[5] = src[2]*src[5];
		tmp[6] = src[0]*src[7];
		tmp[7] = src[3]*src[4];
		tmp[8] = src[0]*src[6];
		tmp[9] = src[2]*src[4];
		tmp[10] = src[0]*src[5];
		tmp[11] = src[1]*src[4];

		/* calculate second 8 elements (cofactors) */
		pOut->m20 = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
		pOut->m20 -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
		pOut->m21 = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
		pOut->m21 -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
		pOut->m22 = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
		pOut->m22-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
		pOut->m23 = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
		pOut->m23-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
		pOut->m30 = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
		pOut->m30-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
		pOut->m31 = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
		pOut->m31-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
		pOut->m32 = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
		pOut->m32-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
		pOut->m33 = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
		pOut->m33-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

		/* calculate determinant */
		det = (src[0] * pOut->m00) + (src[1] * pOut->m01) + (src[2] * pOut->m02) + (src[3] * pOut->m03);

		if( pDeterminant != nullptr)
			*pDeterminant = det;

		if( det == 0.0f)
		{
			return nullptr;
		}

		/* calculate matrix inverse */
		det = 1.0f / det;

		for ( j = 0; j < 4; j++)
		{
			for( i = 0; i < 4; i++)
			{
				pOut->m[j][i] *= det;
			}
		}
	#endif

		return pOut;
	}

	MATRIX * i3Matrix_SetRotateAxis( MATRIX * pOut, const VEC3D * pV, REAL32 Angle)
	{
	#if 0
		REAL32 c, s, _c, x, y, z;
		REAL32 _yc, _xc, zs, xs, ys;	// ,_zc;
		REAL32 _xyc, _xzc, _yzc;

		c = i3Math::cos( Angle);
		s = i3Math::sin( Angle);
		_c = 1.0f - c;
		x = pV->vec[0];
		y = pV->vec[1];
		z = pV->vec[2];

		//_zc = z * _c;
		_yc = y * _c;
		_xc = x * _c;
		zs = z * s;
		ys = y * s;
		xs = x * s;

		_xyc = y * _xc;
		_xzc = z * _xc;
		_yzc = z * _yc;

		pOut->m00 = c + (x * _xc);
		pOut->m10 = _xyc - zs;
		pOut->m20 = _xzc + ys;
		pOut->m30 = 0.0f;

		pOut->m01 = _xyc + zs;
		pOut->m11 = c + (y * _yc);
		pOut->m21 = _yzc - xs;
		pOut->m31 = 0.0f;

		pOut->m02 = _xzc - ys;
		pOut->m12 = -(_yzc - xs);
		pOut->m22 = c + (z * z * _c);
		pOut->m32 = 0.0f;

		pOut->m03 = 0.0f;
		pOut->m13 = 0.0f;
		pOut->m23 = 0.0f;
		pOut->m33 = 1.0f;

	#else
		__asm__ (
			".set		push\n"
			".set		noreorder\n"

			// s000 : sin
			// s001 : cos
			// s002 : _c = 1.0 - cos
			// c010 : pV
			// s020 : _xc
			// s021 : _yc
			// s030 : xs
			// s031 : ys
			// s032 : zs
			// s100 : _xyc
			// s101 : _xzc
			// s102 : _yzc

			"mfc1			$8,  %2\n"				// t0   = s
			"mtv			$8,  s000\n"			// s000 = t0
			"lv.q			c010,	%1\n"			// pV
			"vcst.s			s001, 0x05\n"			// s001 = 2/PI
			"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
			"vsin.s			s000, s000\n"			// s000 = sin(s000)
			"vcos.s			s001, s000\n"			// s000 = cos(s000)

			"vocp.s			s002, s001\n"			// 	_c = 1.0f - c;

			"vmul.s			s020, s010, s002\n"		// _xc = x * _c;
			"vmul.s			s021, s011, s002\n"		// _yc = y * _c;
			
			"vmul.s			s030, s010, s000\n"		// xs = x * s;
			"vmul.s			s031, s011, s000\n"		// ys = y * s;
			"vmul.s			s032, s012, s000\n"		// zs = z * s;

			"vmul.s			s100, s011,	s020\n"		// _xyc = y * _xc;
			"vmul.s			s101, s012,	s020\n"		// _xzc = z * _xc;
			"vmul.s			s102, s012, s021\n"		// _yzc = z * _yc;

			"vmul.s			s200, s010, s020\n"		// (continue)
			"vsub.s			s210, s100, s032\n"		// pOut->m10 = _xyc - zs;
			"vadd.s			s220, s101, s031\n"		// pOut->m20 = _xzc + ys;
			"vadd.s			s200, s200, s001\n"		// pOut->m00 = c + (x * _xc);

			"vadd.s			s201, s100, s032\n"		// pOut->m01 = _xyc + zs;
			"vmul.s			s211, s011, s021\n"		// (continue)
			"vadd.s			s211, s211, s001\n"		// pOut->m11 = c + (y * _yc);
			"vsub.s			s221, s102, s030\n"		// pOut->m21 = _yzc - xs;

			"vsub.s			s202, s101, s031\n"		// pOut->m02 = _xzc - ys;
			"vsub.s			s212, s102, s030\n"		// (continue)
			"vneg.s			s212, s212\n"			// pOut->m12 = -(_yzc - xs);
			"vmul.s			s222, s012, s012\n"		// z * z (continue)
			"vmul.s			s222, s222, s002\n"		// s222 * _c (continue)
			"vadd.s			s222, s222, s001\n"		// pOut->m22 = c + (z * z * _c);
			
			"vzero.t		r203\n"					// pOut->m30 = 0.0f; 	pOut->m31 = 0.0f; 	pOut->m32 = 0.0f;
			"vidt.q			c230\n"					// pOut->m03 = 0.0f; 	pOut->m13 = 0.0f; 	pOut->m23 = 0.0f; 	pOut->m33 = 1.0f;

			"sv.q			c200,	0 + %0\n"
			"sv.q			c210,	16 + %0\n"
			"sv.q			c220,	32 + %0\n"
			"sv.q			c230,	48 + %0\n"
			
			".set		pop\n"
			: "=m"( *pOut)
			: "m"( *pV), "f"(Angle)
			);
	#endif

		return pOut;
	}

//	05.06.15 by KOMET
#else
MATRIX * i3Matrix::Inverse( MATRIX * pOut, REAL32 * pDeterminant, const MATRIX * pM)
{
	REAL32 tmp[12]; /* temp array for pairs */
	REAL32 src[16]; /* array of transpose source matrix */
	INT32 i, j;
	REAL32 det;

	/* transpose matrix */
	for ( i = 0; i < 4; i++) 
	{
		src[i]		= pM->m[i][0];
		src[i + 4]	= pM->m[i][1];
		src[i + 8]	= pM->m[i][2];
		src[i + 12] = pM->m[i][3];
	}

	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	/* calculate first 8 elements (cofactors) */
	pOut->m00 = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	pOut->m00 -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	pOut->m01 = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	pOut->m01 -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	pOut->m02 = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	pOut->m02 -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	pOut->m03 = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	pOut->m03 -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	pOut->m10 = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	pOut->m10 -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	pOut->m11 = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	pOut->m11 -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	pOut->m12 = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	pOut->m12 -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	pOut->m13 = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	pOut->m13 -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];

	/* calculate second 8 elements (cofactors) */
	pOut->m20 = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	pOut->m20 -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	pOut->m21 = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	pOut->m21 -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	pOut->m22 = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	pOut->m22-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	pOut->m23 = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	pOut->m23-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	pOut->m30 = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	pOut->m30-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	pOut->m31 = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	pOut->m31-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	pOut->m32 = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	pOut->m32-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	pOut->m33 = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	pOut->m33-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

	/* calculate determinant */
	det = (src[0] * pOut->m00) + (src[1] * pOut->m01) + (src[2] * pOut->m02) + (src[3] * pOut->m03);

	if( pDeterminant != nullptr)
		*pDeterminant = det;

	if( det == 0.0f)
	{
		return nullptr;
	}

	/* calculate matrix inverse */
	det = 1.0f / det;

	for ( j = 0; j < 4; j++)
	{
		for( i = 0; i < 4; i++)
		{
			pOut->m[j][i] *= det;
		}
	}

	return pOut;
}

void     i3Matrix::DecomposeEulerAngleXYZ( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m20 < 1.0f)
	{
		if( pMat->m20 > -1.0f)
		{
			angles[1] = asinf( pMat->m20);
			angles[0] = atan2f( -pMat->m21, pMat->m22);
			angles[2] = atan2f( -pMat->m10, pMat->m00);
		}
		else // r02 = -1
		{
			// Not a unique solution: thetaZ - thetaX = atan2f(r10,r11)
			angles[1] = -I3_PI * 0.5f;
			angles[0] = -atan2f( pMat->m01, pMat->m11);
			angles[2] = 0.0f;
		}
	}
	else // r02 = +1
	{
		// Not a unique solution: thetaZ + thetaX = atan2f(r10,r11)
		angles[1] = I3_PI * 0.5f;
		angles[0] = atan2f( pMat->m01, pMat->m11);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleXZY( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m10 < 1.0f)
	{
		if( pMat->m10 > -1.0f)
		{
			angles[1] = asinf( -pMat->m10);                              // Z
			angles[0] = atan2f( pMat->m12, pMat->m11);              // X
			angles[2] = atan2f( pMat->m20, pMat->m00);              // Y
		}
		else // r01 = -1
		{
			// Not a unique solution: thetaY - thetaX = atan2f(-r20,r22)
			angles[1] = I3_PI * 0.5f;
			angles[0] = atan2f( -pMat->m02, pMat->m22);
			angles[2] = 0.0f;
		}
	}
	else // r01 = +1
	{
		// Not a unique solution: thetaY + thetaX = atan2f(-r20,r22)
		angles[1] = -I3_PI * 0.5f;
		angles[0] = atan2f( -pMat->m02, pMat->m22);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleYXZ( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m21 < +1)
	{
		if ( pMat->m21 > -1)
		{
			angles[1] = asinf( -pMat->m21);                     // X
			angles[0] = atan2f( pMat->m20, pMat->m22);     // Y
			angles[2] = atan2f( pMat->m01, pMat->m11);     // Z
		}
		else // r12 = -1
		{
			// Not a unique solution: thetaZ - thetaY = atan2f(-r01,r00)
			angles[1] = +I3_PI * 0.5f;
			angles[0] = -atan2f( -pMat->m10, pMat->m00);
			angles[2] = 0.0f;
		}
	}
	else // r12 = +1
	{
		// Not a unique solution: thetaZ + thetaY = atan2f(-r01,r00)
		angles[1] = -I3_PI * 0.5f;
		angles[0] = atan2f( -pMat->m10, pMat->m00);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleYZX( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m01 < 1.0f)
	{
		if( pMat->m01 > -1.0f)
		{
			angles[1] = asinf( pMat->m01);                               // Z
			angles[0] = atan2f( -pMat->m02, pMat->m00);    // Y
			angles[2] = atan2f( -pMat->m21, pMat->m11);    // X
		}
		else // r10 = -1
		{
			// Not a unique solution: thetaX - thetaY = atan2f(r21,r22)
			angles[1] = -I3_PI * 0.5f;
			angles[0] = -atan2f( pMat->m12, pMat->m22);
			angles[2] = 0.0f;
		}
	}
	else
	{
		// Not a unique solution: thetaX + thetaY = atan2f(r21,r22)
		angles[1] = +I3_PI * 0.5f;
		angles[0] = atan2f( pMat->m12, pMat->m22);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleZXY( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m12 < 1.0f)
	{
		if( pMat->m12 > -1.0f)
		{
			angles[1] = asinf( pMat->m12);                               // X
			angles[0] = atan2f( -pMat->m10, pMat->m11);    // Z
			angles[2] = atan2f( -pMat->m02, pMat->m22);    // Y
		}
		else // r21 = -1
		{
			// Not a unique solution: thetaY - thetaZ = atan2f(r02,r00)
			angles[1] = -I3_PI * 0.5f;
			angles[0] = -atan2f( pMat->m20, pMat->m00);
			angles[2] = 0.0f;
		}
	}
	else // r21 = +1
	{
		// Not a unique solution: thetaY + thetaZ = atan2f(r02,r00)
		angles[1] = +I3_PI * 0.5f;
		angles[0] = atan2f( pMat->m20, pMat->m00);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleZYX( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m02 < 1.0f)
	{
		if( pMat->m02 > -1.0f)
		{
			angles[1] = asinf( -pMat->m02);                     // Y
			angles[0] = atan2f( pMat->m01, pMat->m00);     // Z
			angles[2] = atan2f( pMat->m12, pMat->m22);     // X
		}
		else // r20 = -1
		{
			// Not a unique solution: thetaX - thetaZ = atan2f(-r12,r11)
			angles[1] = +I3_PI * 0.5f;
			angles[0] = atan2f( -pMat->m21, pMat->m11);
			angles[2] = 0.0f;
		}
	}
	else // r20 = +1
	{
		// Not a unique solution: thetaX + thetaZ = atan2f(-r12,r11)
		angles[1] = -I3_PI * 0.5f;
		angles[0] = atan2f( -pMat->m21, pMat->m11);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleXYX( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m00 < 1.0f)
	{
		if( pMat->m00 > -1.0f)
		{
			angles[1] = acos( pMat->m00);                              // Y
			angles[0] = atan2f( pMat->m01, -pMat->m02);    // X0
			angles[2] = atan2f( pMat->m10, pMat->m20);              // X1
		}
		else // r00 = -1
		{
			// Not a unique solution: thetaX1 - thetaX0 = atan2f(-r12,r11)
			angles[1] = I3_PI;
			angles[0] = -atan2f( -pMat->m21, pMat->m11);
			angles[2] = 0;
		}
	}
	else // r00 = +1
	{
		// Not a unique solution: thetaX1 + thetaX0 = atan2f(-r12,r11)
		angles[1] = 0.0f;
		angles[0] = atan2f( -pMat->m21, pMat->m11);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleXZX( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m00 < +1.0f)
	{
		if( pMat->m00 > -1.0f)
		{
			angles[1] = acos( pMat->m00);                              // Z
			angles[0] = atan2f( pMat->m02, pMat->m01);              // X0
			angles[2] = atan2f( pMat->m20, -pMat->m10);    // X1
		}
		else // r00 = -1
		{
			// Not a unique solution: thetaX1 - thetaX0 = atan2f(r21,r22)
			angles[1] = I3_PI;
			angles[0] = -atan2f( pMat->m12, pMat->m22);
			angles[2] = 0.0f;
		}
	}
	else // r00 = +1
	{
		// Not a unique solution: thetaX1 + thetaX0 = atan2f(r21,r22)
		angles[1] = 0.0f;
		angles[0] = atan2f( pMat->m12, pMat->m22);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleYXY( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m11 < +1.0f)
	{
		if( pMat->m11 > -1.0f)
		{
			angles[1] = acos( pMat->m11);                              // X
			angles[0] = atan2f( pMat->m10, pMat->m12);              // Y0
			angles[2] = atan2f( pMat->m01, -pMat->m21);    // Y1
		}
		else // r11 = -1
		{
			// Not a unique solution: thetaY1 - thetaY0 = atan2f(r02,r00)
			angles[1] = I3_PI;
			angles[0] = -atan2f( pMat->m20, pMat->m00);
			angles[2] = 0.0f;
		}
	}
	else // r11 = +1
	{
		// Not a unique solution: thetaY1 + thetaY0 = atan2f(r02,r00)
		angles[1] = 0.0f;
		angles[0] = atan2f( pMat->m20, pMat->m00);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleYZY( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m11 < +1.0f)
	{
		if( pMat->m11 > -1.0f)
		{
			angles[1] = acos( pMat->m11);                              // Z
			angles[0] = atan2f( pMat->m12, -pMat->m10);    // Y0
			angles[2] = atan2f( pMat->m21, pMat->m01);              // Y1
		}
		else // r11 = -1
		{
			// Not a unique solution: thetaY1 - thetaY0 = atan2f(-r20,r22)
			angles[1] = I3_PI;
			angles[0] = -atan2f( -pMat->m02, pMat->m22);
			angles[2] = 0.0f;
		}
	}
	else // r11 = +1
	{
		// Not a unique solution: thetaY1 + thetaY0 = atan2f(-r20,r22)
		angles[1] = 0.0f;
		angles[0] = atan2f( -pMat->m02, pMat->m22);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleZXZ( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m22 < +1.0f)
	{
		if( pMat->m22 > -1.0f)
		{
			angles[1] = acos( pMat->m22);                              // X
			angles[0] = atan2f( pMat->m20, -pMat->m21);    // Z0
			angles[2] = atan2f( pMat->m02, pMat->m12);              // Z1
		}
		else // r22 = -1
		{
			// Not a unique solution: thetaZ1 - thetaZ0 = atan2f(-r01,r00)
			angles[1] = I3_PI;
			angles[0] = -atan2f( -pMat->m10, pMat->m00);
			angles[2] = 0.0f;
		}
	}
	else // r22 = +1
	{
		// Not a unique solution: thetaZ1 + thetaZ0 = atan2f(-r01,r00)
		angles[1] = 0.0f;
		angles[0] = atan2f( -pMat->m10, pMat->m00);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::DecomposeEulerAngleZYZ( MATRIX * pMat, REAL32 angles[3])
{
	if( pMat->m22 < +1.0f)
	{
		if( pMat->m22 > -1.0f)
		{
			angles[1] = acos( pMat->m22);                              // Y
			angles[0] = atan2f( pMat->m21, pMat->m20);              // Z0
			angles[2] = atan2f( pMat->m12, -pMat->m02);    // Z1
		}
		else // r22 = -1
		{
			// Not a unique solution: thetaZ1 - thetaZ0 = atan2f(r10,r11)
			angles[1] = I3_PI;
			angles[0] = -atan2f( pMat->m01, pMat->m11);
			angles[2] = 0.0f;
		}
	}
	else // r22 = +1
	{
		// Not a unique solution: thetaZ1 + thetaZ0 = atan2f(r10,r11)
		angles[1] = 0.0f;
		angles[0] = atan2f( pMat->m01, pMat->m11);
		angles[2] = 0.0f;
	}
}

void     i3Matrix::ComposeEulerAngleXYZ( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateX( pMat, angles[0]);
	PostRotateAxis( pMat,i3Matrix::GetUp(pMat), angles[1]);
	PostRotateAxis( pMat,i3Matrix::GetAt(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleXZY( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateX( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleYXZ( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateY( pMat,angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat),  angles[2]);
}

void     i3Matrix::ComposeEulerAngleYZX( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateY( pMat,angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleZXY( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateZ( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleZYX( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateZ( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleXYX( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateX( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleXZX( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateX( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleYXY( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateY( pMat,angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat),  angles[2]);
}

void     i3Matrix::ComposeEulerAngleYZY( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateY( pMat,angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleZXZ( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateZ( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetRight(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[2]);
}

void     i3Matrix::ComposeEulerAngleZYZ( MATRIX * pMat, REAL32 angles[3])
{
	SetRotateZ( pMat, angles[0]);
	PostRotateAxis( pMat, i3Matrix::GetUp(pMat), angles[1]);
	PostRotateAxis( pMat, i3Matrix::GetAt(pMat), angles[2]);
}


MATRIX * i3Matrix_SetRotateAxis( MATRIX * pOut, const VEC3D * pV, REAL32 Angle)
{	
#ifdef I3_WINDOWS
	D3DXMatrixRotationAxis( (D3DXMATRIX *) pOut, (D3DXVECTOR3 *) pV, Angle);
#else 
	//이 소스는 특수한 상태에서 버그가 있습니다. 차후 수정은 진균씨가 해줄겁니다..^^;;;;성백
	REAL32 c, s, _c, x, y, z;
	REAL32 _yc, _xc, zs, xs, ys;	// ,_zc;
	REAL32 _xyc, _xzc, _yzc;

	c = cosf( Angle);
	s = sinf( Angle);
	_c = 1.0f - c;
	x = pV->vec[0];
	y = pV->vec[1];
	z = pV->vec[2];

	//_zc = z * _c;
	_yc = y * _c;
	_xc = x * _c;
	zs = z * s;
	ys = y * s;
	xs = x * s;

	_xyc = y * _xc;
	_xzc = z * _xc;
	_yzc = z * _yc;

	pOut->m00 = c + (x * _xc);
	pOut->m10 = _xyc - zs;
	pOut->m20 = _xzc + ys;
	pOut->m30 = 0.0f;

	pOut->m01 = _xyc + zs;
	pOut->m11 = c + (y * _yc);
	pOut->m21 = _yzc - xs;
	pOut->m31 = 0.0f;

	pOut->m02 = _xzc - ys;
	pOut->m12 = -(_yzc - xs);
	pOut->m22 = c + (z * z * _c);
	pOut->m32 = 0.0f;

	pOut->m03 = 0.0f;
	pOut->m13 = 0.0f;
	pOut->m23 = 0.0f;
	pOut->m33 = 1.0f;
#endif
	return pOut;
}
#endif
