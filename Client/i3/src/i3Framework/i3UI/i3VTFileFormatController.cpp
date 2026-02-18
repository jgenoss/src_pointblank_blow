#include "i3FrameworkPCH.h"
#include "i3VTFileFormatController.h"

#include "i3VirtualTextureDefine.h"

#include "i3UIManager.h"
#include "i3VTSubsetController.h"

#include "i3VTFileFormat.h"


i3VTFileFormatController::i3VTFileFormatController(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData) : 
	m_Owner(pOwner), m_pFFData(pFFData)
{
	
}

i3VTFileFormatController::~i3VTFileFormatController()
{

}

namespace vt_detail
{

	static void	CloseFile(__HANDLE_TYPE hFile)
	{
#if defined( I3_WINDOWS)
		::CloseHandle(hFile);
#endif
	}

	static void Unregister(I3VT_IOHANDLE * pInfo)
	{
		I3ASSERT(pInfo != nullptr);

		if (pInfo->m_hFile != INVALID_HANDLE_VALUE)
		{
			vt_detail::CloseFile(pInfo->m_hFile);
		}

		i3MemFree(pInfo);
	}

}

void	i3VTFileFormatController::RemoveAllFileHandle()					// 비가상함수..
{
	i3VTFileFormatData* pFFData = this->m_pFFData;

	I3VT_IOHANDLE * pHandle;

	for (size_t i = 0; i < pFFData->m_FileHandleList.size(); i++)
	{
		pHandle = pFFData->m_FileHandleList[i];
		vt_detail::Unregister(pHandle);
	}

	pFFData->m_FileHandleList.clear();

}

void	i3VTFileFormatController::ResetAllFileHandleOffset(UINT64 zero_idx_offset)
{
	i3VTFileFormatData* pFFData = this->m_pFFData;

	if (pFFData->m_FileHandleList.empty())
		return;

	I3VT_IOHANDLE* h = pFFData->m_FileHandleList[0];

	h->m_LastDataOffset = zero_idx_offset;

	const size_t num = pFFData->m_FileHandleList.size();

	for (size_t i = 1; i < num; ++i)
	{
		h = pFFData->m_FileHandleList[i];
		h->m_LastDataOffset = 2048;		// offset 0는 Invalid Offset으로 사용하기 때문에 2048의 offset을 항상 준다.
	}
}




namespace vt_detail
{
	static void TruncateFile(__HANDLE_TYPE hFile, UINT64 offset)
	{
#if defined( I3_WINDOWS)
		LONG lo, hi;

		lo = (LONG)(offset & 0xFFFFFFFF);
		hi = (LONG)((offset >> 32) & 0xFFFFFFFF);

		SetFilePointer(hFile, lo, &hi, FILE_BEGIN);

		SetEndOfFile(hFile);
#endif
	}

	static void Truncate(I3VT_IOHANDLE * hndl)
	{
		vt_detail::TruncateFile(hndl->m_hFile, hndl->m_LastDataOffset);
	}
}

void	i3VTFileFormatController::TruncateAllFileHandle()
{
	i3VTFileFormatData* pFFData = this->m_pFFData;

	for (size_t i = 0; i < pFFData->m_FileHandleList.size(); i++)
	{
		I3VT_IOHANDLE * pHandle = pFFData->m_FileHandleList[i];

		vt_detail::Truncate(pHandle);
	}

}
