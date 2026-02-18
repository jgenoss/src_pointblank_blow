#if !defined ( _I3REGISTRY_H__ )
#define _I3REGISTRY_H__

#include "i3RegData.h"
#include "i3RegKey.h"
#include "i3RegistrySet.h"

extern I3_EXPORT_SCENE i3RegistrySet * g_pRegistry;

namespace i3Registry
{
	I3_EXPORT_SCENE	UINT32		Init(const char * pszFileName);
	I3_EXPORT_SCENE bool		Save( const char * pszFileName);
	I3_EXPORT_SCENE void		End(const char * pszFileNames = nullptr);
	I3_EXPORT_SCENE void		Destroy();

	I3_EXPORT_SCENE i3RegKey	* FindKey( const char * pszKeyName, i3RegKey * pRoot = nullptr);
	I3_EXPORT_SCENE i3RegData	* FindData( i3RegKey * pKey, const char * pszDataName);

	/*통합 Pef에서 사용되는 여러 국가중 현재 사용되는 국가에 대한 인덱스를 지정한다.
		Index : 현재 사용되는 국가에 대한 인덱스
		MaxCount : 사용되는 국가에 총 수
		만든이 : 이정우(2010년 12월 15일)
	*/
	I3_EXPORT_SCENE void Set_LocationIndex(UINT32 Index, UINT32 MaxCount);

	I3_EXPORT_SCENE INT32 Get_LocationIndex();
	I3_EXPORT_SCENE INT32 Get_MaxLocationCount();


	/*Pef 데이터중에 Stamp(__MakeTime, __UpdateTime) 데이터 읽기 유무를 지정한다.
		LoadStamp : Stamp 데이터 읽기 유무
		만든이 : 이정우(2011년 5월 25일)
	*/
	I3_EXPORT_SCENE void Set_LoadStamp(bool LoadStamp);
	I3_EXPORT_SCENE bool Get_LoadStamp();

	/*스트링이 Stamp(__MakeTime, __UpdateTime) 데이터 인지 판단 한다.
		Name : 검사 할 스트링
		만든이 : 이정우(2011년 5월 25일)
	*/
	I3_EXPORT_SCENE bool IsStampString(const char* Name);
};
#endif