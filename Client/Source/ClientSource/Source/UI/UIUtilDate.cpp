#include "pch.h"

#include "UIUtil.h"

// __DATE__ 를 int로
i3::string GameUI::pre_macro_date_to_string()
{
	const int comfile_date_len = 12;

	// error check
	if (comfile_date_len - 1 != strlen(__DATE__))
		return "";

	const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	char s_month[5] = {0,};
	int iyear = 0, iday = 0;

	sscanf_s(__DATE__, "%s %d %d", s_month, sizeof(s_month), &iday, &iyear);
	int imonth = (strstr(month_names, s_month) - month_names) / 3 + 1;

	// 2010/12/01
	return i3::format_string("%d/%d/%d", iyear, imonth, iday);
}

// __TIME__ 을 int로
i3::string GameUI::pre_macro_time_to_string()
{
	const int comfile_time_len = 9;

	if (comfile_time_len -1 != strlen(__TIME__))
		return "";

	int ihour = 0, iminute = 0, isec = 0;
	sscanf_s(__TIME__, "%d:%d:%d", &ihour, &iminute, &isec);

	// 03:05:11
	return i3::format_string("%d:%d:%d", ihour, iminute, isec);
}