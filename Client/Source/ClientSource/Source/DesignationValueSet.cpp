#include "pch.h"
#include "GlobalVariables.h"
#include "DesignationValueSet.h"



#define			FILE_NAME_DESIGNATION_VALUE		"Config/DesignationValue.pef"

CDesignationValueSet * g_pDesignationValueSet = NULL;

I3_CLASS_INSTANCE( CDesignationValueSet);

CDesignationValueSet::CDesignationValueSet(void)
{
	g_pDesignationValueSet = this;

	m_encryptShiftCode = (UINT8)((i3Math::Rand() % 7) + 1);

	// 초기값을 설정합니다.
	InitDefault();

	// 저장된값을 로드합니다.
	Load();
}

CDesignationValueSet::~CDesignationValueSet(void)
{

}

REAL32 CDesignationValueSet::getValue( INT32 id)
{
	if( id > -1 && id < DESIGNATION_ID_COUNT)
	{
		return DECRYPT_REALVALUE( m_fValue[id], m_encryptShiftCode);
	}
	else
		return 0.f;
}

void CDesignationValueSet::_setValue( INT32 id, REAL32 val)
{
	I3_BOUNDCHK( id, DESIGNATION_ID_COUNT);

	GlobalFunc::EncryptValue( &val, m_encryptShiftCode);

	m_fValue[id] = val;
}

void CDesignationValueSet::_setUserTitleEffectText( DESIGNATION_ID id, const char * pszIn)
{
	INT32 i;

	I3_BOUNDCHK( id, DESIGNATION_ID_COUNT);

	INT32 len = i3::generic_string_size( pszIn);

	m_UserTitleEffectText[ id]._Id = (UINT8) ((i3Math::Rand() % 7) + 1);

	if( len > 0)
	{
		i3::string_ncopy_nullpad( m_UserTitleEffectText[ id]._Text, pszIn, len);

		GlobalFunc::EncryptValue( m_UserTitleEffectText[ id]._Text, len, m_UserTitleEffectText[ id]._Id);

		for( i = len + 2; i < sizeof( m_UserTitleEffectText[ id]._Text); i++)
		{
			m_UserTitleEffectText[ id]._Text[ i] = (char) (i3Math::Rand() % 8);
		}
	}
	else
		m_UserTitleEffectText[ id]._Text[0] = '\0';
}

void CDesignationValueSet::getUserTitleEffectText( DESIGNATION_ID id, char * pszOut)
{
	if( id < 0 || id >= DESIGNATION_ID_COUNT)
	{
		pszOut[0] = '\0';
	}
	else
	{
		INT32 len = i3::generic_string_size( m_UserTitleEffectText[ id]._Text);

		if( len > 0)
		{
			DECRYPT_STRING( pszOut, m_UserTitleEffectText[ id]._Text, len, m_UserTitleEffectText[ id]._Id);
		}
		else
			pszOut[0] = '\0';
	}
}


