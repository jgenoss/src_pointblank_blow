#include "pch.h"
#include "UIClanFunction.h"
#include "Minimap.h"



#include "MinimapUtil.h"

#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"

#include "ClientStageInfoUtil.h"

using namespace minimap_new;

Minimap* Minimap::m_pInstance = 0;

Minimap::Minimap() :
m_pFrame(0),
m_pImageBox(0),
m_pCurTex(0)
{
	i3mem::FillZero( &m_FrameInfo, sizeof( m_FrameInfo));
	i3mem::FillZero( &m_TexInfo, sizeof( m_TexInfo));
}

Minimap::~Minimap()
{
	m_pInstance = nullptr;

	I3_SAFE_RELEASE( m_pCurTex );

	i3::cu::for_each_safe_release(m_pTextures, &i3Texture::Release);

}

bool Minimap::Create(i3UIFrameWnd * pFrame)
{
	m_pFrame = pFrame;

	if (m_pFrame == nullptr) return false;

	if (_LoadMapInfo() == false) return false;
	if (_LoadMapTextures() == false) return false;
	_CreateMinimapImageBox();

	return true;
}

bool Minimap::_LoadMapInfo()
{
	const char* stageName = g_pFramework->getStageInfo(g_pFramework->GetCurrentStageIndex())->GetName();
	if (stageName == nullptr) return false;

	i3IniParser parser;
	char file[MAX_PATH] = {0,};
	sprintf_s(file, "World/%s/TabMinimap/info_%s.txt", stageName, stageName);

	if (!parser.OpenFromFile(file)) return false;

	bool isInfoRight = true;
	if (!parser.ReadSection("Default")) isInfoRight = false;
	if (!parser.GetValue("frameWindowWidth",  0.0, &m_FrameInfo.w))							isInfoRight = false;
	if (!parser.GetValue("frameWindowHeight", 0.0, &m_FrameInfo.h))							isInfoRight = false;
	if (!parser.GetValue("CenterX", 0.0f, &m_TexInfo.mapImageInfo.posCenterX))				isInfoRight = false;
	if (!parser.GetValue("CenterY", 0.0f, &m_TexInfo.mapImageInfo.posCenterY))				isInfoRight = false;
	if (!parser.GetValue("Width",   0.0f, &m_TexInfo.mapImageInfo.w))						isInfoRight = false;
	if (!parser.GetValue("Height",  0.0f, &m_TexInfo.mapImageInfo.h))						isInfoRight = false;
	if (!parser.GetValue("RatioWidthBaked",   0.0f, &m_TexInfo.mapImageInfo.bakedRatioW))	isInfoRight = false;
	if (!parser.GetValue("RatioHeightBaked",  0.0f, &m_TexInfo.mapImageInfo.bakedRatioH))	isInfoRight = false;
	if (!parser.GetValue("AdjustWidth",  0.0f, &m_TexInfo.mapImageInfo.ratio.AdjustWidth))	isInfoRight = false;
	if (!parser.GetValue("AdjustHeight", 0.0f, &m_TexInfo.mapImageInfo.ratio.AdjustHeight)) isInfoRight = false;
	if (!parser.GetValue("totalTexCnt", 0, &m_TexInfo.totalCount))							isInfoRight = false;

	if (isInfoRight == false) return false;

	m_FrameInfo.halfW = m_FrameInfo.w * 0.5f;
	m_FrameInfo.halfH = m_FrameInfo.h * 0.5f;
	m_TexInfo.mapImageInfo.ratio.w = (m_TexInfo.mapImageInfo.w * m_TexInfo.mapImageInfo.bakedRatioW) / _GetWorldMapSize().x;
	m_TexInfo.mapImageInfo.ratio.h = (m_TexInfo.mapImageInfo.h * m_TexInfo.mapImageInfo.bakedRatioH) / _GetWorldMapSize().z;

	sprintf_s(m_TexInfo.name, "World/%s/TabMinimap/tab_mini_%s.i3i", stageName, stageName);
	parser.Close();

	return true;
}

