#include "i3FrameworkPCH.h"

#include "i3VTFileFormatController_Legacy.h"
#include "i3VTSubsetController.h"
#include "i3VirtualTexture.h"
#include "i3UIManager.h"
#include "i3VTFileFormat.h"

#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/extract_filename.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_is_iless.h"

#include "i3VTFileFormatController_New.h"
#include "i3VTFileFormatController_New_Tool.h"

#include "i3UIFilePathMgr.h"

#include "i3Base/string/system/get_case_sensitive_filepath.h"

// 레거시 위주로 작동한다. 툴에서는 별도로 따로 쓰이는 함수이기 때문에 외부에선 가상함수 분기를 거치게 된다..
#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가합니다. 2017.02.14. soon9
#endif

struct ALIGN4 I3VT_REFTEX_HEADER
{
	UINT32		m_Count = 0;
	UINT32		pad[8] = { 0 };
};

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

namespace vt_detail
{
	void setVTexInfoToCtx(i3VirtualTexture* owner, INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, INT32 mipmaps, INT32 TileSz); // VirtualTexture.cpp에 구현.
	void getIndex_ByPixel(INT32 px, INT32 py, INT32 * idxX, INT32 * idxY);	// VirtualTexture.cpp에 구현.
	I3VT_TILE * getTileByIndex(const i3VirtualTextureData* pData, INT32 xidx, INT32 yidx);	// VirtualTexture.cpp에 구현.
	void RemoveAllRefTextureImp(i3VirtualTextureData* inoutData, i3VirtualTextureCache* inoutCache);	// VirtualTexture.cpp에 구현.

	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx);				// i3VTFileFormatUtil.cpp
	bool FindWrongTexture(const char* szFull);	//	i3VTFileFormatUtil.cpp

	bool TryOpenFileStream(const char* pszPath, i3FileStream& outFile); //	i3VTFileFormatUtil.cpp
	void WaitIOCompletion(i3VirtualTextureIO* io);	// i3VTFileFormatUtil.cpp
	__HANDLE_TYPE OpenVTexFile(const char * pszPath, bool bCreate); // i3VTFileFormatUtil.cpp
	UINT64 GetVTexFileSize(__HANDLE_TYPE hFile); // i3VTFileFormatUtil.cpp
	bool CheckRefTextureFiles(const i3VirtualTexture* pVTex);	// i3VTFileFormatUtil.cpp
	void CopyAndSortRefTextures(const i3::vector<I3VT_TEXINFO*>& srcList, 
		i3::vector<I3VT_TEXINFO*>& destList, bool bOldVersion); // i3VTFileFormatUtil.cpp

	void CreateTileTable(const i3VirtualTextureCtx& Ctx, I3VT_TILE*& inoutTileTable);	// i3VTFileFormatUtil.cpp
	void ResetAllTileData(const i3VirtualTextureCtx& Ctx, I3VT_TILE* inoutTileTable);  // i3VTFileFormatUtil.cpp
	INT32 AdjustUnit(const i3VirtualTextureCtx& Ctx, INT32 x, bool bRoundOff); //  i3VTFileFormatUtil.cpp

	I3VT_TEXINFO* AddRefTexture(i3VirtualTextureData* pData, i3Texture * pTex, INT32 x, INT32 y, UINT64 date = 0L); //  i3VTFileFormatUtil.cpp

	void    SettleTextureImp(i3VirtualTexture* pVTex, INT32 x, INT32 y, i3Texture* pTex, I3VT_TEXINFO* pVTexInfo, INT32 subsetIdx); //  i3VTFileFormatUtil.cpp

	bool	PlaceTextureImp_FindSameTextureInfo(i3VirtualTexture* pVTex, i3Texture * pTex, INT32 * pX, INT32 * pY);
	bool	PlaceTextureImp_FindNewPosition(i3VirtualTexture* pVTex, i3Texture * pTex, INT32 * pX, INT32 * pY);


	void UpdateRefTextureToTile(i3VirtualTexture* pVTex)
	{
		INT32 sx, sy, ex, ey, x, y;
		I3VT_TEXINFO * pInfo;
		I3VT_TILE * pTile;

		const INT32 numRefTex = pVTex->getRefTextureCount();

		for (INT32 i = 0; i < numRefTex; i++)
		{
			pInfo = pVTex->getRefTexture(i);

			//	pInfo->m_pTex = nullptr;
			i3VTWeakTex_StrongRelease(pInfo->m_pWeakTex);			// 텍스쳐만 리셋해둔다..함수이름 바꿔야되나..
			pInfo->m_bLoaded = 0;


			vt_detail::getIndex_ByPixel(pInfo->m_X, pInfo->m_Y, &sx, &sy);
			vt_detail::getIndex_ByPixel(pInfo->m_X + pInfo->m_Width - 1, pInfo->m_Y + pInfo->m_Height - 1, &ex, &ey);

			for (y = sy; y <= ey; y++)
			{
				for (x = sx; x <= ex; x++)
				{
					pTile = vt_detail::getTileByIndex(pVTex->getData(), x, y);

					pTile->setTexInfo(pInfo);
					//	pTile->setRefTexture( i);

				}
			}

		}
	}
	
	// 구버전에서만 이 함수를 쓴다.. 신규포맷은 이것 쓰지 않음..
	bool LoadRefTextureInfoImp(i3VTFileFormatController* pController)	
	{
		char szPath[MAX_PATH];
		i3FileStream file;
		UINT32 i, Count;
		bool bResult = false;

		i3VirtualTexture* pVTex = pController->GetVTex();
		i3VirtualTextureData* pVTexData = pVTex->getData();
		i3VirtualTextureCache* pVTexCache = pVTex->getCache();
		vt_detail::RemoveAllRefTextureImp(pVTexData, pVTexCache);
		

		i3::stack_string uniqueRefTexPath;
		i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), uniqueRefTexPath);
		i3::change_file_ext(uniqueRefTexPath, "i3RefTex");

		// 이 파일은 로케일에 존재하는데 공통폴더에 있는것과 사실상 같지만 로케일쪽을 읽는 것으로 수정한다..
		// 원본 엔진코드상에서는 공통폴더에 있는걸 가져다 쓰는 것처럼 보인다...
		i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

		i3::stack_string strFullPath = strLocalePath;
		strFullPath += '/';
		strFullPath += uniqueRefTexPath;



		if (file.Open(strFullPath.c_str(), STREAM_READ | STREAM_SHAREREAD) == false)
		{
			I3PRINTLOG(I3LOG_NOTICE, "Could not open %s file.", strFullPath);
			return false;
		}

		{
			I3VT_REFTEX_HEADER header;

			if (file.Read(&header, sizeof(header)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a header of referenced texture information file (%s).", szPath);
				goto ErrHandle;
			}

			Count = header.m_Count;
		}

		I3ASSERT(Count < 100000);

		for (i = 0; i < Count; i++)
		{
			I3VT_TEXINFO * pInfo;

			pInfo = new I3VT_TEXINFO;
			NetworkDump_Malloc(pInfo, sizeof(I3VT_TEXINFO), __FILE__, __LINE__);

			I3VT_TEXINFO_FILE fileInfo;			// 실제 리소스텍스쳐파일 이름과 너비 높이가 들어감..그외에 버추얼텍스쳐상에서의 픽셀수준 x,y위치도 들어가게된당.

			if (file.Read(&fileInfo, sizeof(fileInfo)) == STREAM_ERR)
			{
				//I3PRINTLOG(I3LOG_WARN,  "Could not load a referenced texture information file (%s).", szPath);
				delete pInfo;
				continue;
				//goto ErrHandle;
			}

			// 아 그냥 그지 같애....i3UIFilePathMgr::i()->ObtainUniquePath()로 다 아작...
			i3::stack_string uniqueTexInfoPath;
			i3UIFilePathMgr::i()->ObtainUniquePath(fileInfo.m_szName, uniqueTexInfoPath);
			i3::make_unix_path(uniqueTexInfoPath);

			if (i3UI::getToolDevMode())		// 툴에 한해서..신규포맷은 i3VTexList 읽을때 처리해둬야함..
			{

				if (i3UIFilePathMgr::i()->IsLanguageLocaleEnabled())
				{

					i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);
				
					i3::stack_string strLocaleFullPath = strLocalePath;
					strLocaleFullPath += '/';
					strLocaleFullPath += uniqueTexInfoPath;

					// 로케일에 없으면 공통에서 꺼내 복사해야한다..
					if (!vt_detail::FindWrongTexture(strLocaleFullPath.c_str()))
					{
						i3::rc_string strCommonPath = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);

						i3::stack_string strCommonFullPath = strCommonPath;
						strCommonFullPath += '/';
						strCommonFullPath += uniqueTexInfoPath;

						if (vt_detail::FindWrongTexture(strCommonFullPath.c_str()))
						{
							if (CopyFile(strCommonFullPath.c_str(), strLocaleFullPath.c_str(), false))
							{
								i3::stack_string msg;
								i3::sprintf(msg, "파일복사 성공 : %s", strLocaleFullPath);
								::MessageBox(NULL, msg.c_str(), "Copy", MB_OK);
							}
							else
							{
								i3::stack_string msg;
								i3::sprintf(msg, "파일복사 실패 : %s", strLocaleFullPath);
								::MessageBox(NULL, msg.c_str(), "error", MB_OK);
							}

						}
						else // 공통폴더에도 없으면....어떡해야되지..
						{
							i3::stack_string msg;
							i3::sprintf(msg, "로케일 및 공통폴더에 파일이 모두 없다. : %s", strLocaleFullPath);
							::MessageBox(NULL, msg.c_str(), "error", MB_OK);

						}
					}
				}
				
			}

			i3::safe_string_copy(fileInfo.m_szName, uniqueTexInfoPath, _countof(fileInfo.m_szName));

			pInfo->CopyFromFile(&fileInfo);

			{
				char conv[MAX_PATH];
				char code[16];
				UINT128 * pTemp = (UINT128 *)code;

				i3::safe_string_copy(conv, pInfo->m_strName, MAX_PATH);
				i3::to_upper(conv);
				i3::make_unix_path(conv);

				i3MD5::Gen(conv, i3::generic_string_size(conv), code);

				pInfo->m_CODE = *pTemp;					// m_CODE로 미리 해시값 뽑아내는 것 좋지 않다고 생각함..파일로 저장하는건 특히 안좋다..
			}											// 파일로 저장하고 활용하는 것은 해쉬함수 변경을 무척 어렵게 만든다..그렇게 하지 말자...

			pVTexData->m_linearTexInfoList.push_back(pInfo);
			pVTexData->m_str_refTexIdx_set.insert(pInfo);
		}

		// 일단은 소팅을 걸어두자..케이스없는 알파벳 순서가 가장 무난하다...

		struct comp
		{
			bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
			{
				return	i3::generic_is_iless(lhs->m_strName, rhs->m_strName);
			}
		};
		i3::sort(pVTexData->m_linearTexInfoList.begin(), pVTexData->m_linearTexInfoList.end(), comp());
		//

		bResult = true;

	ErrHandle:

		file.Close();

		if (bResult)
			vt_detail::UpdateRefTextureToTile(pVTex);

		return bResult;
	}
	
	void Fill_I3VT_TEXINFO_FILE(const I3VT_TEXINFO* src, I3VT_TEXINFO_FILE* dest)
	{
		i3::stack_string strOldStyle;
		i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(src->m_strName, strOldStyle);

		i3::safe_string_copy(dest->m_szName, strOldStyle, sizeof(dest->m_szName));

		dest->m_CODE = src->m_CODE;
		dest->m_Width = src->m_Width;
		dest->m_Height = src->m_Height;
		dest->m_X = src->m_X;
		dest->m_Y = src->m_Y;

		dest->m_bBlockAutoTrashTex = src->m_bBlockAutoTrashTex ? TRUE : FALSE;
	}


	bool SaveRefTextureInfoImp(i3VTFileFormatController* pController)
	{
		i3::vector<I3VT_TEXINFO*>	list_for_sort;

		i3VirtualTexture* pVTex = pController->GetVTex();

		i3FileStream file;
		I3VT_TEXINFO * pInfo;
		bool bResult = false;

		i3::stack_string uniqueRefTexPath;
		i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), uniqueRefTexPath);
		i3::change_file_ext(uniqueRefTexPath, "i3RefTex");

		// 이 파일은 로케일에 존재하는데 공통폴더에 있는것과 사실상 같지만 로케일쪽을 읽는 것으로 수정한다..
		// 원본 엔진코드상에서는 공통폴더에 있는걸 가져다 쓰는 것처럼 보인다...
		i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

		i3::stack_string strFullPath = strLocalePath;
		strFullPath += '/';
		strFullPath += uniqueRefTexPath;


		if (file.Create(strFullPath.c_str(), STREAM_WRITE) == false)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not create %s file.", strFullPath);
			return false;
		}

		{
			I3VT_REFTEX_HEADER header;

			i3mem::FillZero(&header, sizeof(header));
			header.m_Count = pVTex->getRefTextureCount();

			if (file.Write(&header, sizeof(header)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not save a header of referenced texture information file (%s).", strFullPath);
				goto ErrHandle;
			}
		}

		i3VirtualTextureData* pVTexData = pVTex->getData();
	
		vt_detail::CopyAndSortRefTextures(pVTexData->m_linearTexInfoList, list_for_sort, true);

		const size_t numRefTex = list_for_sort.size();

		for (size_t i = 0; i < numRefTex; i++)
		{
			pInfo = list_for_sort[i]; //	pVTex->getRefTexture(i);

			if (i3::contain_string(pInfo->m_strName, "../") >= 0)
			{
				continue;
			}

			I3VT_TEXINFO_FILE fileInfo;
			ZeroMemory(&fileInfo, sizeof(I3VT_TEXINFO_FILE));

			i3::stack_string refinedPath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pInfo->m_strName, refinedPath);

			pInfo->m_strName = refinedPath;			// UI_XXX 포함 이전 경로를 모두 제낀다...
			

			Fill_I3VT_TEXINFO_FILE(pInfo, &fileInfo);

			if (file.Write(&fileInfo, sizeof(fileInfo)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not save a referenced texture information file (%s).", strFullPath);
				goto ErrHandle;
			}
		}

		bResult = true;

	ErrHandle:

		file.Close();

		return bResult;
	}
	

	void	SettleTexture_Legacy(i3VTSubsetController* pSubsetController, 
		INT32 x, INT32 y, i3Texture * pTex, I3VT_TEXINFO* refTexInfo)
	{
		// 해당 Tile이 배치된 Subset File을 구한다.

		INT32	idxSubset = pSubsetController->FindOrCreateMatchedSubset(pTex->GetNameString(), pTex->GetDataSize());
		//I3TRACE( "TEX (%s) -> %d\n", pTex->GetName(), idxSubset);

		i3VirtualTexture* pVTex = pSubsetController->GetVTex();

		vt_detail::SettleTextureImp(pVTex, x, y, pTex, refTexInfo, idxSubset);
	}



}



