#if !defined( __I3_GAMERES_OBJECT_H_)
#define __I3_GAMERES_OBJECT_H_

#include "i3GameRes.h"
#include "i3Object.h"

class I3_EXPORT_FRAMEWORK i3GameResObject : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResObject);

protected:
	i3Object	*	m_pObject;

public:
	i3GameResObject();
	virtual ~i3GameResObject();

	void			SetGameObject( i3Object * pObject);
	i3Object	*	getGameObject()	{	return m_pObject;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif