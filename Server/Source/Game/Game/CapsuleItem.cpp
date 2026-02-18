#include "pch.h"
#include "CapsuleItem.h"

I3_CLASS_INSTANCE(CCapsuleItem, CCapsuleRand);

CCapsuleItem::CCapsuleItem()
{
}

CCapsuleItem::~CCapsuleItem()
{
	for( INT32 i = 0 ; i < m_i32ItemCount ; i++ )
	{
		I3MEM_SAFE_FREE( m_ppui32RewardItemID[ i ] );
		I3MEM_SAFE_FREE( m_ppui32RewardItemArgCount[ i ] );		// 2.0
		I3MEM_SAFE_FREE( m_ppui32RewardItemArgPeriod[ i ] );	// 2.0
		I3MEM_SAFE_FREE( m_ppui32RewardItemArg[ i ] );			// 1.0
	}
	I3MEM_SAFE_FREE( m_pi32RewardCount );
	I3MEM_SAFE_FREE( m_pi32RewardJackpot );
	I3MEM_SAFE_FREE( m_ppui32RewardItemID );
	I3MEM_SAFE_FREE( m_ppui32RewardItemArgCount );		// 2.0
	I3MEM_SAFE_FREE( m_ppui32RewardItemArgPeriod );		// 2.0
	I3MEM_SAFE_FREE( m_ppui32RewardItemArg );			// 1.0
}

BOOL	CCapsuleItem::Create( UINT32 ui32ItemID, INT32 i32ProbCount, INT32 i32ItemCount )
{
	m_ui32TItemID				= ui32ItemID;

	m_i32ItemCount				= i32ItemCount;

	m_pi32RewardCount			= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ItemCount ); 
	m_pi32RewardJackpot			= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ItemCount ); 
	m_ppui32RewardItemID		= (UINT32**)i3MemAlloc( sizeof(UINT32*) * m_i32ItemCount ); 
	m_ppui32RewardItemArgCount	= (UINT32**)i3MemAlloc( sizeof(UINT32*) * m_i32ItemCount );		// 2.0
	m_ppui32RewardItemArgPeriod	= (UINT32**)i3MemAlloc( sizeof(UINT32*) * m_i32ItemCount );		// 2.0
	m_ppui32RewardItemArg		= (UINT32**)i3MemAlloc( sizeof(UINT32*) * m_i32ItemCount );		// 1.0

	return CCapsuleRand::Create( i32ProbCount );
}

void	CCapsuleItem::SetReward( INT32 i32Idx, INT32 i32Count, INT32 i32Jackpot )
{
	m_pi32RewardCount[i32Idx]			= i32Count;
	m_pi32RewardJackpot[i32Idx]			= i32Jackpot;
	m_ppui32RewardItemID[i32Idx]		= (UINT32*)i3MemAlloc( sizeof(UINT32) * m_pi32RewardCount[i32Idx] ); 
	m_ppui32RewardItemArgCount[i32Idx]	= (UINT32*)i3MemAlloc( sizeof(UINT32) * m_pi32RewardCount[i32Idx] );	// 2.0
	m_ppui32RewardItemArgPeriod[i32Idx]	= (UINT32*)i3MemAlloc( sizeof(UINT32) * m_pi32RewardCount[i32Idx] );	// 2.0
	m_ppui32RewardItemArg[i32Idx]		= (UINT32*)i3MemAlloc( sizeof(UINT32) * m_pi32RewardCount[i32Idx] );	// 1.0
}

// 2.0용 메서드
void	CCapsuleItem::InsertReward( INT32 i32Idx, INT32 i32SIdx, UINT32 ui32ItemID, UINT32 ui32ItemArgCount, UINT32 ui32ItemArgPeriod )
{
	m_ppui32RewardItemID[i32Idx][i32SIdx]			= ui32ItemID;
	m_ppui32RewardItemArgCount[i32Idx][i32SIdx]		= ui32ItemArgCount;
	m_ppui32RewardItemArgPeriod[i32Idx][i32SIdx]	= ui32ItemArgPeriod;
}

// 1.0용 메서드
void	CCapsuleItem::InsertReward10( INT32 i32Idx, INT32 i32SIdx, UINT32 ui32ItemID, UINT32 ui32ItemArg )
{
	m_ppui32RewardItemID[i32Idx][i32SIdx]			= ui32ItemID;
	m_ppui32RewardItemArg[i32Idx][i32SIdx]			= ui32ItemArg;
}

INT32	CCapsuleItem::GetRewardCount( UINT8 ui8Idx )
{
	if( ui8Idx >= (UINT8)m_i32ItemCount )	return 0;

	return m_pi32RewardCount[ui8Idx];
}

INT32	CCapsuleItem::GetRewardJackPot( UINT8 ui8Idx )
{
	if( ui8Idx >= (UINT8)m_i32ItemCount )	return 0;

	return m_pi32RewardJackpot[ui8Idx];
}

UINT32	CCapsuleItem::GetRewardItemID( UINT8 ui8Idx, INT32 i32SIdx )
{
	return m_ppui32RewardItemID[ui8Idx][i32SIdx];
}

// 2.0용 메서드
UINT32	CCapsuleItem::GetRewardItemArgCount( UINT8 ui8Idx, INT32 i32SIdx )
{
	return m_ppui32RewardItemArgCount[ui8Idx][i32SIdx];
}

// 2.0용 메서드
UINT32	CCapsuleItem::GetRewardItemArgPeriod( UINT8 ui8Idx, INT32 i32SIdx )
{
	return m_ppui32RewardItemArgPeriod[ui8Idx][i32SIdx];
}

// 1.0용 메서드
UINT32	CCapsuleItem::GetRewardItemArg( UINT8 ui8Idx, INT32 i32SIdx )
{
	return m_ppui32RewardItemArg[ui8Idx][i32SIdx];
}