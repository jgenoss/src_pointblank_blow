#include "pch.h"
#include "MCardMgr.h"

#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/itl/range/algorithm/rfind.h"

#include "i3Base/itl/unordered_vector.h"

#include "MCardSetInfo.h"
#include "MCardInfo.h"
#include "MCardQuestInfo.h"
#include "MCardActionInfo.h"

#include "MCardSet.h"
#include "MCard.h"
#include "MCardQuest.h"
#include "MCardCallback.h"


MCardMgr::MCardMgr() : m_selCardSetIndex(-1), m_pCardInfoForReward(nullptr), 
	m_bIsCardRewardExist(false), m_bIsCardRewardFinal(false), m_bIsAnyQuestCompleted(false)
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		m_apMCardSet[i] = new MCardSet(i);
	}
}

MCardMgr::~MCardMgr()
{
	i3::cu::for_each_delete(m_vecCardSetInfoByID);
	i3::for_each(m_apMCardSet, m_apMCardSet+MAX_CARDSET_PER_USER, i3::fu::delete_fun() );
}

// 일단 내부 구현해놓고...나중에 config.h의 ImportStringFile과 함께 리팩토링하여 정리하는게 좋겠다..
namespace
{

	i3RegKey *	_FindKey(const i3::literal_range& strKeyName, i3RegKey * pRoot, char KeySep)
	{
		typedef i3::literal_range::iterator		iterator;
		
		i3::literal_range strTest = strKeyName;
		i3::stack_string strFound;

		i3RegKey * pFoundKey = pRoot;

		while( !strTest.empty() )
		{
			iterator it_found = i3::range::find( strTest, KeySep );
			
			strFound.assign(strTest.begin(), it_found);
		
			pFoundKey = (i3RegKey *)i3TreeNode::FindNodeByName( pFoundKey, strFound.c_str() );
			if (pFoundKey == nullptr)
				return nullptr;
			
			if ( it_found == strTest.end() )
				break;

			strTest.set_begin(++it_found);
		}

		return pFoundKey;
	}

	bool ImportStringFile(i3RegKey* pRegRoot, const i3::literal_range& fileName)
	{
		// 임무카드는 스크립트 텍스트를 pef와 연동시킴...
		i3FileStream file;
		if( !file.Open( fileName.c_str(), STREAM_READ | STREAM_SHAREREAD))
		{
			return false;	//String 파일이 없다.
		}
		
		i3RegKey * pFindKey = nullptr;
		i3RegData * pFindData = nullptr;
		WCHAR16 wszReadLine[2048] = L"";

		i3::rc_wstring	wstrLocal;
		i3::string		strRegKey;
		i3::string		strRegData;

		file.SetPosition(sizeof(WCHAR16), STREAM_BEGIN);

		while( file.GetLastError() != i3Stream::STREAM_EOS)
		{
			INT32 nReadLength = file.ReadLineW(wszReadLine, sizeof(wszReadLine) );
			if( nReadLength == 0 )
				continue;

			//Label 과 Local을 분리한다.

			std::size_t wszReadLineLen = i3::generic_string_size(wszReadLine);		// nReadLength 변수결과값을 신뢰하기가 어려워보인다..
			i3::const_wchar_range rng( wszReadLine, wszReadLine + wszReadLineLen );
			if (rng.empty())
				continue;

			i3::const_wchar_range::iterator it_sep = i3::range::find( rng, L'\t');
			if( it_sep == rng.end() )
				continue;

			i3::const_wchar_range wLabelRng( rng.begin(), it_sep);
			wstrLocal.assign( ++it_sep, rng.end() );

			if (wstrLocal.empty())
				continue;

			//Label에서 RegKey와 RegData를 분리한다.

			i3::const_wchar_range::iterator it_dir_sep = i3::range::find( wLabelRng, L'\\' );		

			if ( it_dir_sep != wLabelRng.end() )		// 최소한 1개의 키는 존재해야됨..
			{
				i3::const_wchar_range test_rng(++it_dir_sep, wLabelRng.end() );

				it_dir_sep = i3::range::rfind(test_rng, L'\\');				// 가장 마지막 구분자 존재여부임..존재하면 RegKey와 RegData스트링 양쪽 모두 존재한다..

				i3::utf16_to_mb( i3::const_wchar_range(wLabelRng.begin(), it_dir_sep), strRegKey);

				if ( it_dir_sep != test_rng.end() )
				{
					i3::utf16_to_mb( i3::const_wchar_range(++it_dir_sep, test_rng.end()), strRegData);
				}
				else
				{
					strRegData.clear();
				}
			}
			else
			{
				strRegKey.clear();	strRegData.clear();
			}

			//i3RegistrySet에서 해당 Registry 키를 찾는다.
			pFindKey = _FindKey(strRegKey.c_str(), pRegRoot, '\\');

			if( pFindKey)
			{
				//찾은 Registry 키에서 해당 Registry Data를 찾는다.
				pFindData = pFindKey->FindDataByName( strRegData.c_str() );
				//없으면 만들라. (만드는 타입은 i3RegString으로 고정...)
				if ( pFindData == nullptr )
				{
					i3RegString * pRegStr = i3RegString::new_object();
					pRegStr->SetName(strRegData);
					pFindKey->AddData(pRegStr);

					pFindData = pRegStr;
				}		

				//값을 넣어라.
				switch( pFindData->getDataType())
				{
				case I3REG_TYPE_STRING:
					{
						if( i3::generic_string_size( wstrLocal ) > 0)
						{
							//	문자 데이터중에 문자형 '\\n'이 있으면 아스키 '\n'로 다시 변환해야 한다.
							i3::const_wchar_range::iterator it = i3::range::find( wstrLocal, L'\\');
							if ( it != wstrLocal.end() )
							{
								++it; 
								if ( it != wstrLocal.end() && *it == L'n')
								{
									i3::wstring temp = wstrLocal;
									i3::generic_string_replace_all( temp, L"\\n", L"\n");
									wstrLocal = temp;
								}
							}
						}

						//	찾아서 값을 넣는다.
						((i3RegString*) pFindData)->setValue( wstrLocal);
					}
					break;
				case I3REG_TYPE_INT32:
					{
						INT32 val = i3::atoi(wstrLocal);

						//	찾아서 값을 넣는다.
						((i3RegINT32*) pFindData)->setValue( val);
					}
					break;
				case I3REG_TYPE_REAL32:
					{
						REAL32 val = (REAL32) i3::atof(wstrLocal);

						//	찾아서 값을 넣는다.
						((i3RegREAL32*) pFindData)->setValue( val);
					}
					break;
				default:
					{
						I3PRINTLOG(I3LOG_NOTICE,"i3RegistrySet::ImportStringFile invliad type : %d", pFindData->getDataType());
					}
					break;
				}
			}	
		
		}
		
		return true;
	}

