#ifndef __MODULE_TASK_H__
#define __MODULE_TASK_H__

class CModuleFileManager;
class CModuleNetManager;

class cModuleTask
{
	CModuleFileManager*		m_pFileManager;
	CModuleNetManager*		m_pNetManager;

public		: 
	cModuleTask(); 
	virtual ~cModuleTask(); 

	INT32					Create();
	void					Destroy();
};

extern cModuleTask * g_pModuleTask; 

#endif
