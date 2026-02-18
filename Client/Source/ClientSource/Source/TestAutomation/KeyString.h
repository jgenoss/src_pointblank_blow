#pragma once

#if defined( TEST_AUTOMATION)

namespace CaseRun
{

	I3I_KEY			GetKey( const wchar_t * pszName);
	const wchar_t *	GetKeyName(I3I_KEY key);
}

#endif
