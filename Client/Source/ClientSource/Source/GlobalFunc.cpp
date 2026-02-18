#include "pch.h"
#include "GlobalFunc.h"
#include "GlobalVariables.h"
#include "Weapon/WeaponItemID.h"

#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/contain_string.h"

CRandomArray g_RandomArray;

const char * GlobalFunc::GetLocaleFolderName(INT32 languageType)
{
	switch(languageType)
	{
	case RESOURCE_LANGUAGE_DEV:
		return "Dev";

	case RESOURCE_LANGUAGE_KOREA:
		return "Korea";

	case RESOURCE_LANGUAGE_THAILAND:
		return "Thai";

	case RESOURCE_LANGUAGE_INDONESIA:
		return "Indonesia";

	case RESOURCE_LANGUAGE_RUSSIA:
		return "Russia";

	case RESOURCE_LANGUAGE_TURKEY:
		return "Turkey";

	case RESOURCE_LANGUAGE_BRAZIL:
		return "Brazil";

	case RESOURCE_LANGUAGE_LATIN_AMERICA:
		return "Latin_America";

	case RESOURCE_LANGUAGE_NORTHAMERICA:
		return "NorthAmerica";

	case RESOURCE_LANGUAGE_ITALY:
		return "Italy";

	case RESOURCE_LANGUAGE_ENGLISH:
		return "English";

	case RESOURCE_LANGUAGE_MIDDLE_EAST:
		return "MiddleEast";

	case RESOURCE_LANGUAGE_PHILIPPINES:
		return "Philippines";

	case RESOURCE_LANGUAGE_TAIWAN:
		return "Taiwan";

	case RESOURCE_LANGUAGE_SINGMAL:
		return "Singmal";

	case RESOURCE_LANGUAGE_VIETNAM:
		return "Vietnam";

	case RESOURCE_LANGUAGE_JAPAN:
		return "Japan";
	
	default:
		return "Dev";
	}
}

bool GlobalFunc::IsFileInLocaleFolder( char * pszPath, const char * pszLocale)
{
	if( pszLocale != nullptr)
	{
		char szFullPath[ MAX_PATH];
		i3::sprintf( szFullPath, "Locale/%s/%s", pszLocale, pszPath);
		if( i3FileStream::isFile( szFullPath) == true)
		{
			i3::generic_string_ncopy( pszPath, szFullPath, MAX_PATH);
			return true;
		}

		char szSplitLocale[ MAX_PATH];
		INT32 pos = i3::contain_string( pszLocale, "/");
		if( pos != -1)
		{
			i3::generic_string_ncopy( szSplitLocale, pszLocale, pos);
			szSplitLocale[ pos] = 0;

			i3::sprintf( szFullPath, "Locale/%s/%s", szSplitLocale, pszPath);
			if( i3FileStream::isFile( szFullPath) == true)
			{
				i3::generic_string_ncopy( pszPath, szFullPath, MAX_PATH);
				return true;
			}
		}
	}

	return false;
}

void GlobalFunc::GetNowFrameMatrix(MATRIX* pMat, i3Transform* pTrans)
{
	I3ASSERT(pMat);
	I3ASSERT(pTrans);

	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	I3ASSERT(pTracer);

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( g_pViewer->getDeltaTime());
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	
	pTrans->CalcUpdatedMatrix(pMat, pTracer);
}

bool GlobalFunc::CheckAvailableWeaponUsage(UINT8 WeaponFlag, WEAPON_SLOT_TYPE eUsage)
{
	bool IsKnuckleMode = (WeaponFlag & LOCK_KNUCKLE)?true:false;

	if(eUsage == WEAPON_SLOT_MELEE && IsKnuckleMode)
	{
		return TESTBIT(WeaponFlag, LOCK_KNUCKLE)?true:false;
	}

	if(eUsage == WEAPON_SLOT_THROWING2)
		return true;
	return TESTBIT(WeaponFlag, LOCK_PRIMARY >> eUsage)?true:false;
}

i3Object* GlobalFunc::Get_i3ObjectFromShape(NxShape * pShape)
{
	if( pShape == nullptr)
		return nullptr;

	i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pShape->userData;

	if( !i3::same_of<i3PhysixShapeSet*>(pShapeSet))
		return nullptr;

	i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();

	if( !i3::kind_of<i3Object*>(pReceiver))
		return nullptr;

	i3Object* pObj = ((i3Object*)pReceiver);

	return pObj;
}

