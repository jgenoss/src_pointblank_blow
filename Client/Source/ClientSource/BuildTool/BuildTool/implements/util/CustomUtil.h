#pragma once

#include "stdafx.h"

void RemoveDuplicates(std::vector<PathPtr>& inoutPathPtrList);
bool ConvertBufferToLineTexts(const std::vector<BYTE>& buff, std::vector<std::string>& out);
bool ConvertBufferToLineTexts(const std::vector<BYTE>& buff, std::vector<std::wstring>& out);
void ConvertLineTextsToBuffer(const std::vector<std::wstring>& in, std::vector<BYTE>& out);
void ConvertLineTextsToBuffer(const std::vector<std::string>& in, std::vector<BYTE>& out);
void RemoveNullTerminates(std::vector<BYTE>& inout);