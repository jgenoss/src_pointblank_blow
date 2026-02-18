#if !defined( __I3_GAME_RES_H)
#define __I3_GAME_RES_H

#include "i3GameResDefine.h"

class i3GameObjBase;
class i3Framework;

class I3_EXPORT_FRAMEWORK i3GameRes : public i3ResourceObject
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3GameRes, i3ResourceObject);

protected:
	// Persistent Members
	INT32		m_Type = 0;

	i3::vector<i3GameObjBase*>		m_InstanceList;

public:
	i3GameRes(void);
	virtual ~i3GameRes(void);

	INT32		getType(void)					{ return m_Type; }
	void		setType( INT32 type)			{ m_Type = type; }

	virtual void	Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone);

	////////////////////////
	// Objects
	void			AddInstance( i3GameObjBase * pObj);			// ¹ü¿ë Object
	void			RemoveInstance( i3GameObjBase * pObj);
	void			RemoveAllInstance(void);
	INT32			getInstanceCount(void)				{ return (INT32)m_InstanceList.size(); }
	i3GameObjBase *		getInstance( INT32 idx)			{ return m_InstanceList[idx]; }

	virtual void	Create(void) {}
	virtual void	Destroy( void) {}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual void	OnLostDevice( bool bLostDevice);
	virtual void	OnRevive( i3RenderContext * pCtx);

	virtual UINT32	SaveToFile( i3Stream * pStream);
	virtual UINT32	LoadFromFile( i3Stream * pStream, INT32 nVersion);
	virtual UINT32	LoadFromFile(i3IniParser * pParser, INT32 nVersion, INT32 nType);

#if defined( I3_DEBUG)
public:
	UINT32		m_FileSize = 0;
#endif
};

#endif