	bool	FillMCardQuest(const QUEST_CTX* ctx, MCardQuestInfo* outQuestInfo)
	{
		bool bResult = true;
	

		switch(ctx->m_ID)
		{
		case 0:
			bResult = false;
			break;
		case 1:		// 0 - Action, 1 - Stage, 2 - Count, 3 - Acc(누적, 연속), 4 - Team, 5 - 결과 카운트
			{

				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID(STAGE_ID(ctx->m_Arg[1]) );	// QuestDef.h의 함수.
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];
				outQuestInfo->m_Team   = TEAM_TYPE(ctx->m_Arg[4]);
				outQuestInfo->m_bAccum = (ctx->m_Arg[3] != 0);
			}
			break;
		case 2:	   //0 - Action, 1 - Stage, 2 - Count, 3 - 무기종, 4 - 무기넘버, 5 - 결과 카운트 (누적)	
			{

				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID(STAGE_ID(ctx->m_Arg[1]) );
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];
				outQuestInfo->m_WeaponClassType = WEAPON_CLASS_TYPE(ctx->m_Arg[3]);
				outQuestInfo->m_WeaponNumber    = ctx->m_Arg[4];
				outQuestInfo->m_bAccum = true;
			}
			break;
		case 3:   // 0 - Action, 1 - Stage, 2 - Count, 3 - 무기종, 4 - 무기넘버, 5 - 결과 카운트 (연속)
			{
				ResetMCardQuestInfo(outQuestInfo);
				outQuestInfo->m_QuestID = ctx->m_ID;
				outQuestInfo->m_ActionID = (ACTION_ID)ctx->m_Arg[0];

				outQuestInfo->m_StageID = getQuestStageID( STAGE_ID(ctx->m_Arg[1]) );
				outQuestInfo->m_TargetCompleteCount = ctx->m_Arg[2];	
				outQuestInfo->m_WeaponClassType = WEAPON_CLASS_TYPE(ctx->m_Arg[3]);
				outQuestInfo->m_WeaponNumber    = ctx->m_Arg[4];
				outQuestInfo->m_bAccum = false;
			}
			break;
		default:
			bResult = false;
			break;
		}

		return bResult;
	}

	void	FillMCardPrizeInfo_Mqf(const QUEST_ITEM_INFO_NEW& itemInfo, MCardPrizeInfo_Mqf* outPrize)
	{
		i3::svector<MCardPrizeInfo_Mqf_ItemInfo>& vecPrizeItem = outPrize->vecPrizeItem;

		outPrize->point = itemInfo.m_ui32Point;
		outPrize->exp = itemInfo.m_ui32Exp;
		outPrize->prize = itemInfo.m_ui32Prize;

		INT32 itemCount = 0;
		for (INT k = 0; k <MAX_QUEST_ITEM_CARD ; ++k)
		{
			// 유효한 값만 찾는다..
			if (itemInfo.m_Item[k].m_ui32ItemID > 0 && itemInfo.m_Item[k].m_i8AuthType > 0 && itemInfo.m_Item[k].m_ui32Arg > 0)
				++itemCount;
		}

		vecPrizeItem.resize(itemCount);

		itemCount = 0;

		for (INT k = 0; k <MAX_QUEST_ITEM_CARD ; ++k)
		{
			// 유효한 값만 찾는다..
			if (itemInfo.m_Item[k].m_ui32ItemID > 0 && itemInfo.m_Item[k].m_i8AuthType > 0 && itemInfo.m_Item[k].m_ui32Arg > 0)
			{
				vecPrizeItem[itemCount].m_ui32ItemID = itemInfo.m_Item[k].m_ui32ItemID;
				vecPrizeItem[itemCount].m_ui32Arg = itemInfo.m_Item[k].m_ui32Arg;
				vecPrizeItem[itemCount].m_i8AuthType = itemInfo.m_Item[k].m_i8AuthType;
				++itemCount;
			}
		}
	}

}

#define MAX_QUEST_REWARD_ITEM_COUNT		4

bool		MCardMgr::LoadRegistryFile(const i3::rc_string& strFileName, 
										   const i3::rc_string& scriptTextFileTitle)
{
	i3RegistrySet regFile;
	INT32 nReadBytes = regFile.LoadRegistrySet(strFileName.c_str(), REGISTRY_FILE_BINARY);

	if (nReadBytes == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", strFileName );		
		return false;
	}
	
	i3RegKey * pRootRegKey = regFile.getRoot();
	i3RegKey* pMainKey = nullptr;
	{
		pMainKey = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("Quest"));
		if (pMainKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey Quest");		
			return false;
		}
	}
	
	// Quest_국가.txt 연동 부분..
	const char* pszLanguageSuffix =	GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage);

	i3::stack_string textFilePath;
#if defined( MULTYLANGUAGE)
	i3::sprintf(textFilePath, "Locale/%s/String/%s_%s.txt", pszLanguageSuffix, scriptTextFileTitle, pszLanguageSuffix);
#else
	i3::sprintf(textFilePath, "Script/String/%s/%s_%s.txt", pszLanguageSuffix, scriptTextFileTitle, pszLanguageSuffix);
