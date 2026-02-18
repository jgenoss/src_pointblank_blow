#include "pch.h"
#include "UIRouletteObject.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

I3_CLASS_INSTANCE( RouletteObject);//, i3Object);



const char * s_AnimName_Roulette[] =
{
	
	"Object/Roulette/Roulette_Animation01_Stop.i3a",
	"Object/Roulette/Roulette_Animation02_Stop.i3a",
	"Object/Roulette/Roulette_Animation03_Stop.i3a",
	"Object/Roulette/Roulette_Animation04_Stop.i3a",
	"Object/Roulette/Roulette_Animation05_Stop.i3a",
	"Object/Roulette/Roulette_Animation06_Stop.i3a",
	"Object/Roulette/Roulette_Animation07_Stop.i3a",
	"Object/Roulette/Roulette_Animation08_Stop.i3a",
	"Object/Roulette/Roulette_Animation09_Stop.i3a",
	"Object/Roulette/Roulette_Animation10_Stop.i3a",
	"Object/Roulette/Roulette_Animation11_Stop.i3a",
	"Object/Roulette/Roulette_Animation12_Stop.i3a",
	"Object/Roulette/Roulette_Animation13_Stop.i3a",


	"Object/Roulette/Roulette_Animation01_Idle.i3a",
	"Object/Roulette/Roulette_Animation02_Idle.i3a",
	"Object/Roulette/Roulette_Animation03_Idle.i3a",
	"Object/Roulette/Roulette_Animation04_Idle.i3a",
	"Object/Roulette/Roulette_Animation05_Idle.i3a",
	"Object/Roulette/Roulette_Animation06_Idle.i3a",
	"Object/Roulette/Roulette_Animation07_Idle.i3a",
	"Object/Roulette/Roulette_Animation08_Idle.i3a",
	"Object/Roulette/Roulette_Animation09_Idle.i3a",
	"Object/Roulette/Roulette_Animation10_Idle.i3a",
	"Object/Roulette/Roulette_Animation11_Idle.i3a",
	"Object/Roulette/Roulette_Animation12_Idle.i3a",
	"Object/Roulette/Roulette_Animation13_Idle.i3a",

	"Object/Roulette/Roulette_Animation01_Start.i3a",
	"Object/Roulette/Roulette_Animation02_Start.i3a",
	"Object/Roulette/Roulette_Animation03_Start.i3a",
	"Object/Roulette/Roulette_Animation04_Start.i3a",
	"Object/Roulette/Roulette_Animation05_Start.i3a",
	"Object/Roulette/Roulette_Animation06_Start.i3a",
	"Object/Roulette/Roulette_Animation07_Start.i3a",
	"Object/Roulette/Roulette_Animation08_Start.i3a",
	"Object/Roulette/Roulette_Animation09_Start.i3a",
	"Object/Roulette/Roulette_Animation10_Start.i3a",
	"Object/Roulette/Roulette_Animation11_Start.i3a",
	"Object/Roulette/Roulette_Animation12_Start.i3a",
	"Object/Roulette/Roulette_Animation13_Start.i3a",
	
};