i3VTFileFormatController_Legacy::i3VTFileFormatController_Legacy(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData)
	: i3VTFileFormatController(pOwner, pFFData)
{
	m_pSubsetController = new i3VTSubsetController(pOwner);		// 널값을 허용해선 안된다..
}

i3VTFileFormatController_Legacy::i3VTFileFormatController_Legacy(i3VirtualTexture* pOwner,
	i3VTFileFormatData* pFFData, i3VTSubsetController* pSCToMove) :
	i3VTFileFormatController(pOwner, pFFData), m_MainFileBaseOffset(0)
{
	
	m_pSubsetController = pSCToMove;		// pSCToMove가 있던 자리는 nullptr이 되어야 move가 됨..

	const i3::rc_string& strName = GetVTex()->GetNameString();
	i3::rc_string strNewName;
	i3::change_file_ext_copy(strNewName, strName, "i3VTex");
	GetVTex()->SetName(strNewName);

	i3VTFileFormatController_Legacy::SetupMainFileBaseOffset();

	// 신규포맷쪽은 I3VT_TEXINFO에 m_CODE값이 전혀 없기 때문에 반영해주어야한다..
	i3VirtualTextureData* pVTData =	GetVTex()->getData();

	const size_t numTexInfo = pVTData->m_linearTexInfoList.size();
	i3::stack_string conv;

	for (size_t i = 0; i < numTexInfo; ++i)
	{
		I3VT_TEXINFO* texInfo = pVTData->m_linearTexInfoList[i];

		{
			char code[16];
			UINT128 * pTemp = (UINT128 *)code;
			conv = texInfo->m_strName;
			i3::to_upper(conv);
			i3::make_unix_path(conv);
			i3MD5::Gen(const_cast<char*>(conv.c_str()), conv.size(), code);
			texInfo->m_CODE = *pTemp;					// m_CODE로 미리 해시값 뽑아내는 것 좋지 않다고 생각함..파일로 저장하는건 특히 안좋다..
		}											// 파일로 저장하고 활용하는 것은 해쉬함수 변경을 무척 어렵게 만든다..그렇게 하지 말자...
	}
	
}

