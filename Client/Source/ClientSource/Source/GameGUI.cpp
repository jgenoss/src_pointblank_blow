#include "pch.h"
#include "GameGUI.h"


static	I3COLOR m_colorTable[GCT_MAX];

void GameGUI::InitColor(void)
{
	i3Color::Set(&m_colorTable[GCT_DEFAULT],      (UINT8) 173, 173, 173, 255);	
	i3Color::Set(&m_colorTable[GCT_FOCUS],        (UINT8) 215, 211, 210, 255);	
	i3Color::Set(&m_colorTable[GCT_STATIC],       (UINT8) 153, 153, 153, 255);	
	i3Color::Set(&m_colorTable[GCT_INVERSE],      (UINT8)   0,   0,   0, 255);
	i3Color::Set(&m_colorTable[GCT_NOTIFY],       (UINT8) 246, 223, 153, 255);
	i3Color::Set(&m_colorTable[GCT_BLUE],         (UINT8) 103, 109, 131, 255);
	i3Color::Set(&m_colorTable[GCT_MY_SLOT],      (UINT8) 253, 167,   0, 255);	
	i3Color::Set(&m_colorTable[GCT_TEAM_CHAT],    (UINT8) 98, 180, 253, 255);	
	i3Color::Set(&m_colorTable[GCT_CLAN_CHAT],    (UINT8) 80, 255,  60, 255);		
	i3Color::Set(&m_colorTable[GCT_WISPER_CHAT],  (UINT8) 237,  86, 250, 255);
	i3Color::Set(&m_colorTable[GCT_MEGAPHONE_CHAT],  (UINT8) 253,  139, 248, 255);
	i3Color::Set(&m_colorTable[GCT_VALUE],		  (UINT8) 255, 255, 255, 255);
	i3Color::Set(&m_colorTable[GCT_RED],          (UINT8) 210,	 0,	  0, 255);
	i3Color::Set(&m_colorTable[GCT_LIGHT_GREEN],  (UINT8) 139, 217,  37, 255);
	i3Color::Set(&m_colorTable[GCT_YELLOW],			(UINT8) 255, 255,  0, 255);
	i3Color::Set(&m_colorTable[GCT_MATCHMAKING],   (UINT8) 43, 236,	223, 255);
	i3Color::Set(&m_colorTable[GCT_INVALID_C],	  (UINT8) 66, 66,	66, 255);	
	i3Color::Set(&m_colorTable[GCT_VALID_C],		  (UINT8) 205, 205,	205, 255);	

	i3Color::Set(&m_colorTable[GCT_BOSS_MODE],    (UINT8) 0, 254, 254, 255);	
	i3Color::Set(&m_colorTable[GCT_SNIPER_MODE],   (UINT8) 220, 20, 60, 255);	
	i3Color::Set(&m_colorTable[GCT_SHOTGUN_MODE],   (UINT8) 50, 180, 140, 255);	
	i3Color::Set(&m_colorTable[GCT_DEFENCE_MODE],	(UINT8) 210, 50, 210, 255);
	i3Color::Set(&m_colorTable[GCT_SUDDEN_DEATH_MODE],   (UINT8)  255, 0, 0, 255);	
	i3Color::Set(&m_colorTable[GCT_HEAD_HUNTER_MODE],   (UINT8)  0x16, 0xf0, 0x99, 255);	
	i3Color::Set(&m_colorTable[GCT_HEAD_KILLER_MODE],   (UINT8)  0xf3, 0x13, 0x94, 255);	
	i3Color::Set(&m_colorTable[GCT_KNUCKLE_MODE],   (UINT8) 150, 80, 140, 255);	
	i3Color::Set(&m_colorTable[GCT_DEATHMATCH_MODE],    (UINT8) 130, 235, 90, 255);	
	i3Color::Set(&m_colorTable[GCT_BOMB_MODE],     (UINT8) 51, 153, 255, 255);													
	i3Color::Set(&m_colorTable[GCT_DESTROY_MODE],    (UINT8) 204, 102, 255, 255);
	i3Color::Set(&m_colorTable[GCT_TRAINING_MODE],	(UINT8) 255, 204, 0, 255);
	i3Color::Set(&m_colorTable[GCT_ANNIHILATION_MODE],   (UINT8)  255, 101, 51, 255);	
	i3Color::Set(&m_colorTable[GCT_SHOOTINGFIELD_MODE],		(UINT8) 230, 135, 50, 255);
}

