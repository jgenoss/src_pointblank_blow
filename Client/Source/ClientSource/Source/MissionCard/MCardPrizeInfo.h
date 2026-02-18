#pragma once


struct MCardPrizeInfo_Mqf_ItemInfo
{
	UINT32		m_ui32ItemID = 0;				// 지급할 아이템의 코드
	UINT32		m_ui32Arg = 0;					// 이 아이템을 몇개를 줄것인가
	INT8		m_i8AuthType = 0;				// 지급할 아이템의 지급 타입 ( 개수제 , 기간제 )
};

struct MCardPrizeInfo_Mqf
{
	UINT32	point = 0;
	UINT32  exp = 0;
	UINT32  prize = 0;		
	i3::svector<MCardPrizeInfo_Mqf_ItemInfo>	vecPrizeItem;
};

void InitMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo);
void ResetMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo);