bool Minimap::_LoadMapTextures()
{
	i3Texture* tex = g_pFramework->GetResourceManager()->LoadTexture(m_TexInfo.name);
	if (tex == nullptr) return false;

	if (m_TexInfo.totalCount > 0)
	{
		m_pTextures.push_back(tex);

		char imageName[MAX_PATH] = {0, };
		i3Texture* additionalTex = nullptr;
		const char* stageName = g_pFramework->getStageInfo(g_pFramework->GetCurrentStageIndex())->GetName();
		if (stageName == nullptr) return false;

		for (INT32 i = 1; i < m_TexInfo.totalCount; ++i)
		{
			sprintf_s(imageName, "World/%s/TabMinimap/tab_mini_%s_%02d.i3i", stageName, stageName, i+1);
			additionalTex = g_pFramework->GetResourceManager()->LoadTexture(imageName);
			if (additionalTex == nullptr) return false;

			m_pTextures.push_back(additionalTex);
		}
	}

	m_pCurTex = tex;
	I3_MUST_ADDREF(m_pCurTex);

	return true;
}

void Minimap::_CreateMinimapImageBox()
{
	I3ASSERT(m_pFrame != nullptr);

	REAL32 x, y, w, h;
	_GetMinimapImageRect(x, y, w, h);

	m_pImageBox = i3UIImageBoxEx::new_object();
	m_pImageBox->AddTexture(m_pCurTex, 
						    static_cast<INT32>(x), 
						    static_cast<INT32>(y), 
						    static_cast<INT32>(w), 
						    static_cast<INT32>(h));

	for (INT32 i = 1; i < m_TexInfo.totalCount; i++)
		m_pImageBox->AddTexture(m_pTextures[i], 
							    static_cast<INT32>(x),
							    static_cast<INT32>(y), 
							    static_cast<INT32>(w), 
							    static_cast<INT32>(h));

	m_pImageBox->CreateBaseSceneObjects();
	m_pFrame->AddControl(m_pImageBox);
	m_pImageBox->setPos(0, 0, 0);
	I3_MUST_RELEASE(m_pImageBox);
	m_pImageBox->SetCurrentImage(0);
		
	ucf::AdjustControlPosToParentCenter(m_pImageBox);
	
}

void Minimap::Update(REAL32 deltaSeconds)
{
	_ChangeMapBG();
}

void Minimap::_ChangeMapBG()
{
	m_pImageBox->SetCurrentImage(_GetMyCurFloor());
}

VEC3D Minimap::_GetWorldMapSize()
{
	VEC3D worldSize;

	i3World* world = g_pFramework->GetCurrentStage()->getWorld();
	I3ASSERT(world != nullptr);

	i3AttrSet* attr = world->getWorldRoot();
	I3ASSERT(attr != nullptr);

	i3BoundBox box;
	attr->GetWrappingBound(&box);

	worldSize.x  = box.GetMax()->x - box.GetMin()->x;
	worldSize.y  = box.GetMax()->y - box.GetMin()->y;
	worldSize.z  = box.GetMax()->z - box.GetMin()->z;

	return worldSize;
}

INT32 Minimap::_GetMyCurFloor() const
{
	INT32 floor = 0;

	i3MapSectorTable* table = g_pFramework->GetCurrentStage()->getWorld()->getMapSectorTable();
	if (table != nullptr)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if( !pChara ) return floor;

		INT32 myID = pChara->getPortalID();
		i3::pack::MAP_SECTOR_INFO* sector = table->getMapSector(myID);
		I3ASSERT(sector != nullptr);

		floor = sector->m_Floor;
	}

	return floor;
}

void Minimap::_GetMinimapImageRect(REAL32& x, REAL32& y, REAL32& w, REAL32& h) const
{
	REAL32 halfW = m_TexInfo.mapImageInfo.w * 0.5f;
	REAL32 halfH = m_TexInfo.mapImageInfo.h * 0.5f;

	x	= m_TexInfo.mapImageInfo.posCenterX - halfW;
	y	= m_TexInfo.mapImageInfo.posCenterY - halfH;
	w	= m_TexInfo.mapImageInfo.w;
	h	= m_TexInfo.mapImageInfo.h;
}

