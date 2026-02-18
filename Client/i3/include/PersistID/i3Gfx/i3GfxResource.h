#if !defined( __I3G_RESOURCE_H)
#define __I3G_RESOURCE_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3PersistantElement.h"
#include "i3GfxDefine.h"

class i3RenderContext;

enum I3G_RESTORE_TYPE
{
	I3G_RESTORE_NONE	= -1,
	I3G_RESTORE_FILE,
	I3G_RESTORE_MEM,
};

class I3_EXPORT_GFX i3GfxResource : public i3ResourceObject
{
	I3_ABSTRACT_CLASS_DEFINE( i3GfxResource);

protected:
	i3GfxResource *			m_pNextRes;

	UINT32					m_Usage;
	I3G_RESOURCE_TYPE		m_ResType;

	// Volatile member
	I3G_RESTORE_TYPE		m_RestoreType;
	char					m_szResPath[260];
	char *					m_pResData;
	UINT32					m_ResDataSize;
	UINT64					m_ResOffset;
public:
	i3GfxResource(void);
	virtual ~i3GfxResource(void);

	void				LinkToResList(void);
	void				UnlinkFromResList(void);
	i3GfxResource *		getNextRes(void)						{ return m_pNextRes; }

	UINT32				GetUsageFlag(void)						{ return m_Usage; }
	virtual void		SetUsageFlag( UINT32 flag);

	I3G_RESTORE_TYPE	getRestoreType(void)					{ return m_RestoreType; }
	void				setRestoreType( I3G_RESTORE_TYPE type)	{ m_RestoreType = type; }

	char *				getResourceLoadPath(void)					{ return m_szResPath; }
	void				setResourceLoadPath( const char * pszPath)	{ i3String::Copy( m_szResPath, pszPath); }

	char *				getRestoreData(void)						{ return m_pResData; }
	void				setRestoreData( char * pData)				{ m_pResData = pData; }

	UINT64				getResourceLoadOffset(void)					{ return m_ResOffset; }
	void				setResourceLoadOffset( UINT64 off)			{ m_ResOffset = off; }

	void				setRestorePoint( i3Stream * pStream);

	virtual BOOL		Destroy( bool bLostDevice);
	virtual BOOL		isDestroyed( void);
	virtual BOOL		Revive( i3RenderContext * pCtx);

	I3G_RESOURCE_TYPE	GetType(void)							{ return m_ResType; }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	static i3GfxResource *		getHeadOfGfxResource(void);

#if defined( I3_DEBUG)
	static void			DumpTextures(void);
#endif
};

#endif
