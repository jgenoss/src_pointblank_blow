#pragma once

class i3UIEFramework : public i3Framework
{
	I3_CLASS_DEFINE( i3UIEFramework, i3Framework );

public:
	i3UIEFramework(void);
	virtual ~i3UIEFramework(void);

public:
	virtual void OnCreate() override;
	//virtual void OnUpdate( REAL32 fDeltaTime);
};
