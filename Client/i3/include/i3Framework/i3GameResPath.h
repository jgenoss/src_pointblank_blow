#if !defined( __I3_GAMERES_PATH_H_)
#define	__I3_GAMERES_PATH_H_

#include "i3GameRes.h"
#include "i3GamePath.h"

class I3_EXPORT_FRAMEWORK i3GameResPath : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResPath, i3GameRes);

protected:
	i3GamePath	*	m_pPath = nullptr;

public:
	i3GameResPath();
	virtual ~i3GameResPath();

	i3GamePath	*	getPath(void)					{	return m_pPath;}
	void			setPath( i3GamePath * pPath)	{	I3_REF_CHANGE( m_pPath, pPath);}
public:
	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};


#endif