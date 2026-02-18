#pragma once

#include "../Crypto/MD5.h"

bool GetMD5_Live(i3PackVolume* vol, const i3::string& rel_file_path, Crypto::MD5Handle md5h,
	Crypto::MD5Result& out, bool& is_enc);

bool GetMD5_MediaQA(const i3::wstring& base_path, const i3::string& rel_file_path,
	Crypto::MD5Handle md5h, Crypto::MD5Result& out);