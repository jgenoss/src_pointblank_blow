#pragma once

#include "i3Base/itl/vector_fwd.h"
#include "i3Base/string/rc_string_fwd.h"

class i3ResourceFile;

I3_EXPORT_BASE
void CopyFromBufferToStringList(i3::vector<i3::rc_string>& out, const char *Buf, bool bClear = true);

I3_EXPORT_BASE
void CopyFromStringListToBuffer( const i3::vector<i3::rc_string>& in, char *Buf, INT32 Limit, INT32 * pOffsetTable = nullptr, bool isCRLF = true);

I3_EXPORT_BASE
INT32 GetStringListBufferSize( const i3::vector<i3::rc_string>& StrList );

I3_EXPORT_BASE
UINT32 SaveFromStringListToResourceFile(const i3::vector<i3::rc_string>& in, i3ResourceFile * pResFile);

I3_EXPORT_BASE
UINT32 LoadFromResourceFileToStringList(i3::vector<i3::rc_string>& out, i3ResourceFile * pResFile);

