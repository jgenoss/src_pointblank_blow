#if !defined( __GLOBAL_FUNC_H)
#define __GLOBAL_FUNC_H

#include "GlobalVariables.h"

#if defined( I3_DEBUG)
	#define	MEMDUMP_NEW(p,s)	NetworkDump_Malloc(p,s,__FILE__,__LINE__)
	#define MEMDUMP_DELETE(p)	NetworkDump_Free(p)
#else
	#define	MEMDUMP_NEW(p,s)	__noop
	#define MEMDUMP_DELETE(p)	__noop
#endif

#if defined(I3_DEBUG)
	#define ENC_SHIFT_WEAPONINFO_RESNAME		0
	#define ENC_SHIFT_CHARA_RESNAME				0
	#define ENC_SHIFT_CHARA_RESNAME2			0
	#define ENC_SHIFT_CHARA_INFO				0
	#define ENC_SHIFT_EQUIP_RESNAME				0
	#define ENC_SHIFT_PHYSIX					0
	#define ENC_SHIFT_CHARA_HP					0
#else
	#define MEM_ENCRYPTION				///< 메모리 암호화
	#define MEM_ENCRYPTION_LV1			///< 중요도 C
	#define MEM_ENCRYPTION_LV2			///< 중요도 B
	#define MEM_ENCRYPTION_LV3			///< 중요도 A
	
	#define ENC_SHIFT_WEAPONINFO_RESNAME		1
	#define ENC_SHIFT_CHARA_RESNAME				2
	#define ENC_SHIFT_CHARA_RESNAME2			5
	#define ENC_SHIFT_CHARA_INFO				4
	#define ENC_SHIFT_EQUIP_RESNAME				7
	#define ENC_SHIFT_PHYSIX					3
	#define ENC_SHIFT_CHARA_HP					5
#endif


#define ENCRYPTION_MAX_HP_VALUE					150	///< 최대 캐릭터 HP

#define RANDOM_ARRAY_NUM 8

class CRandomArray
{
private:
	UINT32 m_uiRandom[RANDOM_ARRAY_NUM];

public:
	CRandomArray();
	void Randomize();
	UINT32 GetRandom(UINT32 idx);
};

extern CRandomArray g_RandomArray;



namespace GlobalFunc
{
	const char *	GetLocaleFolderName(INT32 languageType);

	bool		IsFileInLocaleFolder( char * pszPath, const char * pszLocale);

	void		GetNowFrameMatrix(MATRIX* pMat, i3Transform* pTrans);
	i3Object*	Get_i3ObjectFromShape(NxShape * pShape);
	CHARA_HIT_PART	GetHitPartRaptor( const char * pszCollName);
	bool		CheckAvailableWeaponUsage(UINT8 WeaponFlag, WEAPON_SLOT_TYPE eUsage);

	i3Transform *	FindTransformFromPhysXSceneObj( i3SceneObject * pSceneObj);

	/** \brief 오브젝트 상태 변경
		\param[in] pObject 오브젝트 포인터
		\param[in] nState 바뀔 오브젝트 상태
		\param[in] rLocalTime 바뀔 오브젝트 타임
		\param[in] bAction 스테이트 변경시 액션 실행 여부(Effect를 뿌리려면 true)
		*/
	void		ChangeObjState_( i3Object * pObject, INT32 nState, REAL32 rLocalTime, bool bAction = false);

//#if defined( I3_DEBUG)
	void					PB_TRACE(const char *format, ...);
//#else
//	inline void				PB_TRACE(const char *format, ...) {}
//#endif // I3_DEBUG

	void		EncryptValue( char * pData, UINT32 len, UINT32 size, UINT8 c);
	void		EncryptValue( void * pData, UINT32 len, UINT8 c);
	void		EncryptValue( REAL32* pArg, UINT8 c);
	void		EncryptValue( INT32* pArg, UINT8 c);
	void		EncryptValue( UINT32* pArg, UINT8 c);
	void		EncryptValue( UINT16* pArg, UINT8 c);
	void		EncryptValue( UINT8* pArg, UINT8 c);
	void		EncryptValue(i3::string& inout, UINT8 c);
	void		EncryptValue(i3::rc_string& inout, UINT8 c);
	void		EncryptValue(i3::rc_wstring& inout, UINT8 c);

