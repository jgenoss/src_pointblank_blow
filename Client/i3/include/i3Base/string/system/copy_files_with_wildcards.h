#pragma once

#include "i3Base/itl/range/literal_range.h"
#include "i3Base/itl/vector.h"

namespace i3
{
	
	I3_EXPORT_BASE 
	void copy_files_with_wildcard(const i3::literal_range& src_dir,
	const i3::vector<i3::literal_range>& wildcard_file_list,
	const i3::literal_range& dest_dir, bool recursive = true, bool overwrite_enable = true);
	
	I3_EXPORT_BASE
	void copy_files_with_wildcard(const i3::wliteral_range& src_dir,
	const i3::vector<i3::wliteral_range>& wildcard_file_list,
	const i3::wliteral_range& dest_dir, bool recursive = true, bool overwrite_enable = true);
	
}