#pragma once

//
//  여기에 없는 것들은 i3Base/string/ext 혹은 i3Base/string/algorithm 쪽에서 찾아서 명시적 인클루드가 필요함..
//  자주 쓰인다고 판단되면, 이쪽 헤더에 등록...
//

#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"

#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/contain_string_tail.h"

#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/itoa_comma.h"

#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/generic_string_ncat.h"

#include "i3Base/string/ext/generic_string_copy.h"
#include "i3Base/string/ext/generic_string_ncopy.h"

#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/generic_string_replace_all_copy.h"

#include "i3Base/string/ext/generic_string_size.h"

#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/erase_string.h"

#include "i3Base/string/ext/is_matched_wildcard.h"

#include "i3Base/string/ext/ltrim.h"
#include "i3Base/string/ext/rtrim.h"
#include "i3Base/string/ext/lrtrim.h"

#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/mb_to_utf8.h"
#include "i3Base/string/ext/utf8_to_mb.h"

#include "i3Base/string/ext/safe_string_copy.h"

#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/sprintf_formatter.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/format_rcstring.h"

#include "i3Base/string/ext/string_ncopy_nullpad.h"

#include "i3Base/string/ext/strtok_fast.h"			// strtok.h는 일단 배제함..

#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/compare/generic_ncompare.h"
#include "i3Base/string/compare/generic_nicompare.h"

#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/compare/generic_is_nequal.h"
#include "i3Base/string/compare/generic_is_niequal.h"

#include "i3Base/string/compare/generic_is_less.h"
#include "i3Base/string/compare/generic_is_iless.h"
#include "i3Base/string/compare/generic_is_nless.h"
#include "i3Base/string/compare/generic_is_niless.h"

#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/algorithm/to_lower.h"