	// 아래는 부스트에 있는 하나의 모듈..
	template <bool B, class T = void> struct enable_if_c { typedef T type; };
	template <class T> struct enable_if_c<false, T> {};

	template<class T> typename enable_if_c<__is_enum(T),
		void>::type		EncryptValue(T* pArg, UINT8 c)	{	INT32 arg; EncryptValue(&arg, c); *pArg = T(arg);}

	void		DecryptValue( const void * pVal, UINT32 len, UINT8 c);
	INT32		DecryptValue( const INT32 * pVal, UINT8 c);
	UINT32		DecryptValue( const UINT32 * pVal, UINT8 c);
	REAL32		DecryptValue( const REAL32 * pVal, UINT8 c);
	void		DecryptValue( char * pszOut, const char * pszIn, UINT32 len, UINT8 c);
	void		DecryptValue( i3::string& out, const i3::string& in, UINT8 c);
	void		DecryptValue( i3::rc_string& out, const i3::rc_string& in, UINT8 c);
	void		DecryptValue( i3::rc_wstring& out, const i3::rc_wstring& in, UINT8 c);

	void		DecryptValue( I3COLOR * pOut, const I3COLOR * pIn, UINT8 c);
	void		DecryptValue( VEC3D * pOut, const VEC3D * pIn, UINT8 c);
	void		DecryptValue( VEC4D * pOut, const VEC4D * pIn, UINT8 c);
	UINT16		DecryptValue( const UINT16 * pVal, UINT8 c);
	UINT8		DecryptValue( const UINT8 * pVal, UINT8 c);
							
	// 1바이트씩 나눠서 비트 회전  2011.07.13 양승천
	void		BitRotateEncryptForOneByte( UINT8 * pBuf, UINT32 Length, UINT32 c);
	void		BitRotateDecryptForOneByte( UINT8 * pBuf, UINT32 Length, UINT32 c);


	T_ItemID	GetRandParts( EQUIP::ePART parts, T_ItemID charaItemID);
	T_ItemID	GetRandWeapon( WEAPON_SLOT_TYPE type);

	// 값이 없으면 pValue의 값 건드리지 않고 false 값 보냄.

	template<typename T>
	bool VAR_FIND_REG_DATA(i3RegKey * pKey, const char * pszDataName, T& pValue)
	{
		T temp = T(NULL);
		FIND_REG_DATA(pKey, pszDataName, &temp);

		if (temp != T(NULL))
		{
			pValue = temp;
			return true;
		}
		else
		{
			return false;
		}
	}
}

#if defined( MEM_ENCRYPTION)
	#define ENCRYPT_VALUE(a,c)				GlobalFunc::EncryptValue( &(a), c)
	#define DECRYPT_VALUE(a,c)				GlobalFunc::DecryptValue( &(a), c)
	#define DECRYPT_REALVALUE(a,c)			GlobalFunc::DecryptValue((REAL32*)&(a),c)
	#define DECRYPT_INTVALUE(a,c)			GlobalFunc::DecryptValue((INT32*)&(a),c)
	#define DECRYPT_STRING(out,in,len,c)	GlobalFunc::DecryptValue((char*)(out),(char*)(in),(len),c)
	#define DECRYPT_U8(a,c)					GlobalFunc::DecryptValue((UINT8*)&(a),c)
	#if defined( MEM_ENCRYPTION_LV1)
		#define DECRYPT_VALUE1(a,c)			GlobalFunc::DecryptValue(&(a),c)
	#else
		#define DECRYPT_VALUE1(a,c)			a
	#endif

	#if defined( MEM_ENCRYPTION_LV2)
		#define DECRYPT_VALUE2(a,c)			GlobalFunc::DecryptValue(&(a),c)
	#else
		#define DECRYPT_VALUE2(a,c)			a
	#endif