namespace tu
{
	void ProcessInput()
	{
		tabminimap_default.ProcessInput();
	}

	void ProcessRoundEnd()
	{
		tabminimap_default.ProcessRoundEnd();
	}

	void ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
	{
		tabminimap_default.ProcessGameEvent(event, arg, UserData);
	}

	UINT16 GetTeamScore(TEAM_TYPE team)
	{
		UINT16 score = 0;

		INT32 stageType = static_cast<INT32>(MyRoomInfoContext::i()->getStage()->GetStageMode());
		switch (stageType)
		{
		case STAGE_MODE_BOMB:
		case STAGE_MODE_ANNIHILATION:
		case STAGE_MODE_DESTROY:
		case STAGE_MODE_DEFENCE:
		case STAGE_MODE_CONVOY :
		case STAGE_MODE_RUN_AWAY :
			{
				score = ScoreContext::i()->GetTeamWinRound(team);
			} 
			break;
		case STAGE_MODE_ESCAPE:
			{
				score = ScoreContext::i()->GetDinoEscapeScore(team);
			}
			break;

		default: 
			score = ScoreContext::i()->GetTeamScore(team)->m_KillCount;
			break;
		}

		return (UINT16)score;
	}

	INT32 GetRestTeamScore(TEAM_TYPE team)
	{
		UINT16 score = 0;

		if( !CStageInfoUtil::IsKillMode(*MyRoomInfoContext::i()->getStage()) )
		{
			if (team == TEAM_RED)
				score = static_cast<UINT16>(TabMinimap_RoomInfo::i()->_GetTotalRoundCount()) - ScoreContext::i()->GetRedTeamWinRound();
			else
				score = static_cast<UINT16>(TabMinimap_RoomInfo::i()->_GetTotalRoundCount()) - ScoreContext::i()->GetBlueTeamWinRound();
		} 
		else
			score = static_cast<UINT16>(TabMinimap_RoomInfo::i()->_GetLimitKillCount()) - ScoreContext::i()->GetTeamScore(team)->m_KillCount;

		return score;
	}

	bool IsNPC(INT32 slotIdx)
	{
		CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(slotIdx);
		if (chara == nullptr) return false;

		INT32 teamType			= tu::GetTeamType(slotIdx);
		INT32 myTeamType		= tu::GetMyTeamType();
		bool  isAi				= chara->isAI();
		bool  isTrainningStage	= (MyRoomInfoContext::i()->getStage()->IsAiMode());

		if (isAi || ((isTrainningStage) && (teamType != myTeamType)))
			return true;

		return false;
	}
	
	bool CanBeShown(INT32 slotIdx)
	{
		STAGE_MODE stage		=  MyRoomInfoContext::i()->getStage()->GetStageMode();
		bool isBombMissionMode	= (stage == STAGE_MODE_BOMB) ? true : false;
		bool isAnnihilateMode	= (stage == STAGE_MODE_ANNIHILATION) ? true : false;
		bool isPlaying			= UserContext::i()->IsPlayingSlot(slotIdx) ? true : false;

		bool showListAvail = false;
		if (isBombMissionMode || isAnnihilateMode )
		{
			if (isPlaying)
				showListAvail = true;
		}
		else
			showListAvail = true;

		if (IsInBattleField(slotIdx) || IsNPC(slotIdx))
			if (showListAvail) return true;

		return false;
	}

	bool IsInBattleField(INT32 slotIdx)
	{
		return (BattleSlotContext::i()->getSlotInfo(slotIdx)->_State == SLOT_STATE_BATTLE);
	}

	TEAM_TYPE GetMyTeamType()	
	{ 
		TEAM_TYPE myTeam = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getMyTeam());
		return myTeam; 
	}

	bool IsRoomOwner(INT32 slotIdx)
	{
		return (BattleSlotContext::i()->getMainSlotIdx() == slotIdx) ;
	}

	UINT8 GetPing(INT32 slotIdx)
	{
		UINT8 ping = BattleServerContext::i()->GetPingInfo()->GetPing( slotIdx);	
		if( ping > 4 ) return 4;
		return ping; 
	}


}