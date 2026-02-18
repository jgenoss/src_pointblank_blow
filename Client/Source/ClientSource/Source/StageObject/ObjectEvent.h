#if !defined( __OBJECT_Event_H__)
#define __OBJECT_Event_H__
#include "GameControledObject.h"

enum OBJ_EVENT_TYPE
{
	OBJ_EVENT_NONE			= 0,
	OBJ_EVENT_ESCAPE,
	OBJ_EVENT_MISSION,
	OBJ_EVENT_MISSION_TRIGGER,
	OBJ_EVENT_MAX
};

class CGameObjectEvent : public CGameControledObject
{
private:
	I3_EXPORT_CLASS_DEFINE( CGameObjectEvent, CGameControledObject);

protected:
	INT32			m_iObjEventType;
	INT32			m_iObjUseCount;

public:
	CGameObjectEvent();
	virtual ~CGameObjectEvent();

	virtual UINT32	OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;
#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif
public:
	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override {SetEnable(true);}
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;
	virtual void	SetControledObjectState( INT32 state) override { }
	virtual INT32	GetControledObjectState(void) override  {	return -1;}

	OBJ_EVENT_TYPE	getEventType( void)							{ return (OBJ_EVENT_TYPE)m_iObjEventType;}		//OBJ_EVENT_TYPE
	void			setEventType( INT32 type)					{ m_iObjEventType = type;}

	INT32			getEventUseCount( void)						{ return m_iObjUseCount;}
	void			setEventUseCount( INT32 icount)				{ m_iObjUseCount = icount;}
};

#endif