RouletteObject::RouletteObject()
{

	m_rRotationTime = 0.f;
	m_rStateTime =0.f;
	m_nStar =0;
	m_nSlot =-1;

	for(INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
	{
		m_slot[i] = new RS_SLOT_INFO;
		m_slot[i]->Reset();
		
		m_arRouletteIconTexBindAttr[i] = NULL;
		m_arRouletteSlotTexBindAttr[i] = NULL;
	}

	for(INT32 i=0; i<3; i++)
	{
		m_pRouletteWeaponTexture[i] = NULL;
		m_pRouletteCharaMaleTexture[i]= NULL;
		m_pRouletteCharaFemaleTexture[i]= NULL;

	}
	for(INT32 i=0; i<4; i++)
	{
		m_pRouletteSlotGradeTexture[i] = NULL;

	}
	 m_pRouletteBoxTexture =NULL;
	 m_pRouletteItemTexture=NULL;
	 m_pRouletteNotSettingTexture=NULL;

	
	 for(INT32 i=0; i<ROULETTE_COUNT; i++)
	 {
			m_pAnim[ i]= NULL;
	 }
}

/*virtual*/ RouletteObject::~RouletteObject()
{


     for(INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
	{
		if( m_slot[i]) delete m_slot[i];
	}

	for(INT32 i=0; i<ROULETTE_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pAnim[ i]);
	}

	for(INT32 i=0; i<3; i++)
	{
		I3_SAFE_RELEASE( m_pRouletteWeaponTexture[i]);
		I3_SAFE_RELEASE( m_pRouletteCharaMaleTexture[i]);
		I3_SAFE_RELEASE( m_pRouletteCharaFemaleTexture[i]);
		
	}
	for(INT32 i=0; i<4; i++)
	{
		I3_SAFE_RELEASE( m_pRouletteSlotGradeTexture[i]);

	}

	I3_SAFE_RELEASE( m_pRouletteBoxTexture);
	I3_SAFE_RELEASE( m_pRouletteItemTexture);
	I3_SAFE_RELEASE( m_pRouletteNotSettingTexture);

	m_SlotIndex.clear();
	m_WriteData.clear();
	
}
void RouletteObject::InitAnimation()
{
	for( INT32 i = 0; i < ROULETTE_COUNT; i++)
	{
		if( s_AnimName_Roulette[i] != NULL)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_Roulette[i]);
			if( pAnimation == NULL)
			{
				I3WARN("Couldn't load Animation : %s\n", s_AnimName_Roulette[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			pAnimation->Release();  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	m_pSceneObj->BuildBoneList();

	SetRouletteAnimState( ROULETTE_IDLE_CASE_0,0.f);
	SetRouletteState(ROULETTE_STATE_IDLE);


}
//////////////////////////////////////////////////////////////////////////
//	NAME		: InitRoulette()
//	DESCRIPTION : 메트리얼 세팅 및 텍스쳐를 생성합니다.
//////////////////////////////////////////////////////////////////////////

void RouletteObject::InitRoulette()
{
	i3LOD* pLOD = NULL;
	i3Body*pBody = getSceneObject()->GetBody();
	pLOD = pBody->getLOD(0);
	I3ASSERT(pLOD);



	for (INT32 i =0; i <ROULETTE_MAX_SLOT ; i++)
	{
		//아이콘
		char strName[MAX_STRING_COUNT];
		sprintf_s(strName, "Slot_0%d_icon_Material", i + 1);
		m_arRouletteIconTexBindAttr[i] = (i3TextureBindAttr*)CharaTextureUtil::FindTextureBindInAttrSet(pLOD, strName, i3TextureBindAttr::static_meta(), false);
		m_arRouletteIconSet[i] = (i3AttrSet*)i3Scene::FindNodeByName( pLOD->getShapeNode(), strName);
		//슬롯컬러바
		sprintf_s(strName, "Slot_0%d_colorbar_Material", i + 1);
		m_arRouletteSlotTexBindAttr[i] = (i3TextureBindAttr*)CharaTextureUtil::FindTextureBindInAttrSet(pLOD, strName, i3TextureBindAttr::static_meta(), false);
		m_arRouletteSlotSet[i] = (i3AttrSet*)i3Scene::FindNodeByName( pLOD->getShapeNode(), strName);

	}

	m_pRouletteWeaponTexture[0] = g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_weapon_green.i3i");
	m_pRouletteWeaponTexture[1] = g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_weapon_blue.i3i");
	m_pRouletteWeaponTexture[2] = g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_weapon_cherry.i3i");

	m_pRouletteCharaMaleTexture[0]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_man_green.i3i");
	m_pRouletteCharaMaleTexture[1]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_man_blue.i3i");
	m_pRouletteCharaMaleTexture[2]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_man_cherry.i3i");

	m_pRouletteCharaFemaleTexture[0]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_girl_green.i3i");
	m_pRouletteCharaFemaleTexture[1]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_girl_blue.i3i");
	m_pRouletteCharaFemaleTexture[2]= g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_Character_girl_charry.i3i");




	m_pRouletteBoxTexture =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_diff.i3i");
	m_pRouletteItemTexture =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_icon_item_diff.i3i");

	m_pRouletteNotSettingTexture =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Slot_icon_gray.i3i");


	m_pRouletteSlotGradeTexture[0] =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/Slot_colorbar_gray.i3i");
	m_pRouletteSlotGradeTexture[1] =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/ColorBar/Slot_colorbar_green.i3i");
	m_pRouletteSlotGradeTexture[2] =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/ColorBar/Slot_colorbar_blue.i3i");
	m_pRouletteSlotGradeTexture[3] =  g_pFramework->GetResourceManager()->LoadTexture("Object/Roulette/Texture/ColorBar/Slot_colorbar_cherry.i3i");


	m_WriteData.clear();
	m_SlotIndex.clear();
}


void RouletteObject::Create( i3Framework * pFramework, i3GameResObject * pRes)
{
	pRes = (i3GameResObject *) g_pFramework->QuaryResource( "Roulette.i3Obj");
	if( pRes)
	{

		if(m_pAnim[0] ==  NULL)
		{
			InitAnimation();
		}
		i3Object::Create(pFramework, pRes);
	}
	InitRoulette();
	
}

void RouletteObject::ResetObject()
{
	SetRouletteAnimState(ROULETTE_IDLE_CASE_0,0.f);

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: WriteData(INT32 TapIndex)
//	DESCRIPTION : 서버에 보낼 정보를 저장합니다.
//////////////////////////////////////////////////////////////////////////
void	RouletteObject::WriteData(INT32 TapIndex)
{
	
	for (INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
	{
		if(m_slot[i]->eItemGrade != RS_ITEM_GRADE_LOSE	)
		{
			m_WriteData.push_back(i);
		}
	
	}


	g_pGameContext->SetRouletteInfo()->_eTab =(RS_TAB)TapIndex;
	g_pGameContext->SetRouletteInfo()->_ui8ItemCount = 	(UINT8)(m_WriteData.size());

	for (size_t i =0; i<	m_WriteData.size() ; i++)
	{
		g_pGameContext->SetRouletteInfo()->_RSRitemInfo[i]._eItemGrade = m_slot[m_WriteData[i] ]->eItemGrade;
		g_pGameContext->SetRouletteInfo()->_RSRitemInfo[i]._ui8RsIndex = m_slot[m_WriteData[i] ]->nItemIndex;
	}

	for(size_t i =	m_WriteData.size(); i<	ROULETTE_MAX_SLOT ; i++)
	{
		g_pGameContext->SetRouletteInfo()->_RSRitemInfo[i]._eItemGrade = (RS_ITEM_GRADE)0;
		g_pGameContext->SetRouletteInfo()->_RSRitemInfo[i]._ui8RsIndex = 0;
	}
	
	m_WriteData.clear();
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: _FindSlot()
//	DESCRIPTION : 아이템 등급과 인덱스를 검색하여 슬롯번호를 찾습니다.
//////////////////////////////////////////////////////////////////////////
INT32	RouletteObject::_FindSlot(RS_ITEM_GRADE eGrade,INT32 nIndex, INT32 nTest)
{

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if (nTest > 0)
	{
		return nTest-1;
	}
#endif


	// eGrade 가 RS_ITEM_GRADE_JACKPOT이면 잭팟애니메이션 리턴합니다.
	if (eGrade == RS_ITEM_GRADE_JACKPOT)
	{
		return 12;
	}
	// eGrade 가RS_ITEM_GRADE_LOSE가 아니면 검색후 당첨 애니메이션을 리턴합니다.
	for (INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
	{

		if (m_slot[i]->eItemGrade == eGrade && eGrade != RS_ITEM_GRADE_LOSE	)
		{
			if ((m_slot[i]->nItemIndex == nIndex))
			{

				return i;
			}
		}
	}
	//eGrade가 RS_ITEM_GRADE_LOSE일 경우  
	for (INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
	{
		if(m_slot[i]->eItemGrade == RS_ITEM_GRADE_LOSE	)
		{
			m_SlotIndex.push_back(i);
		}
	}
	
	INT32 nTemp;
	INT32 ReturnArg;
	nTemp = i3Math::Rand()%(INT32)m_SlotIndex.size();
	ReturnArg = m_SlotIndex[nTemp];
	//사용한 m_SlotIndex를 비웁니다.
	m_SlotIndex.clear();
	
	return ReturnArg;


}
//////////////////////////////////////////////////////////////////////////
//	NAME		: ConvertItemIdToClassType(UINT32 pitemid)
//	DESCRIPTION : 아이템 등급과 인덱스를 검색하여 슬롯번호를 찾습니다.
//////////////////////////////////////////////////////////////////////////
RS_ITEM_TYPE RouletteObject::ConvertItemIdToClassType(UINT32 pitemid)
{
	INT32 Class = GET_ITEM_FLAG_TYPE(pitemid);
//	INT32 num = GET_ITEM_FLAG_NUMBER( pitemid);

	switch((ITEM_TYPE) Class)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			return RS_WEAPON;
		}
		break;
	case ITEM_TYPE_CHARA:
		{
			CCharaInfo * pInfo = (CCharaInfo*) g_pCharaInfoDataBase->GetInfo( pitemid);
			if( pInfo != NULL)
			{
				I3ASSERT( i3::same_of<CCharaInfo*>(pInfo)); //->IsExactTypeOf( CCharaInfo::static_meta()) );
				if (pInfo->GetCharaSexType() == 0)
				{
					return RS_CHARA_MALE;
				}
				else
				{
					return RS_CHARA_FEMALE;
				}
			}
		}
		break;
	case ITEM_TYPE_HEADGEAR:
		{
			return RS_CHARA_MALE;
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
		{
			return RS_ITEM;
		}
		break;
	default:
		return RS_ITEM_NULL;

	}


		return RS_ITEM_NULL;
}
//////////////////////////////////////////////////////////////////////////
//	NAME		: SetItem()
//	DESCRIPTION : 순차적으로 슬롯에 아이템을 셋팅합니다.
//////////////////////////////////////////////////////////////////////////
void	RouletteObject::SetItem(RS_ITEM_INFO ItemInfo, INT32 TapIndex)
{

	// 유저가 선택할수 있는 마지막 슬롯 수가 10번슬롯이므로 ROULETTE_MAX_SLOT-2 로 세팅합니다.
	for (INT32 i= 0;i<ROULETTE_MAX_SLOT-2;i++)
	{
		if(m_slot[i]->eItemGrade == RS_ITEM_GRADE_NOT_SETTING)
		{
			m_slot[i]->eItemGrade = (RS_ITEM_GRADE)ItemInfo._eItemGrade;
			m_slot[i]->eItemType =ConvertItemIdToClassType(g_pGameContext->ConvertGoodsID(ItemInfo._ui32GoodsID));
			m_slot[i]->nItemIndex = ItemInfo._ui8RsIndex;
			//하드코딩 ㅠㅠ
			if (m_slot[i]->eItemType == RS_WEAPON)
			{
				m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteWeaponTexture[ItemInfo._eItemGrade-1] );
			}
			else if (m_slot[i]->eItemType == RS_CHARA_MALE)
			{
				m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteCharaMaleTexture[ItemInfo._eItemGrade-1] );
			}
			else if (m_slot[i]->eItemType == RS_CHARA_FEMALE)
			{
				m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteCharaFemaleTexture[ItemInfo._eItemGrade-1] );
			}
			else if (m_slot[i]->eItemType ==RS_ITEM)
			{
				m_arRouletteIconTexBindAttr[i]->SetTexture(m_pRouletteItemTexture );
			}

			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[ItemInfo._eItemGrade] );
			SetBoomItem(i,ItemInfo._eItemGrade);
			m_nStar+=ItemInfo._eItemGrade;

			return;
		}
		
	}

	//임시코드입니다. 별등급이 5개가 되면 남은 슬롯을 꽝텍스쳐로 세팅합니다.
	if (_GetStar() == TapIndex)
	{
		for (INT32 i= TapIndex; i<ROULETTE_MAX_SLOT;i++)
		{
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteBoxTexture );
			m_slot[i]->eItemGrade = RS_ITEM_GRADE_LOSE;
		}
	}

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: CheckStar(INT32 nTemp)
//	DESCRIPTION : 슬롯에 맞는 등급을 체크할 수 있도록 체크하는 함수입니다.
//				  가지고 있는 등급과 현재 클릭한 등급의 합이 요구하는 등급을
//				  넘어갈 경우 false를 리턴합니다.
//////////////////////////////////////////////////////////////////////////
bool RouletteObject::CheckStar(INT32 nTemp,INT32 nTemp2)
{
	if (nTemp2<_GetStar()+nTemp)
	{
		return false;
	}
	else
	return true;
}
//////////////////////////////////////////////////////////////////////////
//	NAME		: isCheckRoulette()
//	DESCRIPTION : 룰렛이 다 채워지지 않으면 에러팝업을 오픈합니다.
//////////////////////////////////////////////////////////////////////////
bool RouletteObject::isCheckRoulette()
{

		//현재 슬롯에 RS_ITEM_GRADE_NOT_SETTING 값이 세팅되어 있다면 False를 리턴한다.
		for (INT32 i = 0; i < ROULETTE_MAX_SLOT; i++)
		{
			
			if (m_slot[i]->eItemGrade == RS_ITEM_GRADE_NOT_SETTING	)
			{
				return false;
			}

		}

		return true;
}
//////////////////////////////////////////////////////////////////////////
//	NAME		: InitButton()
//	DESCRIPTION : 초기화 버튼처리입니다.
//////////////////////////////////////////////////////////////////////////

bool RouletteObject::InitButton(INT32 Grade)
{


	

	for (INT32 i= 0; i< Grade; i++)
	{

		m_slot[i]->Reset();
		m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteNotSettingTexture );
		m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[RS_ITEM_GRADE_LOSE] );

	}


	for (INT32 i= Grade; i<ROULETTE_MAX_SLOT; i++)
	{

		m_slot[i]->eItemGrade = RS_ITEM_GRADE_LOSE;
		m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteBoxTexture );
		m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[RS_ITEM_GRADE_LOSE] );

	}

	_SetStar(0);

	return true;

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: Shuffle()
//	DESCRIPTION : 슬롯을 섞습니다.
//////////////////////////////////////////////////////////////////////////
void RouletteObject::Shuffle()
{

	INT32 nArg1,nArg2;

	//100번 swap을 진행합니다.
	for (INT32 i=0;i<100;i++)
	{
		nArg1 = i3Math::Rand()%ROULETTE_MAX_SLOT;
		nArg2 = i3Math::Rand()%ROULETTE_MAX_SLOT;
		std::swap(m_slot[nArg1],m_slot[nArg2]);

	
	}


	//Shuffle된 정보를 기준으로 SetTexture를 진행합니다.
	for (INT32 i=0;i<ROULETTE_MAX_SLOT;i++)
	{

		if (m_slot[i]->eItemGrade == RS_ITEM_GRADE_LOSE )
		{
			 m_slot[i]->eItemType = RS_ITEM_NULL;

		}

		INT32 nTemp = m_slot[i]->eItemType;
		INT32 nGrade =m_slot[i]->eItemGrade;

		switch(nTemp)
		{

		case RS_CHARA_MALE	:
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteCharaMaleTexture[nGrade-1] );
			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[nGrade] );
			break;
		case RS_CHARA_FEMALE	:	
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteCharaFemaleTexture[nGrade-1] );
			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[nGrade] );
			break;
		case RS_WEAPON		:	
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteWeaponTexture[nGrade-1] );
			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[nGrade] );
			break;
		case RS_ITEM			:
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteItemTexture );
			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[nGrade] );

			break;

		default:
			m_arRouletteIconTexBindAttr[i]->SetTexture( m_pRouletteBoxTexture );
			m_arRouletteSlotTexBindAttr[i]->SetTexture( m_pRouletteSlotGradeTexture[RS_ITEM_GRADE_LOSE] );

			break;

		}

	}

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: SetBoomItem()
//	DESCRIPTION : 꽝아이템 설정합니다.
//////////////////////////////////////////////////////////////////////////
void RouletteObject::SetBoomItem(INT32 pindex, INT32 ItemType)
{

	switch(ItemType)
	{
	case RS_ITEM_GRADE_1 :
		return;
		break;
	case RS_ITEM_GRADE_2 :
		m_arRouletteIconTexBindAttr[pindex+1]->SetTexture( m_pRouletteBoxTexture );
		m_slot[pindex+1]->eItemGrade = RS_ITEM_GRADE_LOSE;
		m_slot[pindex+1]->eItemType =	RS_ITEM_NULL;	
		break;
	case RS_ITEM_GRADE_3 :
		m_arRouletteIconTexBindAttr[pindex+1]->SetTexture( m_pRouletteBoxTexture );
		m_slot[pindex+1]->eItemGrade = RS_ITEM_GRADE_LOSE;
		m_slot[pindex+1]->eItemType =	RS_ITEM_NULL;	
		m_arRouletteIconTexBindAttr[pindex+2]->SetTexture( m_pRouletteBoxTexture );
		m_slot[pindex+2]->eItemGrade =RS_ITEM_GRADE_LOSE;
		m_slot[pindex+2]->eItemType =	RS_ITEM_NULL;	

		break;

	}


}
void RouletteObject::ResetObjectOnInit()
{
	/*i3Stage * pStage = g_pFramework->GetCurrentStage();
	I3ASSERT(pStage);*/
	
}

