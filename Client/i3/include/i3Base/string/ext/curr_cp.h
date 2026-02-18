#pragma once

namespace i3Language {	I3_EXPORT_BASE UINT32 GetCurrentCodePage(void); }

namespace i3
{
	__forceinline unsigned int	curr_cp() { return i3Language::GetCurrentCodePage();  }
}
