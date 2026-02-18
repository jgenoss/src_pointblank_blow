
#ifndef _OPTIEXPSETTINGS_H_
#define _OPTIEXPSETTINGS_H_

#include "IOptiExpSettings.h"

#include <string>
#include <hash_map>


class OptiExpSettings : public IOptiExpSettings
{
	friend class IOptiExpSettings;

public:
	OptiExpSettings();
	virtual ~OptiExpSettings();

	// implement IOptiExpSettings
	virtual const char* GetTitle() = 0;

	virtual INT32 GetNumExportOperation();

	virtual const char* GetExportOperationTitle(INT32 index);
	virtual OptiExpTaskVec* GetExportOperTasks(INT32 operIndex);
	virtual void ApplyFileName(INT32 operIndex, char* name) {}

	void FileName_ApplySuffix(char* name, const char* suffix, const char* ext);

protected:
	virtual void GenerateOperation();
	void ClearOperation();

protected:
	i3::vector<i3::string>							m_operTitle;

	typedef i3::vector<OptiExpTaskVec*>				OperVec;
	OperVec											m_oper;

	static OptiExpSettings**						ms_expSettings;
	static INT32									ms_numSettings;
};


class OptiExpSettingsWorld : public OptiExpSettings
{
public:
	OptiExpSettingsWorld();
	virtual const char* GetTitle();

	virtual void ApplyFileName(INT32 operIndex, char* name);
};


class OptiExpSettingsCharacter : public OptiExpSettings
{
public:
	OptiExpSettingsCharacter();
	virtual const char* GetTitle();
};


#endif