void RouletteObject::SetRouletteState(ROULETTE_STATE eState)
{
		m_eRouletteState = eState;

}
void RouletteObject::SetRouletteAnimState(ROULETTE_ANIM_STATE eState,REAL32 rStartTime)
{
	m_rStateTime = 0.f;
	m_eRouletteAnimState = eState;
	m_pSceneObj->OnChangeTime(0.0f);
	m_pSceneObj->PlayAnim( m_pAnim[ m_eRouletteAnimState],I3_ANIM_BLEND_MODE_BLEND,rStartTime, 0, 1.f, I3_ANIM_PLAY_MODE_REPEAT, I3_ANIM_PLAY_MODE_CLAMP);
	
}
bool RouletteObject::IsRouletteIdleState()
{

	switch(m_eRouletteAnimState)
	{
	case ROULETTE_IDLE_CASE_0:
	case ROULETTE_IDLE_CASE_1:
	case ROULETTE_IDLE_CASE_2:
	case ROULETTE_IDLE_CASE_3:
	case ROULETTE_IDLE_CASE_4:
	case ROULETTE_IDLE_CASE_5:
	case ROULETTE_IDLE_CASE_6:
	case ROULETTE_IDLE_CASE_7:
	case ROULETTE_IDLE_CASE_8:
	case ROULETTE_IDLE_CASE_9:
	case ROULETTE_IDLE_CASE_10:
	case ROULETTE_IDLE_CASE_11:
	case ROULETTE_IDLE_CASE_12:

		return true;
			break;
	default:

		return false;
	}
}


