#if !defined( _OBJECT_POWERGENERATOR_H__)
#define _OBJECT_POWERGENERATOR_H__

#include "GameControledObject.h"

class CGameObjectPowerGenerator : public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectPowerGenerator, CGameControledObject);

protected:

public:
	CGameObjectPowerGenerator(void);
	virtual ~CGameObjectPowerGenerator(void);

public:
	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState( INT32 state) override;
	virtual INT32	GetControledObjectState(void) override {	return -1;}
};

#endif