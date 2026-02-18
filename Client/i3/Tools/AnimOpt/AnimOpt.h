
#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/generic_string_ncat.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/remove_end_slash.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

struct ANIMPACK_INFO
{
	bool			_b16Bits = false;
	bool			_bRemoveTrans = false;
	i3Animation *	_pAnim = nullptr;
	char			_szPath[MAX_PATH] = { 0 };
} ;


HINSTANCE GetAnimOptModuleInstance();