i3VTFileFormatController_Legacy::~i3VTFileFormatController_Legacy()
{
	delete m_pSubsetController;
}

// 신규포맷 툴용 컨트롤러를 생성..나중에 교환...
i3VTFileFormatController*	i3VTFileFormatController_Legacy::MoveConvertedController()
{
	i3VTFileFormatController* pConverted = nullptr;

	if (i3UI::getToolDevMode())
	{
		pConverted = new i3VTFileFormatController_New_Tool(this->GetVTex(), this->GetVTFileFormatData(), m_pSubsetController);
		m_pSubsetController = nullptr;
	}
	else
	{
		const i3::rc_string& strName = GetVTex()->GetNameString();

		pConverted = new i3VTFileFormatController_New(this->GetVTex(), this->GetVTFileFormatData());
			
		i3::rc_string strNewName;
		i3::change_file_ext_copy(strNewName, strName, "i3VTexList");
		pConverted->GetVTex()->SetName(strNewName);
	}

	delete this;			// delete self ..  이후로는 이 객체를 쓰면 안됨..
	return pConverted;
}


bool	i3VTFileFormatController_Legacy::SaveRefTextureInfo()
{
	return vt_detail::SaveRefTextureInfoImp(this);
}

bool	i3VTFileFormatController_Legacy::LoadRefTextureInfo()
{
	return vt_detail::LoadRefTextureInfoImp(this);
}


