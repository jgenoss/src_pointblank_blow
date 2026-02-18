#if !defined( __I3_NAMED_ELEMENT_H)
#define __I3_NAMED_ELEMENT_H

#include "i3PersistantElement.h"
#include "i3ResourceFile.h"


#if !defined( MAX_NAME)
#define	MAX_NAME			32
#endif

#if defined( I3_DEBUG)
// 디버그와 툴에서만 픽스드로 사용. 릴리즈는 메모리를 줄이기위해 가변으로 작동하도록 합니다. swoongoo.jung
#define FIXED_NAME_FIELD
#endif

class I3_EXPORT_BASE i3NamedElement : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3NamedElement, i3PersistantElement);
private:
	i3::rc_string	m_Name;

public:
	// 함수는 우선 이정도로 놔둔뒤..가능하면 모두 rc_string인수/리턴타입으로 바꿀것..
	const char *			GetName(void)	const {  return m_Name.c_str();  }
	const i3::rc_string&	GetNameString() const { return m_Name; }

	virtual void	SetName( const i3::rc_string& strName) { m_Name = strName; }

	bool			hasName(void) const { return !m_Name.empty(); }

	#if defined( I3_DEBUG)
	virtual void				_getInfoString( char * pszStr, const INT32 nStrMax);
	#endif

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnQueryStrings( i3::vector<i3::rc_string>& StrList) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};



#endif