#endif

	if ( !ImportStringFile(pRootRegKey, textFilePath) )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", textFilePath );		
		return false;
	}
			

	{
	// 1. 카드셋 문자열 정보 확인
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("CardSet"));
	
		if (pKey)
		{		
			i3::vector<MCardSetInfo*> vecCardSetInfoByID;
			const INT32 keyCount = pKey->getChildCount();

			for(INT32 i = 0; i < keyCount; i++)
			{
				i3RegKey* childKey = (i3RegKey*)pKey->getChild(i);

				const i3::rc_string& childKeyName = childKey->GetNameString();


				INT32 cardSetID = -1;
				GetKeyData(childKey, "CardSetId", &cardSetID);
				
				if (cardSetID < 1) continue;
				
				if (i3::vu::is_value_valid(vecCardSetInfoByID, cardSetID) == true)
				{
					MCardSetInfo* pAlreadySet = vecCardSetInfoByID[cardSetID];
					I3PRINTLOG(I3LOG_FATAL, "%s : Already Set exists (from SetID : %d)", childKeyName, pAlreadySet->setID);
					delete pAlreadySet;
					vecCardSetInfoByID[cardSetID] = nullptr;
				}
				
				
				MCardSetInfo* pNewSetInfo = new MCardSetInfo;
				InitMCardSetInfo(pNewSetInfo);
				i3::vu::set_value_force(vecCardSetInfoByID, cardSetID, pNewSetInfo);
				
				pNewSetInfo->setID = cardSetID;

				GetKeyData(childKey, "CardSetName", pNewSetInfo->wstrName);
				GetKeyData(childKey, "CardSetTitle", pNewSetInfo->wstrTitle);
				GetKeyData(childKey, "FileName", pNewSetInfo->wstrFileName);
				GetKeyData(childKey, "BonusBadgeMaster", &pNewSetInfo->bonusBadgeMaster);
				GetKeyData(childKey, "UITemplateName", pNewSetInfo->wstrUITemplateName);
				GetKeyData(childKey, "UITemplateIndex", &pNewSetInfo->UITemplateIndex);

				GetKeyData(childKey, "CardSetPeriod", pNewSetInfo->wstrEventPeriod);
			}

			m_vecCardSetInfoByID.assign(vecCardSetInfoByID.begin(), vecCardSetInfoByID.end());
		}
	}

	// 2. 보상 문자열 정보 확인 (4개 밖에 안쓰기 때문에, V1과 달리 대폭 삭제 정리합니다...(pef/txt 모두 )
	{
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("Prize"));

		if (pKey)
		{	
			// 약장, 휘장, 훈장 RegKey 테이블
			i3RegKey* prizeTypeKey[BADGE_TYPE_COUNT];
			prizeTypeKey[BADGE_TYPE_RIBBON] = static_cast<i3RegKey*>(pKey->FindChildByName("Ribbon"));
			prizeTypeKey[BADGE_TYPE_ENSIGN] = static_cast<i3RegKey*>(pKey->FindChildByName("Ensign"));
			prizeTypeKey[BADGE_TYPE_MEDAL] = static_cast<i3RegKey*>(pKey->FindChildByName("Medal"));
			prizeTypeKey[BADGE_TYPE_MASTER] = static_cast<i3RegKey*>(pKey->FindChildByName("Master"));

			for (INT32 i = 0 ; i < BADGE_TYPE_COUNT ; ++i)
			{
				if (prizeTypeKey[i])
				{
					prizeTypeKey[i]->GetData("Description", m_awstrMedalDescription[i]);
				}
			}
		}
	}
	
	// 3. Quest폴더에 연계되어 있는 mqf파일들을 읽고 MCardSetInfo에 적재..
	{
		
		QUEST_CTX	loadedQuest[MAX_QUEST_PER_CARDSET];		// 16*10이라고 한다... (실제로는 4*10정도로만 쓰고 빈곳이 많을 것 같다..)
		QUEST_ITEM_INFO_NEW2		itemInfoNew;

		i3::stack_string strQuestFileName;

		const size_t numCardSet = m_vecCardSetInfoByID.size();

		for (size_t i = 0 ; i < numCardSet ; ++i)
		{
			MCardSetInfo* pSet = m_vecCardSetInfoByID[i];
			if (pSet == nullptr)
				continue;

			const i3::rc_wstring& wstrFileName = pSet->wstrFileName;
			
			if (wstrFileName.empty())
				continue;

			i3::utf16_to_mb(wstrFileName, strQuestFileName);

			if ( QuestFileLoadEx(strQuestFileName.c_str(), loadedQuest, &itemInfoNew ) < 0)
			{
				I3PRINTLOG(I3LOG_FATAL, "not found file : %s\n", strQuestFileName);
				continue;
			}
				
			UINT32 count = 0;
			
			for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)		// 10
			{
				pSet->mCardInfo[j].reset(new MCardInfo);
				
				MCardInfo* pCardInfo = pSet->mCardInfo[j].get();

				InitMCardInfo(pCardInfo);

				for (INT32 k = 0; k < MAX_QUEST_PER_CARD_NEW ; ++k, ++count)	// 4
				{
					const QUEST_CTX& questCtx =	loadedQuest[count];
					
					MCardQuestInfo* new_quest_info = new MCardQuestInfo;

					bool bRes = FillMCardQuest(&questCtx, new_quest_info);

					if (bRes)
					{
						pCardInfo->aQuestInfo[k].reset(new_quest_info);
					}
					else
					{
						delete new_quest_info;
					}
				}
			}
			// 아직 끝난게 아님..itemInfoNew를 적절이 담아야한다...
			
			pSet->totPoint = 0;
			pSet->totExp = 0;	

			for (INT32 j = 0; j < MAX_CARD_PER_CARDSET ; ++j)
			{	
				MCardInfo* pCardInfo = pSet->mCardInfo[j].get();
				MCardPrizeInfo_Mqf& mCardPrize = pCardInfo->mCardPrize;
				QUEST_ITEM_INFO_NEW& itemInfo =	itemInfoNew.m_CardReward[j];

				FillMCardPrizeInfo_Mqf(itemInfo, &mCardPrize);
				pSet->totPoint	+= itemInfo.m_ui32Point;
				pSet->totExp	+= itemInfo.m_ui32Exp;
			}
			
		//	itemInfoNew.m_CompleteReward 가 전혀 반영되어 있지 않았던 상태로 이제 반영하는게 좋을듯 싶다.
			{
				MCardPrizeInfo_Mqf& mCardPrize = pSet->prizeCompletion;
				QUEST_ITEM_INFO_NEW& itemInfo = itemInfoNew.m_CompleteReward;
				FillMCardPrizeInfo_Mqf(itemInfo, &mCardPrize);
			}

		}
	}
	
	// 4. Action String 연결
	{
		i3RegKey* pKey = static_cast<i3RegKey*>(pMainKey->FindChildByName("Action"));

		if (pKey)
		{
			i3::vector<i3::rc_wstring> vecActionStrID;
			i3::vector<i3::rc_wstring> vecActionStrText;
			
			i3RegKey* pIDNameKey = static_cast<i3RegKey*>(pKey->FindChildByName("ID"));

			if (pIDNameKey)
			{
				const INT32 numData = pIDNameKey->GetDataCount();

				for (INT32 i = 0 ; i < numData ; ++i)
				{
					i3RegData* data = pIDNameKey->GetData(i);

					if (data->getDataType() == I3REG_TYPE_STRING)
					{
						const i3::rc_string& strID = data->GetNameString();
						int nID = i3::atoi(strID);
						const i3::rc_wstring& wstrText = static_cast<i3RegString*>(data)->getValue();
						i3::vu::set_value_force(vecActionStrID, nID, wstrText);
					}
				}
			}
			
			i3RegKey* pActionStringKey = static_cast<i3RegKey*>(pKey->FindChildByName("String"));

			if (pActionStringKey)
			{
				const INT32 numData = pActionStringKey->GetDataCount();
				for (INT32 i = 0 ; i < numData ; ++i)
				{
					i3RegData* data = pActionStringKey->GetData(i);		
					
					if (data->getDataType() == I3REG_TYPE_STRING)
					{
						const i3::rc_string& strID = data->GetNameString();
						int nID = i3::atoi(strID);
						const i3::rc_wstring& wstrText = static_cast<i3RegString*>(data)->getValue();

						i3::string strText;		i3::utf16_to_utf8(wstrText, strText);

						i3::vu::set_value_force(vecActionStrText, nID, GAME_RCSTRING(strText) );
					}
				}
			}

			const size_t numActionID = vecActionStrID.size();
			const size_t numActionString = vecActionStrText.size();

			const size_t numAction = ( numActionID < numActionString ) ? 
										numActionString : numActionID;

			m_vecActionByID.resize(numAction);
			
			for (size_t i = 0 ; i < numActionID ; ++i)
				m_vecActionByID[i].m_wstrActionID = vecActionStrID[i];
			for (size_t i = 0 ; i < numActionString ; ++i)
				m_vecActionByID[i].m_wstrActionName = vecActionStrText[i];
		}
	}

	return true;
}


