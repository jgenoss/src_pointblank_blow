#if !defined( __I3_REG_ARRAY_H__)
#define __I3_REG_ARRAY_H__

#include "i3RegData.h"
#include <map>

//여러개의 i3RegData를 담고 있는 i3RegData 클래스 이다.
//설명	:	Name -> i3RegData::GetName();
//				Type ->  i3RegArray::getElementType();
class I3_EXPORT_SCENE i3RegArray : public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegArray, i3RegData );

protected:
	UINT m_nElementDataType = I3REG_TYPE_INT32;

	typedef std::vector< i3RegData* > RegDataBank;
	RegDataBank m_Value;
	
public:
	inline i3RegArray & operator = ( const i3RegArray & Value)
	{
		m_Value = Value.m_Value;

		return *this;
	}

	i3RegArray * get()	{	return this;}
	void		 set( i3RegArray & Value);

	std::vector<i3RegData* >* getValue()	{	return &m_Value;}
	void	setValue( const std::vector< i3RegData* > & nValue)	
	{
		m_Value = nValue;
	}

public:
	void setElementType(UINT32 Type)	{	m_nElementDataType = Type;	}
	UINT32 getElementType() const {	return m_nElementDataType;	}

	void clearRegData();
	i3RegData* createRegData() const;
	void addRegData(i3RegData * pData);

	void removeRegData(size_t Index);

	//Element에 i3RegData를 변경한다.
	//주위 : 기존에 있는 i3RegData는 메모리 해제 된다.
	void replaceRegData(size_t Index, i3RegData * Data);

	//Element에 i3RegData를 변경한다.
	//주위 : 기존에 있는 i3RegData는 메모리 해제 안 된다.
	void replaceRegData(size_t Index, INT32 nValue);
	void replaceRegData(size_t Index, REAL32 fValue);
	void replaceRegData(size_t Index, const i3::rc_wstring& val);
	void replaceRegData(size_t Index, const VEC2D & Vec2D);
	void replaceRegData(size_t Index, const VEC3D & Vec3D);
	void replaceRegData(size_t Index, const VEC4D & Vec4D);
	void replaceRegData(size_t Index, const I3COLOR & Col);
	void replaceRegData(size_t Index, const MATRIX & Matrix);

	bool isSameRegData(size_t Index, INT32 nValue);
	bool isSameRegData(size_t Index, REAL32 fValue);
	bool isSameRegData(size_t Index, const char * Value);
	bool isSameRegData(size_t Index, const VEC2D & Vec2D);
	bool isSameRegData(size_t Index, const VEC3D & Vec3D);
	bool isSameRegData(size_t Index, const VEC4D & Vec4D);
	bool isSameRegData(size_t Index, const I3COLOR & Col);
	bool isSameRegData(size_t Index, const MATRIX & Matrix);

	i3RegData* operator[](size_t Index)
	{
		I3ASSERT( Index < m_Value.size() );

		return m_Value[Index];
	}

	size_t getSize() const	{	return m_Value.size();	}

protected:
	static std::vector< std::pair< INT32, INT32 > > * m_pSkipFPBank;
	static bool m_bUpdateSkipFP;
	void _Internal_UpdateSkipFP();

	UINT32 _SaveValue(i3Stream * pStream, size_t size);

	/*Stream에서 값을 얻어 온다.
		size : Stream 에서 읽어 올 데이터 크기
		SpecificNation : 특정 국가 데이터 읽기 유무
		IsStamp :	Stamp 값 읽기 유무
	*/
	UINT32 _LoadValue(i3Stream * pStream, size_t size, bool SpecificNation, bool IsStamp = false);

public:
	static void ReleaseSkipFPBank();

public:
	i3RegArray();
	virtual ~i3RegArray();
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void GetValueByString( i3::wstring& out ) override;
};

#endif //#if !defined( __I3_REG_INT32_H__)