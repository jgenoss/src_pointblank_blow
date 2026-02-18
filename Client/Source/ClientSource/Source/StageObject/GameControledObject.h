#if !defined( _CGAMECONTROLED_OBJECT_H__)
#define _CGAMECONTROLED_OBJECT_H__

class CGameControledObject : public i3Object
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( CGameControledObject, i3Object);

protected:
	bool			m_bManualUpdate;

public:
	CGameControledObject(void);
	virtual ~CGameControledObject( void);

public:
	virtual void	InitObject()	= 0;
	virtual void	ResetObject()	= 0;
	virtual void	EnableObject()	= 0;
	virtual void	ProcessObject( REAL32 rDeltaSeconds);

	virtual void	SetControledObjectState( INT32 state)=0;
	virtual INT32	GetControledObjectState(void) {	return -1;}

	virtual void	ProcessIntrude( const GAMEINFO_OBJECT_NETDATA * pNetData) {}

	bool			isManualUpdate( void)			{ return m_bManualUpdate; }
};

#endif