void CDesignationValueSet::InitDefault(void)
{
	INT32 i;

	m_fValue[DESIGNATION_ID_NONE] = 0.0f;

	for( i = 1; i < DESIGNATION_ID_COUNT; i++)
	{
		_setValue( (DESIGNATION_ID) i, 0.1f);
	}

	__InitUserTitleEffectText();

	for( i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
		m_tDesignationSet[ i]._ShiftCode = (UINT8)((i3Math::Rand() % 7) + 1);

	// A Group
	m_tDesignationSet[0].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO0"));/*일반 대원*/
	m_tDesignationSet[0].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO0"));/*Member*/
	m_tDesignationSet[0].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO0"));/*최초로 부여 받는 기본적인 호칭으로 가장 낮은 등급의 호칭*/
	m_tDesignationSet[0].setType( DESIGNATION_TYPE_1);

	m_tDesignationSet[1].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO1"));/*선임 대원*/
	m_tDesignationSet[1].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO1"));/*Junior*/
	m_tDesignationSet[1].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO1"));/*어느 정도 전투경험을 쌓은 대원에게 부여하는 호칭*/
	m_tDesignationSet[1].setType( DESIGNATION_TYPE_2);

	m_tDesignationSet[2].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO2"));/*특등 대원*/
	m_tDesignationSet[2].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO2"));/*Veteran*/
	m_tDesignationSet[2].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO2"));/*노련한 전투 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[2].setType( DESIGNATION_TYPE_3);

	m_tDesignationSet[3].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO3"));/*특수 대원*/
	m_tDesignationSet[3].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO3"));/*Specialist*/
	m_tDesignationSet[3].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO3"));/*탁월한 전투 능력을 가진 대원에게 부여하는 호칭*/
	m_tDesignationSet[3].setType( DESIGNATION_TYPE_4);

	// B Group
	m_tDesignationSet[4].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO4"));/*돌격 부대원*/
	m_tDesignationSet[4].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO4"));/*Assult Soldier*/
	m_tDesignationSet[4].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO4"));/*돌격 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[4].setType( DESIGNATION_TYPE_5);

	m_tDesignationSet[5].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO5"));/*저격 부대원*/
	m_tDesignationSet[5].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO5"));/*Backup Soldier*/
	m_tDesignationSet[5].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO5"));/*저격 계열 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[5].setType( DESIGNATION_TYPE_6);

	m_tDesignationSet[6].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO6"));/*습격 부대원*/
	m_tDesignationSet[6].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO6"));/*Assail Soldier*/
	m_tDesignationSet[6].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO6"));/*습격 계열 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[6].setType( DESIGNATION_TYPE_7);

	// C Group
	m_tDesignationSet[7].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO7"));/*일반 돌격대원*/
	m_tDesignationSet[7].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO7"));/*Assaulter*/
	m_tDesignationSet[7].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO7"));/*선두에서 돌격의 임무를  수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[7].setType( DESIGNATION_TYPE_8);

	m_tDesignationSet[14].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO14"));/*선임 돌격대원*/
	m_tDesignationSet[14].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO14"));/*Junior Assaulter*/
	m_tDesignationSet[14].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO14"));/*두려움을 모르는 용맹한 돌격대원에게 부여하는 호칭*/
	m_tDesignationSet[14].setType( DESIGNATION_TYPE_9);

	m_tDesignationSet[21].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO21"));/*수석 돌격대원*/
	m_tDesignationSet[21].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO21"));/*Senior Assaulter*/
	m_tDesignationSet[21].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO21"));/*수많은 돌격임무를 완수한 대원에게 부여하는 호칭*/
	m_tDesignationSet[21].setType( DESIGNATION_TYPE_10);

	m_tDesignationSet[28].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO28"));/*특등 돌격장교*/
	m_tDesignationSet[28].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO28"));/*Veteran Assult Officer*/
	m_tDesignationSet[28].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO28"));/*오직 앞으로 나아갈 줄 아는 용맹한 대원에게 부여하는 호칭*/
	m_tDesignationSet[28].setType( DESIGNATION_TYPE_11);

	m_tDesignationSet[32].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO32"));/*돌격 지휘관*/
	m_tDesignationSet[32].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO32"));/*Assault Commander*/
	m_tDesignationSet[32].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO32"));/*돌격 임무로 전투의 승리를 안겨 주는 빛 같은 존재*/
	m_tDesignationSet[32].setType( DESIGNATION_TYPE_12);

	m_tDesignationSet[38].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO38"));/*광폭한 맹수*/
	m_tDesignationSet[38].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO38"));/*Rowdy Beast*/
	m_tDesignationSet[38].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO38"));/*빛 보다 빠르고 폭풍보다 강한 자*/
	m_tDesignationSet[38].setType( DESIGNATION_TYPE_13);

	// E Group
	m_tDesignationSet[9].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO9"));/*일반 저격병*/
	m_tDesignationSet[9].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO9"));/*Sniper*/
	m_tDesignationSet[9].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO9"));/*저격임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[9].setType( DESIGNATION_TYPE_14);

	m_tDesignationSet[16].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO16"));/*선임 저격병*/
	m_tDesignationSet[16].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO16"));/*Junior Sniper*/
	m_tDesignationSet[16].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO16"));/*높은 명중률을 보이는 대원에게 부여하는 호칭*/
	m_tDesignationSet[16].setType( DESIGNATION_TYPE_15);

	m_tDesignationSet[23].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO23"));/*수석 저격병*/
	m_tDesignationSet[23].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO23"));/*Senior Sniper*/
	m_tDesignationSet[23].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO23"));/*언제 어디서나 목표를 쓰러트리는 자에게 부여하는 호칭*/
	m_tDesignationSet[23].setType( DESIGNATION_TYPE_16);

	m_tDesignationSet[29].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO29"));/*특등 저격장교*/
	m_tDesignationSet[29].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO29"));/*Veteran Sniper*/
	m_tDesignationSet[29].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO29"));/*한치의 오차도 없는 사격술을 가진 자에게 부여하는 호칭 */
	m_tDesignationSet[29].setType( DESIGNATION_TYPE_17);

	m_tDesignationSet[34].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO34"));/*저격 지휘관*/
	m_tDesignationSet[34].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO34"));/*Snipe Commander*/
	m_tDesignationSet[34].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO34"));/*적에게 죽음을 선사하는 보이지 않는 존재*/
	m_tDesignationSet[34].setType( DESIGNATION_TYPE_18);

	m_tDesignationSet[39].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO39"));/*은밀한 암살자*/
	m_tDesignationSet[39].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO39"));/*Still Assassin*/
	m_tDesignationSet[39].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO39"));/*칼날같이 예리한 신의 정확도를 가진 자*/
	m_tDesignationSet[39].setType( DESIGNATION_TYPE_19);

	// G Group
	m_tDesignationSet[11].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO11"));/*일반 습격대원*/
	m_tDesignationSet[11].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO11"));/*Assailant*/
	m_tDesignationSet[11].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO11"));/*습격 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[11].setType( DESIGNATION_TYPE_20);

	m_tDesignationSet[18].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO18"));/*선임 습격대원*/
	m_tDesignationSet[18].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO18"));/*Junior Assailant*/
	m_tDesignationSet[18].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO18"));/*노련한 움직임으로 습격 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[18].setType( DESIGNATION_TYPE_21);

	m_tDesignationSet[25].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO25"));/*수석 습격대원*/
	m_tDesignationSet[25].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO25"));/*Senior Assailant*/
	m_tDesignationSet[25].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO25"));/*수많은 습격임무를 완수한 대원에게 부여하는 호칭*/
	m_tDesignationSet[25].setType( DESIGNATION_TYPE_22);

	m_tDesignationSet[30].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO30"));/*특등 습격장교*/
	m_tDesignationSet[30].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO30"));/*Veteran Assail Officer*/
	m_tDesignationSet[30].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO30"));/*습격 임무로 혁혁한 공을 세운 대원에게 부여하는 호칭*/
	m_tDesignationSet[30].setType( DESIGNATION_TYPE_23);

	m_tDesignationSet[36].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO36"));/*습격 지휘관*/
	m_tDesignationSet[36].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO36"));/*Assail Commander*/
	m_tDesignationSet[36].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO36"));/*평온한 곳에서 벼락같은 죽음을 선사하는 존재*/
	m_tDesignationSet[36].setType(  DESIGNATION_TYPE_24);

	m_tDesignationSet[41].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO41"));/*교활한 침입자*/
	m_tDesignationSet[41].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO41"));/*Nimble Sneaker*/
	m_tDesignationSet[41].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO41"));/*천둥처럼 빠르게 달려들어 모든 것을 죽음으로 바꿔 놓는 자*/
	m_tDesignationSet[41].setType( DESIGNATION_TYPE_25);

    // D Group
	m_tDesignationSet[8].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO8"));/*일반 침투병*/
	m_tDesignationSet[8].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO8"));/*Infiltrator*/
	m_tDesignationSet[8].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO8"));/*침투 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[8].setType( DESIGNATION_TYPE_26);

	m_tDesignationSet[15].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO15"));/*수석 침투병*/
	m_tDesignationSet[15].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO15"));/*Senior Infiltrator*/
	m_tDesignationSet[15].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO15"));/*수많은 침투 경험을 가진 노련한 대원에게 부여하는 호칭*/
	m_tDesignationSet[15].setType( DESIGNATION_TYPE_27);

	m_tDesignationSet[22].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO22"));/*특등 침투장교*/
	m_tDesignationSet[22].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO22"));/*Veteran Infiltrator Officer*/
	m_tDesignationSet[22].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO22"));/*소리 없이 빠르고 강하게 침투하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[22].setType( DESIGNATION_TYPE_28);

	m_tDesignationSet[33].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO33"));/*침투 지휘관*/
	m_tDesignationSet[33].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO33"));/*Infiltrator Commander*/
	m_tDesignationSet[33].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO33"));/*귀신같이 스며들어 모든 것을 앗아가는 존재*/
	m_tDesignationSet[33].setType( DESIGNATION_TYPE_29);



	// F Group
	m_tDesignationSet[10].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO10"));/*일반사수*/
	m_tDesignationSet[10].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO10"));/*Gunner*/
	m_tDesignationSet[10].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO10"));/*사수 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[10].setType( DESIGNATION_TYPE_30);

	m_tDesignationSet[17].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO17"));/*수석사수*/
	m_tDesignationSet[17].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO17"));/*Senior Gunner*/
	m_tDesignationSet[17].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO17"));/*노련한 사격술을 가진 대원에게 부여하는 호칭*/
	m_tDesignationSet[17].setType( DESIGNATION_TYPE_31);

	m_tDesignationSet[24].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO24"));/*특등 사수장교*/
	m_tDesignationSet[24].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO24"));/*Veteran Gun Officer*/
	m_tDesignationSet[24].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO24"));/*전장의 기둥역할을 하는 사수에게 부여하는 호칭*/
	m_tDesignationSet[24].setType( DESIGNATION_TYPE_32);

	m_tDesignationSet[35].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO35"));/*사격 지휘관*/
	m_tDesignationSet[35].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO35"));/*Shot Commander*/
	m_tDesignationSet[35].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO35"));/*전장의 불꽃을 만들어내는  존재*/
	m_tDesignationSet[35].setType( DESIGNATION_TYPE_33);

	m_tDesignationSet[40].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO40"));/*예리한 명사수*/
	m_tDesignationSet[40].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO40"));/*Shooting Star*/
	m_tDesignationSet[40].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO40"));/*총구의 불꽃으로 전장의 피비린내를 풍기는 자*/
	m_tDesignationSet[40].setType( DESIGNATION_TYPE_34);


	// H Group
	m_tDesignationSet[12].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO12"));/*일반 타격대원*/
	m_tDesignationSet[12].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO12"));/*Blower*/
	m_tDesignationSet[12].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO12"));/*타격 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[12].setType( DESIGNATION_TYPE_35);

	m_tDesignationSet[19].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO19"));/*수석 타격대원*/
	m_tDesignationSet[19].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO19"));/*Senior Blower*/
	m_tDesignationSet[19].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO19"));/*타격 임무에 월등한 능력을 가진 대원에게 부여하는 호칭 */
	m_tDesignationSet[19].setType( DESIGNATION_TYPE_36);

	m_tDesignationSet[26].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO26"));/*특등 타격장교*/
	m_tDesignationSet[26].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO26"));/*Veteran Blower*/
	m_tDesignationSet[26].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO26"));/*두려움 없는 불도저 같은 대원에게 부여하는 호칭*/
	m_tDesignationSet[26].setType( DESIGNATION_TYPE_37);

	m_tDesignationSet[37].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO37"));/*타격 지휘관*/
	m_tDesignationSet[37].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO37"));/*Blow Commander*/
	m_tDesignationSet[37].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO37"));/*강한 타격의 선율을 선사하는 공포의 존재*/
	m_tDesignationSet[37].setType( DESIGNATION_TYPE_38);

	m_tDesignationSet[42].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO42"));/*난폭한 파괴자*/
	m_tDesignationSet[42].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO42"));/*Supreme Buster*/
	m_tDesignationSet[42].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO42"));/*폭풍같이 강력한 타격으로 영혼조차 잃게  만드는 자*/
	m_tDesignationSet[42].setType( DESIGNATION_TYPE_39);

	// I Group
	m_tDesignationSet[13].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO13"));/*일반 폭파대원*/
	m_tDesignationSet[13].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO13"));/*Blaster*/
	m_tDesignationSet[13].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO13"));/*폭파 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[13].setType( DESIGNATION_TYPE_40);

	m_tDesignationSet[20].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO20"));/*선임 폭파대원*/
	m_tDesignationSet[20].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO20"));/*Junior Blaster*/
	m_tDesignationSet[20].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO20"));/*빠르고 안정적으로 폭파 임무를 수행하는 대원에게 부여하는 호칭*/
	m_tDesignationSet[20].setType( DESIGNATION_TYPE_41);

	m_tDesignationSet[27].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO27"));/*수석 폭파대원*/
	m_tDesignationSet[27].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO27"));/*Senior Blaster*/
	m_tDesignationSet[27].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO27"));/*수많은 폭파 경험을 가진 대원에게 부여하는 호칭*/
	m_tDesignationSet[27].setType( DESIGNATION_TYPE_42);

	m_tDesignationSet[31].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO31"));/*특등 폭파장교*/
	m_tDesignationSet[31].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO31"));/*Veteran Blaster*/
	m_tDesignationSet[31].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO31"));/*폭파기술의 천부적인 재능을 가진 대원에게 부여하는 호칭*/
	m_tDesignationSet[31].setType( DESIGNATION_TYPE_43);

	m_tDesignationSet[43].setName( GAME_STRING("STR_TBL_DESIGNATION_NAME_NO43"));/*폭파 지휘관*/
	m_tDesignationSet[43].setName_E( GAME_STRING("STR_TBL_DESIGNATION_NAME2_NO43"));/*Blast Commander*/
	m_tDesignationSet[43].setExplain( GAME_STRING("STR_TBL_DESIGNATION_EXPLAIN_NO43"));/* 악마의 검은 구름을 몰고 다니는 종말의 창시자*/
	m_tDesignationSet[43].setType( DESIGNATION_TYPE_44);

}

void CDesignationValueSet::Save(void)
{
	char szTableName[ MAX_STRING_COUNT];
	i3RegistrySet * pDesignationReg = i3RegistrySet::new_object();

	for( INT32 i = 0; i < DESIGNATION_ABILITY_COUNT; i++)
	{
		// Plus Effect
		sprintf_s( szTableName, "Move_Ability_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_MOVE_ABILITY_PLUS_1 + i));

		sprintf_s( szTableName, "Fire_Speed_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRE_SPEED_PLUS_1 + i));

		sprintf_s( szTableName, "Move_Speed_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_MOVE_SPEED_PLUS_1 + i));

		sprintf_s( szTableName, "AddSpeed_Ability_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_1 + i));

		sprintf_s( szTableName, "FallDown_Damage_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_1 + i));

		sprintf_s( szTableName, "Reload_Time_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_RELOAD_TIME_REDUCE_1 + i));

		sprintf_s( szTableName, "Fire_Range_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRE_RANGE_PLUS_1 + i));

		sprintf_s( szTableName, "Horizontal_Reaction_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1 + i));

		sprintf_s( szTableName, "Vertical_Reaction_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1 + i));

		sprintf_s( szTableName, "Target_Declnation_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1 + i));

		sprintf_s( szTableName, "Throw_Speed_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_THROW_SPEED_PLUS_1 + i));

		sprintf_s( szTableName, "Bomb_Time_Reduce_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_BOMB_TIME_REDUCE_1 + i));

		sprintf_s( szTableName, "Bomb_Range_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_BOMB_RANGE_PLUS_1 + i));

		sprintf_s( szTableName, "Jump_Ability_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_JUMP_ABILITY_PLUS_1 + i));

		sprintf_s( szTableName, "Weapon_Reaction_Control_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_1 + i));

		sprintf_s( szTableName, "DPD_Rate_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_DPD_RATE_PLUS_1 + i));

		sprintf_s( szTableName, "First_Shot_Rate_Plus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRST_SHOT_PLUS_1 + i));


		// Minus Effect
		sprintf_s( szTableName, "FallDown_Damage_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_1 + i));

		sprintf_s( szTableName, "AddSpeed_Ability_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_1 + i));

		sprintf_s( szTableName, "Jump_Ability_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_JUMP_ABILITY_MINUS_1 + i));

		sprintf_s( szTableName, "Move_Speed_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_MOVE_SPEED_MINUS_1 + i));

		sprintf_s( szTableName, "Vertical_Reaction_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1 + i));

		sprintf_s( szTableName, "Horizontal_Reaction_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_1 + i));

		sprintf_s( szTableName, "Target_Declination_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1 + i));

		sprintf_s( szTableName, "Reloae_Time_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_RELOAD_TIME_INCREASE_1 + i));

		sprintf_s( szTableName, "Fire_Range_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRE_RANGE_MINUS_1 + i));

		sprintf_s( szTableName, "Throw_Speed_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_THROW_SPEED_MINUS_1 + i));

		sprintf_s( szTableName, "Fire_Speed_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRE_SPEED_MINUS_1 + i));

		sprintf_s( szTableName, "Bomb_Time_Increase_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_BOMB_TIME_INCREASE_1 + i));

		sprintf_s( szTableName, "Bomb_Range_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_BOMB_RANGE_MINUS_1 + i));

		sprintf_s( szTableName, "Weapon_Reaction_Control_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_1 + i));

		sprintf_s( szTableName, "DPD_Rate_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_DPD_RATE_MINUS_1 + i));

		sprintf_s( szTableName, "First_Shot_Rate_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_FIRST_SHOT_MINUS_1 + i));

		sprintf_s( szTableName, "Move_Ability_Minus_%d", i+1);
		pDesignationReg->getRoot()->AddData( szTableName, getValue( DESIGNATION_ID_MOVE_ABILITY_MINUS_1 + i));
	}


	char FilePath[MAX_PATH] = {};
	ConvertLocalePath(FilePath, FILE_NAME_DESIGNATION_VALUE, LOCALE_PATH_CONFIG);
	pDesignationReg->SaveRegistrySet(FilePath);

	I3_SAFE_RELEASE( pDesignationReg);
}

void CDesignationValueSet::Load(void)
{
	REAL32 rVal;
	char szTableName[ MAX_STRING_COUNT];

	i3RegKey * pKey = Config::_GetPefFileRegKey("DesignationValue");
	if( pKey != NULL)
	{
		for( INT32 i = 0; i < DESIGNATION_ABILITY_COUNT; i++)
		{
			// Plus Effect
			sprintf_s( szTableName, "Move_Ability_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_MOVE_ABILITY_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Fire_Speed_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRE_SPEED_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Move_Speed_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_MOVE_SPEED_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "AddSpeed_Ability_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "FallDown_Damage_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Reload_Time_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_RELOAD_TIME_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Fire_Range_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRE_RANGE_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Horizontal_Reaction_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Vertical_Reaction_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Target_Declnation_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Throw_Speed_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_THROW_SPEED_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Bomb_Time_Reduce_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_BOMB_TIME_REDUCE_1 + i, rVal);

			sprintf_s( szTableName, "Bomb_Range_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_BOMB_RANGE_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Jump_Ability_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_JUMP_ABILITY_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "Weapon_Reaction_Control_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "DPD_Rate_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_DPD_RATE_PLUS_1 + i, rVal);

			sprintf_s( szTableName, "First_Shot_Rate_Plus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRST_SHOT_PLUS_1 + i, rVal);

			// Minus Effect
			sprintf_s( szTableName, "FallDown_Damage_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "AddSpeed_Ability_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Jump_Ability_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_JUMP_ABILITY_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Move_Speed_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_MOVE_SPEED_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Vertical_Reaction_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "Horizontal_Reaction_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "Target_Declination_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "Reload_Time_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_RELOAD_TIME_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "Fire_Range_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRE_RANGE_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Throw_Speed_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_THROW_SPEED_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Fire_Speed_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRE_SPEED_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Bomb_Time_Increase_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_BOMB_TIME_INCREASE_1 + i, rVal);

			sprintf_s( szTableName, "Bomb_Range_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_BOMB_RANGE_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Weapon_Reaction_Control_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "DPD_Rate_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_DPD_RATE_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "First_Shot_Rate_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_FIRST_SHOT_MINUS_1 + i, rVal);

			sprintf_s( szTableName, "Move_Ability_Minus_%d", i+1);
			GetKeyData(pKey, szTableName, &rVal);
			_setValue( DESIGNATION_ID_MOVE_ABILITY_MINUS_1 + i, rVal);
		}


		i3RegKey * pDesignationSet = Config::FindKey( "DesignationSet", pKey);
		for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
		{
			sprintf_s( szTableName, "Set_%d", i + 1);
			i3RegKey * pSet = Config::FindKey( szTableName, pDesignationSet);
			if( pSet != NULL)
			{
				INT32 value = 0;

				GetKeyData(pSet, "_ApplyType", &value);
				m_tDesignationSet[ i].setApplyType( (DESIGNATION_APPLY_TYPE) value);

				GetKeyData(pSet, "_Type", &value);
				m_tDesignationSet[ i].setType( (DESIGNATION_TYPE) value);

				for( INT32 j = 0; j < DESIGNATION_EFFECT_COUNT; j++)
				{
					sprintf_s( szTableName, "_PlusEffect_%d", j);
					GetKeyData(pSet, szTableName, &value);
					m_tDesignationSet[ i].setPlusEffect( j, (UINT8) value);

					sprintf_s( szTableName, "_MinusEffect_%d", j);
					GetKeyData(pSet, szTableName, &value);
					m_tDesignationSet[ i].setMinusEffect( j, (UINT8) value);

					sprintf_s( szTableName, "_AddEffect_%d", j);
					GetKeyData(pSet, szTableName, &value);
					m_tDesignationSet[ i].setAddEffect( j, (UINT8) value);

					value = 0;
					sprintf_s( szTableName, "_AuthWeaponClass_%d", j);
					GetKeyData(pSet, szTableName, &value);
					m_tDesignationSet[ i].setAuthWeaponClass( j, (UINT8) value);

					value = 0;
					sprintf_s( szTableName, "_AuthWeaponNumber_%d", j);
					GetKeyData(pSet, szTableName, &value);
					m_tDesignationSet[ i].setAuthWeaponNumber( j, (UINT8) value);
				}
			}
		}

	}

//	I3_SAFE_RELEASE( pDesignationReg);
}

void CDesignationValueSet::__InitUserTitleEffectText( void)
{
	// 호칭 속성
	_setUserTitleEffectText( DESIGNATION_ID_NONE, "");

	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_PLUS_1, GAME_STRING("STR_HEADER_SPEED_PLUS_1"));					/*이동 속도 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_PLUS_2, GAME_STRING("STR_HEADER_SPEED_PLUS_2"));					/*이동 속도 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_PLUS_3, GAME_STRING("STR_HEADER_SPEED_PLUS_3"));					/*이동 속도 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_PLUS_4, GAME_STRING("STR_HEADER_SPEED_PLUS_4"));					/*이동 속도 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_PLUS_5, GAME_STRING("STR_HEADER_SPEED_PLUS_5"));					/*이동 속도 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_PLUS_1, GAME_STRING("STR_HEADER_FIRE_SPEED_PLUS_1"));				/*발사 속도 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_PLUS_2, GAME_STRING("STR_HEADER_FIRE_SPEED_PLUS_2"));				/*발사 속도 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_PLUS_3, GAME_STRING("STR_HEADER_FIRE_SPEED_PLUS_3"));				/*발사 속도 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_PLUS_4, GAME_STRING("STR_HEADER_FIRE_SPEED_PLUS_4"));				/*발사 속도 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_PLUS_5, GAME_STRING("STR_HEADER_FIRE_SPEED_PLUS_5"));				/*발사 속도 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_PLUS_1, GAME_STRING("STR_HEADER_SPEED_PLUS_1"));					/*이동 속도 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_PLUS_2, GAME_STRING("STR_HEADER_SPEED_PLUS_2"));					/*이동 속도 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_PLUS_3, GAME_STRING("STR_HEADER_SPEED_PLUS_3"));					/*이동 속도 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_PLUS_4, GAME_STRING("STR_HEADER_SPEED_PLUS_4"));					/*이동 속도 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_PLUS_5, GAME_STRING("STR_HEADER_SPEED_PLUS_5"));					/*이동 속도 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_PLUS_1, GAME_STRING("STR_HEADER_JUMP_PLUS_1"));					/*점프 능력 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_PLUS_2, GAME_STRING("STR_HEADER_JUMP_PLUS_2"));					/*점프 능력 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_PLUS_3, GAME_STRING("STR_HEADER_JUMP_PLUS_3"));					/*점프 능력 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_PLUS_4, GAME_STRING("STR_HEADER_JUMP_PLUS_4"));					/*점프 능력 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_PLUS_5, GAME_STRING("STR_HEADER_JUMP_PLUS_5"));					/*점프 능력 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_1, GAME_STRING("STR_HEADER_AGILITY_PLUS_1"));				/*움직임 능력 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_2, GAME_STRING("STR_HEADER_AGILITY_PLUS_2"));				/*움직임 능력 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_3, GAME_STRING("STR_HEADER_AGILITY_PLUS_3"));				/*움직임 능력 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_4, GAME_STRING("STR_HEADER_AGILITY_PLUS_4"));				/*움직임 능력 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_PLUS_5, GAME_STRING("STR_HEADER_AGILITY_PLUS_5"));				/*움직임 능력 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_1, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_REDUCE_1"));		/*낙하 데미지 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_2, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_REDUCE_2"));		/*낙하 데미지 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_3, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_REDUCE_3"));		/*낙하 데미지 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_4, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_REDUCE_4"));		/*낙하 데미지 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_REDUCE_5, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_REDUCE_5"));		/*낙하 데미지 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_REDUCE_1, GAME_STRING("STR_HEADER_RELOAD_PLUS_1"));					/*재장전 속도 +1 {col:240, 30, 40, 255}(베레모 착용시){/col}*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_REDUCE_2, GAME_STRING("STR_HEADER_RELOAD_PLUS_2"));					/*재장전 속도 +2 {col:240, 30, 40, 255}(베레모 착용시){/col}*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_REDUCE_3, GAME_STRING("STR_HEADER_RELOAD_PLUS_3"));					/*재장전 속도 +3 {col:240, 30, 40, 255}(베레모 착용시){/col}*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_REDUCE_4, GAME_STRING("STR_HEADER_RELOAD_PLUS_4"));					/*재장전 속도 +4 {col:240, 30, 40, 255}(베레모 착용시){/col}*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_REDUCE_5, GAME_STRING("STR_HEADER_RELOAD_PLUS_5"));					/*재장전 속도 +5 {col:240, 30, 40, 255}(베레모 착용시){/col}*/

	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_PLUS_1, GAME_STRING("STR_HEADER_RANGE_PLUS_1"));					/*사정거리 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_PLUS_2, GAME_STRING("STR_HEADER_RANGE_PLUS_2"));					/*사정거리 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_PLUS_3, GAME_STRING("STR_HEADER_RANGE_PLUS_3"));					/*사정거리 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_PLUS_4, GAME_STRING("STR_HEADER_RANGE_PLUS_4"));					/*사정거리 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_PLUS_5, GAME_STRING("STR_HEADER_RANGE_PLUS_5"));					/*사정거리 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_1, GAME_STRING("STR_HEADER_HORZ_REACTION_REDUCE_1"));		/*수평 반동 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_2, GAME_STRING("STR_HEADER_HORZ_REACTION_REDUCE_2"));		/*수평 반동 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_3, GAME_STRING("STR_HEADER_HORZ_REACTION_REDUCE_3"));		/*수평 반동 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_4, GAME_STRING("STR_HEADER_HORZ_REACTION_REDUCE_4"));		/*수평 반동 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_REDUCE_5, GAME_STRING("STR_HEADER_HORZ_REACTION_REDUCE_5"));		/*수평 반동 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_1, GAME_STRING("STR_HEADER_VERTICAL_REACTION_REDUCE_1"));	/*수직 반동 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_2, GAME_STRING("STR_HEADER_VERTICAL_REACTION_REDUCE_2"));	/*수직 반동 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_3, GAME_STRING("STR_HEADER_VERTICAL_REACTION_REDUCE_3"));	/*수직 반동 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_4, GAME_STRING("STR_HEADER_VERTICAL_REACTION_REDUCE_4"));	/*수직 반동 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_REDUCE_5, GAME_STRING("STR_HEADER_VERTICAL_REACTION_REDUCE_5"));	/*수직 반동 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_1, GAME_STRING("STR_HEADER_HIT_RATE_PLUS_1"));				/*명중률 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_2, GAME_STRING("STR_HEADER_HIT_RATE_PLUS_2"));				/*명중률 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_3, GAME_STRING("STR_HEADER_HIT_RATE_PLUS_3"));				/*명중률 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_4, GAME_STRING("STR_HEADER_HIT_RATE_PLUS_4"));				/*명중률 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_REDUCE_5, GAME_STRING("STR_HEADER_HIT_RATE_PLUS_5"));				/*명중률 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_PLUS_1, GAME_STRING("STR_HEADER_THROWING_PLAS_1"));				/*투척 거리 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_PLUS_2, GAME_STRING("STR_HEADER_THROWING_PLAS_2"));				/*투척 거리 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_PLUS_3, GAME_STRING("STR_HEADER_THROWING_PLAS_3"));				/*투척 거리 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_PLUS_4, GAME_STRING("STR_HEADER_THROWING_PLAS_4"));				/*투척 거리 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_PLUS_5, GAME_STRING("STR_HEADER_THROWING_PLAS_5"));				/*투척 거리 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_REDUCE_1, GAME_STRING("STR_HEADER_EXPLOSION_TIME_MINUS_1"));		/*폭파 지연시간 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_REDUCE_2, GAME_STRING("STR_HEADER_EXPLOSION_TIME_MINUS_2"));		/*폭파 지연시간 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_REDUCE_3, GAME_STRING("STR_HEADER_EXPLOSION_TIME_MINUS_3"));		/*폭파 지연시간 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_REDUCE_4, GAME_STRING("STR_HEADER_EXPLOSION_TIME_MINUS_4"));		/*폭파 지연시간 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_REDUCE_5, GAME_STRING("STR_HEADER_EXPLOSION_TIME_MINUS_5"));		/*폭파 지연시간 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_PLUS_1, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_PLUS_1"));		/*폭파 범위 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_PLUS_2, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_PLUS_2"));		/*폭파 범위 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_PLUS_3, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_PLUS_3"));		/*폭파 범위 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_PLUS_4, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_PLUS_4"));		/*폭파 범위 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_PLUS_5, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_PLUS_5"));		/*폭파 범위 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_MINUS_1, GAME_STRING("STR_HEADER_SPEED_MINUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_MINUS_2, GAME_STRING("STR_HEADER_SPEED_MINUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_MINUS_3, GAME_STRING("STR_HEADER_SPEED_MINUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_MINUS_4, GAME_STRING("STR_HEADER_SPEED_MINUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_ABILITY_MINUS_5, GAME_STRING("STR_HEADER_SPEED_MINUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_1, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_PLUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_2, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_PLUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_3, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_PLUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_4, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_PLUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_PLUS_5, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_PLUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_PLUS_1, GAME_STRING("STR_HEADER_DPD_RATE_PLUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_PLUS_2, GAME_STRING("STR_HEADER_DPD_RATE_PLUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_PLUS_3, GAME_STRING("STR_HEADER_DPD_RATE_PLUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_PLUS_4, GAME_STRING("STR_HEADER_DPD_RATE_PLUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_PLUS_5, GAME_STRING("STR_HEADER_DPD_RATE_PLUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_PLUS_1, GAME_STRING("STR_HEADER_FIRST_SHOT_PLUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_PLUS_2, GAME_STRING("STR_HEADER_FIRST_SHOT_PLUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_PLUS_3, GAME_STRING("STR_HEADER_FIRST_SHOT_PLUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_PLUS_4, GAME_STRING("STR_HEADER_FIRST_SHOT_PLUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_PLUS_5, GAME_STRING("STR_HEADER_FIRST_SHOT_PLUS_5"));


	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_1, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_MINUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_2, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_MINUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_3, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_MINUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_4, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_MINUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_WEAPON_REACTION_CONTROL_MINUS_5, GAME_STRING("STR_HEADER_WEAPON_REACTION_CONTROL_MINUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_MINUS_1, GAME_STRING("STR_HEADER_DPD_RATE_MINUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_MINUS_2, GAME_STRING("STR_HEADER_DPD_RATE_MINUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_MINUS_3, GAME_STRING("STR_HEADER_DPD_RATE_MINUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_MINUS_4, GAME_STRING("STR_HEADER_DPD_RATE_MINUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_DPD_RATE_MINUS_5, GAME_STRING("STR_HEADER_DPD_RATE_MINUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_MINUS_1, GAME_STRING("STR_HEADER_FIRST_SHOT_MINUS_1"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_MINUS_2, GAME_STRING("STR_HEADER_FIRST_SHOT_MINUS_2"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_MINUS_3, GAME_STRING("STR_HEADER_FIRST_SHOT_MINUS_3"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_MINUS_4, GAME_STRING("STR_HEADER_FIRST_SHOT_MINUS_4"));
	_setUserTitleEffectText( DESIGNATION_ID_FIRST_SHOT_MINUS_5, GAME_STRING("STR_HEADER_FIRST_SHOT_MINUS_5"));

	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_1, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_INCREASE_1"));	/*낙하 데미지 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_2, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_INCREASE_2"));	/*낙하 데미지 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_3, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_INCREASE_3"));	/*낙하 데미지 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_4, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_INCREASE_4"));	/*낙하 데미지 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_FALLDOWN_DAMAGE_INCREASE_5, GAME_STRING("STR_HEADER_FALLDOWN_DAMAGE_INCREASE_5"));	/*낙하 데미지 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_1, GAME_STRING("STR_HEADER_AGILITY_MINUS_1"));				/*움직임 능력 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_2, GAME_STRING("STR_HEADER_AGILITY_MINUS_2"));				/*움직임 능력 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_3, GAME_STRING("STR_HEADER_AGILITY_MINUS_3"));				/*움직임 능력 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_4, GAME_STRING("STR_HEADER_AGILITY_MINUS_4"));				/*움직임 능력 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_ADDSPEED_ABILITY_MINUS_5, GAME_STRING("STR_HEADER_AGILITY_MINUS_5"));				/*움직임 능력 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_MINUS_1, GAME_STRING("STR_HEADER_JUMP_MINUS_1"));					/*점프 능력 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_MINUS_2, GAME_STRING("STR_HEADER_JUMP_MINUS_2"));					/*점프 능력 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_MINUS_3, GAME_STRING("STR_HEADER_JUMP_MINUS_3"));					/*점프 능력 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_MINUS_4, GAME_STRING("STR_HEADER_JUMP_MINUS_4"));					/*점프 능력 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_JUMP_ABILITY_MINUS_5, GAME_STRING("STR_HEADER_JUMP_MINUS_5"));					/*점프 능력 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_MINUS_1, GAME_STRING("STR_HEADER_SPEED_MINUS_1"));					/*이동 속도 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_MINUS_2, GAME_STRING("STR_HEADER_SPEED_MINUS_2"));					/*이동 속도 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_MINUS_3, GAME_STRING("STR_HEADER_SPEED_MINUS_3"));					/*이동 속도 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_MINUS_4, GAME_STRING("STR_HEADER_SPEED_MINUS_4"));					/*이동 속도 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_MOVE_SPEED_MINUS_5, GAME_STRING("STR_HEADER_SPEED_MINUS_5"));					/*이동 속도 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_1, GAME_STRING("STR_HEADER_VERTICAL_REACTION_INCREASE_1"));	/*수직 반동 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_2, GAME_STRING("STR_HEADER_VERTICAL_REACTION_INCREASE_2"));	/*수직 반동 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_3, GAME_STRING("STR_HEADER_VERTICAL_REACTION_INCREASE_3"));	/*수직 반동 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_4, GAME_STRING("STR_HEADER_VERTICAL_REACTION_INCREASE_4"));	/*수직 반동 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_VERTICAL_REACTION_INCREASE_5, GAME_STRING("STR_HEADER_VERTICAL_REACTION_INCREASE_5"));	/*수직 반동 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_1, GAME_STRING("STR_HEADER_HORZ_REACTION_INCREASE_1"));		/*수평 반동 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_2, GAME_STRING("STR_HEADER_HORZ_REACTION_INCREASE_2"));		/*수평 반동 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_3, GAME_STRING("STR_HEADER_HORZ_REACTION_INCREASE_3"));		/*수평 반동 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_4, GAME_STRING("STR_HEADER_HORZ_REACTION_INCREASE_4"));		/*수평 반동 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_HORIZONTAL_REACTION_INCREASE_5, GAME_STRING("STR_HEADER_HORZ_REACTION_INCREASE_5"));		/*수평 반동 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_1, GAME_STRING("STR_HEADER_HIT_RATE_MINUS_1"));				/*명중률 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_2, GAME_STRING("STR_HEADER_HIT_RATE_MINUS_2"));				/*명중률 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_3, GAME_STRING("STR_HEADER_HIT_RATE_MINUS_3"));				/*명중률 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_4, GAME_STRING("STR_HEADER_HIT_RATE_MINUS_4"));				/*명중률 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_TARGET_DECLINATION_INCREASE_5, GAME_STRING("STR_HEADER_HIT_RATE_MINUS_5"));				/*명중률 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_INCREASE_1, GAME_STRING("STR_HEADER_RELOAD_MINUS_1"));		/*재장전 속도 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_INCREASE_2, GAME_STRING("STR_HEADER_RELOAD_MINUS_2"));		/*재장전 속도 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_INCREASE_3, GAME_STRING("STR_HEADER_RELOAD_MINUS_3"));		/*재장전 속도 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_INCREASE_4, GAME_STRING("STR_HEADER_RELOAD_MINUS_4"));		/*재장전 속도 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_RELOAD_TIME_INCREASE_5, GAME_STRING("STR_HEADER_RELOAD_MINUS_5"));		/*재장전 속도 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_MINUS_1, GAME_STRING("STR_HEADER_RANGE_MINUS_1"));				/*사정거리 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_MINUS_2, GAME_STRING("STR_HEADER_RANGE_MINUS_2"));				/*사정거리 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_MINUS_3, GAME_STRING("STR_HEADER_RANGE_MINUS_3"));				/*사정거리 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_MINUS_4, GAME_STRING("STR_HEADER_RANGE_MINUS_4"));				/*사정거리 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_RANGE_MINUS_5, GAME_STRING("STR_HEADER_RANGE_MINUS_5"));				/*사정거리 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_MINUS_1, GAME_STRING("STR_HEADER_THROWING_MINUS_1"));			/*투척 거리 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_MINUS_2, GAME_STRING("STR_HEADER_THROWING_MINUS_2"));			/*투척 거리 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_MINUS_3, GAME_STRING("STR_HEADER_THROWING_MINUS_3"));			/*투척 거리 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_MINUS_4, GAME_STRING("STR_HEADER_THROWING_MINUS_4"));			/*투척 거리 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_THROW_SPEED_MINUS_5, GAME_STRING("STR_HEADER_THROWING_MINUS_5"));			/*투척 거리 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_MINUS_1, GAME_STRING("STR_HEADER_FIRE_SPEED_MINUS_1"));		/*발사 속도 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_MINUS_2, GAME_STRING("STR_HEADER_FIRE_SPEED_MINUS_2"));		/*발사 속도 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_MINUS_3, GAME_STRING("STR_HEADER_FIRE_SPEED_MINUS_3"));		/*발사 속도 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_MINUS_4, GAME_STRING("STR_HEADER_FIRE_SPEED_MINUS_4"));		/*발사 속도 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_FIRE_SPEED_MINUS_5, GAME_STRING("STR_HEADER_FIRE_SPEED_MINUS_5"));		/*발사 속도 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_INCREASE_1, GAME_STRING("STR_HEADER_EXPLOSION_TIME_PLUS_1"));	/*폭파 지연시간 +1*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_INCREASE_2, GAME_STRING("STR_HEADER_EXPLOSION_TIME_PLUS_2"));	/*폭파 지연시간 +2*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_INCREASE_3, GAME_STRING("STR_HEADER_EXPLOSION_TIME_PLUS_3"));	/*폭파 지연시간 +3*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_INCREASE_4, GAME_STRING("STR_HEADER_EXPLOSION_TIME_PLUS_4"));	/*폭파 지연시간 +4*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_TIME_INCREASE_5, GAME_STRING("STR_HEADER_EXPLOSION_TIME_PLUS_5"));	/*폭파 지연시간 +5*/

	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_MINUS_1, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_MINUS_1"));	/*폭파 범위 -1*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_MINUS_2, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_MINUS_2"));	/*폭파 범위 -2*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_MINUS_3, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_MINUS_3"));	/*폭파 범위 -3*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_MINUS_4, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_MINUS_4"));	/*폭파 범위 -4*/
	_setUserTitleEffectText( DESIGNATION_ID_BOMB_RANGE_MINUS_5, GAME_STRING("STR_HEADER_EXPLOSION_RANGE_MINUS_5"));	/*폭파 범위 -5*/

	_setUserTitleEffectText( DESIGNATION_ID_GET_SLOT2,				GAME_STRING("STR_HEADER_DESIGNATION_GET_SLOT2"));				/*호칭 슬롯2 획득*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_SLOT3,				GAME_STRING("STR_HEADER_DESIGNATION_GET_SLOT3"));				/*호칭 슬롯3 획득*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_BERET,				GAME_STRING("STR_HEADER_GET_BERET"));							/*베레모 획득\n(재장전 애니메이션 포함)*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_DUAL_KNIFE,			GAME_STRING("STR_HEADER_AUTHORITY_DUAL_KNIFE"));			/*Dual Knife 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_DUAL_HANDGUN,		GAME_STRING("STR_HEADER_AUTHORITY_DUAL_HANDGUN"));		/*Dual Handgun 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_870MCS_W,			GAME_STRING("STR_HEADER_AUTHORITY_870MCS_W"));				/*870MCS W 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_SG550_S,			GAME_STRING("STR_HEADER_AUTHORITY_SG550_S"));				/*SG550 S 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_DRAGUNOV_G,			GAME_STRING("STR_HEADER_AUTHORITY_DRAGUNOV_G"));			/*Dragunov G 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_SPECTRE_W,			GAME_STRING("STR_HEADER_AUTHORITY_SPECTRE_W"));			/*Spectre W 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_C5,					GAME_STRING("STR_HEADER_AUTHORITY_C5"));							/*C-5 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_G36C_EXT,			GAME_STRING("STR_HEADER_AUTHORITY_G36C_EXT"));				/*G36C Ext. 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_L115A1,				GAME_STRING("STR_HEADER_AUTHORITY_L115A1"));					/*L115A1 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_CIC,				GAME_STRING("STR_HEADER_AUTHORITY_P99"));						/*P99&HAK 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_P90_EXT,			GAME_STRING("STR_HEADER_AUTHORITY_P90_EXT"));				/*P90 Ext. 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_SPAS15,				GAME_STRING("STR_HEADER_AUTHORITY_SPAS15"));					/*SPAS-15 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_PSG1_S,				GAME_STRING("STR_HEADER_AUTHORITY_PSG1_S"));					/*PSG-1 S 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_MP5K_G,				GAME_STRING("STR_HEADER_AUTHORITY_MP5K_G"));					/*MP5K G 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_AMOK_KUKRI,			GAME_STRING("STR_HEADER_AUTHORITY_KUKRII"));				/*Amok Kukri 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_WP_SMOKE,			GAME_STRING("STR_HEADER_AUTHORITY_WP_SMOKE"));				/*WP Smoke 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_KRISS_SUPER_V,		GAME_STRING("STR_HEADER_AUTHORITY_KRISS_SUPER_V"));	/*Kriss Super V 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_DUAL_DESERTEAGLE,	GAME_STRING("STR_HEADER_AUTHORITY_DUAL_D_EAGLE"));	/*Dual D-Eagle 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_AK_SOPMOD,			GAME_STRING("STR_HEADER_AUTHORITY_AK_SOPMOD"));			/*AK SOPMOD 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_AUG_A3,				GAME_STRING("STR_HEADER_AUTHORITY_AUGA3"));			/*AUG A3 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_M1887W,				GAME_STRING("STR_HEADER_AUTHORITY_M1887W"));			/*AUG A3 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_MINI_AXE,			GAME_STRING("STR_HEADER_AUTHORITY_MINI_AXE"));				/*Mini Axe 구입 권한*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_SOMETHING,			GAME_STRING("STR_HEADER_GET_SOMETHING"));					/*????  (미공개)*/
	_setUserTitleEffectText( DESIGNATION_ID_GET_AKS74U,				GAME_STRING("STR_HEADER_AUTHORITY_AKS74U"));

	/*AKS74U 구입 권한*/
}

DESIGNATION_SET * CDesignationValueSet::GetDesignationSetByType( INT32 idx)
{
	I3ASSERT( idx > -1 && idx <= MAX_DEFAULT_USER_TITLE_COUNT);

	for( INT32 i=0; i<MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if( m_tDesignationSet[ i].getType() == idx)
			return &m_tDesignationSet[ i];

	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------//

void DESIGNATION_SET::getName( char * pszOut)
{
	INT32 len = i3::generic_string_size( _szName);
	if( len > 0)
	{
		DECRYPT_STRING( pszOut, _szName, len, _ShiftCode);
	}
	else
		pszOut[0] = '\0';
}

void DESIGNATION_SET::setName( const char * pszIn)
{
	INT32 i;

	INT32 len = i3::generic_string_size( pszIn);

	if( len > 0)
	{	
		i3::string_ncopy_nullpad( _szName, pszIn, len);
		
		GlobalFunc::EncryptValue( _szName, len, _ShiftCode);

		for( i = len + 2; i < sizeof( _szName); i++)
		{
			_szName[ i] = (char) (i3Math::Rand() % 8);
		}
	}
	else
		_szName[0] = '\0';
}

void DESIGNATION_SET::getName_E( char * pszOut)
{
	INT32 len = i3::generic_string_size( _szName_E);
	if( len > 0)
	{
		DECRYPT_STRING( pszOut, _szName_E, len, _ShiftCode);
	}
	else
		pszOut[0] = '\0';
}

void DESIGNATION_SET::setName_E( const char * pszIn)
{
	INT32 i;
	INT32 len = i3::generic_string_size( _szName_E);

	if( len > 0)
	{
		i3::string_ncopy_nullpad( _szName_E, pszIn, len);

		GlobalFunc::EncryptValue( _szName_E, len, _ShiftCode);

		for( i = len + 2; i < sizeof( _szName_E); i++)
		{
			_szName_E[ i] = (char) (i3Math::Rand() % 8);
		}
	}
	else
		_szName_E[0] = '\0';
}

void DESIGNATION_SET::getExplain( char * pszOut)
{
	INT32 len = i3::generic_string_size( _szExplain);
	if( len > 0)
	{
		DECRYPT_STRING( pszOut, _szExplain, len, _ShiftCode);
	}
	else
		pszOut[0] = '\0';
}

void DESIGNATION_SET::setExplain( const char * pszIn)
{
	INT32 i;
	INT32 len = i3::generic_string_size( _szExplain);

	if( len > 0)
	{
		i3::string_ncopy_nullpad( _szExplain, pszIn, len);

		GlobalFunc::EncryptValue( _szExplain, len, _ShiftCode);

		for( i = len; i < sizeof( _szExplain); i++)
		{
			_szExplain[ i] = (char) (i3Math::Rand() % 8);
		}
	}
	else
		_szExplain[0] = '\0';
}

DESIGNATION_APPLY_TYPE DESIGNATION_SET::getApplyType( void)
{
	return (DESIGNATION_APPLY_TYPE) DECRYPT_INTVALUE( _ApplyType, _ShiftCode);
}

void DESIGNATION_SET::setApplyType( DESIGNATION_APPLY_TYPE type)
{
	GlobalFunc::EncryptValue( (INT32*)&type, _ShiftCode);

	_ApplyType = type;
}

DESIGNATION_TYPE DESIGNATION_SET::getType( void)
{
	return (DESIGNATION_TYPE) DECRYPT_INTVALUE( _Type, _ShiftCode);
}

void DESIGNATION_SET::setType( DESIGNATION_TYPE type)
{
	GlobalFunc::EncryptValue( (INT32*)&type, _ShiftCode);

	_Type = type;
}

UINT8 DESIGNATION_SET::getPlusEffect( INT32 idx)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);
	return DECRYPT_U8( _PlusEffect[idx], _ShiftCode);
}

void DESIGNATION_SET::setPlusEffect( INT32 idx, UINT8 val)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);

	GlobalFunc::EncryptValue( &val, sizeof(val), _ShiftCode);

	_PlusEffect[idx] = val;
}

UINT8 DESIGNATION_SET::getMinusEffect( INT32 idx)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);
	return DECRYPT_U8( _MinusEffect[idx], _ShiftCode);
}

void DESIGNATION_SET::setMinusEffect( INT32 idx, UINT8 val)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);

	GlobalFunc::EncryptValue( &val, sizeof(val), _ShiftCode);

	_MinusEffect[ idx] = val;
}

UINT8 DESIGNATION_SET::getAddEffect( INT32 idx)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);
	return DECRYPT_U8( _AddEffect[idx], _ShiftCode);
}

void DESIGNATION_SET::setAddEffect( INT32 idx, UINT8 val)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);

	GlobalFunc::EncryptValue( &val, sizeof(val), _ShiftCode);

	_AddEffect[ idx] = val;
}


UINT8 DESIGNATION_SET::getAuthWeaponClass( INT32 idx)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);
	return DECRYPT_U8( _AuthWeaponClass[idx], _ShiftCode);
}

void DESIGNATION_SET::setAuthWeaponClass( INT32 idx, UINT8 val)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);

	GlobalFunc::EncryptValue( &val, sizeof(val), _ShiftCode);

	_AuthWeaponClass[ idx] = val;
}

UINT8 DESIGNATION_SET::getAuthWeaponNumber( INT32 idx)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);
	return DECRYPT_U8( _AuthWeaponNumber[idx], _ShiftCode);
}

void DESIGNATION_SET::setAuthWeaponNumber( INT32 idx, UINT8 val)
{
	I3_BOUNDCHK( idx, DESIGNATION_EFFECT_COUNT);

	GlobalFunc::EncryptValue( &val, sizeof(val), _ShiftCode);

	_AuthWeaponNumber[ idx] = val;
}

BOOL IsWeaponByDesignationEffect( DESIGNATION_ID nDesignationEffect)
{
	// 호칭 능력이 호칭 무기 구입 권한인 경우 TRUE 반환, 일반 능력일 경우 FALSE
	switch(nDesignationEffect)
	{
	case DESIGNATION_ID_GET_DUAL_KNIFE:
	case DESIGNATION_ID_GET_DUAL_HANDGUN:
	case DESIGNATION_ID_GET_870MCS_W:
	case DESIGNATION_ID_GET_SG550_S:
	case DESIGNATION_ID_GET_DRAGUNOV_G:
	case DESIGNATION_ID_GET_SPECTRE_W:
	case DESIGNATION_ID_GET_C5:
	case DESIGNATION_ID_GET_G36C_EXT:
	case DESIGNATION_ID_GET_L115A1:
	case DESIGNATION_ID_GET_CIC:
	case DESIGNATION_ID_GET_P90_EXT:
	case DESIGNATION_ID_GET_SPAS15:
	case DESIGNATION_ID_GET_PSG1_S:
	case DESIGNATION_ID_GET_MP5K_G:
	case DESIGNATION_ID_GET_AMOK_KUKRI:
	case DESIGNATION_ID_GET_WP_SMOKE:
	case DESIGNATION_ID_GET_KRISS_SUPER_V:
	case DESIGNATION_ID_GET_DUAL_DESERTEAGLE:
	case DESIGNATION_ID_GET_AK_SOPMOD:
	case DESIGNATION_ID_GET_MINI_AXE:
	case DESIGNATION_ID_GET_AUG_A3:
	case DESIGNATION_ID_GET_M1887W:
	case DESIGNATION_ID_GET_AKS74U:
	case DESIGNATION_ID_GET_WEAPON:
		return TRUE;
	}

	return FALSE;
}