bool i3VTFileFormatController_Legacy::Open(const char * inputPath)
{

	i3::stack_string strUniqueName;

	bool bResult = false;
	char * pBuf = nullptr;
	UINT32 sz, tileCount;
	i3FileStream file;

	INT8	id[4];
	INT32 version = 0;

	i3::stack_string realFilePath;
	i3::change_file_ext_copy(realFilePath, inputPath, "i3VTex");

	if (vt_detail::TryOpenFileStream(realFilePath.c_str(), file) == false)
		return false;

	I3VT_TILE*& inoutTileTable = GetVTex()->getData()->m_pTiles;

	// Version 확인을 위해, 우선 4 byte 만을 읽어 들인다.
	if (file.Read(id, 4) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load a file signature from %s file.", realFilePath);
		goto ErrHandle;
	}

	file.SetPosition(-4, STREAM_CURRENT);

	if (i3::generic_is_niequal((const char *)id, "VTV1", 4))
		version = 1;
	else if (i3::generic_is_niequal((const char *)id, "VTV2", 4))
		version = 2;

	if (version == 0)
	{
		I3PRINTLOG(I3LOG_WARN, "%s file is not a valid virtual texture file!", realFilePath);
		goto ErrHandle;
	}

	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();

	pFFData->m_strCurrFullPath = file.GetFileNameString();		// pszPath와 같다..

														//
														// File Header와 Tile Information을 읽어들일 메모리 버퍼를 할당
	{
		UINT32 szTileInfo, cnt;

		cnt = (I3VT_VTEX_SIZE / I3VT_TILE_SIZE);

		tileCount = cnt * cnt;

		if (version == 1)
			szTileInfo = sizeof(i3::pack::VIRTUALTEXTURE_TILE_1) * tileCount;
		else
			szTileInfo = sizeof(i3::pack::VIRTUALTEXTURE_TILE_2) * tileCount;

		sz = sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER) + szTileInfo;

		// 2048의 배수로 조정
		UINT32 allocsize = _ALIGN_(sz, 2048);

		pBuf = (char *)i3MemAlloc(allocsize);
		if (pBuf == nullptr)
		{
			goto ErrHandle;
		}

		// 파일에서 File Header와 Tile Information을 읽어들인다.
		//if( file.Read( pBuf, sz) == STREAM_ERR)
		if (file.Read(pBuf, allocsize) == STREAM_ERR)
		{

			DWORD err = ::GetLastError();
			if (err != ERROR_HANDLE_EOF)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a file header from %s file. %x", realFilePath, err);
				goto ErrHandle;
			}

		}

		file.Close();
	}

	///////////////////
	// 기존 정보 읽어들이기.
	{
		i3::pack::VIRTUALTEXTURE_FILE_HEADER	* pHeader;
		UINT64				  TileOffset = sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER);

		// Header
		{
			pHeader = (i3::pack::VIRTUALTEXTURE_FILE_HEADER *)pBuf;

			vt_detail::setVTexInfoToCtx(GetVTex(), pHeader->m_Width, pHeader->m_Height,
				(I3G_IMAGE_FORMAT)pHeader->m_Format, pHeader->m_MipmapLevel, I3VT_TILE_SIZE);

			TileOffset = pHeader->m_TileTableOffset;					// 호환성 문제로 로딩쪽 입력은 남겨놓는다..
			m_MainFileBaseOffset = pHeader->m_DataBaseOffset;		// LEGACY ONLY이다..
		}

		//////////////////
		// Tile Info Table


		{
			vt_detail::CreateTileTable(*GetVTex()->getContext(), inoutTileTable);
		}

		// Tile Information 읽어 들이기
		{
			UINT32 i;

			if (version == 1)
			{
				i3::pack::VIRTUALTEXTURE_TILE_1 * pSrc = (i3::pack::VIRTUALTEXTURE_TILE_1*)(pBuf + TileOffset);

				for (i = 0; i < tileCount; i++)
				{
					//	m_pTiles[i].setRefTexture( pSrc[i].m_idxRefTex);			아예 로딩 수준에서 제낀다..
					inoutTileTable[i].setFileOffset(pSrc[i].m_Offset);
				}
			}
			else
			{
				i3::pack::VIRTUALTEXTURE_TILE_2 * pSrc = (i3::pack::VIRTUALTEXTURE_TILE_2*)(pBuf + TileOffset);

				for (i = 0; i < tileCount; i++)
				{
					//	m_pTiles[i].setRefTexture( pSrc[i].m_idxRefTex);			아예 로딩 수준에서 제낀다...
					inoutTileTable[i].setSubset(pSrc[i].m_idxFile);
					inoutTileTable[i].setFileOffset(pSrc[i].m_Offset);
				}
			}
		}
	}
	// 파일 구조 설정.
	///////////////////

	bResult = true;

	//
	// 구버전이든 신버전이든 네이밍에 UIRe를 일단 뗀다..  호환성을 위해 구버전인 경우에만 저장시에 UIRe등을 다시 복귀시키도록 한다..
	
	i3UIFilePathMgr::i()->ObtainUniquePath(realFilePath, strUniqueName);
	GetVTex()->SetName(strUniqueName);

	m_pSubsetController->LoadAllSubsetInfo();			// 서브셋 파일 콘트롤러는 레거시에서만 쓰이게된다..

	vt_detail::LoadRefTextureInfoImp(this);

	this->RemoveAllFileHandle();
	this->CreateAllFileHandle();