const MCardActionInfo*	MCardMgr::GetMCardActionInfoByID( ACTION_ID actionID) const
{
	return &m_vecActionByID[actionID];
}

size_t		MCardMgr::GetMCardActionInfoCountByID() const
{
	return m_vecActionByID.size();	
}


const MCardSetInfo*	MCardMgr::FindMCardSetInfoBySetType( INT32 setType ) const
{ 
	if (setType < 0 || setType >= INT32(m_vecCardSetInfoByID.size()))
		return nullptr;
	return m_vecCardSetInfoByID[setType];
}

INT32		MCardMgr::GetMCardSetCountBySetType() const
{
	return INT32(m_vecCardSetInfoByID.size());
}

namespace
{

	void ActivateMCardSet(MCardSet* pSet, INT32 setType)
	{
		if (setType == QUEST_CARDSET_TYPE_NONE)
		{
			pSet->Deactivate();	return;
		}

		const MCardSetInfo* pSetInfo = MCardMgr::i()->FindMCardSetInfoBySetType(setType);
		if (pSetInfo == nullptr)
		{
			pSet->Deactivate();
			return;
		}

		pSet->Activate(pSetInfo);
	}

}

namespace 
{

	void UpdateSelectedMCardSetMCardMCardQuest(const QUESTING_INFO* questingInfo, INT32& outSelMCardSetIndex, MCardSet** inoutMCardSetArray )
	{

		outSelMCardSetIndex = questingInfo->m_ActiveIndexUse;

		if (outSelMCardSetIndex < 0 || outSelMCardSetIndex >= MAX_CARDSET_PER_USER)
			outSelMCardSetIndex = -1;
		
		if ( inoutMCardSetArray[outSelMCardSetIndex]->IsActive() == false )
			outSelMCardSetIndex = -1;

		for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
		{
			MCardSet* pSet = inoutMCardSetArray[i];

			if (pSet->IsActive() == false)
				continue;

			UINT8 cardIdx = questingInfo->m_pActiveIndexCardSet[i];

			pSet->SelectCardIndex( cardIdx);

			for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)
			{
				MCard* card = pSet->GetMCard( j );

				UINT16 questFlag = questingInfo->m_pActiveIndexCard[i][j];
				UINT16 bitMask = 0x000F;

				for (INT32 k = 0; k < MAX_QUEST_PER_CARD_NEW ; ++k , bitMask <<= 4)
				{
					bool bQuestComplete = ( (questFlag & bitMask) == bitMask );
					card->SetQuestCompletion_OffBattle(k, bQuestComplete);
				}
			}

		}

	}

}

void	MCardMgr::UpdateUserMCardInfoFromServer(const QUESTING_INFO* questingInfo, 	
													const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ])	// 퀘스트 완료 상태
{
	// 1. 4개의 셋아이디를 활성화카드셋 배열에 옮김..
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		UINT8 ui8CardType = questingInfo->m_pCardSetType[i];
		ActivateMCardSet(m_apMCardSet[i], ui8CardType);
	}
	
	// 2. 완료 여부를 적재한다..
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		MCardSet* pSet = m_apMCardSet[i];

		if (pSet->IsActive() == false)
			continue;

		for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)
		{
			MCard* pCard = pSet->GetMCard(j);

			for (INT32 k = 0 ; k < MAX_QUEST_PER_CARD_NEW ; ++k)
			{
				MCardQuest* pActiveQuest = pCard->GetMCardQuest(k);

				INT32 contextIdx =	i*MAX_CARD_PER_CARDSET*MAX_QUEST_PER_CARD_NEW + j*MAX_QUEST_PER_CARD_NEW + k;
				pActiveQuest->SetCompleteCount_OffBattle(aQuestCtxActive[contextIdx]);
			}
			
		}
	}

	// 3. 현재 선택된 셋 /  셋내부에 선택된 카드 /  카드 안에 현재 퀘스트 인덱스 등을 넘긴다..
	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}

}

