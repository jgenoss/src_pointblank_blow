#if !defined( __I3_GAME_RES_H)
#define __I3_GAME_RES_H

#define I3_GAMERES_STYLE_COMMON			0x00000001
#define	I3_GAMERES_STYLE_EXTERNAL		0x00000002

#include "i3GameResDefine.h"

class i3GameObjBase;
class i3Framework;

class I3_EXPORT_FRAMEWORK i3GameRes : public i3ResourceObject
{
	I3_ABSTRACT_CLASS_DEFINE( i3GameRes);

protected:
	// Persistent Members
	UINT32		m_Style;
	INT32		m_Type;

	i3List		m_InstanceList;

public:
	i3GameRes(void);
	virtual ~i3GameRes(void);

	void		setStyle( UINT32 style)			{ m_Style = style; }
	UINT32		getStyle(void)					{ return m_Style; }
	void		addStyle( UINT32 mask)			{ m_Style |= mask; }
	void		removeStyle( UINT32 mask)		{ m_Style &= ~mask; }
	bool		isStyle( UINT32 mask)			{ return (m_Style & mask) == mask; }
	bool		isCommonResource(void)			{ return isStyle( I3_GAMERES_STYLE_COMMON); }
	bool		isExternal(void)				{ return isStyle( I3_GAMERES_STYLE_EXTERNAL); }

	INT32		getType(void)					{ return m_Type; }
	void		setType( INT32 type)			{ m_Type = type; }

	virtual void	Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone);

	////////////////////////
	// Objects
	void			AddInstance( i3GameObjBase * pObj);			// ¹ü¿ë Object
	void			RemoveInstance( i3GameObjBase * pObj);
	void			RemoveAllInstance(void);
	INT32			getInstanceCount(void)				{ return m_InstanceList.GetCount(); }
	i3GameObjBase *		getInstance( INT32 idx)			{ return (i3GameObjBase *) m_InstanceList.Items[idx]; }

	virtual void	Create(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual UINT32	SaveToFile( i3Stream * pStream);
	virtual UINT32	LoadFromFile( i3Stream * pStream, INT32 nVersion);
};

#endif
