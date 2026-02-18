#ifndef __CAPSULEITEM_H__
#define __CAPSULEITEM_H__

#include "CapsuleRand.h"

class CCapsuleItem : public CCapsuleRand
{
	I3_CLASS_DEFINE(CCapsuleItem);

	UINT32			m_ui32ItemID;

	INT32			m_i32ItemCount;

	INT32*			m_pi32RewardCount;
	INT32*			m_pi32RewardJackpot;
	UINT32**		m_ppui32RewardItemID;

	// 구버전과 신버전 호환을 위해 양쪽 다 삽입합니다.	2011.10.19 안국정
	INT8			m_i8INIVersion;					// 버전값. 1 or 2
	UINT32**		m_ppui32RewardItemArgCount;		// 신버전(2.0)용
	UINT32**		m_ppui32RewardItemArgPeriod;	// 신버전(2.0)용
	UINT32**		m_ppui32RewardItemArg;			// 구버전(1.0)용

	char			m_szName[MAX_PATH];				// 캡슐아이템명

public:
	CCapsuleItem();
	virtual ~CCapsuleItem();

	BOOL			Create( UINT32 ui32ItemID, INT32 i32ProbCount, INT32 i32ItemCount, const char* szName );

	void			SetReward( INT32 i32Idx, INT32 i32Count, INT32 i32Jackpot );
	void			InsertReward( INT32 i32Idx, INT32 i32SIdx, UINT32 ui32ItemID, UINT32 ui32ItemArgCount, UINT32 ui32ItemArgPeriod );	// 신버전(2.0) 용
	void			InsertReward10( INT32 i32Idx, INT32 i32SIdx, UINT32 ui32ItemID, UINT32 ui32ItemArg );								// 구버전(1.0) 용


	INT32			GetRewardCount( UINT8 ui8Idx );
	INT32			GetRewardJackPot( UINT8 ui8Idx );
	UINT32			GetRewardItemID( UINT8 ui8Idx, INT32 i32SIdx );
	UINT32			GetRewardItemArgCount( UINT8 ui8Idx, INT32 i32SIdx );	// 신버전(2.0)용
	UINT32			GetRewardItemArgPeriod( UINT8 ui8Idx, INT32 i32SIdx );	// 신버전(2.0)용
	UINT32			GetRewardItemArg( UINT8 ui8Idx, INT32 i32SIdx );		// 구버전(1.0)용

	UINT32			GetItemID()					{	return m_ui32ItemID;	}
	INT32			GetItemCount()				{	return m_i32ItemCount;	}
	char*			GetName()					{	return m_szName;		}
};

#endif