void	MCardMgr::ActivateCardSetSlotFromServer(const QUESTING_INFO* questingInfo, INT32 setSlot)
{
	I3ASSERT( setSlot < MAX_CARDSET_PER_USER);

	UINT8 ui8CardType = questingInfo->m_pCardSetType[setSlot];
	ActivateMCardSet(m_apMCardSet[setSlot], ui8CardType);		// 액티브된 것은 퀘스트에서는 수행 완료카운트를 0으로 리셋.
	
	// 현재 선택된 셋 /  셋내부에 선택된 카드 /  카드 안에 현재 퀘스트 인덱스 등을 넘긴다..
	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}
}

void	MCardMgr::UpdateCardSetSlotsFromServer(const QUESTING_INFO* questingInfo)
{
	//
	// 삭제의 경우 (그리고 만약 있다면 끝이 아닌 삽입의 경우) 기존 셋은 수행카운트가 유지되어야 되는 문제가 있다.. 변경 이전 셋타입과 비교해야될듯??
	// 문제를 쉽게 하려면 m_apActiveMCardSet 배열을 통째로 옮기고 같은 것은 그대로 유지..
	// 다른 것은 Activate시키는 쪽으로 처리한다...
	//

	i3::unordered_vector<MCardSet*> 
		vecMCardSet_Old( m_apMCardSet, m_apMCardSet+MAX_CARDSET_PER_USER);

	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
		m_apMCardSet[i] = nullptr;					// 일단 모두 nullptr로 변경..
		
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		UINT8 ui8CardType = questingInfo->m_pCardSetType[i];
		
		const size_t numOldSet = vecMCardSet_Old.size();

		for (size_t j = 0 ; j < numOldSet ; ++j)
		{
			MCardSet* pOldSet = vecMCardSet_Old[j];
			
			UINT8 ui8CardType_Old = QUEST_CARDSET_TYPE_NONE;

			if (pOldSet->GetMCardSetInfo())
				ui8CardType_Old = UINT8(pOldSet->GetMCardSetInfo()->setID);

			if (ui8CardType_Old == ui8CardType)
			{
				m_apMCardSet[i] = pOldSet;
				m_apMCardSet[i]->m_myIndex = i;			// myIndex 동기화를 잊으면 안됨..
				vecMCardSet_Old.erase(j);
				break;
			}
		}
	}

	// 새 것만 처리...
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		if ( m_apMCardSet[i] == nullptr )
		{
			I3ASSERT(!vecMCardSet_Old.empty());
			m_apMCardSet[i] = vecMCardSet_Old.back();
			m_apMCardSet[i]->m_myIndex = i;
			vecMCardSet_Old.pop_back();

			UINT8 ui8CardType = questingInfo->m_pCardSetType[i];	// 
			ActivateMCardSet(m_apMCardSet[i], ui8CardType);
		}
	}

	{
		UpdateSelectedMCardSetMCardMCardQuest(questingInfo, m_selCardSetIndex, m_apMCardSet);
	}
	
}

i3::rc_wstring MCardMgr::GetPrizeDescription(BADGE_TYPE t) const
{ 
	if (t == BADGE_TYPE_UNKOWN)
		return i3::rc_wstring();
	return m_awstrMedalDescription[t]; 
}

void		MCardMgr::ModifyQuestFromServer(INT32 activeSetIndex, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx)
{
	I3ASSERT(activeSetIndex < MAX_CARDSET_PER_USER);
	I3ASSERT(cardIdx < MAX_CARD_PER_CARDSET);
	I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);

	MCardSet* pSet = m_apMCardSet[activeSetIndex];
	MCard* pCard = pSet->GetMCard(cardIdx);
	MCardQuest* pActiveQuest = pCard->GetMCardQuest(questIdx);
	
	const MCardQuestInfo* pQuest = pActiveQuest->GetMCardQuestInfo();
	I3ASSERT(pQuest != nullptr);			// 일단 존재해야한다고 가정한다..
	
	FillMCardQuest(&ctx, const_cast<MCardQuestInfo*>(pQuest));		// MCardQuest 내용 변경..
	pActiveQuest->SetCompleteCount_OffBattle( ctx.m_Arg[MAX_QUEST_ARG_COMPLETE]);		// 완료카운트는 따로 셋팅해준다..
}

void	MCardMgr::ModifyCardPrizeFromServer(INT32 activeSetIndex, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo)
{
	I3ASSERT(activeSetIndex < MAX_CARDSET_PER_USER);
	I3ASSERT(cardIdx < MAX_CARD_PER_CARDSET);

	MCardSet* pSet = m_apMCardSet[activeSetIndex];
	MCard* pActiveCard = pSet->GetMCard(cardIdx);

	const MCardInfo* pCardInfo = pActiveCard->GetMCardInfo();
	I3ASSERT(pCardInfo != nullptr);			// 일단 존재해야한다고 가정한다..

	FillMCardPrizeInfo_Mqf(questItemInfo, const_cast<MCardPrizeInfo_Mqf*>(&pCardInfo->mCardPrize) );		// 서버에서 주는 경우, const를 떼고 변경..
}

