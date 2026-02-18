#include "i3FrameworkPCH.h"

#include "i3VTFileFormat.h"

#include "i3VirtualTexture.h"
#include "i3VTFileFormatController.h"
#include "i3VTFileFormatController_Legacy.h"
#include "i3VTFileFormatUtil.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"

#include "i3UIFilePathMgr.h"
#include "i3UIManager.h"

namespace vt_detail
{
	void setVTexInfoToCtx(i3VirtualTexture* owner, INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, INT32 mipmaps, INT32 TileSz); // VirtualTexture.cpp에 구현.
	void CreateTileTable(const i3VirtualTextureCtx& Ctx, I3VT_TILE*& inoutTileTable);	// i3VTFileFormatUtil.cpp
}


i3VTFileFormat::i3VTFileFormat(i3VirtualTexture* pOwner) 
{
	m_pController = new i3VTFileFormatController_Legacy(pOwner, &m_Data);		// 이 변수는 널값을 허용하면 안된다..
																		//
	m_Data.m_FileHandleList.reserve(32);
}

i3VTFileFormat::~i3VTFileFormat()
{
	m_pController->RemoveAllFileHandle();

	i3::cu::for_each_SafeFree_clear(m_Data.m_FileHandleList);			// 위 함수와 중복..위에거가 더 좋음...좀더 고민할것..

	delete m_pController;
}

void	i3VTFileFormat::ConvertController()
{
	m_pController = m_pController->MoveConvertedController();
}

bool	i3VTFileFormat::IsControllerNewFormat() const
{
	return m_pController->IsControllerNewFormat();
}

// Create함수는 새 생성이고 로딩처리가 아님..
bool	i3VTFileFormat::Create(const char * pszPath, I3G_IMAGE_FORMAT fmt, bool bNewFormat)
{
	i3::stack_string strModPath;

	if (bNewFormat)
	{
		i3::change_file_ext_copy(strModPath, pszPath, "i3VTexList");
	}
	else
	{
		i3::change_file_ext_copy(strModPath, pszPath, "i3VTex");
	}

	//
	i3UIFilePathMgr::i()->Enablei3VTexListFormat(bNewFormat);			// 여기서 경로를 비튼다..
	//

	if (bNewFormat != m_pController->IsControllerNewFormat())
		m_pController = m_pController->MoveConvertedController();
	
	i3FileStream file;
	bool bResult = false;

	i3VirtualTexture* pVTex = m_pController->GetVTex();
	i3VirtualTextureCtx& Ctx = *pVTex->getContext();
	i3VirtualTextureData* pData = pVTex->getData();

	vt_detail::setVTexInfoToCtx(pVTex, I3VT_VTEX_SIZE, I3VT_VTEX_SIZE, fmt, 1, I3VT_TILE_SIZE);

	// 파일 생성 전에 디렉토리 마련이 필요할수 있다..
	{
		i3::stack_string dirToCreate;
		i3::extract_directoryname(strModPath, dirToCreate);
		if (i3System::IsDirectory(dirToCreate.c_str()) == false)
			i3System::PrepareDirectory(dirToCreate.c_str());
	}
	//

	i3::stack_string uniquePath;

	if (file.Create(strModPath.c_str(), STREAM_READ | STREAM_WRITE) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not create %s virtual texture file.", strModPath);
		goto ErrHandle;
	}

	//////////////////
	// Tile Info Table
	{
		vt_detail::CreateTileTable(Ctx, pData->m_pTiles);
		m_pController->SetupMainFileBaseOffset();
	}

	///////////////////
	// 파일 구조를 설정.
	{
		this->Flush(&file);
	}
	// 파일 구조 설정.
	///////////////////

	bResult = true;

	i3UIFilePathMgr::i()->ObtainUniquePath(strModPath, uniquePath);

	pVTex->SetName(uniquePath);

	// Async I/O를 위해 등록...
	m_pController->CreateAllFileHandle();

ErrHandle:

	if (bResult == false)
	{
		// Error 상황.
		I3MEM_SAFE_FREE(pData->m_pTiles);
	}

	// 실질적인 Async File I/O는 i3VirtualTextureIO에 의해 이루어지기 때문에
	// 여기에서는 기본 정보만 저장한 후, 파일을 닫는다.
	file.Close();

	return bResult;
}


I3VT_IOHANDLE *	i3VTFileFormat::getFileHandle(INT32 idx) const
{
	if ((INT32)m_Data.m_FileHandleList.size() <= idx)
		return nullptr;

	return m_Data.m_FileHandleList[idx];
}

//
//
// 버추얼 텍스쳐 오픈시에 고르지 말고, UIL파일 로딩시로 일단 옮겨야 할것임..
//
//

bool i3VTFileFormat::Open(const char * pszPath)
{
	bool is_new_format = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	if (m_pController->IsControllerNewFormat() != is_new_format)		// 포맷이 서로 다른 경우 변환해준다...
	{
		m_pController = m_pController->MoveConvertedController();
	}

	return m_pController->Open(pszPath);
}

bool i3VTFileFormat::ReOpen()
{
	const i3::rc_string& strVTexName = m_pController->GetVTex()->GetNameString();

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(strVTexName, strUniquePath);

	i3::rc_string strVTexPath = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
	bool isVTexListFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	i3::stack_string strFull = strVTexPath;
	strFull += '/';
	strFull += strUniquePath;

	bool bResult = false;

	if (vt_util::IsVirtualTextureFileInHardDisk(strFull, isVTexListFormat))
	{
		bResult = i3VTFileFormat::Open(strFull.c_str());
	}
	else
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
		const i3::rc_string& strScenePath =	i3UI::getManager()->GetCurrentScenePath();

		i3::stack_string strDir;
		i3::extract_directoryname(strScenePath, strDir);

		strFull = strDir;
		if (!strFull.empty())
			strFull += '/';

		i3::stack_string fileNameOnly;
		i3::extract_filename(strUniquePath, fileNameOnly);
		
		strFull += fileNameOnly;
		
		if (vt_util::IsVirtualTextureFileInHardDisk(strFull, isVTexListFormat))
		{
			bResult = i3VTFileFormat::Open(strFull.c_str());
		}
	}

	return bResult;
}


bool i3VTFileFormat::Flush(i3FileStream * pFile)
{
	return m_pController->Flush(pFile);			// 상태변경때문에 코드보완 필요..
}

bool i3VTFileFormat::Defragment(void)
{
	return m_pController->Defragment();			// 상태변경때문에 코드보완 필요..
}

// Virtual Texture의 해당 위치에 Texture를 배치한다.
// - MIPMAP Level 0만이 복사된다.
// - Format은 Virtual Texture에 맞게 변환된다.
bool i3VTFileFormat::PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY)
{
	return m_pController->PlaceTexture(pTex, pX, pY);
}

i3VTSubsetController* i3VTFileFormat::GetSubsetController() const
{
	return m_pController->GetSubsetController();
}

// 호환성 때문에 포함해야한다..
bool	i3VTFileFormat::LoadRefTextureInfo()
{
	return m_pController->LoadRefTextureInfo();
}

bool	i3VTFileFormat::SaveRefTextureInfo()
{
	return m_pController->SaveRefTextureInfo();
}