void RouletteObject::OnUpdate( REAL32 tm)
{
	i3Object::OnUpdate(tm);

	
	switch(m_eRouletteAnimState)
	{

	case ROULETTE_END_CASE_0:
	case ROULETTE_END_CASE_1:
	case ROULETTE_END_CASE_2:
	case ROULETTE_END_CASE_3:
	case ROULETTE_END_CASE_4:
	case ROULETTE_END_CASE_5:
	case ROULETTE_END_CASE_6:
	case ROULETTE_END_CASE_7:
	case ROULETTE_END_CASE_8:
	case ROULETTE_END_CASE_9:
	case ROULETTE_END_CASE_10:
	case ROULETTE_END_CASE_11:
	case ROULETTE_END_CASE_12:
		{
			REAL32 rDuration = m_pAnim[ m_eRouletteAnimState]->GetDuration();
			
			if(  m_rStateTime + tm >= rDuration -0.1f )
			{
			
				UINT32 nTemp;
				nTemp  =  (UINT32)m_eRouletteAnimState +13;
				SetRouletteAnimState((ROULETTE_ANIM_STATE)nTemp,0.f);
				SetRouletteState(ROULETTE_STATE_FINISH);
			}
		}



		break;
	case ROULETTE_IDLE_CASE_0:
	case ROULETTE_IDLE_CASE_1:
	case ROULETTE_IDLE_CASE_2:
	case ROULETTE_IDLE_CASE_3:
	case ROULETTE_IDLE_CASE_4:
	case ROULETTE_IDLE_CASE_5:
	case ROULETTE_IDLE_CASE_6:
	case ROULETTE_IDLE_CASE_7:
	case ROULETTE_IDLE_CASE_8:
	case ROULETTE_IDLE_CASE_9:
	case ROULETTE_IDLE_CASE_10:
	case ROULETTE_IDLE_CASE_11:
	case ROULETTE_IDLE_CASE_12:
	{
	
		SetRouletteState(ROULETTE_STATE_IDLE);
		//RESULT_ACK받앗다면
		if(g_pGameContext->getRecvRouletteStartPacket())
		{
			I3TRACE("룰렛 ACK 패킷 받음\n");
			SetRouletteState(ROULETTE_STATE_START);
			
			VEC3D vPos;
			i3Vector::Set( &vPos,1.f,1.f,0.f);
			g_pSndMng->StartRouletteSnd(GSND_ROULETTE_START,&vPos,false);
			UINT32 nTemp;
			nTemp  =  (UINT32)m_eRouletteAnimState +13;
			SetRouletteAnimState((ROULETTE_ANIM_STATE)nTemp,0.f);
			g_pGameContext->SetRecvRouletteStartPacket(false);
			
			//슬롯을 찾습니다.
			_SetSlot(_FindSlot((RS_ITEM_GRADE)g_pGameContext->GetRSItemGrade(),g_pGameContext->GetRSIndex(),g_pGameContext->GetRSTestCode()));
			
		}

			
	}
		break;

	case ROULETTE_START_CASE_0:
	case ROULETTE_START_CASE_1:
	case ROULETTE_START_CASE_2:
	case ROULETTE_START_CASE_3:
	case ROULETTE_START_CASE_4:
	case ROULETTE_START_CASE_5:
	case ROULETTE_START_CASE_6:
	case ROULETTE_START_CASE_7:
	case ROULETTE_START_CASE_8:
	case ROULETTE_START_CASE_9:
	case ROULETTE_START_CASE_10:
	case ROULETTE_START_CASE_11:
	case ROULETTE_START_CASE_12:
		{

			REAL32 rDuration = m_pAnim[ m_eRouletteAnimState]->GetDuration();
			if( m_pSceneObj->GetBody()->getPlayTime(0) + tm > rDuration)
			{
				SetRouletteAnimState((ROULETTE_ANIM_STATE)_GetSlot(),0.1f - tm);

				VEC3D vPos;
				i3Vector::Set( &vPos,1.f,1.f,0.f);
				g_pSndMng->StartRouletteSnd((GSND_ROULETTE_TYPE)(_GetSlot()+7),&vPos,false);
				
			}
	

		}
		break;
	default:
		{
			I3ASSERT_0;
		}
		break;
	}

	m_rStateTime += tm;
	
}