ErrHandle:

	I3MEM_SAFE_FREE(pBuf);

	if (bResult == false)
	{
		// Error 상황.
		I3MEM_SAFE_FREE(inoutTileTable);
	}

	//DumpTiles( "D:/Dump2");

	return bResult;

}

void	i3VTFileFormatController_Legacy::SetupMainFileBaseOffset()
{
	i3VirtualTextureCtx& Ctx = *GetVTex()->getContext();

	// Tile Information 공간 확보.
	UINT64 TileOffset = sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER);

	// Tile Data 저장 위치
	UINT32		szTileInfo = sizeof(i3::pack::VIRTUALTEXTURE_TILE_2) * Ctx.m_cntTileTotal;

	m_MainFileBaseOffset = _ALIGN_(TileOffset + szTileInfo, 2048);
}


bool i3VTFileFormatController_Legacy::Flush(i3FileStream * pFile)
{
	i3VirtualTexture* pVTex = GetVTex();
	i3VirtualTextureIO* pIO = pVTex->GetIO();
	i3VirtualTextureCtx& Ctx = *pVTex->getContext();
	i3VirtualTextureData* pData = pVTex->getData();

	i3VTFileFormat* pFF = pVTex->GetFileFormat();

	i3FileStream file;
	bool bResult = false;
	bool bRecreateHandle = false;

	// 기존에 IO Manager에 의해 열려있다면 닫아야 함.
	vt_detail::WaitIOCompletion(pIO);

	if (pFF->getFileHandleCount() > 0)
	{
		RemoveAllFileHandle();

		bRecreateHandle = true;
	}

	if (pFile == nullptr)
	{
		i3::stack_string strUniquePath;

		i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), strUniquePath);

		i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

		i3::stack_string strFullPath = strVTexDir;
		strFullPath += '/';
		strFullPath += strUniquePath;

		bool bFileRes = false;
	
		if (i3System::IsFile(strFullPath.c_str()))
		{
			bFileRes = file.Open(strFullPath.c_str(), STREAM_SHAREWRITE | STREAM_WRITE | STREAM_SHAREREAD);
		}
		else
		{
			bFileRes = file.Create(strFullPath.c_str(), STREAM_SHAREWRITE | STREAM_WRITE | STREAM_SHAREREAD);
		}

		if (bFileRes == false)
		{
			DWORD err = ::GetLastError();
			if (err == ERROR_IO_PENDING)
			{
				while ( bFileRes == false)
				{
					i3Timer::Sleep(10);

					if (i3System::IsFile(strFullPath.c_str()))
					{
						bFileRes = file.Open(strFullPath.c_str(), STREAM_SHAREWRITE | STREAM_WRITE | STREAM_SHAREREAD);
					}
					else
					{
						bFileRes = file.Create(strFullPath.c_str(), STREAM_SHAREWRITE | STREAM_WRITE | STREAM_SHAREREAD);
					}
				}
			}
			else
			{
				I3PRINTLOG(I3LOG_WARN, "Could not open %s file for updating a file header.", strFullPath);
			}
		}

		pFile = &file;
	}

	// Async I/O에서는 읽어들이는 크기가 반드시 2048(DVD 고려)의 배수가 될 필요가 있다.
	// 그러므로 File Header와 Tile Information 부분을 포함할 수 있는, 
	// 2048의 배수가 되는 크기의 메모리를 할당한 다음 필요한 정보를 설정해야 한다.

	char * pBuf = nullptr;
	i3::pack::VIRTUALTEXTURE_FILE_HEADER	* pHeader;
	UINT32 szBuf;

	{
		UINT32 szTileInfo = sizeof(i3::pack::VIRTUALTEXTURE_TILE_2) * Ctx.m_cntTileTotal;
		UINT32 sz = sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER) + szTileInfo;

		// 2048의 배수로 Align
		szBuf = _ALIGN_(sz, 2048);

		pBuf = (char *)i3MemAlloc(szBuf);

		i3mem::FillZero(pBuf, szBuf);
	}

	// Header

	UINT64 TileTableOffset = sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER);

	{
		pHeader = (i3::pack::VIRTUALTEXTURE_FILE_HEADER *)pBuf;

		pHeader->m_Format = (UINT32)Ctx.m_Format;
		pHeader->m_MipmapLevel = Ctx.m_MipmapLevel;
		pHeader->m_Width = Ctx.m_Width;
		pHeader->m_Height = Ctx.m_Height;

		pHeader->m_TileTableOffset = TileTableOffset;				// 호환성때문에 기록을 남겨둠..
		pHeader->m_DataBaseOffset = m_MainFileBaseOffset;
	}

	// Tile Information
	{
		/// 구버전과 완벽한 동기화를 위해서 m_linearTexInfoList의 인덱스 대신 m_CODE에 의한 인덱스 소팅으로 변경한 목록으로 인덱스 검색...

		i3::vector<I3VT_TEXINFO*>	list_for_sort;

		vt_detail::CopyAndSortRefTextures(pData->m_linearTexInfoList, list_for_sort, true);
//		list_for_sort = pData->m_linearTexInfoList;

//		struct comp
//		{
//			bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const { return lhs->m_CODE < rhs->m_CODE; }
//		};
//		i3::stable_sort(list_for_sort.begin(), list_for_sort.end(), comp());

		i3::pack::VIRTUALTEXTURE_TILE_2 * pTileInfo = (i3::pack::VIRTUALTEXTURE_TILE_2 *)(pBuf + TileTableOffset);

		for (INT32 i = 0; i < Ctx.m_cntTileTotal; i++)
		{
			pTileInfo[i].m_idxFile = pData->m_pTiles[i].getSubset();

			I3VT_TEXINFO* pInfo = pData->m_pTiles[i].getTexInfo();
			INT32 res_idx = -1;
			UINT64 fileOffset = 0;

			if (pInfo != nullptr)
			{
				res_idx = i3::vu::int_index_of(list_for_sort, pInfo);
				fileOffset = pData->m_pTiles[i].getFileOffset();
				I3ASSERT(res_idx != -1);
			}

			pTileInfo[i].m_idxRefTex = res_idx; // m_pTiles[i].getRefTexture();
			pTileInfo[i].m_Offset = fileOffset; // pData->m_pTiles[i].getFileOffset();
		}
	}

	// Writing
	{
		pFile->SetPosition(0, STREAM_BEGIN);

		if (pFile->Write(pBuf, (INT32)szBuf) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not write the file header on %s file.", pVTex->GetNameString());
			goto ErrHandle;
		}

		pFile->Close();
	}

	m_pSubsetController->SaveAllSubsetInfo();

	// Texture Info File 갱신
	if (pData->m_linearTexInfoList.size() > 0)
	{
		vt_detail::SaveRefTextureInfoImp(this);
	}

	if (bRecreateHandle)
	{
		this->CreateAllFileHandle();
	}

	bResult = true;

