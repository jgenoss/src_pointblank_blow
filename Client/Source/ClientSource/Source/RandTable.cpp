#include "pch.h"
#include "RandTable.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CRandTable);//, i3ElementBase );

CRandTable::CRandTable()
{
	m_pRandTable		= nullptr;
	m_nTableCount		= 0;
	m_bCreate			= false;

	m_nCode				= 0;
	m_Max = 0;
	m_Seed = 0;
	m_Ti = 0;
	i3mem::FillZero( m_mag01, sizeof( m_mag01));
	i3mem::FillZero( m_t, sizeof( m_t));
}

CRandTable::~CRandTable()
{
	Destroy();
}

void CRandTable::Create( INT32 nSeed, INT32 nMax, INT32 nTableCount )
{
	m_nTableCount		= nTableCount;
	m_Max				= nMax;
	m_Seed				= nSeed + 1;

	CreateRandTable();

	m_bCreate	= true;
}

void CRandTable::Destroy( void )
{
	I3MEM_SAFE_FREE_POINTER( m_pRandTable );
	m_nTableCount	= 0;
	m_nCode			= 0;
	m_bCreate		= false;
}

void CRandTable::CreateRandTable( void )
{
	INT32	i;

	I3MEM_SAFE_FREE_POINTER( m_pRandTable );
	I3ASSERT( m_nTableCount > 0 );
	m_pRandTable	= (INT32*) i3MemAlloc( sizeof( INT32 ) * m_nTableCount );

	setrandomseed( m_Seed );

	for( i = 0; i < m_nTableCount; i++ )
	{
		m_pRandTable[ i ] = random( 0, m_Max - 1 );
	}

	// 테이블을 한번더 썩어줍니다.
	SuppllingTable();
}

void CRandTable::setrandomseed( INT32 seed )
{
	m_t[ 0 ]= seed & 0xffffffff;

	for( m_Ti = 1; m_Ti < CMATH_N; m_Ti++ )
		m_t[ m_Ti ] = (69069 * m_t[ m_Ti -1 ] ) & 0xffffffff;
}

INT32 CRandTable::random( INT32 min, INT32 max )
{
	INT32 i;
	INT32	nRand = 0;
	
	m_mag01[ 0 ] = 0;
	m_mag01[ 1 ] = CMATH_MATRIX_A;

	if( max < min ) return 0;
	/* x가 0이나 1이면 mag01[x] = x * MATRIX_A */
	   
	if( m_Ti >= CMATH_N )
	{ /* 한번에 N개의 워드를 생성 */
		if( m_Ti == CMATH_N + 1 ) /* sgenrand()가 호출된적이없다면 */
	   		setrandomseed(4357); // 기본초기 종자값을 사용
	   	   
		for( i = 0; i < CMATH_N - CMATH_M; i++)
		{
	   		nRand = (m_t[ i ] & CMATH_UPPER_MASK) | (m_t[ i+1 ] & CMATH_LOWER_MASK);
	   		m_t[ i ] = m_t[ i + CMATH_M ] ^ (nRand>>1) ^ m_mag01[ nRand & 0x1 ];
		}

		for( ; i < CMATH_N-1; i++)
		{
	   		nRand = (m_t[ i ] & CMATH_UPPER_MASK) | (m_t[ i+1 ] & CMATH_LOWER_MASK);
	   		m_t[ i ] = m_t[ i + (CMATH_M - CMATH_N) ] ^ (nRand>>1) ^ m_mag01[nRand & 0x1];
		}

		nRand = (m_t[ CMATH_N-1 ] & CMATH_UPPER_MASK) | (m_t[ 0 ] & CMATH_LOWER_MASK);
		m_t[ CMATH_N - 1 ] = m_t[ CMATH_M - 1 ] ^ (nRand>>1) ^ m_mag01[nRand & 0x1];

		m_Ti=0;
	}

	nRand = m_t[ m_Ti++ ];
	nRand ^= CMATH_TEMPERING_SHIFT_U(nRand);
	nRand ^= CMATH_TEMPERING_SHIFT_S(nRand) & CMATH_TEMPERING_MASK_B;
	nRand ^= CMATH_TEMPERING_SHIFT_T(nRand) & CMATH_TEMPERING_MASK_C;
	nRand ^= CMATH_TEMPERING_SHIFT_L(nRand);

	return (nRand + min ) % max;
}

void CRandTable::swap( INT32 * pS, INT32 * pD )
{
	INT32	nTemp;

	nTemp = *pS;
	*pS = *pD;
	*pD = nTemp;
}

void CRandTable::SuppllingTable( void )
{
	INT32	i, j;

	for( i = 0; i < m_nTableCount; i++ )
	{
		j = random( i, m_nTableCount - 1 );

		swap( &m_pRandTable[ i ], &m_pRandTable[ j ] );
	}
}

INT32 CRandTable::GetRand( INT32 nCode )
{
	if( !m_bCreate )		return 0;

	INT32	nRand = 0;
	INT32	code = nCode % m_nTableCount;

	I3ASSERT( m_pRandTable != nullptr );
	//m_nCode	= code;

	nRand = m_pRandTable[ code ];

	return nRand;
}

INT32 CRandTable::GetRand( void )
{
	INT32	nRand = 0;

	if( m_pRandTable == nullptr )
		return 0;

	INT32	code = m_nCode % m_nTableCount;

	nRand = m_pRandTable[ code ];

	return nRand;
}

INT32 CRandTable::GetPreCode()
{	
	INT32 rt = m_nCode;

	m_nCode++;
	if( m_nCode >= m_nTableCount )
		m_nCode = 0;

	return rt;
}