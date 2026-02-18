#pragma once


namespace i3
{
	
	I3_EXPORT_BASE
	bool collect_directoryname_and_filename(const i3::literal_range& strDir, i3::vector<i3::rc_string>& out_vecSubDir,
		i3::vector<i3::rc_string>& out_vecFileName);
	
	I3_EXPORT_BASE
	bool collect_directoryname_and_filename(const i3::wliteral_range& strDir, i3::vector<i3::rc_wstring>& out_vecSubDir,
		i3::vector<i3::rc_wstring>& out_vecFileName);

}
