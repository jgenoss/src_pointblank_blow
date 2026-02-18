#if !defined( __I3_NAMED_ELEMENT_H)
#define __I3_NAMED_ELEMENT_H

#include "i3PersistantElement.h"
#include "i3MetaTemplate.h"
#include "i3ResourceFile.h"
#include "i3List.h"

#if !defined( MAX_NAME)
#define	MAX_NAME			32
#endif

class I3_EXPORT_BASE i3NamedElement : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3NamedElement);
private:
	/*
	char*			m_pszName;
	UINT32			m_nNameBufSize;
	BOOL			m_bOptimizeNameBuf;

	void			_FreeNameBuf();
	void			_SetName( const char * pszName );
	*/
	char			m_szName[256];

public:
	i3NamedElement(void);
	virtual ~i3NamedElement(void);

	const char *	GetName(void)							{ return (const char *) m_szName; }
	virtual void	SetName( const char * pszName );
	//void			SetNameBufSize( UINT32 nSize );
	//void			SetOptimizeNameBuf( BOOL bTrue )		{ /*m_bOptimizeNameBuf = bTrue;*/ }
	//BOOL			IsOptimizeNameBuf(){ return m_bOptimizeNameBuf; }

	bool			hasName(void)							{ return m_szName[0] != 0; }

	#if defined( I3_DEBUG)
	virtual void				_getInfoString( char * pszStr, const INT32 nStrMax);
	#endif

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnQueryStrings( i3StringList * pStrList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
