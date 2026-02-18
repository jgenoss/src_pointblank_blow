#if !defined( __I3_PERSISTANT_ELEMENT_H)
#define __I3_PERSISTANT_ELEMENT_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"
#include "i3XML.h"
#include "itl/vector_fwd.h"

class i3ResourceFile;
class i3StringList;
class i3XMLFile;

#define		I3_PERSIST_STATE_EXTERNAL_RES		0x00000001
#define		I3_PERSIST_STATE_LOADED				0x00000002
#define		I3_PERSIST_STATE_MODIFIED			0x10000000

class I3_EXPORT_BASE i3PersistantElement : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3PersistantElement);
private:
	UINT32		m_PersistState;

protected:

public:
	i3PersistantElement(void);
	virtual ~i3PersistantElement(void);

	BOOL				IsExternRes(void)					{ return (m_PersistState & I3_PERSIST_STATE_EXTERNAL_RES) != 0;	}
	void				SetExternRes( BOOL bTrue = TRUE )	
	{
		if( bTrue)
			m_PersistState |= I3_PERSIST_STATE_EXTERNAL_RES;
		else
			m_PersistState &= ~I3_PERSIST_STATE_EXTERNAL_RES;
	}

	BOOL				IsLoaded(void)						{ return (m_PersistState & I3_PERSIST_STATE_LOADED) != 0; }
	void				SetLoaded( BOOL bTrue = TRUE )		
	{
		if( bTrue)
			m_PersistState |= I3_PERSIST_STATE_LOADED;
		else
			m_PersistState &= ~I3_PERSIST_STATE_LOADED;
	}

	virtual void		OnQueryStrings( i3StringList * pStrList);
	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32		OnSave( i3ResourceFile * pResFile) = 0;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) = 0;

	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void		OnInitAfterLoad(void);
};

#endif
