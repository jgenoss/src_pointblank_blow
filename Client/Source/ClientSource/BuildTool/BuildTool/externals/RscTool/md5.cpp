#include "stdafx.h"
#include "md5.h"

static const DWORD	ReadBlockSize = 16384;
#define	ENC_READ_UNIT	2048 // ENC용으로 i3PackFile.cpp 참고한다.


bool GetMD5_Live(i3PackVolume* vol, const i3::string& rel_file_path, Crypto::MD5Handle md5h,
	Crypto::MD5Result& out, bool& is_enc)
{

	i3FileStream* stream = vol->getCurrThreadStream();

	if (true != stream->IsOpened())
	{
		const i3::rc_string& strFileName = vol->getVolumeFilename();

		if (true == strFileName.empty())
		{
			return false;
		}

		if (stream->Open(strFileName, STREAM_READ | STREAM_SHAREREAD) == FALSE)
		{
			return false;
		}
	}

	// LIVE의 경우 되도록 빠르게 i3Pack파일의 내부 파일 포인터 위치를 검색하는게 좋지만...
	// 일단은 신경 쓰지 않는다..
	// ENC여부에 의해서 분기해야한다..
	if (!Crypto::MD5_Init(md5h))
		return false;

	I3_PACK_FILE_REF* file_info = NULL;
	i3PackNode* root = vol->getRoot();

	if (root == NULL)
		return false;

	i3PackNode* found_node = root->FindPath(rel_file_path.c_str(), &file_info);

	if (found_node == NULL || file_info == NULL)
		return false;

	is_enc = (file_info->m_Attr & I3_PACK_FILE_ATTR_SECURE) != 0;
	
	stream->SetPosition(file_info->m_Offset, STREAM_BEGIN);

	bool bSuccess = true;
	if (is_enc)
	{
		i3::svector<BYTE>	buff(ENC_READ_UNIT);
		const DWORD FileSize = file_info->m_Size;
		INT32 to_read_size;		INT32 read_size;

		for (DWORD i = 0; i < FileSize;)
		{
			to_read_size = min(ENC_READ_UNIT, FileSize - i);

			read_size = stream->Read(&buff[0], to_read_size);

			if (read_size == STREAM_ERR)
			{
				bSuccess = false;	break;
			}

			BitRotateDecript(&buff[0], read_size, 3);
			Crypto::MD5_Update(md5h, &buff[0], read_size);

			i += to_read_size;
		}

		stream->SetPosition(0, STREAM_BEGIN);
	}
	else
	{
		const DWORD dwFileSize = file_info->m_Size;		// 귀찮아서 파일사이즈 구함..
		const DWORD numFixedBlock = dwFileSize / ReadBlockSize;
		const DWORD remBufferSize = dwFileSize % ReadBlockSize;
		INT32 read_size;

		i3::svector<BYTE>	buff(ReadBlockSize);

		for (DWORD i = 0; i < numFixedBlock; ++i)
		{
			read_size = stream->Read(&buff[0], ReadBlockSize);

			if (read_size == STREAM_ERR)
			{
				bSuccess = false;	break;
			}

			Crypto::MD5_Update(md5h, &buff[0], read_size);
		}

		if (remBufferSize)
		{
			read_size = stream->Read(&buff[0], remBufferSize);
			if (read_size == STREAM_ERR)
				bSuccess = false;
			else
				Crypto::MD5_Update(md5h, &buff[0], read_size);
		}
	}

	if (bSuccess)
		Crypto::MD5_Final(md5h, out);

	return bSuccess;
}

//
// 일단 16K단위로 읽고 끊는 작업을 반복처리한다.
//


bool GetMD5_MediaQA(const i3::wstring& base_path, const i3::string& rel_file_path,
	Crypto::MD5Handle md5h, Crypto::MD5Result& out)
{

	if (!Crypto::MD5_Init(md5h))
		return false;

	i3::wstring wrel_file_path;
	i3::mb_to_utf16(rel_file_path, wrel_file_path);

	i3::wstring full_path = base_path;
	full_path += L'/';	full_path += wrel_file_path;

	DWORD dwFileAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	HANDLE hFile = ::CreateFileW(full_path.c_str(), dwFileAccess, dwShareMode,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	const DWORD dwFileSize = ::GetFileSize(hFile, NULL);		// 귀찮아서 파일사이즈 구함..
	const DWORD numFixedBlock = dwFileSize / ReadBlockSize;
	const DWORD remBufferSize = dwFileSize % ReadBlockSize;

	i3::svector<BYTE>	buff(ReadBlockSize);
	DWORD dwReadSize;

	for (DWORD i = 0; i < numFixedBlock; ++i)
	{
		BOOL res = ::ReadFile(hFile, &buff[0], ReadBlockSize, &dwReadSize, NULL);

		if (res == FALSE)
		{
			::CloseHandle(hFile);
			return false;
		}

		Crypto::MD5_Update(md5h, &buff[0], dwReadSize);
	}

	if (remBufferSize)
	{
		BOOL res = ::ReadFile(hFile, &buff[0], remBufferSize, &dwReadSize, NULL);
		if (res == FALSE)
		{
			::CloseHandle(hFile);
			return false;
		}

		Crypto::MD5_Update(md5h, &buff[0], dwReadSize);
	}

	::CloseHandle(hFile);

	Crypto::MD5_Final(md5h, out);

	return true;
}