ErrHandle:
	I3MEM_SAFE_FREE(pBuf);

	return bResult;
}


void	i3VTFileFormatController_Legacy::CreateAllFileHandle()
{
	// 항상 완전히 Unregister되었다가 Register되는 시나리오만 가정한다.
	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();

	I3ASSERT((INT32)pFFData->m_FileHandleList.size() == 0);

	// 미리 Handle 개수만큼 공간을 잡는다.
	pFFData->m_FileHandleList.resize(m_pSubsetController->getSubsetCount() + 1);

	const INT32 numFileHandleToCreate = m_pSubsetController->getSubsetCount() + 1;

	for (INT32 i = 0; i < numFileHandleToCreate; i++)
	{
		i3VTFileFormatController_Legacy::CreateFileHandle(i);
	}

}


//
// 여기서는 정수 인수가 그렇게 썩 좋다고 생각은 안들지만, 기존 버전과 어느정도 공통점을 가지려면
// 즉, m_FileHandleList의 인덱스를 이용하고자 한다면, 인수를 정수로 받는것이 피할수 없는 선택같다..
// 좀더 직접적으로 편하게 하려면 I3VT_TEXINFO 내부에 I3VT_IOHANDLE의 포인터가 직접 들어가는게 가장 무난하다.
//

I3VT_IOHANDLE*	i3VTFileFormatController_Legacy::CreateFileHandle(INT32 idxSubset)	// 이것 역시 가상함수일 필요가 있는지 조사해야함..
{
	// 기존에 등록되어 있는가를 판단.
	// I3ASSERT(idxSubset < INT32(m_FileHandleList.size()));

	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();
	
	I3VT_IOHANDLE * pInfo = nullptr;

	if (idxSubset < INT32(pFFData->m_FileHandleList.size()))
		pInfo = pFFData->m_FileHandleList[idxSubset];

	if (pInfo != nullptr)			// 이미 있으면 그냥 생성 안하는 쪽으로 가닥.?
		return pInfo;

	// 새로운 놈. 등록하자.
	pInfo = (I3VT_IOHANDLE *)i3MemAlloc(sizeof(I3VT_IOHANDLE));

	pInfo->m_idxSubset = idxSubset;
	pInfo->m_hFile = nullptr;


	const i3::rc_string& strName = (idxSubset == 0) ? GetVTex()->GetNameString()
		: m_pSubsetController->getSubset(idxSubset - 1)->m_strPath;


	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(strName, strUniquePath);

	i3::rc_string strVTexPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

	i3::stack_string strFullPath = strVTexPath;
	strFullPath += '/';
	strFullPath += strUniquePath;
	

	if (i3System::IsFile(strFullPath.c_str()))
		pInfo->m_hFile = vt_detail::OpenVTexFile(strFullPath.c_str(), false);
	else
		pInfo->m_hFile = vt_detail::OpenVTexFile(strFullPath.c_str(), true);

	if (pInfo->m_hFile == INVALID_HANDLE_VALUE)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not open %s virtual texture file.", strFullPath);
		i3MemFree(pInfo);
		return nullptr;
	}

