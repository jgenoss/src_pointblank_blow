#if !defined( __I3_LEVEL_GROUP_H)
#define __I3_LEVEL_GROUP_H

#include "i3LevelElement3D.h"

class i3LevelElement3D;

class I3_EXPORT_TDK i3LevelGroup : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelGroup, i3NamedElement);
protected:
	i3::vector<i3LevelElement3D*>	m_ElmList;

public:
	virtual ~i3LevelGroup(void);

	INT32					getElementCount(void)					{ return (INT32)m_ElmList.size(); }
	i3LevelElement3D *		getElement( INT32 idx)					{ return m_ElmList[idx]; }
	void					AddElement( i3LevelElement3D * pObj);
	void					RemoveElement( i3LevelElement3D * pObj);
	INT32					FindElement( i3LevelElement3D * pObj);
	void					RemoveAllElement(void);

	void					Select(void);
	void					Unselect(void);

	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
};

#endif
