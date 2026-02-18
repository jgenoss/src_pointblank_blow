#pragma once

#include "ItemToolFrameWork.h"

class cItemToolSubFrameWork : public cItemToolFrameWork
{
	
public:
	I3_CLASS_DEFINE(cItemToolSubFrameWork, cItemToolFrameWork);

	cItemToolSubFrameWork(void);
	virtual ~cItemToolSubFrameWork(void);

public:
	virtual void OnCreate();
	virtual BOOL Create(i3Viewer * pViewer);
	virtual void OnUpdate( RT_REAL32 timeDelta);

	virtual void OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void OnMouseMove(UINT32 nFlag, POINT point);



};
