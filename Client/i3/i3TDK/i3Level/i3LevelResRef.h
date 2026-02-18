#if !defined( __I3LV_RESREF_H)
#define __I3LV_RESREF_H

#include "i3LevelRes.h"

#define		I3LV_RESREF_ICON_GROUPROOT		0x00000001

class i3LevelScene;

class I3_EXPORT_TDK i3LevelResRef : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResRef, i3TreeNode);
protected:
	i3LevelRes *			m_pRes = nullptr;
	UINT32					m_IconStyle = 0;

public:
	virtual ~i3LevelResRef(void);

	i3LevelRes *	getRes(void)			{ return m_pRes; }
	void			setRes( i3LevelRes * pRes)
	{
		I3_REF_CHANGE( m_pRes, pRes);
	}

	UINT32			getIconStyle(void)				{ return m_IconStyle; }
	void			setIconStyle( UINT32 style)		{ m_IconStyle = style; }

	const char *			getName(void) const
	{
		if( m_pRes != nullptr)
			return m_pRes->GetName();

		return GetName();
	}

	const i3::rc_string&	getNameString() const 
	{
		if (m_pRes != nullptr)
			return m_pRes->GetNameString();
		return GetNameString();
	}

	void			BindScene( i3LevelScene * pScene);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