void	MCardMgr::UpdateSelectedCardQuestFromServer_OnBattle(INT32 questIdx, INT32 completeCountOfQuest, bool isQuestComplete)
{
	//I3ASSERT(m_selCardSetIndex != -1);
	if (m_selCardSetIndex < 0)
		return;

	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];

	MCard* pActiveCard = pSet->GetSelectedMCard();

	if (pActiveCard && pActiveCard->IsActive())
	{
		bool bCompleteQuest = false;
		bool bCompleteMissionCard = false;

		I3ASSERT(questIdx < MAX_QUEST_PER_CARD_NEW);
		
		MCardQuest* activeQuest =	pActiveCard->GetMCardQuest(questIdx);
		activeQuest->SetCompleteCount_OnBattle(completeCountOfQuest);		// 게임 중 동적 카운팅으로 
																	// 완료되지 않은 채로 게임을 도중에 관뒀을경우에는 원래값으로도 복귀해야함..
																	//
		
		if (pActiveCard->IsQuestCompleted_OnBattle(questIdx) == false && 
			isQuestComplete == true)
		{
			pActiveCard->SetQuestCompletion_OnBattle(questIdx, true);	// 한 개 퀘스트 완료 지점..
			bCompleteQuest = true;

			// pActiveCard를 기점으로 전체 퀘스트 완료되었는지 여부를 콜백으로 세우는게 좋겠다..
			if ( pActiveCard->IsAllQuestCompleted_OnBattle() )
				bCompleteMissionCard = true;
		}

		m_pCallback->OnCurrQuestCompletionCountChanged_OnBattle(questIdx, bCompleteMissionCard, bCompleteQuest);

		if (bCompleteQuest)
		{
			GameEventSender::i()->SetEvent(EVENT_N_QUEST_END_NOTICE);	// 배틀 중 완료된 이벤트 보내기 (처리가 되는지는 의문임)
			m_pCallback->OnCurrQuestCompleted_OnBattle(questIdx, bCompleteMissionCard);			// HUD의 AddChallengeClear() 가 연결되어야 한다..
		}
		
		if (bCompleteMissionCard)
			m_pCallback->OnCurrMissionCardCompleted_OnBattle(questIdx);
	}

}

// 게임 포기의 케이스는 클라사이드 3개, 서버사이드 1개임..
// 1. EVENT_GIVEUP_BATTLE을 클라에서 SEND할때 동시에..
// 2. 강퇴투표 결과로 강퇴자가 자동으로 빠져나오게 되는 경우..( PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK )
// 3. EVENT_M_TIMEOUTCS (배틀을 오랫동안 방치한 경우) .. 
// 4. EVENT_M_TIMEOUTCN (배틀을 오랫동안 방치한 경우) .. 

void	MCardMgr::ProcessGiveupBattle()
{
	// 게임 포기의 경우 퀘스트 완료는 모두 취소된다..
	if (m_selCardSetIndex == -1)
		return;
	
	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
	MCard* pCard = pSet->GetSelectedMCard();

	if (pCard == nullptr)
		return;
	if (pCard->IsActive() == false)
		return;

	pCard->CancelCompletion();
		
}

void	MCardMgr::ReceiveSelectAndResultFromServer(bool isFinalCardCompleted, bool isEndProtocal )	// 클라송수신의 수신으로도 쓰이고, 배틀 종료 후에는 일방적으로 받기도 하는 것 같다..
{
	// 배틀 아닌 바깥에서 호출되며, 이곳에서 Quest 및 카드 완료를 깨끗하게 설정해줘도 되겠다... 
	if ( m_selCardSetIndex == -1)
		return;

	MCardSet* pSelectedSet = m_apMCardSet[m_selCardSetIndex];
	MCard*	pSelectedCard = pSelectedSet->GetSelectedMCard();
	
	if (pSelectedCard == nullptr || pSelectedCard->IsActive() == false )
		return;

	if( !isEndProtocal )
		m_bIsCardRewardFinal = isFinalCardCompleted;		// ENDBATTLE 시점에서 이처리는 하면 안됨.
	
	bool bAnyQuestComplete = pSelectedCard->IsDifferentComplete_OnOffBattle();

	if (bAnyQuestComplete == false)		// 딱히 배틀에서 나온 경우가 아닌것 같은 경우 일단 리턴..
	{
		pSelectedCard->ConfirmCompletion();				// 배틀 종료시점이 아니더라도 딱히 영향을 주지 않을 것이다.
		return;
	}
		
	m_pCardInfoForReward = pSelectedCard->GetMCardInfo();
	m_bIsAnyQuestCompleted = true;		// 퀘스트 수행갱신이 있으면 일단 true... 나중에 보상표시단계에서 리셋해야됨..

	bool bOnBattleAllComplete = pSelectedCard->IsAllQuestCompleted_OnBattle();
	bool bOffBattleAllComplete = pSelectedCard->IsAllQuestCompleted_OffBattle();
	bool bAllQuestComplete   = (bOffBattleAllComplete == false && bOnBattleAllComplete == true);
		
	pSelectedCard->ConfirmCompletion();				// 배틀 종료시점이 아니더라도 딱히 영향을 주지 않을 것이다.
			
	// 배틀 종료 후 들어오기도 하는데, 이때 보상팝업과 관련된 정보는 그냥 콜백으로.....
	INT32 prevCardIndex = pSelectedCard->GetMyIndex();
	INT32 nextCardIndex = prevCardIndex;

	if (bAllQuestComplete)
	{
		m_bIsCardRewardExist = true;

		m_pCallback->OnCurrMissionCardCompleted_AfterBattle(prevCardIndex, pSelectedCard->GetMCardInfo());
		// 카드 완료 시점에서, 다음 번카드로 자동으로 이동해야한다.....
		nextCardIndex = pSelectedSet->SelectNextAvailableCardIndex();
		//
	}
	else
	{
		m_bIsCardRewardExist = false;
		m_bIsCardRewardFinal = false;			// 수행이 되지 않은 것이면 최종보상플래그가 잘못된것임..
	}

	// 서버에 전송하는 것이 있음..
	MCardMgr::SendClientSelectStateToServer();		
	//

	if ( bAllQuestComplete ) // 선택카드가 이동된 경우를 위한 콜백....
	{
		m_pCallback->OnNextMissionCardAutoSelecting_AfterBattle( prevCardIndex, nextCardIndex );	// SEND는 보냈지만...아직 서버에서는 받지 상태....
	}																								// 더이상 선택될 카드가 없는 경우에도 보내도록 한다.

}

