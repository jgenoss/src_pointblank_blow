#if !defined( __I3G_RESOURCE_H)
#define __I3G_RESOURCE_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3PersistantElement.h"
#include "i3GfxDefine.h"
#include "i3Base/string/ext/safe_string_copy.h"

class i3RenderContext;

enum I3G_RESTORE_TYPE
{
	I3G_RESTORE_NONE	= -1,
	I3G_RESTORE_FILE,
	I3G_RESTORE_MEM,
};

typedef void (*CALLBACK_EXTERNAL_OUTOFMEMORY)(void);

#if defined( I3_DEBUG)
struct I3G_RES_STAT
{
	INT32		m_TotalVRAM = 0;
	INT32		m_TexCount = 0;
	UINT32		m_TexSize = 0;

	INT32		m_RTCount = 0;
	UINT32		m_RTSize = 0;

	INT32		m_VBCount = 0;
	UINT32		m_VBSize = 0;

	INT32		m_IBCount = 0;
	UINT32		m_IBSize = 0;

	INT32		m_ShaderCount = 0;
	UINT32		m_ShaderSize = 0;

	void		reset(void)
	{
		m_TotalVRAM = 0;

		m_TexCount = 0;
		m_TexSize = 0;

		m_RTCount = 0;
		m_RTSize = 0;

		m_VBCount = 0;
		m_VBSize = 0;

		m_IBCount = 0;
		m_IBSize = 0;

		m_ShaderCount = 0;
		m_ShaderSize = 0;
	}
};
#endif

class I3_EXPORT_GFX i3GfxResource : public i3ResourceObject
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3GfxResource, i3ResourceObject);

protected:
	i3GfxResource *			m_pNextRes = nullptr;

	UINT32					m_Usage = 0;
	I3G_RESOURCE_TYPE		m_ResType = I3G_RESOURCE_TYPE_NONE;

	// Volatile member
	I3G_RESTORE_TYPE		m_RestoreType = I3G_RESTORE_NONE;
	i3::rc_string			m_strResPath;
	char *					m_pResData = nullptr;
	UINT32					m_ResDataSize = 0;
	UINT64					m_ResOffset = 0;
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

	const char *		getResourceLoadPath(void) const			{ return m_strResPath.c_str(); }
	void				setResourceLoadPath( const char * pszPath)	{ m_strResPath = pszPath; }
	void				setResourceLoadPath(const i3::rc_string& strPath) { m_strResPath = strPath; }

	char *				getRestoreData(void)						{ return m_pResData; }
	void				setRestoreData( char * pData)				{ m_pResData = pData; }

	UINT64				getResourceLoadOffset(void)					{ return m_ResOffset; }
	void				setResourceLoadOffset( UINT64 off)			{ m_ResOffset = off; }

	void				setRestorePoint( i3Stream * pStream);

	virtual bool		Destroy( bool bLostDevice);
	virtual bool		isDestroyed(void) { return true; }
	virtual bool		Revive( i3RenderContext * pCtx);

	virtual void		MakeRestoreData(void) {}

	I3G_RESOURCE_TYPE	GetType(void)							{ return m_ResType; }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	static i3GfxResource *		getHeadOfGfxResource(void);

	static void			DeleteStaticMember();

	static				CALLBACK_EXTERNAL_OUTOFMEMORY 	m_CallBackExternalOutOfMemory;

	static void			setExternalOutOfMemory( CALLBACK_EXTERNAL_OUTOFMEMORY proc)	{ m_CallBackExternalOutOfMemory = proc;}

#if defined( I3_DEBUG)
	static void			DumpTextures(void);
	static void			DumpVertexArrays(void);
	static void			DumpIndexArrays( void);
	static void			Dump( void);

	virtual	void		OnQueryStat(I3G_RES_STAT * pInfo) {}
	static void			GetStat( I3G_RES_STAT * pInfo);

protected:
	SYSTEMTIME			m_CreateTime;
#endif
};

#endif
