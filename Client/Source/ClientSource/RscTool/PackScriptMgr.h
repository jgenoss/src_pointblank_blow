#pragma once

class PackScriptFolder;
class PackScriptFile;
struct PreprocessMessageCallback;

#include "PackScriptFileMapElement.h"


class PackScriptMgr : public i3::shared_ginst<PackScriptMgr>
{
public:
	PackScriptMgr();
	~PackScriptMgr();

	bool						LoadPackScriptFolder();
	
	PackScriptFolder*			GetPackScriptRoot() const { return m_root;  }
	
	const PackScriptFileMapElement*	FindPackScriptFileMapElement_Upper(const i3::fixed_string& strPath) const;
	const PackScriptFileMapElement*	FindPackScriptFileMapElement(const i3::rc_string& strPath) const;

	void						BuildPackScriptMap(PreprocessMessageCallback* cb);

private:
	PackScriptFolder*			m_root;
	
	i3::unordered_map<i3::fixed_string, PackScriptFileMapElement>	m_file_map;

};