void	MCardMgr::SendClientSelectStateToServer()
{
	// 
	QUEST_ACTIVE_INDEX questActiveIdx;
	
	if (m_selCardSetIndex == -1)
	{
		questActiveIdx._idxUse = 0xFF;
		questActiveIdx._idxCard = 0xFF;
		questActiveIdx._idxQuest = 0xFFFF;
	}
	else
	{
		questActiveIdx._idxUse = UINT8(m_selCardSetIndex);

		MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
		MCard* pSelCard = pSet->GetSelectedMCard();
		
		if (pSelCard == nullptr)
		{
			questActiveIdx._idxCard = 0xFF;
			questActiveIdx._idxQuest = 0xFFFF;
		}
		else
		{
			questActiveIdx._idxCard = UINT8(pSelCard->GetMyIndex());
			
			UINT16 questFlag = 0x0000;

			for (INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; ++i)
			{
				UINT16 val = pSelCard->IsQuestCompleted_OffBattle(i) ? 0xF : 0x0;
				questFlag |= (val) << (i * 4);
			}

			questActiveIdx._idxQuest = questFlag;
		}	
	}
	
	GameEventSender::i()->SetEvent(EVENT_QUEST_CHANGE_ACTIVE_IDX, &questActiveIdx);
	
}

INT32	MCardMgr::FindCardSetSlotWithCardSetType(INT32 cardSetType) const
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		MCardSet* pSet = m_apMCardSet[i];

		if (pSet->IsActive() == false)
			continue;
		
		if (pSet->GetMCardSetInfo()->setID == cardSetType)
			return i;
	}

	return -1;	
}



void	MCardMgr::SelectActiveCardSetToServer(INT32 selSetIdx)	// 카드인덱스는 기존 저장된 인덱스 그대로 적용..
{
	if (selSetIdx < 0 || selSetIdx >= MAX_CARDSET_PER_USER)
		selSetIdx = -1;
	else
	if ( m_apMCardSet[selSetIdx]->IsActive() == false)
		selSetIdx = -1;

	m_selCardSetIndex = selSetIdx;

	MCardMgr::SendClientSelectStateToServer();
}

void	MCardMgr::SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx)
{
	if (selSetIdx < 0 || selSetIdx >= MAX_CARDSET_PER_USER)
		selSetIdx = -1;
	else
	if ( m_apMCardSet[selSetIdx]->IsActive() == false)
		selSetIdx = -1;

	m_selCardSetIndex = selSetIdx;
	MCardMgr::SelectActiveCardToServer(selCardIdx);
}

void	MCardMgr::SelectActiveCardToServer(INT32 selCardIdx)
{
	if (m_selCardSetIndex == -1)
		return;
	
	MCardSet* pSet = m_apMCardSet[m_selCardSetIndex];
	pSet->SelectCardIndex(selCardIdx);
	MCardMgr::SendClientSelectStateToServer();
}


// 서버에서 주는 목록으로 해야할 것 같다..

INT32	MCardMgr::FindNextAvailableCardSetSlotIndex() const
{
	if (m_selCardSetIndex == -1)
	{
		// 없으면 서버목록 인덱스 처음부터 보유 순서대로로 처리..
		const size_t numServerList = m_vecCardSetTypeList_Server.size();

		for (size_t i = 0 ; i < numServerList ; ++i)
		{
			INT32 setType =	m_vecCardSetTypeList_Server[i];

			INT32 slotIdx = MCardMgr::FindCardSetSlotWithCardSetType(setType);

			if (slotIdx != -1)
				return slotIdx;
		}
		
		return -1;
	}

	// 선택된 세트가 있으면 그 놈 다음걸로 처리하도록 해야한다..
	
	MCardSet* pCurrSet = m_apMCardSet[ m_selCardSetIndex ];
	INT32 selSetType = pCurrSet->GetMCardSetInfo()->setID;
	INT32 selServerIdx = m_vecServerListIdx_BySetType[selSetType];
	
	I3ASSERT(selServerIdx != -1);

	const INT32 numServerList = INT32(m_vecCardSetTypeList_Server.size());

	INT32 nextSel = selServerIdx + 1;
	if (nextSel >= numServerList)
		nextSel = 0;

	while(nextSel != selServerIdx)
	{
		INT32 nextSetType = m_vecCardSetTypeList_Server[nextSel];

		INT32 slotIdx = MCardMgr::FindCardSetSlotWithCardSetType(nextSetType);
		
		if (slotIdx != -1)
			return slotIdx;

		++nextSel;
		if (nextSel >= numServerList)
			nextSel = 0;
	}

	return -1;
}


namespace
{
	class MCardCallback_None : public MCardCallback
	{
		virtual void OnCurrQuestCompletionCountChanged_OnBattle(INT32 questIdx, bool bCompleteCard, bool bCompleteQuest) {}
		virtual void OnCurrQuestCompleted_OnBattle(INT32 questIdx, bool bCompleteCard) {}
		virtual void OnCurrMissionCardCompleted_OnBattle(INT32 lastQuestIdx) {}

		virtual void OnCurrMissionCardCompleted_AfterBattle(INT32 completeCardIndex, const MCardInfo* card ) {}
		virtual void OnNextMissionCardAutoSelecting_AfterBattle(INT32 prevCardIdx, INT32 currCardIdx) {}
		virtual void OnUpdateAllHUDOnBattle() {}
	} default_callback;
}

void	MCardMgr::SetCallback(MCardCallback* callback)
{
	m_pCallback = (callback) ? callback : &default_callback;
}

MCardSet*	MCardMgr::GetSelectedMCardSet() const
{
	if (m_selCardSetIndex == -1)
		return nullptr;
	return m_apMCardSet[m_selCardSetIndex];
}

INT32		MCardMgr::GetSelectedMCardSetIndex() const
{
	return m_selCardSetIndex;
}

MCard*	MCardMgr::GetSelectedMCard() const
{
	MCardSet*	pSet = MCardMgr::GetSelectedMCardSet();
	if (pSet == nullptr)
		return nullptr;
	return pSet->GetSelectedMCard();
}

void		MCardMgr::UpdateAllHUDCallbackOnBattle()
{
	m_pCallback->OnUpdateAllHUDOnBattle();	// pHUD->ChangeChallenge(); 를 대체함..
}