//HIT_CHARA_PART GlobalFunc::GetRoughHitPart( const char * pszCollName)
//{
//	CHARA_HIT_PART part = GlobalFunc::GetHitPartRaptor(pszCollName);
//
//	switch( part)
//	{
//	case CHARA_HIT_HEAD:			return HIT_CHARA_PART_HEAD;
//	case CHARA_HIT_NECK	:
//	case CHARA_HIT_NECK1	:
//	case CHARA_HIT_NECK2	:
//	case CHARA_HIT_LCLAVICLE :
//	case CHARA_HIT_RCLAVICLE :
//	case CHARA_HIT_BODY1 :
//	case CHARA_HIT_BODY2 :
//	case CHARA_HIT_BODY3 :
//	case CHARA_HIT_ROOT	:			return HIT_CHARA_PART_BODY;
//	case CHARA_HIT_RUARM:
//	case CHARA_HIT_RDARM:
//	case CHARA_HIT_LUARM:
//	case CHARA_HIT_LDARM:			return HIT_CHARA_PART_BODY;
//	case CHARA_HIT_LHAND:
//	case CHARA_HIT_RHAND:			return HIT_CHARA_PART_ARM;
//	case CHARA_HIT_LULEG:
//	case CHARA_HIT_LDLEG:
//	case CHARA_HIT_RULEG:
//	case CHARA_HIT_RDLEG:			
//	case CHARA_HIT_LFOOT:
//	case CHARA_HIT_RFOOT:			return HIT_CHARA_PART_LEG;
//	case CHARA_HIT_NECK_LOWER:			
//	case CHARA_HIT_NECK1_LOWER:			
//	case CHARA_HIT_NECK2_LOWER:		return HIT_CHARA_PART_NECK;
//	}
//	
//	return HIT_CHARA_PART_NA;
//}

//#if defined( I3_DEBUG)

static char PB_TempStr[2048];

void GlobalFunc::PB_TRACE(const char *format, ...)
{
	va_list marker;

	va_start( marker, format);

	vsprintf_s( PB_TempStr, format, marker);

	va_end( marker);

	StringCbCat(PB_TempStr, 2048, "\n");

	I3TRACE(PB_TempStr);
	//I3PRINTLOG(I3LOG_NOTICE,PB_TempStr);
}


//#endif // I3_DEBUG

