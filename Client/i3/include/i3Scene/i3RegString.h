#if !defined( _I3REG_STRING_H__)
#define _I3REG_STRING_H__

#include "i3RegData.h"
#include "i3Base/string/compare/generic_icompare.h"

class I3_EXPORT_SCENE i3RegString : public i3RegData
{
	I3_EXPORT_CLASS_DEFINE( i3RegString, i3RegData);

protected:
	i3::rc_wstring  m_wstrString;			// 완전히 변경함.. (일단 완전 변경 후..  메모리 소요를 이유로 들어 별도 스트링 타입 요구되면, 그때 별도의 타입으로 MB스트링 처리할 것임)(2014.07.01.수빈)
	
public:


//	inline bool operator ==( const i3RegString & value) const
//	{
//		return i3::generic_is_iequal( value.m_wstrString, m_wstrString);				// 대소문자 구별을 원래 해야되는데...일단 방치..(이건 나중에 다시 고쳐야한다..)		
//	}																					// 아마도 MFC툴이나 게임쪽에서 리스트콘트롤이나 컨테이너 정렬을 위해 쓰는걸수도 있다. (잘못된 사용) (2014.07.01.수빈)

//	inline bool	operator != ( const i3RegString & value) const
//	{
//		return !i3::generic_is_iequal( value.m_wstrString, m_wstrString);
//	}

	inline i3RegString & operator = ( const i3RegString & Value)
	{
		if( this != &Value)
		{
			Set( Value.m_wstrString); 
		}
		return *this;
	}

	inline i3RegString & operator = ( const i3::rc_wstring& value)
	{
		Set(value);
		return *this;
	}

//	inline bool	operator < ( const i3RegString & value) const
//	{
//		return i3::generic_icompare(m_wstrString,  value.m_wstrString) < 0;				// 모두 잘못된 사용임.방치..(대소문자 구별을 원래 해야한다..)(2014.07.01.수빈)
//	}

//	inline bool	operator > ( const i3RegString & value) const
//	{
//		return i3::generic_icompare(m_wstrString,  value.m_wstrString) > 0;				// 모두 잘못된 사용임.방치..(대소문자 구별을 원래 해야한다..)(2014.07.01.수빈)
//	}

	void					Set( const i3::rc_wstring& strString);

//	const i3::rc_string&	getString() const { return m_wstrString; }
//	const char*				getValue() const	{	return m_strString.c_str();}		// 이제 리터럴 리턴은 다 막는다...

	const i3::rc_wstring&	getValue() const { return m_wstrString; }
	void					setValue(const i3::rc_wstring& Value) { Set( Value); }
	
public:
	i3RegString();
	i3RegString( const i3::rc_wstring& strString);
	i3RegString( const i3RegString &OtherString);

	virtual ~i3RegString();
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void	GetValueByString( i3::wstring& out ) override;
};

#endif	// _I3REG_DATA_H__