#if defined( I3_DEBUG)
	i3::generic_string_copy(pInfo->m_szPath, strFullPath);
#endif

	pInfo->m_LastDataOffset = vt_detail::GetVTexFileSize(pInfo->m_hFile);
	if (pInfo->m_LastDataOffset == 0)
		pInfo->m_LastDataOffset = 2048;				// Offset 0는 Invalid Offset으로 사용되고 있기 때문에 2048을 최소 Offset으로 사용한다.

	pInfo->m_LastDataOffset = _ALIGN_(pInfo->m_LastDataOffset, 2048);		// Async I/O를 위해 2048 Align될 필요가 있다.

	i3::vu::set_value_force(pFFData->m_FileHandleList, idxSubset, pInfo);

	return pInfo;

}

bool i3VTFileFormatController_Legacy::Defragment()
{
	//2015-01-15 PB_ENG 김대영 국가를 지정하면 해당 국가 폴더내 IMAGE로 작업 할 수 있도록 수정 합니다.
	i3VirtualTextureIO* pIO = GetVTex()->GetIO();

	vt_detail::WaitIOCompletion(pIO);

	// 배치된 Referenced Texture들을 모두 재배치한다.

	// 1. Referenced Texture들의 원본 파일이 모두 존재하는지 검사한다.
	//    이 중, 하나라도 없으면 재배치를 시작하지 않는다.
	if (vt_detail::CheckRefTextureFiles(GetVTex()) == false)
	{
		return false;
	}

	i3VirtualTextureCtx& Ctx = *GetVTex()->getContext();

	i3VirtualTextureCache_Allocator*	pAllocator = i3VirtualTextureCache_Allocator::new_object();
	pAllocator->Create(Ctx.m_cntTileTotal);
	pAllocator->SetSpace((REAL32)Ctx.m_Width, (REAL32)Ctx.m_Height);


	// 2. Ref Texture들을 추가된 시간을 기준으로 Sorting 한다. 이 경우엔 list에 카피하고 소팅처리..

	i3VirtualTextureData* pVTData = GetVTex()->getData();

	i3::vector<I3VT_TEXINFO*> list;
	vt_detail::CopyAndSortRefTextures(pVTData->m_linearTexInfoList, list, true);

	// 3. Ref Texture List를 제거하기 전 Backup
	pVTData->m_linearTexInfoList.clear();
	pVTData->m_str_refTexIdx_set.clear();	// 매핑도 새로 해야될것 같다...매핑구조 제거..


	// 4. 재배치를 위해 Tile 정보와 File Offset 정보를 초기화
	{

		vt_detail::ResetAllTileData(Ctx, pVTData->m_pTiles);

		// Subset 정보들을 모두 제거해둔다.
		// 새로운 Subset 정보들은 Ref. Texture들을 처리하면서 자동으로 추가된다.

		{

			this->RemoveAllFileHandle();

			// 디렉토리 준비가 안되어 있으면 생성해야한다..
			{
				i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);

				if (i3System::IsDirectory(strVTexDir.c_str()) == false)
					i3System::PrepareDirectory(strVTexDir.c_str());
			}
			
			m_pSubsetController->RemoveAllSubsetAndFiles();
			this->CreateAllFileHandle();
		}

		// 여러 Virtual Texture 파일들의 위치를 초기화해둔다.
		this->ResetAllFileHandleOffset(m_MainFileBaseOffset);
	}

	i3UI::VirtualTexture_ProgressRange(list.size());
	// 5. list에 있는 Ref Texture들을 순서대로 읽어들여 재배치한다.
	for (size_t i = 0; i < list.size(); i++)
	{
		i3UI::VirtualTexture_ProgressPos(i);
		i3Texture * pTex;
		INT32 x, y;

		I3VT_TEXINFO* pInfo = list[i];

		// 		if( strcmp(pInfo->m_strName.c_str(), "res/selectimage.i3i") == 0)
		// 		{
		// 			continue;	
		// 		}

		// 기존의 Ref Texture 중에 동일한 것이 존재하는지 확인--> 주석이 틀렸...
		// 그게 아니고, 전부 새로 배치하는데, 배치하는 도중에 같은 파일이름인게 2개이상인 경우 나중 것을 스킵해버린다는 뜻임...
		//
		{
			I3VT_TEXINFO* already_exist_info = GetVTex()->FindRefTexture(pInfo->m_strName.c_str());

			// 파일이름이 같으면 이미 존재하는 놈으로 취급하는데, 2개이상이 됨...포인터가 같지 않은 경우로 일단 판단된다...

			if (already_exist_info != nullptr)
			{
				continue;					// 나중에 몽땅 지우기 때문에 continue로 충분
			}
		}


		{
			//2015-01-13 PB_ENG 김대영 국가를 지정하고 어드저스트 할 경우 해당 국가 폴더의 이미지 파일들 대상으로 작업하도록 변경합니다. (수정전 : 국가 상관없이 UIRE 폴더 대상으로 작업 )
			//					또한 위에서 한번 파일이 있는지 검사하는데 또 검사할 필요가 없다고 생각하여 검사하는 부분은 제거하였습니다.


			i3::make_unix_path(pInfo->m_strName);

			i3ImageFile file;
			
			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pInfo->m_strName, strUniquePath);

			i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);

			i3::stack_string fullPath = strLocalePath;
			fullPath += '/';
			fullPath += strUniquePath;

			pTex = file.Load(fullPath.c_str());

			I3ASSERT(pTex != nullptr);

			i3::stack_string corrected_path;
			i3::get_case_sensitive_unix_filepath(fullPath, corrected_path);
			i3::stack_string corrected_uniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(corrected_path, corrected_uniquePath);

			// TODO : // 애초에 소문자데이터로 저장된 파일이름이라 디스크에서 대소문자확실한 원본 파일명을 얻어와서 등록하는게 좋겠다.
			// 
			pTex->SetResourcePath(corrected_uniquePath.c_str());
		

			{
				REAL32 w, h;
				VEC2D alloced;

				// 공간 배치가 항상 Tile 단위로 이루어지도록 크기를 Tile의 배수에 맞춘다.
				w = (REAL32)vt_detail::AdjustUnit(Ctx, pTex->GetWidth(), false);
				h = (REAL32)vt_detail::AdjustUnit(Ctx, pTex->GetHeight(), false);

				if (pAllocator->Alloc(w, h, i, &alloced) == false)
				{
					I3PRINTLOG(I3LOG_WARN, "Not enough space!!! It should not be happened.");
				}
				else
				{
					x = (INT32)alloced.x;
					y = (INT32)alloced.y;

					I3TRACE("SETTLE :\t%d\t%d\t%d\t%d\t%s\n", x, y, pTex->GetWidth(), pTex->GetHeight(), pTex->GetName());

					vt_detail::WaitIOCompletion(pIO);

					I3VT_TEXINFO* newTexInfo = vt_detail::AddRefTexture(pVTData, pTex, x, y, pInfo->m_AddedDate);

					vt_detail::SettleTexture_Legacy(m_pSubsetController, x, y, pTex, newTexInfo);
				}
			}

			I3_MUST_RELEASE(pTex);
		}

	}

	// 열려있는 모든 Subset 파일들을 해당 위치에서 Truncate 시킨다.
	{
		this->TruncateAllFileHandle();
	}


	// AddRefTexture는 구버전 소팅이 걸려서 m_linearTexInfoList를 예전방식순으로 돌려놓는다...
	// 원래는 그냥 영문자 순서대로 놓기로 했으므로 여기서 다시 소팅해준다..

	struct comp
	{
		bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
		{
			return	i3::generic_is_iless(lhs->m_strName, rhs->m_strName);
		}
	};
	i3::sort(pVTData->m_linearTexInfoList.begin(), pVTData->m_linearTexInfoList.end(), comp());
	//
	

	I3_SAFE_RELEASE(pAllocator);

	//VerifyRef2Tile();

	i3VirtualTextureCache* pCache = GetVTex()->getCache();

	if (pCache != nullptr)
		pCache->ResetCache();

	//지워 줘야지.. 고럼..
	I3VT_TEXINFO * pInfo;
	for (size_t i = 0; i < list.size(); ++i)
	{
		pInfo = list[i];
		i3VTWeakTex_StrongRelease(pInfo->m_pWeakTex);
		I3_MUST_DELETE(pInfo);
	}

	list.clear();

	//	i3VirtualTexture::_UpdateRefTextureToTile();		// reference texture index가 꼬일수도 있었기 때문에 재설정한다..
	// 애초에 삽입하면서 인덱스를 넣은게 실수라고 생각된다..
	return true;

}

bool i3VTFileFormatController_Legacy::PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY)
{
	i3VirtualTexture* pVTex = GetVTex();

	if (vt_detail::PlaceTextureImp_FindSameTextureInfo(pVTex, pTex, pX, pY))
		return true;

	bool bResult = vt_detail::PlaceTextureImp_FindNewPosition(pVTex, pTex, pX, pY);

	if (bResult)
	{
		i3VirtualTextureData* pVTData = pVTex->getData();
		I3VT_TEXINFO* newTexInfo = vt_detail::AddRefTexture(pVTData, pTex, *pX, *pY);

		vt_detail::SettleTexture_Legacy(m_pSubsetController, *pX, *pY, pTex, newTexInfo);
	}

	return bResult;
}

