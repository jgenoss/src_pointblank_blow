
#ifndef _OPTIEXPPLUGHANDLERIMPL_H_
#define _OPTIEXPPLUGHANDLERIMPL_H_

#include "i3OptiExpPlug.h"


class IOptiExpSettings;

class OptiExpPlugHandlerImpl : public IOptiExpPlugHandler
{
	// IOptiExpPlugHandler
public:	
	virtual void Destroy();

	virtual bool ShowExportDlg(HWND hParent);


	// OptiExpPlugHandlerImpl
public:
	OptiExpPlugHandlerImpl(OptiExpPlugCallBack* callBack);
	~OptiExpPlugHandlerImpl();

	bool Init();
	void Finalize();

	IOptiExpSettings* GetOptiExpSettings() { return m_expSettings; }
	OptiExpPlugCallBack* GetCallBack() { return m_callBack; }

protected:
	IOptiExpSettings*				m_expSettings;
	OptiExpPlugCallBack*			m_callBack;	
};



#endif