I3COLOR* GameGUI::GetColor(GUI_COLOR_TYPE colorType)
{
	I3ASSERT(colorType >= 0);
	I3ASSERT(colorType < GCT_MAX);

	return m_colorTable + colorType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace	GameGUI
{

void	SetRankStatic(i3GuiStatic * pStatic)
{
	static SIZE32 Interval	= {RANK_MARK_INTERVAL_SIZE, RANK_MARK_INTERVAL_SIZE };
	static SIZE32 ImgSize	= {RANK_MARK_SIZE, RANK_MARK_SIZE};
	static SIZE32 Size		= {256, 256};

	pStatic->SetAllShapeEnable(false);
	pStatic->SetShapeEnable(0, true);
	pStatic->SetUVPositioner(Interval, Size, &ImgSize);
}

const I3COLOR *		GameModeColor(UINT32 GameMode)
{
	GUI_COLOR_TYPE	ColorType = GCT_DEFAULT;
	
	switch( GameMode)
	{
	case STAGE_MODE_DEATHMATCH		:	ColorType = GCT_DEATHMATCH_MODE;	break;
	case STAGE_MODE_BOMB			:	ColorType = GCT_BOMB_MODE;			break;
	case STAGE_MODE_DESTROY			:	ColorType = GCT_DESTROY_MODE;		break;
	case STAGE_MODE_DEFENCE			:	ColorType = GCT_DEFENCE_MODE;		break;
	case STAGE_MODE_ANNIHILATION	:	ColorType = GCT_ANNIHILATION_MODE;	break;
	case STAGE_MODE_ESCAPE			:	ColorType = GCT_BOSS_MODE;			break;
	default:						ColorType = GCT_DEATHMATCH_MODE;	break;
	}

	return GameGUI::GetColor(ColorType);
}

void	RemoveColorKeyword(const char * pStr, char * OutStr, INT32 OutStrSize)
{
	string	str = pStr;
	RemoveColorKeyword(str);

	i3::safe_string_copy(OutStr, str.c_str(), OutStrSize);
}

void	RemoveColorKeyword(string & str)
{
	enum
	{
		KEYWORD_BEGIN_SIZE = 5,	// "{col:"
		KEYWORD_END_SIZE = 5,	// "{/col"
	};

	string	test_str = str;
	std::transform(test_str.begin(), test_str.end(), test_str.begin(), tolower);

	while (true)
	{
		// 시작 키워드 삭제 (대소문자 구분함)
		// revision 59158 2013_0522 유영정 위의 transform 으로 컬러키워드의 대소문자는 구분하지않음
		size_t sIdx = test_str.find("{col");
		if ( sIdx == string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_BEGIN_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}

	while (true)
	{
		// 종료 키워드 삭제 (대소문자 구분함)
		size_t sIdx = test_str.find("{/col");
		if ( sIdx == string::npos)
			break;

		size_t erase_count = test_str.find('}', sIdx);
		if ( erase_count != string::npos)
			erase_count = erase_count-sIdx+1;
		else
			erase_count = KEYWORD_END_SIZE;

		test_str.erase(sIdx, erase_count);
		str.erase(sIdx, erase_count);
	}
}

ResizingForm::ResizingForm()
{
	Reset();
}

void ResizingForm::Reset()
{
	for (int a = 0 ; a < 3 ; ++a)
		for (int b = 0 ; b < 3 ; ++b)
			idx[a][b] = -1;
}

int ResizingForm::Get( int x, int y ) const
{
	I3_BOUNDCHK(x, 3);
	I3_BOUNDCHK(y, 3);
	return idx[x][y];
}

void ResizingForm::Set( int x, int y, int idx )
{
	I3_BOUNDCHK(x, 3);
	I3_BOUNDCHK(y, 3);
	this->idx[x][y] = idx;
}

bool ResizingControl( i3GuiControl * pControl, const ResizingForm * pForm, REAL32 Width, REAL32 Height )
{
	I3ASSERT(pControl != nullptr);
	I3ASSERT(pForm != nullptr);

	i3GuiControlData * pData = pControl->GetControlData();
	I3ASSERT( pData->m_pShape != nullptr);

	// Control Shape 순서는 아래와 같습니다.
	// StageReady_Root.guiproj에 저장되어 있습니다.
	//	0	1	2
	//	3	8	4
	//	5	6	7

	// 리사이징은 1, 3, 8, 4, 6 Shape만 하고
	// 1, 6은 폭만 3, 4는 높이만 8은 높이, 폭 모두 크기를 재조정합니다.
	// 부분적으로 재조정하는 이유는 전부 크기가 재조정되면
	// 모양이 일그러지기 때문입니다.
	i3GuiImage * pArrImage[3][3] = {};
	for ( int a = 0 ; a < 3 ; ++a )
		for ( int b = 0 ; b < 3 ; ++b )
		{
			int idx = pForm->Get(a,b);
			if ( idx != -1 )
				pArrImage[a][b] = pData->m_pShape->getImage(idx);
		}


	REAL32 CenterHorzSize;		// 중앙열의 폭.
	REAL32 CenterVertSize;		// 중앙행의 높이
	REAL32 RightColumnPosX;		// 오른쪽열의 시작위치
	REAL32 BottomRowPosY;		// 아래쪽행의 시작위치
	REAL32	Width0x0y = (pArrImage[0][0] != nullptr) ? pArrImage[0][0]->getWidth() : 0;
	REAL32	Width0x2y = (pArrImage[0][2] != nullptr) ? pArrImage[0][2]->getWidth() : 0;
	REAL32	Height0x0y = (pArrImage[0][0] != nullptr) ? pArrImage[0][0]->getHeight() : 0;
	REAL32	Height2x0y = (pArrImage[2][0] != nullptr) ? pArrImage[2][0]->getHeight() : 0;

	CenterHorzSize = max(0, Width- (Width0x0y + Width0x2y));
	CenterVertSize = max(0, Height- (Height0x0y + Height2x0y));
	RightColumnPosX = (Width0x0y + CenterHorzSize);
	BottomRowPosY = (Height0x0y + CenterVertSize);

	for ( INT32 i = 0 ; i < 3 ; ++i )
	{
		if ( Width > 0 )
		{
			if (pArrImage[i][1])	pArrImage[i][1]->m_nWidth			= CenterHorzSize;
			if (pArrImage[i][1])	pArrImage[i][1]->m_nOriginalWidth	= CenterHorzSize;
			if (pArrImage[i][2])	pArrImage[i][2]->m_nX				= RightColumnPosX;
			if (pArrImage[i][2])	pArrImage[i][2]->m_nOriginalX		= RightColumnPosX;
		}

		if ( Height > 0 )
		{
			if (pArrImage[1][i])	pArrImage[1][i]->m_nHeight			= CenterVertSize;
			if (pArrImage[1][i])	pArrImage[1][i]->m_nOriginalHeight	= CenterVertSize;
			if (pArrImage[2][i])	pArrImage[2][i]->m_nY				= BottomRowPosY;
			if (pArrImage[2][i])	pArrImage[2][i]->m_nOriginalY		= BottomRowPosY;
		}
	}

	pControl->UpdateRenderObjects();

	return true;
}


}//namespace	GameGUI