CHARA_HIT_PART GlobalFunc::GetHitPartRaptor( const char * pszCollName)
{
	CHARA_HIT_PART	part = CHARA_HIT_UNKNOWN;

	if( pszCollName == nullptr ) return part;

	switch( pszCollName[0])
	{
		case 'H' :
			if( pszCollName[2] == 'H' )				// H_H
			{
				part = CHARA_HIT_HEAD;				// H_Head
			}
			else if( pszCollName[3] == 'J' )
			{
				part = CHARA_HIT_JAW;				// HJ_Jaw
			}
			else if( pszCollName[3] == 'L' )
			{
				part = CHARA_HIT_LHAND;				// HN_L Hand
			}
			else if( pszCollName[3] == 'R' )
			{
				part = CHARA_HIT_RHAND;				// HN_R Hand
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'B' :
			if( pszCollName[2] == 'N' )				// B_N
			{
				if( pszCollName[6] == '0' )
				{
					if(pszCollName[8] == 'L') {
						part = CHARA_HIT_NECK_LOWER;	// B_Neck0_Lower
					}
					else {
						part = CHARA_HIT_NECK;			// B_Neck0
					}
					
				}
				else if( pszCollName[6] == '1' )
				{
					if(pszCollName[8] == 'L') {
						part = CHARA_HIT_NECK_LOWER;	// B_Neck0_Lower
					}
					else {
						part = CHARA_HIT_NECK1;			// B_Neck0
					}
				}
				else if(pszCollName[6] == '2' )
				{
					if(pszCollName[8] == 'L') {
						part = CHARA_HIT_NECK_LOWER;	// B_Neck0_Lower
					}
					else {
						part = CHARA_HIT_NECK2;			// B_Neck0
					}
				}
				else
				{
					I3ASSERT_0;
				}
				
			}
			else if( pszCollName[2] == 'S' )		// B_S
			{
				if( pszCollName[7] == '2' )			// B_Spine2
				{
					part = CHARA_HIT_BODY1;
				}
				else if( pszCollName[7] == '1' )	// B_Spine1
				{
					part = CHARA_HIT_BODY3;
				}
#ifdef DOMI_DINO
				else if ( pszCollName[7] == 0)
				{
					part = CHARA_HIT_BODY3;
				}
#endif
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[2] == 'R' )		// B_Root
			{
				part = CHARA_HIT_ROOT;
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'T' :
			if( pszCollName[6] == '1' )				
			{
				part = CHARA_HIT_TAIL1;			// T_Tail1
			}
			else if( pszCollName[6] == '2' )				
			{
				part = CHARA_HIT_TAIL2;			// T_Tail2
			}
			else if( pszCollName[6] == '3' )				
			{
				part = CHARA_HIT_TAIL3;			// T_Tail3
			}
			else if( pszCollName[6] == '4' )				
			{
				part = CHARA_HIT_TAIL4;			// T_Tail4
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'F' :
			if( pszCollName[2] == 'L' )				
			{
				part = CHARA_HIT_LFOOT;			// F_L Foot
			}
			else if( pszCollName[2] == 'R' )				
			{
				part = CHARA_HIT_RFOOT;			// F_R Foot
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'A' :
			if( pszCollName[1] == 'U' )				// AU
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LUARM;		// AU_L UpperArm
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RUARM;		// AU_R UpperArm
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'D' )		// AD
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LDARM;		// AD_L Forearm
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RDARM;		// AD_R Forearm
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'C' )		// AD
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LCLAVICLE;		// AC_L Clavicle
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RCLAVICLE;		// AC_R Clavicle
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		case 'L' :
			if( pszCollName[1] == 'U' )				// LU
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LULEG;		// LU_L Thigh
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RULEG;		// LU_R Thigh
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'D' )		// LD
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LDLEG;		// LD_L Calf
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RDLEG;		// LD_R Calf
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else if( pszCollName[1] == 'H' )		// LH
			{
				if( pszCollName[3] == 'L' ) 
				{
					part = CHARA_HIT_LHHORSE_LINK;	// LH_L HorseLink
				}
				else if( pszCollName[3] == 'R' ) 
				{
					part = CHARA_HIT_RHHORSE_LINK;	// AD_R Forearm
				}
				else
				{
					I3ASSERT_0;
				}
			}
			else
			{
				I3ASSERT_0;
			}
			break;
		default:
			{
				I3ASSERT_0;
			}
			break;
	}

		
	//I3TRACE( "HIT NAME : %s\n", pszCollName);

	return part;
}

i3Transform * GlobalFunc::FindTransformFromPhysXSceneObj( i3SceneObject * pSceneObj)
{
	i3Transform * pTrans = nullptr;

	i3Node * pNode = i3Scene::FindNodeByExactType( pSceneObj, i3Transform2::static_meta() );
	if( pNode != nullptr)
	{
		if( i3::kind_of<i3Transform*>(pNode))
			pTrans = (i3Transform*) pNode;
	}
	else
	{
		pNode = i3Scene::FindNodeByExactType( pSceneObj, i3ShapeSetContainer::static_meta());
		if( pNode != nullptr)
		{
			while( !i3::kind_of<i3Transform*>(pNode))
			{
				if( pNode->GetParent() == nullptr)
					break;

				pNode = pNode->GetParent();
			}

			if( i3::kind_of<i3Transform*>(pNode))
				pTrans = (i3Transform*) pNode;
		}
	}

	return pTrans;
}

//===========================================================================================//
///							암호화

void GlobalFunc::EncryptValue( char * pData, UINT32 len, UINT32 size, UINT8 c)
{
	if( len > 0)
	{
		EncryptValue( (void*) pData, len, c);
	}

	for( UINT32 i = len+3; i < size; i++)
		pData[i] = (char) (i3Math::Rand() %255);
}

void	GlobalFunc::EncryptValue(i3::string& inout, UINT8 c)
{
	if (inout.empty()) return;
	EncryptValue( &inout[0], INT32(inout.size()), c);
}

void	GlobalFunc::EncryptValue(i3::rc_string& inout, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	if (inout.empty()) return;
	i3::rc_string tmp(inout.c_str(), inout.size());
	EncryptValue( const_cast<char*>(&tmp[0]), INT32(tmp.size()), c);
	inout = tmp;
#endif
}

void	GlobalFunc::EncryptValue(i3::rc_wstring& inout, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	if (inout.empty()) return;
	i3::rc_wstring tmp(inout.c_str(), inout.size());
	EncryptValue( const_cast<wchar_t*>(&tmp[0]), INT32(tmp.size() * sizeof(wchar_t)), c);
	inout = tmp;
#endif
}




void GlobalFunc::EncryptValue( void * pVal, UINT32 len, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	if( (c > 0) && (len > 0))
	{
		// 1바이트씩 나눠서 비트 회전
		BitRotateEncryptForOneByte( (UINT8*) pVal, len, c);
	}
#endif
}

void GlobalFunc::EncryptValue( REAL32* pArg, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	UINT32* pTemp = (UINT32*)pArg;
	*pTemp = (*pTemp ^ g_RandomArray.GetRandom(c));
#endif
}

void GlobalFunc::EncryptValue( INT32* pArg, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	*pArg = *pArg ^ g_RandomArray.GetRandom(c);
#endif
}

void GlobalFunc::EncryptValue( UINT32* pArg, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	*pArg = *pArg ^ g_RandomArray.GetRandom(c);
#endif
}

void GlobalFunc::EncryptValue( UINT16* pArg, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	BitRotateEncryptForOneByte( (UINT8*) pArg, 2, c);
#endif
}

void GlobalFunc::EncryptValue( UINT8* pArg, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	BitRotateEncryptForOneByte( (UINT8*) pArg, 1, c);
#endif
}


void GlobalFunc::DecryptValue( const void * pVal, UINT32 len, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	if( (pVal != 0) && (c > 0))
	{
		// 1바이트씩 나눠서 비트 회전 
		BitRotateDecryptForOneByte( (UINT8*) pVal, len, c);
	}
#endif
}

INT32 GlobalFunc::DecryptValue( const INT32 * pVal, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	INT32 iRet = *pVal ^ g_RandomArray.GetRandom(c);

	return iRet;
#else
	return (INT32)*pVal;
#endif
}

UINT32 GlobalFunc::DecryptValue( const UINT32 * pVal, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	UINT32 Ret = *pVal ^ g_RandomArray.GetRandom(c);

	return Ret;
#else
	return (UINT32)*pVal;
#endif
}

REAL32 GlobalFunc::DecryptValue( const REAL32 * pVal, UINT8 c)
{
#if defined( MEM_ENCRYPTION)
	UINT32* pTemp = (UINT32*)pVal;
	UINT32 uiTemp = (*pTemp ^ g_RandomArray.GetRandom(c));
	REAL32* pRet = (REAL32*)&uiTemp;

	return *pRet;
#else
	return (REAL32)(*pVal);
#endif
}

void GlobalFunc::DecryptValue( char * pszOut, const char * pszIn, UINT32 len, UINT8 c)
{
	I3ASSERT( pszOut != nullptr);

	if( len > 0)
	{
		I3ASSERT( pszIn != nullptr);
		i3::string_ncopy_nullpad( pszOut, pszIn, len);

	#if defined( MEM_ENCRYPTION)
		if( c > 0)
		{
			// 1바이트씩 나눠서 비트 회전 
			BitRotateDecryptForOneByte( (UINT8*) pszOut, len, c);
		}
	#endif
	}
	else
		pszOut[0] = '\0';
}

void	GlobalFunc::DecryptValue( i3::string& out, const i3::string& in, UINT8 c)
{
	if ( !in.empty() )
	{
		out = in;
	#if defined( MEM_ENCRYPTION)
		if( c > 0)
		{
			// 1바이트씩 나눠서 비트 회전 
			BitRotateDecryptForOneByte( (UINT8*) &out[0], INT32(out.size()), c);
		}
	#endif		
	}
	else
	{
		out.clear();
	}
}

void	GlobalFunc::DecryptValue( i3::rc_string& out, const i3::rc_string& in, UINT8 c)
{
	if (!in.empty() )
	{
		
	#if defined( MEM_ENCRYPTION)
		i3::rc_string tmp( in.begin(), in.end() );
		if( c > 0)
		{
			// 1바이트씩 나눠서 비트 회전 
			BitRotateDecryptForOneByte( (UINT8*) &tmp[0], INT32(tmp.size() ), c);
		}
		out = tmp;
	#else
		out = in;
	#endif			

	}
	else
	{
		out.clear();
	}
}

void	GlobalFunc::DecryptValue( i3::rc_wstring& out, const i3::rc_wstring& in, UINT8 c)
{
	if (!in.empty() )
	{

#if defined( MEM_ENCRYPTION)
		i3::rc_wstring tmp( in.begin(), in.end() );
		if( c > 0)
		{
			// 1바이트씩 나눠서 비트 회전 
			BitRotateDecryptForOneByte( (UINT8*) &tmp[0], INT32(tmp.size() * sizeof(wchar_t) ), c);
		}
		out = tmp;
#else
		out = in;
#endif			
	}
	else
	{
		out.clear();
	}
	
}



void GlobalFunc::DecryptValue( I3COLOR * pOut, const I3COLOR * pIn, UINT8 c)
{
	pOut->SetRGBA( pIn->GetRGBA());

#if defined( MEM_ENCRYPTION)
	if( c > 0)
		BitRotateDecryptForOneByte( (UINT8*) pOut, sizeof( I3COLOR), c);
#endif
}

void GlobalFunc::DecryptValue( VEC3D * pOut, const VEC3D * pIn, UINT8 c)
{
	//i3Vector::Copy( pOut, pIn);
	memcpy( pOut, pIn, sizeof(VEC3D) );

#if defined( MEM_ENCRYPTION)
	if( c > 0)
		BitRotateDecryptForOneByte( (UINT8*) pOut, sizeof( VEC3D), c);
#endif
}

void GlobalFunc::DecryptValue( VEC4D * pOut, const VEC4D * pIn, UINT8 c)
{
	//i3Vector::Copy( pOut, pIn);
	memcpy( pOut, pIn, sizeof(VEC4D) );

#if defined( MEM_ENCRYPTION)
	if( c > 0)
		BitRotateDecryptForOneByte( (UINT8*) pOut, sizeof( VEC4D), c);
#endif
}

UINT16 GlobalFunc::DecryptValue( const UINT16 * pVal, UINT8 c)
{
	UINT16 val = *pVal;
#if defined( MEM_ENCRYPTION)
	if( c > 0)
		BitRotateDecryptForOneByte( (UINT8*) &val, 2, c);
#endif

	return val;
}

UINT8 GlobalFunc::DecryptValue( const UINT8 * pVal, UINT8 c)
{
	UINT8 val = *pVal;
#if defined( MEM_ENCRYPTION)
	if( c > 0)
		BitRotateDecryptForOneByte( (UINT8*) &val, 1, c);
#endif

	return val;
}

void GlobalFunc::BitRotateEncryptForOneByte( UINT8 * pBuf, UINT32 Length, UINT32 c)
{		
	UINT8 d1, d2;

	INT32 i, rc;

	rc = 8 - c;

	for( i = 0; i < (INT32) Length; i++)
	{
		d1 = d2 = pBuf[i];

		d1 = (UINT8)((d1 << c) & 0xFF);
		d2 = (UINT8)((d2 >> rc) & 0xFF);

		pBuf[i] = (UINT8)(( d1 | d2) & 0xFF);
	}
}

void GlobalFunc::BitRotateDecryptForOneByte( UINT8 * pBuf, UINT32 Length, UINT32 c)
{
	UINT8 d1, d2;

	INT32 i, rc;

	rc = 8 - c;

	for( i = 0; i < (INT32) Length; i++)
	{
		d1 = d2 = pBuf[i];

		d1 = (UINT8)((d1 >> c) & 0xFF);
		d2 = (UINT8)((d2 << rc) & 0xFF);

		pBuf[i] = (UINT8)(( d1 | d2) & 0xFF);
	}
}

#if defined(DEV_BUILD) || defined(BUILD_RELEASE_QA_VERSION)
void __DevLog(const char *format, ...)
{
	static char s_Str1[2048] = {};
	static char s_Str2[2048] = {};

	va_list marker;
	va_start( marker, format);
	vsprintf_s( s_Str1, format, marker);
	va_end( marker);

	sprintf_s(s_Str2, "%f %s", g_pFramework->GetAccTime(), s_Str1);

	I3PRINTLOG(I3LOG_DEFALUT, s_Str2);

	sprintf_s(s_Str2, "%s%s", s_Str2, "\n");
	
	I3TRACE(s_Str2);
	
}
#endif

i3::vector<INT32> g_vecAI_UsablePrimaryWeaponFlag;
i3::vector<INT32> g_vecAI_UsableSecondaryWeaponFlag;
i3::vector<INT32> g_vecAI_UsableMeleeWeaponFlag;
i3::vector<INT32> g_vecAI_UsableThrowingWeaponFlag;
i3::vector<INT32> g_vecAI_UsableItemWeaponFlag;


INT32 GetAiUsablePrimaryWeaponCount(void)		
{
	// return sizeof(g_aiUsablePrimaryWeaponFlag) / sizeof(g_aiUsablePrimaryWeaponFlag[0]);
	return (INT32)g_vecAI_UsablePrimaryWeaponFlag.size();
}

INT32 GetAiUsableSecondaryWeaponCount(void)		
{
	//return sizeof(g_aiUsableSecondaryWeaponFlag) / sizeof(g_aiUsableSecondaryWeaponFlag[0]);	
	return (INT32)g_vecAI_UsableSecondaryWeaponFlag.size();
}

INT32 GetAiUsableMeleeWeaponCount(void)			
{
	//return sizeof(g_aiUsableMeleeWeaponFlag) / sizeof(g_aiUsableMeleeWeaponFlag[0]);	
	return (INT32)g_vecAI_UsableMeleeWeaponFlag.size();
}

INT32 GetAiUsableThrowingWeaponCount(void)		
{
	//return sizeof(g_aiUsableThrowingWeaponFlag) / sizeof(g_aiUsableThrowingWeaponFlag[0]);	
	return (INT32)g_vecAI_UsableThrowingWeaponFlag.size();
}

INT32 GetAiUsableItemWeaponCount(void)			
{
	//return sizeof(g_aiUsableItemWeaponFlag) / sizeof(g_aiUsableItemWeaponFlag[0]);	
	return (INT32)g_vecAI_UsableItemWeaponFlag.size();
}

TEAM_TYPE SlotIndexToTeamType( INT32 Index )
{
	return static_cast<TEAM_TYPE>(Index % 2);
}

INT32 TeamTypeUserIdxToSlotIndex(INT32 teamType, INT32 userIdx)
{
	const static INT32 slotIdx[TEAM_COUNT][MAX_TEAM_USER] = 
	{
		{ 0, 2, 4, 6, 8, 10, 12, 14 },
		{ 1, 3, 5, 7, 9, 11, 13, 15 }
	};

	if(teamType < 0 || userIdx < 0)
		return -1;

	if(teamType < TEAM_COUNT && userIdx < MAX_TEAM_USER)
		return slotIdx[teamType][userIdx];

	return -1;
}


tstring GetFileSystemName()
{
	TCHAR DriveName[MAX_PATH] = {};
	if ( ! GetModuleFileName(nullptr, DriveName, MAX_PATH-1) )
		return tstring("error 1");

	DriveName[3] = 0;

	TCHAR VolumeName[MAX_PATH];
	TCHAR SystemNameBuffer[MAX_PATH];
	BOOL Rv = ::GetVolumeInformation(DriveName, VolumeName, MAX_PATH, nullptr, nullptr, nullptr, SystemNameBuffer, MAX_PATH);
	if ( ! Rv )
	{
		return tstring("error 2");
	}

	return tstring(SystemNameBuffer);
}

CRandomArray::CRandomArray()
{
	srand(GetTickCount());
	Randomize();
}

UINT32 CRandomArray::GetRandom(UINT32 idx)
{
	return m_uiRandom[idx%RANDOM_ARRAY_NUM];
}

void CRandomArray::Randomize()
{
	for(INT32 i=0; i<RANDOM_ARRAY_NUM; i++)
	{
		m_uiRandom[i] = rand();
	}
}

void GlobalFunc::ChangeObjState_( i3Object * pObject, INT32 nState, REAL32 rLocalTime, bool bAction)
{
	I3ASSERT( pObject != nullptr);

	if( pObject->getStateCount() < nState)
		return;

	I3_OBJECT_STATE * pState = pObject->getState( nState );

	if( pObject->getCurState() != nState )
	{
		I3_OBJECT_STATE * pOldState = pObject->getState( pObject->getCurState());
		
		if( pOldState->m_pTimeEvent != nullptr )
		{
			pOldState->m_pTimeEvent->Stop();
			pOldState->m_pTimeEvent->OnChangeTime( 0.0f);
			pOldState->m_pTimeEvent->Reset();
		}

		if( bAction)
		{
			pState->m_pTimeEvent->setCurrentEventIndex( 0);
			pState->m_pTimeEvent->setLocalTime( 0.f);
			pObject->setCurState( nState );
			return;
		}

		pObject->setCurState( nState );
		rLocalTime = 0.f;
	}

	if( rLocalTime <= 0.f)
		rLocalTime = pState->m_pTimeEvent->getDuration();

	//Repeat
	// revision 63360 I3_TIMEEVENT_REPEAT 로 설정된 Object Scene만 Repeat 하도록 수정
	if( pState->m_pTimeEvent->isStyle( I3_TIMEEVENT_REPEAT))
	{
 		if( pState->m_pTimeEvent->getLocalTime() >= pState->m_pTimeEvent->getDuration() &&
			rLocalTime < pState->m_pTimeEvent->getDuration())
 			pState->m_pTimeEvent->setCurrentEventIndex( 0);
	}


	INT32 event_count = pState->m_pTimeEvent->getEventCount();

	for( INT32 i = 0; i < event_count; i++)
	{
		i3TimeEventInfo * pEventInfo = pState->m_pTimeEvent->getEvent( i);
		i3Action * pAction = pEventInfo->getAction();
		if( pAction == nullptr)
			continue;

		if( pState->m_pTimeEvent->getEventReceiver() != nullptr)
		{
			// 사운드, 이팩트, 폭발 계산을 하지 않습니다.
			// event 순서상 Anim, Break 뒤에 있는 effect, sound event는 재생이 됩니다.
			bool ignored = i3::same_of<i3ActionPlaySound*>(pAction) || i3::same_of<i3ActionEmitEffect*>(pAction) || i3::same_of<i3ActionExplosion*>(pAction);

			if( ignored == false)
			{
				if( pAction->getStyle() & I3_ACTION_STYLE_OWNPROCESS)
					pAction->OnEvent( pState->m_pTimeEvent->getEventReceiver(), rLocalTime, I3_EVT_CODE_ACTIVATE);
				else
					pState->m_pTimeEvent->getEventReceiver()->OnEvent( I3_EVT_ACTION, pAction, pEventInfo->getParam());

				if( i3::same_of<i3ActionPlayAnim*>(pAction))
				{
					if( ((i3ActionPlayAnim*)pAction)->getAnim() != nullptr)
					{	
						REAL32 rAnimTime = rLocalTime - pEventInfo->getStartTime();
						if( rAnimTime > 0.f)
						{
							pObject->getSceneObject()->PlayAnim( ((i3ActionPlayAnim*)pAction)->getAnim(), I3_ANIM_BLEND_MODE_REPLACE,
								rAnimTime, 0.0f, 1.0f, 1.0f, ((i3ActionPlayAnim*)pAction)->getPlayMode(), I3_ANIM_PLAY_MODE_STOP);
						} 
					}
				}
				else if( i3::same_of<i3ActionBreakObject*>(pAction))
				{// 액션 내부에서 애니메이션을 다시 플레이할 수 있습니다.
					// 시간 설정을 다시 해줍니다.
					if( pObject->getSceneObject()->getCurrentAnim() != nullptr)
						pObject->getSceneObject()->SetPlayTime( rLocalTime);
				}
			}
		}
	}
	pState->m_pTimeEvent->setCurrentEventIndex( event_count);
	pState->m_pTimeEvent->setLocalTime( rLocalTime);
}

T_ItemID	GlobalFunc::GetRandParts( EQUIP::ePART parts, T_ItemID charaItemID)
{
	I3ASSERT_RETURN(parts >= 0, 0);

	bool bValid = false;

	INT32 validcnt = 0;
	while( bValid == false)
	{
		INT32 rand = i3Math::Rand();

		INT32 idx = rand % (g_pCharaInfoDataBase->GetInfoCount( parts) + 16);

		const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( parts, idx);
		if( pInfo != nullptr)
		{
			if( pInfo->IsSharedUniq())
			{
				for( UINT32 i = 0; i < MAX_SHARE_RES; i++)
				{
					if( pInfo->GetShareResItemID( i) == 0xFFFFFFFF)
						break;

					if( pInfo->GetShareResItemID( i) == charaItemID)
					{
						I3ASSERT( pInfo->GetITEMID() != 0);
						return pInfo->GetITEMID();
					}
				}
			}
			else
			{
				I3ASSERT( pInfo->GetITEMID() != 0);
				return pInfo->GetITEMID();
			}
		}

		validcnt++;
		if( validcnt > 2)
		{
			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( charaItemID);
			T_ItemID itemID = pCharaDBInfo->GetPartsItemID( parts);
			return itemID;
		}
	}

	I3PRINTLOG(I3LOG_FATAL,  "not found parts!!!");

	return 0;
}

T_ItemID GlobalFunc::GetRandWeapon( WEAPON_SLOT_TYPE type)
{
	INT32 classRand = i3Math::Rand() % 6;

	ITEM_TYPE item = ITEM_TYPE_PRIMARY;

	switch( type)
	{
	case WEAPON_SLOT_PRIMARY :
		{
			switch( classRand)
			{
			case 0 :	classRand = WEAPON_CLASS_ASSAULT;	break;
			case 1 :	classRand = WEAPON_CLASS_SMG;		break;
			case 2 :	classRand = WEAPON_CLASS_SNIPER;	break;
			case 3 :	classRand = WEAPON_CLASS_SHOTGUN;	break;
			case 4 :	classRand = WEAPON_CLASS_MG;		break;
			case 5 :	classRand = WEAPON_CLASS_DUALSMG;	break;
			}
		}
		break;

	case WEAPON_SLOT_SECONDARY :
		{
			switch( classRand)
			{
			case 1 : classRand = WEAPON_CLASS_DUALHANDGUN;	break;
			case 2 : classRand = WEAPON_CLASS_CIC;			break;
			default :classRand = WEAPON_CLASS_HANDGUN;		break;
			}
			item = ITEM_TYPE_SECONDARY;
		}
		break;

	case WEAPON_SLOT_MELEE :
		{
			switch( classRand)
			{
			case 1 : classRand = WEAPON_CLASS_DUALKNIFE;	break;
			default : classRand = WEAPON_CLASS_KNIFE;		break;
			}
			item = ITEM_TYPE_MELEE;
		}
		break;

	case WEAPON_SLOT_THROWING1 :
		{
			classRand = WEAPON_CLASS_THROWING_GRENADE;
			item = ITEM_TYPE_THROWING1;
		}
		break;

	case WEAPON_SLOT_THROWING2 :
		{
			switch( classRand % 3)
			{
			case 1 : classRand = WEAPON_CLASS_THROWING_ATTACK;	break;
			case 2 : classRand = WEAPON_CLASS_THROWING_HEAL;	break;
			default :classRand = WEAPON_CLASS_THROWING_CONTAIN;	break;
			}

			item = ITEM_TYPE_THROWING2;
		}
		break;
	}

	WEAPON::RES_ID num = (WEAPON::RES_ID)(i3Math::Rand() % WEAPON::getItemCount( (WEAPON_CLASS_TYPE) classRand));

	while( g_pWeaponInfoDataBase->getWeaponInfo((WEAPON_CLASS_TYPE)classRand, num) == nullptr)
	{
		num = i3Math::Rand() % WEAPON::getItemCount( (WEAPON_CLASS_TYPE) classRand);
	}

	while( g_pWeaponManager->QueryResource( (WEAPON_CLASS_TYPE) classRand, num) == nullptr)
	{
		num = i3Math::Rand() % WEAPON::getItemCount( (WEAPON_CLASS_TYPE) classRand);
	}

	return MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, item, classRand, num);
}

