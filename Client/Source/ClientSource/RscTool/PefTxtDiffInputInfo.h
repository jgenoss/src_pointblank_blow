#pragma once

enum PathMode;

struct PefTxtDiffInputInfo
{
	PathMode		mode[2];
	i3::rc_string	rel_path[2];  
	int				revision[2];
};