#else
	#define ENCRYPT_VALUE(a,c)				__noop
	#define DECRYPT_VALUE(a,c)				a
	#define DECRYPT_REALVALUE(a,c)			a
	#define DECRYPT_INTVALUE(a,c)			a
	#define DECRYPT_STRING(out,in,len,c)	i3::string_ncopy_nullpad((char*)(out),(char*)(in),(len));
	#define DECRYPT_U8(a,c)					a
	#define DECRYPT_VALUE1(a,c)				a
	#define DECRYPT_VALUE2(a,c)				a
#endif





#if defined( I3_DEBUG)
#define		I3ASSERT_ONCE(exp)	\
	{\
		static bool b = true;\
		if ( b )\
		{\
			I3ASSERT(exp);\
			if ( ! (exp) )\
			{\
				int Rv = MessageBox(g_hWnd, _T("이 에러를 무시하려면 Yes를 누르세요."), _T("Error"), MB_YESNO);\
				if ( IDYES == Rv )\
					b = false;\
			}\
		}\
	}
#else

#define		I3ASSERT_ONCE		I3ASSERT

#endif	//defined( I3_DEBUG)

#define		I3FATAL_ONCE(x)	\
	{\
		static bool b = true;\
		if ( b )\
		{\
			I3PRINTLOG(I3LOG_FATAL, x);\
			int Rv = MessageBox(g_hWnd, _T("이 에러를 무시하려면 Yes를 누르세요."), _T("Error"), MB_YESNO);\
			if ( IDYES == Rv )\
				b = false;\
		}\
	}


#define		RV_EMPTY
#define		CHECK_ASSERT_RV(exp, ret_val)	\
	{\
		I3ASSERT(exp)\
		if (!(exp))	{return ret_val;}\
	}
#define		CHECK_ASSERT(exp)		CHECK_ASSERT_RV(exp, RV_EMPTY)

#if defined(DEV_BUILD) || defined(BUILD_RELEASE_QA_VERSION)
	void __DevLog(const char *format, ...);
#endif


#if defined(DEV_BUILD) || defined(BUILD_RELEASE_QA_VERSION)
	#define		DEV_LOG				__DevLog
#else
	#define		DEV_LOG				__noop
#endif


template<typename T> inline	
void	GetKeyData(i3RegKey * pKey, const char * pDataName, T* pValue)
{
	FIND_REG_DATA(pKey, pDataName, pValue);
}

inline	void	GetKeyData(i3RegKey * pKey, const char * pDataName, i3::rc_wstring& outVal )
{
	FIND_REG_DATA( pKey, pDataName, outVal );
}

inline	void	GetKeyDataString(i3RegKey * pKey, const char * pDataName, i3::rc_wstring& outVal)
{
	i3RegData * pData = i3Registry::FindData( pKey, pDataName );
	i3RegString *pStrData = nullptr;
	if( pData != nullptr )
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray * pArrayData = (i3RegArray*)pData;
			if( pArrayData->getElementType() == I3REG_TYPE_STRING )
			{
				if( pArrayData->getSize() == 1 )
					pStrData = (i3RegString*)(*pArrayData)[ 0 ];
				else
					pStrData = (i3RegString*)(*pArrayData)[ i3Registry::Get_LocationIndex() ];
			}
		}
		else if( pData->getDataType() == I3REG_TYPE_STRING )
		{
			pStrData = (i3RegString *)pData;
		}
	}

	if( pStrData != nullptr) // && pStrData->getValue() != nullptr)
	{
		outVal = pStrData->getValue();
	}
	else
	{
		outVal.clear();
	}
}


template<typename T>
inline	T		GetKeyData(i3RegKey * pKey, const char * pDataName)
{
	T Value;
	i3mem::FillZero(&Value, sizeof(Value));

	FIND_REG_DATA(pKey, pDataName, &Value);

	return Value;
}

// PB가 설치된 드라이브가 NTFS인지 확인
tstring	GetFileSystemName();

#endif // __GLOBAL_FUNC_H 



