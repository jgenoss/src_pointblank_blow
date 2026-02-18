#if !defined( __RANDTABLE_H )
#define __RANDTABLE_H


/* 주기매개변수들 */
#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df
#define CMATH_UPPER_MASK 0x80000000
#define CMATH_LOWER_MASK 0x7fffffff

/* 조절용 매개변수들 */
#define CMATH_TEMPERING_MASK_B			0x9d2c5680
#define CMATH_TEMPERING_MASK_C			0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)		(y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)		(y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)		(y << 15) 
#define CMATH_TEMPERING_SHIFT_L(y)		(y >> 1)


class CRandTable : public i3ElementBase
{
	I3_CLASS_DEFINE( CRandTable, i3ElementBase );
protected:
	bool	m_bCreate;
	INT32	m_nCode;

	INT32	* m_pRandTable;

	INT32	m_nTableCount;
	INT32	m_Seed, m_Max;
	
	UINT32	m_t[ CMATH_N ];
	INT32	m_Ti;
	UINT32	m_mag01[ 2 ];

	INT32	random( INT32 min, INT32 max );
	void	setrandomseed( INT32 seed );
	void	swap( INT32 * pS, INT32 * pD );

public:
	CRandTable();
	virtual ~CRandTable();

	void	Create( INT32 nSeed, INT32 nMax, INT32 nTableCount = 1024 );		// size 1024 * 4bytes
	void	Destroy( void );
	
	void	CreateRandTable( void );
	void	SuppllingTable( void );
	INT32	GetRand( INT32 nCode );
	INT32	GetRand( void );

	INT32	GetCode( void )					{ return m_nCode; }
	void	SetCode( INT32 nCode )			{ m_nCode = nCode; }

	INT32	GetPreCode( void );
	

	INT32	GetSeed( void )					{ return m_Seed; }
	INT32	GetMax( void )					{ return m_Max; }

	bool	CheckCreation( void )			{ return m_bCreate; }
};

#endif	// __RANDTABLE_H
