#if !defined( __I3_PERSISTANT_ELEMENT_H)
#define __I3_PERSISTANT_ELEMENT_H

#include "i3ElementBase.h"
#include "i3XML.h"
#include "itl/vector_fwd.h"
#include "string/rc_string_fwd.h"

class i3ResourceFile;
class i3XMLFile;

typedef UINT8	I3_PERSIST_STATE;
#define		I3_PERSIST_STATE_EXTERNAL_RES		0x01
#define		I3_PERSIST_STATE_LOADED				0x02
#define		I3_PERSIST_STATE_MODIFIED			0x04			// Á¤ÀÇµÇ¾îÁü

class I3_EXPORT_BASE i3PersistantElement : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3PersistantElement, i3ElementBase);
private:
	I3_PERSIST_STATE		m_PersistState = 0;
	
public:
	bool				IsExternRes(void)					{ return (m_PersistState & I3_PERSIST_STATE_EXTERNAL_RES) != 0;	}
	void				SetExternRes( bool bTrue = true )	
	{
		if( bTrue)	m_PersistState |= I3_PERSIST_STATE_EXTERNAL_RES;
		else		m_PersistState &= ~I3_PERSIST_STATE_EXTERNAL_RES;
	}

	bool				IsLoaded(void)						{ return (m_PersistState & I3_PERSIST_STATE_LOADED) != 0; }
	void				SetLoaded( bool bTrue = true )		
	{
		if( bTrue)	m_PersistState |= I3_PERSIST_STATE_LOADED;
		else		m_PersistState &= ~I3_PERSIST_STATE_LOADED;
	}

	bool				IsModified( void)					{ return (m_PersistState & I3_PERSIST_STATE_MODIFIED) != 0; }
	void				SetModified( bool bTrue = true)
	{
		if( bTrue)	m_PersistState |= I3_PERSIST_STATE_MODIFIED;
		else		m_PersistState &= ~I3_PERSIST_STATE_MODIFIED;
	}

	virtual void		OnQueryStrings( i3::vector<i3::rc_string>& StrList);
	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32		OnSave( i3ResourceFile * pResFile) = 0;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) = 0;

	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void		OnInitAfterLoad(void) {}
};



#endif
