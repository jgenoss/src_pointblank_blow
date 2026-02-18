#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Int32.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Int32, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Int32::i3TDKPropertyGridProperty_Int32( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(long int)(*(INT32*)(pInfo->m_pData)), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0)
{
	m_pData = (INT32*)(pInfo->m_pData);
	m_bHasRange = ((pInfo->m_nFlag & PROPERTYFLAG_HASRANGE) != 0) ? true : false;
	m_bNoEdit = ((pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0) ? true : false;
	
	if( m_bHasRange)	// 최대값, 최소값 설정 가능( Real, Vector등도 마찬가지)
	{
		INT32* pRange = (INT32*)(pInfo->m_pUserData);
		m_nMin = pRange[0];
		m_nMax = pRange[1];

		I3MEM_SAFE_FREE( pRange);
	}
			
	if( m_bNoEdit)		// NoEdit가 설정되면 값을 변경하는 것은 불가능
	{
		Enable( FALSE);
	}

	// m_nMin >= m_nMax 이면 Spin Control이 반대로 동작한다.
	// 따라서 가급적이면 Min Max를 설정해주는 것이 좋을 듯.
	I3ASSERT( m_nMin < m_nMax);

	EnableSpinControl( TRUE, m_nMin, m_nMax);		// Spin Control을 옆에 붙일 수 있음.

	UpdateFromProperty();	// 초기값이 최대값,최소값의 범위에 맞는지 체크하기 위해 호출
}

i3TDKPropertyGridProperty_Int32::~i3TDKPropertyGridProperty_Int32()
{
	// 디버그용
	// I3TRACE("Destructor of i3TDKPropertyGridProperty_Int32\n");
}

INT32 i3TDKPropertyGridProperty_Int32::_GetMinMaxData( INT32 nData)
{
	if( m_bHasRange)
	{
		if( nData > m_nMax)		return m_nMax;
		if( nData < m_nMin)		return m_nMin;
	}

	return nData;
}

void i3TDKPropertyGridProperty_Int32::UpdateFromProperty( void* pData)
{
	INT32 val = _GetMinMaxData( (INT32)(_variant_t)m_varValue);
	
	if( pData == nullptr)
		*m_pData = val;									// 실제 변수 업데이트
	else
		*((INT32*)pData) = val;
	
	SetValue( (_variant_t)(long int)val);	// 화면 업데이트 (VT_I4와 VT_INT가 있으므로 long int(VT_I4)로 해주어야 함)
}

void i3TDKPropertyGridProperty_Int32::UpdateToProperty( void)
{
	INT32 val = _GetMinMaxData( *m_pData);

	*m_pData = val;

	SetValue( (_variant_t)(long int)val);
}

#endif