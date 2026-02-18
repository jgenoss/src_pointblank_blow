
#ifndef _IOPTIEXPSETTINGS_H_
#define _IOPTIEXPSETTINGS_H_

#include "OptiExpTask.h"

class IOptiExpSettings
{
public:
	static void CreateExpSettings();
	static void DestroyExpSettings();

	static IOptiExpSettings* GetExpSettings(INT32 index);
	static INT32 GetNumExpSettings();

	virtual const char* GetTitle() = 0;

	virtual INT32 GetNumExportOperation() = 0;
	virtual const char* GetExportOperationTitle(INT32 index) = 0;
	virtual OptiExpTaskVec* GetExportOperTasks(INT32 index) = 0;

	virtual void ApplyFileName(INT32 operIndex, char* name) = 0;
};


#endif
