#if !defined(_I3REG_KEY_H__)
#define _I3REG_KEY_H__

#include "i3RegINT32.h"
#include "i3RegREAL32.h"
#include "i3RegString.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"
#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"
#include "i3RegArray.h"
#include "i3RegData.h"
//#include <deque>

//
// std::deque가 다소 느리게 작동되는 것으로 여겨져서..  ( deque자체의 문제라기보다는, deque
// 에서 std::allocator의 진부한 메모리할당의 문제가 있다.)
// i3::vector로 일단 바꿔본다..(2014.03.14.수빈)
// 현재, std::deque의 push_front 인터페이스는 __maketime/updatetime류에만 쓰이며, 이것은 저장용도이므로...
// 저장을 위한 퍼포먼스를 염두에 둘 필요는 없으며, 실제로 퍼포먼스 차이가 크지 않다..
// 

class I3_EXPORT_SCENE i3RegKey :	public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3RegKey, i3TreeNode);

protected:

	i3::vector<i3RegData*> m_vecValueList;

	//Stamp(__MakeTime, __UpdateTime) 데이터 용 리스트
	i3::vector<i3RegData*> m_StampValueList;

public:
	i3RegKey(void);
	virtual ~i3RegKey(void);

	/*Stamp(__MakeTime, __UpdateTime) 데이터를 일괄 삭제한다.
		만든이 : 이정우(2011년 5월 25일)
	*/
	void ClearStampList();

	//Single 용
	void		AddData( const char* szName, INT32 val);
	void		AddData( const char* szName, REAL32 val);

	void		AddData( const char* szName, const i3::rc_wstring& val);
	
	void		AddData( const char* szName, VEC2D val);
	void		AddData( const char* szName, VEC3D val);
	void		AddData( const char* szName, VEC4D val);
	void		AddData( const char* szName, COLOR val);
//	void		AddData( const char* szName, MATRIX val);

	//Multi 용
	void		AddDataMulti( const char* szName, INT32 val);
	void		AddDataMulti( const char* szName, REAL32 val);
	void		AddDataMulti( const char* szName, const i3::rc_wstring& val);
	void		AddDataMulti( const char* szName, VEC2D val);
	void		AddDataMulti( const char* szName, VEC3D val);
	void		AddDataMulti( const char* szName, VEC4D val);
	void		AddDataMulti( const char* szName, COLOR val);
//	void		AddDataMulti( const char* szName, MATRIX val);

	//Single, Multi 용
	//설명 : Multi 일 경우 게임상에 지역 인덱스에 대한 값을 얻게 된다.
	//참조 : i3Registry::Set_LocationIndex();, i3Registry::Get_LocationIndex();
	bool GetData( const char * pszName, INT32* val);
	bool GetData( const char * pszName, REAL32* val);
	bool GetData( const char * pszName, i3::rc_wstring& outVal);
	bool GetData( const char * pszName, VEC2D* val);
	bool GetData( const char * pszName, VEC3D* val);
	bool GetData( const char * pszName, VEC4D* val);
	bool GetData( const char * pszName, COLOR* val);
	//bool GetData( const char * pszName, MATRIX* val);

	//Multi 용
	//Index : 특정 지역 인덱스
	bool GetDataMulti( const char * pszName, INT32* val, size_t Index);
	bool GetDataMulti( const char * pszName, REAL32* val, size_t Index);
	bool GetDataMulti( const char * pszName, i3::rc_wstring& outVal, size_t Index);
	bool GetDataMulti( const char * pszName, VEC2D* val, size_t Index);
	bool GetDataMulti( const char * pszName, VEC3D* val, size_t Index);
	bool GetDataMulti( const char * pszName, VEC4D* val, size_t Index);
	bool GetDataMulti( const char * pszName, COLOR* val, size_t Index);
	//bool GetDataMulti( const char * pszName, MATRIX* val, size_t Index);

	void		AddData( i3RegData * p);
	void		SetData( i3RegData * p, INT32 nIdx);
	i3RegData *	GetData( INT32 nIdx);

	void		RemoveData( i3RegData * p);
	void		RemoveData( INT32	nIdx);

	i3RegData * FindDataByName( const char * pszName) const;
	i3::vector<i3RegData*>::iterator FindItorByName( const char * pszName);

	i3::vector<i3RegData*>* GetValueList() { return &m_vecValueList; }
	INT32		GetDataCount();

	void PushStemValue(i3RegData* pData) { m_StampValueList.push_back(pData); }

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32  OnLoad( i3ResourceFile * pResFile) override;
};

class I3_EXPORT_SCENE i3RegKeyFile : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3RegKeyFile, i3NamedElement);
protected:
	i3RegKey	*	m_pRootKey = nullptr;

public:
	virtual ~i3RegKeyFile();

	i3RegKey	* getRoot()	{	return m_pRootKey;}
	void		setRoot( i3RegKey * pKey);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual UINT32	OnExportSQLCreate( const char * pFileName ); 
	virtual UINT32	OnExportSQLUpdate( const char * pFileName ); 
};
#endif // _I3REGKEY_H__