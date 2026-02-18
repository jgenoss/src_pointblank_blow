#if !defined( __I3_GAMERES_PATH_H_)
#define	__I3_GAMERES_PATH_H_

#include "i3GameRes.h"
#include "i3GamePath.h"

class I3_EXPORT_FRAMEWORK i3GameResPath : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResPath);

protected:
	i3GamePath	*	m_pPath;

public:
	i3GameResPath();
	virtual ~i3GameResPath();

	i3GamePath	*	gePath(void)					{	return m_pPath;}
	void			setPath( i3GamePath * pPath)	{	I3_REF_CHANGE( m_pPath, pPath);}
public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};


#endif