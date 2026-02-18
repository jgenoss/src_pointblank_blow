#if !defined( __PROPERTY_TEXTURE_BIND_TYPE_H)
#define __PROPERTY_TEXTURE_BIND_TYPE_H

#include "i3treePropertyBase.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_IMAGE_FORMAT Property 
class CPropertyTextureBindType : public CpropertyBase
{
//For Value
private:	
	I3G_TEXTURE_BIND	m_value;
public:

//For Function
private:	
	
public:
	CPropertyTextureBindType(); 
	~CPropertyTextureBindType(); 

	I3G_TEXTURE_BIND	GetValue(void)											{ return m_value;  }
	void				SetValue(I3G_TEXTURE_BIND value)						{ m_value = value; }

	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 

#endif
