#if !defined( _I3LEV_RES_ACTION_H__)
#define _I3LEV_RES_ACTION_H__

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResEvent : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResEvent, i3LevelRes);

protected:
	i3TimeEventGen * m_pTimeSequence = nullptr;

public:
	i3LevelResEvent(void);
	virtual ~i3LevelResEvent(void);

public:
	i3TimeEventGen	*	getTimeEvent(void)	{	return m_pTimeSequence;}
	void				setTimeEvent( i3TimeEventGen * pEvent)	{	I3_REF_CHANGE( m_pTimeSequence, pEvent);}

	void						Load( const char * pszPath);

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual bool				OnValidateBrief( bool bForce) override;
	virtual bool				OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual bool				OnSerialize(void) override;

	virtual void				OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual void				CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual i3GameRes *			CreateGameRes(void) override;
	virtual UINT32				OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32				OnLoad( i3ResourceFile * pResFile) override;
};

#endif //#define _I3LEV_RES_ACTION_H__
