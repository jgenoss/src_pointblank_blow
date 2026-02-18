#pragma once

namespace i3
{

	I3_EXPORT_BASE
	bool collect_directoryname(const i3::literal_range& strDir, i3::vector<i3::rc_string>& out_vecSubDir);

	I3_EXPORT_BASE
	bool collect_directoryname(const i3::wliteral_range& strDir, i3::vector<i3::rc_wstring>& out_vecSubDir);

}
