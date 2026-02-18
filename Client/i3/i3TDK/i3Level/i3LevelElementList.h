#if !defined( __I3LV_ELEMENT_LIST_H)
#define __I3LV_ELEMENT_LIST_H

#include "i3LevelElement.h"

class I3_EXPORT_TDK  i3LevelElementList : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelElementList, i3PersistantElement);
protected:
	i3::vector<i3LevelElement*>		m_ElmList;

public:
	virtual ~i3LevelElementList(void);

	void				add( i3LevelElement * pElm);
	void				removeAll(void);

	INT32				getCount(void)				{ return (INT32)m_ElmList.size(); }
	i3LevelElement *	get( INT32 idx)				{ return m_ElmList[idx]; }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