bool		MCardMgr::RemoveSelectedCardSetAfterFinalPrize()
{
	if (m_selCardSetIndex == -1)
		return false;

	MCardSet* pSet = m_apMCardSet[ m_selCardSetIndex];
	if (!pSet->IsActive())
		return false;
	
	// 이벤트 카드는 서버에서 알아서 지우지 않는 한 안지워지는 모양이다..
	if (pSet->GetMCardSetInfo()->setID == QUEST_CARDSET_TYPE_EVENT)
		return false;			
	
	pSet->Deactivate();
	m_selCardSetIndex = -1;
	return true;
}

//
// 카드삭제는 서버에 보내는게 없다... 아마도 서버에서 이미 지워졌고, 클라에서 이걸 반영하는 모양이다..
//

bool		MCardMgr::RemoveCardSetAfterFinalPrize(INT32 cardSetSlot)
{
	if (cardSetSlot < 0 || cardSetSlot >= MAX_CARDSET_PER_USER)
		return false;

	MCardSet* pSet = m_apMCardSet[ cardSetSlot];
	if (!pSet->IsActive())
		return false;
	
	// 이벤트 카드는 서버에서 알아서 지우지 않는 한 안지워지는 모양이다..
	if (pSet->GetMCardSetInfo()->setID == QUEST_CARDSET_TYPE_EVENT)
		return false;			
	
	pSet->Deactivate();
	if (m_selCardSetIndex == cardSetSlot)
		m_selCardSetIndex = -1;

	return true;
}


i3::rc_wstring MCardMgr::GetCurrHUDDesc(INT32 idxQuest) const
{
	MCard* pCard = MCardMgr::GetSelectedMCard();
	
	if (pCard == nullptr)
		return i3::rc_wstring();
	
	MCardQuest* pQuest = pCard->GetMCardQuest(idxQuest);

	return pQuest->GetCurrHUDDesc();
}

//
// 임무카드 셋 활성화 여부 설정. 이게 여전이 누락되어 있음..
//
void	MCardMgr::UpdateSystemInfoFromServer(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT])
{
	
	// 개수가 맞지 않을 경우 크래쉬 방지용 입니다. 로그 하나 박아주셔도 좋을것 같습니다. -- 20110530 김현우 --
	if( (((QUEST_CARDSET_TYPE_COUNT-1) / 32)+1) != ui8ActiveQuestCount )	return;

	m_vecCardSetTypeList_Server.clear();			// 이쪽은 셋타입을 모으는 곳이고..
	m_vecServerListIdx_BySetType.clear();			// 이쪽은 셋타입을 인덱스로 존재여부를 확인하는 컨테이너..

	i3::vector<INT32> vecCardSetTypeList_Server;
	i3::vector<INT32> vecServerListIdx_BySetType;
	
	vecServerListIdx_BySetType.resize(QUEST_CARDSET_TYPE_COUNT, -1);

	for ( INT32 i = 0 ; i < QUEST_CARDSET_TYPE_COUNT ; ++i )
	{
		if( 0x1 & (pui32ActiveQuest[ i / 32 ] >> ( i % 32 )) )
		{
			vecCardSetTypeList_Server.push_back(i);
			
			vecServerListIdx_BySetType[i] = vecCardSetTypeList_Server.size() - 1;
		}
	}
	
	m_vecCardSetTypeList_Server.assign(vecCardSetTypeList_Server.begin(), vecCardSetTypeList_Server.end());
	m_vecServerListIdx_BySetType.assign(vecServerListIdx_BySetType.begin(), vecServerListIdx_BySetType.end());

	const INT32 numSizeSetType = (	QUEST_CARDSET_TYPE_COUNT < INT32(m_vecCardSetInfoByID.size() ) ) 
									? QUEST_CARDSET_TYPE_COUNT : INT32(m_vecCardSetInfoByID.size() );

	for (INT32 i = 0 ; i < numSizeSetType ; ++i)
	{
		if (m_vecCardSetInfoByID[i])
			m_vecCardSetInfoByID[i]->goodsID = ui32QuestCardGoodsID[i];	
	}

}

INT32		MCardMgr::GetMCardSetCount_Server() const
{
	return 	m_vecCardSetTypeList_Server.size();
}

INT32		MCardMgr::GetMCardSetType_Server(INT32 idx) const // 인수가 셋타입이 아니고 리턴값이 셋타입임..
{
	if ( size_t(idx) >= m_vecCardSetTypeList_Server.size() )
		return QUEST_CARDSET_TYPE_NONE;
	return m_vecCardSetTypeList_Server[idx];
}

bool		MCardMgr::IsExistMCardSet_Server(INT32 MCardSetType) const
{
	if ( size_t(MCardSetType) >= m_vecServerListIdx_BySetType.size() )
		return false;
	return m_vecServerListIdx_BySetType[MCardSetType] != -1;
}

INT32		MCardMgr::GetServerIndexWithMCardSetType(INT32 MCardSetType) const
{
	if ( size_t(MCardSetType) >= m_vecServerListIdx_BySetType.size() )
		return -1;
	return m_vecServerListIdx_BySetType[MCardSetType];
	
}




MCardSet*	MCardMgr::GetMCardSet(INT32 slotIdx) const
{
	return m_apMCardSet[slotIdx];
}

bool		MCardMgr::HasMCardSet(INT32 cardSetType) const
{
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		const MCardSet* pSet = GetMCardSet(i);
		
		if (pSet->IsActive() == false)
			continue;
		
		if ( pSet->GetMCardSetInfo()->setID == cardSetType )
			return true;
	}

	return false;
}


void	MCardMgr::PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, 
								bool& isCardRewardExist, bool& isFinalCard) 
{
	outInfo = m_pCardInfoForReward;
	isAnyQuestCompleted = m_bIsAnyQuestCompleted;
	isCardRewardExist = m_bIsCardRewardExist;
	isFinalCard = m_bIsCardRewardFinal;

	m_pCardInfoForReward = nullptr;
	m_bIsAnyQuestCompleted = false;
	m_bIsCardRewardExist = false;
	m_bIsCardRewardFinal = false;
}
