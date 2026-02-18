#include "i3FrameworkPCH.h"
#include "i3VTFileFormatController_New.h"
#include "i3UIManager.h"
#include "i3VTFileFormat.h"

#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/make_unix_path.h"

#include "i3VTFileFormatController_Legacy.h"
#include "i3VTSubsetController.h"

#include "i3UIFilePathMgr.h"

#include "i3Base/string/system/get_case_sensitive_filepath.h"
#include "i3Base/string/system/traverse_file.h"

namespace vt_detail
{
	void setVTexInfoToCtx(i3VirtualTexture* owner, INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, INT32 mipmaps, INT32 TileSz); // VirtualTexture.cpp에 구현.
	void getIndex_ByPixel(INT32 px, INT32 py, INT32 * idxX, INT32 * idxY);	// VirtualTexture.cpp에 구현.
	I3VT_TILE * getTileByIndex(const i3VirtualTextureData* pData, INT32 xidx, INT32 yidx);	// VirtualTexture.cpp에 구현.
	void RemoveAllRefTextureImp(i3VirtualTextureData* inoutData, i3VirtualTextureCache* inoutCache);	// VirtualTexture.cpp에 구현.


	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx);
	bool FindWrongTexture(const char* szFull);

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



	void MakeLocaleJob_IfToolDevMode(const i3::stack_string& strUniquePath)
	{

		if (i3UIFilePathMgr::i()->IsLanguageLocaleEnabled())
		{

			i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);

			i3::stack_string strLocaleFullPath = strLocalePath;
			strLocaleFullPath += '/';
			strLocaleFullPath += strUniquePath;

			// 로케일에 없으면 공통에서 꺼내 복사해야한다..
			if (!vt_detail::FindWrongTexture(strLocaleFullPath.c_str()))
			{
				i3::rc_string strCommonPath = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);

				i3::stack_string strCommonFullPath = strCommonPath;
				strCommonFullPath += '/';
				strCommonFullPath += strUniquePath;

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

	// void		i3VTSubsetController::RemoveAllSubsetAndFiles() 와 등가가 되는 신규포맷의 
	// i3VTexImage파일 삭제 함수가 필요하므로 여기서 구현한다...
	
	void		RemoveAlli3VTexImageFiles()
	{
		
		i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
		
		i3::stack_string strPathForFind = strVTexDir;
		strPathForFind += '/';
		strPathForFind += "*.*";
		
		const char * _i3VTexSub = "i3VTexImage";
		HANDLE				hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA		ffd;

		char szExt[16];

		hFind = ::FindFirstFile(strPathForFind.c_str(), &ffd);

		BOOL bFind = TRUE;
		while (bFind)
		{
			bFind = ::FindNextFile(hFind, &ffd);

			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				//	i3String::SplitFileExt( ffd.cFileName, szExt, sizeof( szExt));
				i3::extract_fileext(ffd.cFileName, szExt);
				if (i3::generic_is_iequal(szExt, _i3VTexSub))
				{
					i3::stack_string strFullPath = strVTexDir;
					strFullPath += '/';
					strFullPath += ffd.cFileName;
					::DeleteFile(strFullPath.c_str());
				}
			}
		}
		::FindClose(hFind);
		
	}

	// 신버전에서는 Settle이 곧 개별파일 생성을 의미하므로, MatchedSubset을 찾지 말고 곧바로 생성단계에 들어가도록 한다.
	void	SettleTexture_ByVTexImage(i3VirtualTexture* pVTex, INT32 x, INT32 y, i3Texture * pTex, INT32 idxTexInfo, I3VT_TEXINFO* refTexInfo)
	{
		vt_detail::SettleTextureImp(pVTex, x, y, pTex, refTexInfo, idxTexInfo);	// idxTexInfo가 서브셋인덱스를 대체한다..
	}
}

i3VTFileFormatController_New::i3VTFileFormatController_New(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData) : 
	i3VTFileFormatController(pOwner, pFFData)
{
	
}

i3VTFileFormatController*	i3VTFileFormatController_New::MoveConvertedController()
{
//	I3PRINTLOG(I3LOG_WARN, "i3VTFileFormatController_New can't be converted to i3VTFileFormatController_Legacy.");
//	return this;

	i3VTSubsetController* pNewController = new i3VTSubsetController(this->GetVTex());		// 널값을 허용해선 안된다..

	pNewController->LoadAllSubsetInfo();

	i3VTFileFormatController* pConverted = new i3VTFileFormatController_Legacy(this->GetVTex(), this->GetVTFileFormatData(), pNewController);

	pNewController = nullptr;		// 특별히 하는 일은 없으나, 편의상 nullptr셋팅한다..delete하지 않음..
	delete this;

	return pConverted;
}

namespace
{
	// 128바이트 사용...
	struct ALIGN4 I3VT_VTEXIMAGE_HEADER
	{
		UINT8	m_ID[4] = { 'V', 'T', 'I', 'H' };		//				4
		INT32	m_Version = 1;		//				4
		INT32	m_Width = 0;		// 텍스쳐 너비	4
		INT32	m_Height = 0;		// 텍스쳐 높이	4
		BOOL	m_bBlockAutoTrashTex = FALSE;	//		4		
		INT32	m_ImageFileNameSize = 0;	//		4
		UINT8	m_pad[128 - 24] = { 0 };			//		104
	};
}


bool i3VTFileFormatController_New::Open(const char * inputPath)
{
	i3::stack_string strUniqueName;

	bool bResult = false;
	i3FileStream file;
	INT8	id[4];

	i3::stack_string realFilePath;
	i3::change_file_ext_copy(realFilePath, inputPath, "i3VTexList");

	if (vt_detail::TryOpenFileStream(realFilePath.c_str(), file) == false)
		return false;

	// Version 확인을 위해, 우선 4 byte 만을 읽어 들인다.
	if (file.Read(id, 4) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load a file signature from %s file.", realFilePath);
		goto ErrHandle;
	}

	file.SetPosition(-4, STREAM_CURRENT);

	if (i3::generic_is_niequal((const char *)id, "VTL1", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "%s file is not a valid virtual texture list file!", realFilePath);
		goto ErrHandle;
	}

	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();
	pFFData->m_strCurrFullPath = file.GetFileNameString();		// pszPath와 같다..

														//
														// File Header 읽어들임
														// I3VT_PERSIST_TILE_1/2 의 타일테이블은 활용하지 않음...
														//

	{
		i3::pack::VIRTUALTEXTURE_FILE_HEADER_VTEXLIST vtFileHeader;

		if (file.Read(&vtFileHeader, sizeof(i3::pack::VIRTUALTEXTURE_FILE_HEADER_VTEXLIST)) == STREAM_ERR)
		{

			DWORD err = ::GetLastError();
			if (err != ERROR_HANDLE_EOF)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not load a file header from %s file. %x", realFilePath, err);
				goto ErrHandle;
			}

		}

		vt_detail::setVTexInfoToCtx(GetVTex(), vtFileHeader.m_Width, vtFileHeader.m_Height,
			(I3G_IMAGE_FORMAT)vtFileHeader.m_Format, vtFileHeader.m_MipmapLevel, I3VT_TILE_SIZE);

	}

	////////////
	{
		i3VirtualTexture* pVTex = this->GetVTex();
		i3VirtualTextureData* pVTexData = pVTex->getData();
		i3VirtualTextureCache* pVTexCache = pVTex->getCache();
		i3VirtualTextureCtx* pCtx = pVTex->getContext();

		vt_detail::RemoveAllRefTextureImp(pVTexData, pVTexCache);

		this->RemoveAllFileHandle();
		////////////

		i3::stack_string traverseFileDir;
		i3::extract_directoryname(realFilePath, traverseFileDir);

		i3::stack_string traverseFilePath = traverseFileDir;
		traverseFilePath += "/*.i3VTexImage";

		struct i3VTexImageCB : i3::traverse_file_callback_mb
		{
			i3::vector<i3::rc_string>& vecTexImageName;

			i3VTexImageCB(i3::vector<i3::rc_string>& vecTexImageName) : vecTexImageName(vecTexImageName) {}
			virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
			{
				vecTexImageName.push_back(fd.cFileName);
			}
			virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path) { return false; }
		};

		i3::vector<i3::rc_string> vecTexImageName;
		i3VTexImageCB cb(vecTexImageName);

		i3::traverse_file(traverseFilePath, &cb, false);

		const size_t numImageFile = vecTexImageName.size();

		i3::stack_string strFullPath_TexImage;
		i3::vector<char> vecStrBuff;
		i3::vector<I3VT_TEXINFO*>	texInfoList_NoSort;
		i3::vector<I3VT_TEXINFO*>	texInfoList_Sort;

		i3FileStream streamForHeader;

		for (size_t i = 0; i < numImageFile; ++i)
		{
			strFullPath_TexImage = traverseFileDir;
			strFullPath_TexImage += '/';
			strFullPath_TexImage += vecTexImageName[i];

			bool bOpened = streamForHeader.Open(strFullPath_TexImage.c_str(), STREAM_READ|STREAM_SHAREREAD|STREAM_SHAREWRITE);

			if (bOpened == false)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not open %s virtual texture file header.", strFullPath_TexImage);
				continue;
			}

			I3VT_VTEXIMAGE_HEADER header;

			INT32 rc = streamForHeader.Read(&header, sizeof(header));

			if (rc == STREAM_ERR 
				|| header.m_ImageFileNameSize > MAX_PATH)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not read %s virtual texture file header.", strFullPath_TexImage);
				streamForHeader.Close();
				continue;
			}

			vecStrBuff.resize(header.m_ImageFileNameSize);

			rc = streamForHeader.Read(&vecStrBuff[0], header.m_ImageFileNameSize);

			if (rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not read %s virtual texture file header.", strFullPath_TexImage);
				streamForHeader.Close();
				continue;
			}

			streamForHeader.Close();

			i3::stack_string strUniqueFileName;
			i3UIFilePathMgr::i()->ObtainUniquePath(vecStrBuff, strUniqueFileName);
			
			I3VT_TEXINFO* texInfo = new I3VT_TEXINFO;

			texInfo->m_strName = strUniqueFileName;
			texInfo->m_CODE = 0;								// m_CODE의 경우 구버전에서만 처리한다.(저장과 로드 모두 없으면 계산)
			texInfo->m_Width = header.m_Width;
			texInfo->m_Height = header.m_Height;
			texInfo->m_X = 0;								// pVTexList[i].m_X;		// Adjust과정을 통해 후처리함..
			texInfo->m_Y = 0;								// pVTexList[i].m_Y;		// Adjust과정을 통해 후처리함..
			texInfo->m_bBlockAutoTrashTex = (header.m_bBlockAutoTrashTex != FALSE);

		
			if (i3UI::getToolDevMode())
				vt_detail::MakeLocaleJob_IfToolDevMode(strUniqueFileName);	// 툴모드일때 처리가 몹시 길어서, 함수로 뺀다..
			
			texInfoList_NoSort.push_back(texInfo);
		}

		vt_detail::CopyAndSortRefTextures(texInfoList_NoSort, texInfoList_Sort, false);

		i3VirtualTextureCache_Allocator*	pAllocator = i3VirtualTextureCache_Allocator::new_object();
		pAllocator->Create(pCtx->m_cntTileTotal);
		pAllocator->SetSpace((REAL32)pCtx->m_Width, (REAL32)pCtx->m_Height);

		const size_t numInfo = texInfoList_Sort.size();
		VEC2D alloced;

		for (size_t i = 0; i < numInfo; ++i)
		{
			I3VT_TEXINFO* texInfo = texInfoList_Sort[i];
			REAL32 w = (REAL32)vt_detail::AdjustUnit(*pCtx, texInfo->m_Width, false);
			REAL32 h = (REAL32)vt_detail::AdjustUnit(*pCtx, texInfo->m_Height, false);

			if (pAllocator->Alloc(w, h, i, &alloced) == false)
			{
				I3PRINTLOG(I3LOG_WARN, "Not enough space!!! It should not be happened.");
				delete texInfo;
			}
			else
			{
				texInfo->m_X = (INT32)alloced.x;
				texInfo->m_Y = (INT32)alloced.y;
				pVTexData->m_linearTexInfoList.push_back(texInfo);
				pVTexData->m_str_refTexIdx_set.insert(texInfo);
			}
		}

		I3_SAFE_RELEASE(pAllocator);
	}

	
	//  기존 로딩 버전과 큰 차이점을 보이는 쪽으로 생성된 타일테이블에 텍스쳐 타일 정보를 파일이 아닌 계산으로 적재시킨다.
	//////////////////
	// Tile Info Table
	{
		i3VirtualTextureCtx* pVTContext = GetVTex()->getContext();
		i3VirtualTextureData* pVTexData = GetVTex()->getData();

		vt_detail::CreateTileTable(*pVTContext, pVTexData->m_pTiles);				//생성만 해놔요. 파일에 읽어들이고 뭐고 없습니다.  자동 계산해 배치하도록 합니다.

																					// Tile Information 읽어 들이기는 하지 않는다.  (나중에 자동계산해서 처리한다..)
																					// 사실상 타일 위치는 이미 I3VT_TEXINFO에 내장되어있기 때문에 해당위치에 잘 넣기만 하면 된다..

		const size_t numTexture = pVTexData->m_linearTexInfoList.size();

		INT32 sx, sy, ex, ey;

		static const UINT64 startOffset = 2048;	// 현재는 매직넘버인데, i3VirtualTextureIO의 서브셋첫파일포인터 위치는 2048이므로 동일하게 맞춤.

		const UINT32 sizeofTile = i3Gfx::CalcRasterBufferSize(I3VT_TILE_SIZE, I3VT_TILE_SIZE, 1, pVTContext->m_Format);

		//
		// 여기서 서브셋은 파일핸들 인덱스와 등가인데...이것은 m_linearTexInfoList와는 순서가 다르다..
		// 서브셋쪽은 텍스쳐 사이즈 비교가 우선되고, 그다음 문자열 비교로 이루어짐...따라서 setSubset에 인덱스를 매기려면 소팅처리해야한다..
		//i3::vector<I3VT_TEXINFO*> list_for_sort;
		//vt_detail::CopyAndSortRefTextures(pVTexData->m_linearTexInfoList, list_for_sort, false);
		//

		for (size_t i = 0; i < numTexture; ++i)
		{
			I3VT_TEXINFO* pInfo = pVTexData->m_linearTexInfoList[i];			//list_for_sort[i]; //

			vt_detail::getIndex_ByPixel(pInfo->m_X, pInfo->m_Y, &sx, &sy);
			vt_detail::getIndex_ByPixel(pInfo->m_X + pInfo->m_Width - 1, pInfo->m_Y + pInfo->m_Height - 1, &ex, &ey);

			UINT64 fileOffset = startOffset;

			for (INT32 y = sy; y <= ey; y++)
			{
				for (INT32 x = sx; x <= ex; x++)
				{
					I3VT_TILE * pTile = vt_detail::getTileByIndex(GetVTex()->getData(), x, y);

					pTile->setTexInfo(pInfo);
					pTile->setSubset(i);					// 이걸 제거하고 싶은데 이게 안되서 안타깝다...
					pTile->setFileOffset(fileOffset);
					fileOffset += sizeofTile;
					fileOffset = _ALIGN_(fileOffset, 2048);
				}
			}
		}

	}
	
	// 파일 구조 설정.
	///////////////////
	//
	bResult = true;
	//

	// 구버전이든 신버전이든 네이밍에 UIRe를 일단 뗀다..  호환성을 위해 구버전인 경우에만 저장시에 UIRe등을 다시 복귀시키도록 한다..

	i3UIFilePathMgr::i()->ObtainUniquePath(realFilePath, strUniqueName);
	GetVTex()->SetName(strUniqueName);
	
	this->CreateAllFileHandle();

ErrHandle:

	if (bResult == false)
	{
		// Error 상황.
		i3VirtualTextureData* pVTexData = GetVTex()->getData();
		I3MEM_SAFE_FREE(pVTexData->m_pTiles);
	}

	//DumpTiles( "D:/Dump2");

	return bResult;

}

bool i3VTFileFormatController_New::Flush(i3FileStream * pFile)
{
	i3VirtualTexture* pVTex = GetVTex();
	i3VirtualTextureIO* pIO = pVTex->GetIO();
	i3VirtualTextureCtx& Ctx = *pVTex->getContext();
	i3VTFileFormat* pFF = pVTex->GetFileFormat();

	i3FileStream file;
	bool bResult = false;
	bool bRecreateHandle = false;

	// 기존에 IO Manager에 의해 열려있다면 닫아야 함.
	vt_detail::WaitIOCompletion(pIO);

	if (pFF->getFileHandleCount() > 0)
	{
		RemoveAllFileHandle();		// 핸들만 닫지, 파일을 지우는건아님..

		bRecreateHandle = true;
	}

	if (pFile == nullptr)
	{

		i3::stack_string strUniquePath;
		
		i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), strUniquePath);

		i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);

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
				while (bFileRes == false)
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
				return false;
			}
		}

		pFile = &file;
	}

	// Async I/O에서는 읽어들이는 크기가 반드시 2048(DVD 고려)의 배수가 될 필요가 있다.
	// 그러므로 File Header와 Tile Information 부분을 포함할 수 있는, 
	// 2048의 배수가 되는 크기의 메모리를 할당한 다음 필요한 정보를 설정해야 한다.

	// pBuf 기록은 끝...일단 Write하자...
	{
		i3::pack::VIRTUALTEXTURE_FILE_HEADER_VTEXLIST header;
		
		header.m_Format = (UINT32)Ctx.m_Format;
		header.m_MipmapLevel = Ctx.m_MipmapLevel;
		header.m_Width = Ctx.m_Width;
		header.m_Height = Ctx.m_Height;
		
		pFile->SetPosition(0, STREAM_BEGIN);

		if (pFile->Write(&header, (INT32)sizeof(header)) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not write the file header on %s file.", pVTex->GetNameString());
			goto ErrHandle;
		}
		
	}

	// Texture Info File 갱신은 없다...(모두 i3VTexList안으로 흡수됨)

	if (bRecreateHandle)
	{
		this->CreateAllFileHandle();
	}

	bResult = true;

ErrHandle:

	if (pFile == &file)
	{
		pFile->Close();
	}

	return bResult;
}

void	i3VTFileFormatController_New::CreateAllFileHandle()
{
	// 항상 완전히 Unregister되었다가 Register되는 시나리오만 가정한다.

	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();

	I3ASSERT((INT32)pFFData->m_FileHandleList.size() == 0);

	// 미리 Handle 개수만큼 공간을 잡는다.

	const INT32 numFileHandleToCreate = GetVTex()->getRefTextureCount();

	pFFData->m_FileHandleList.resize(numFileHandleToCreate);


	for (INT32 i = 0; i < numFileHandleToCreate; i++)
	{
		i3VTFileFormatController_New::CreateFileHandle(i);
	}

}



I3VT_IOHANDLE*	i3VTFileFormatController_New::CreateFileHandle(INT32 idxTexInfo)	// 이것 역시 가상함수일 필요가 있는지 조사해야함..
{
	i3VTFileFormatData* pFFData = this->GetVTFileFormatData();

	I3VT_IOHANDLE * pInfo = nullptr;

	if (idxTexInfo < INT32(pFFData->m_FileHandleList.size()))
		pInfo = pFFData->m_FileHandleList[idxTexInfo];

	if (pInfo != nullptr)
		return pInfo;

	// 새로운 놈. 등록하자.
	pInfo = (I3VT_IOHANDLE *)i3MemAlloc(sizeof(I3VT_IOHANDLE));
	pInfo->m_idxSubset = idxTexInfo;			//서브셋은 더이상 유지되지 않는다..union이라도 써서 멤버이름이라도 변경할까..
	pInfo->m_hFile = nullptr;


	I3VT_TEXINFO* pTexInfo = GetVTex()->getRefTexture(idxTexInfo);

	const i3::rc_string& strName = pTexInfo->m_strName;		// 원본이미지파일이라 확장자는 기존이미지형식..
															// 신규확장자는 여기서 정해도 좋을것 같다..

	i3::stack_string strFileNameOnly;

	i3::extract_filename(strName, strFileNameOnly);

	i3::change_file_ext(strFileNameOnly, "i3VTexImage");			// 이정도 이름이면 무난해서 이걸로 선택한다..

	i3::stack_string strFullPath;

	i3::rc_string strVTexPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);

	strFullPath = strVTexPath;
	strFullPath += '/';
	strFullPath += strFileNameOnly;

	if (i3System::IsFile(strFullPath.c_str()))		// 존재시에..
	{
		pInfo->m_hFile = vt_detail::OpenVTexFile(strFullPath.c_str(), false);
	}
	else   // 새로 생성 시..
	{
		i3FileStream streamForHeader;

		streamForHeader.Create(strFullPath.c_str(), STREAM_SHAREWRITE | STREAM_WRITE | STREAM_SHAREREAD );

		I3VT_VTEXIMAGE_HEADER header;
		
		header.m_Width = pTexInfo->m_Width;
		header.m_Height = pTexInfo->m_Height;
		header.m_bBlockAutoTrashTex = pTexInfo->m_bBlockAutoTrashTex;
		header.m_ImageFileNameSize = INT32(pTexInfo->m_strName.size());

		UINT32 rc =	streamForHeader.Write(&header, sizeof(header));

		if (rc == STREAM_ERR) 
		{
			I3PRINTLOG(I3LOG_WARN, "Could not write %s virtual texture file header.", strFullPath);
			i3MemFree(pInfo);
			return nullptr;
		}
		
		rc = streamForHeader.Write(pTexInfo->m_strName.c_str(), header.m_ImageFileNameSize);

		if (rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not write %s virtual texture file header.", strFullPath);
			i3MemFree(pInfo);
			return nullptr;
		}

		streamForHeader.Close();

		pInfo->m_hFile = vt_detail::OpenVTexFile(strFullPath.c_str(), false);	// true); // true값 이제 쓰지 않음..
	}

	if (pInfo->m_hFile == INVALID_HANDLE_VALUE)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not open %s virtual texture file.", strFullPath);
		i3MemFree(pInfo);
		return nullptr;
	}

#if defined( I3_DEBUG)
	i3::safe_string_copy(pInfo->m_szPath, strFullPath, MAX_PATH);
#endif

	pInfo->m_LastDataOffset = vt_detail::GetVTexFileSize(pInfo->m_hFile);
	if (pInfo->m_LastDataOffset == 0)
		pInfo->m_LastDataOffset = 2048;				// Offset 0는 Invalid Offset으로 사용되고 있기 때문에 2048을 최소 Offset으로 사용한다.

	pInfo->m_LastDataOffset = _ALIGN_(pInfo->m_LastDataOffset, 2048);		// Async I/O를 위해 2048 Align될 필요가 있다.

	i3::vu::set_value_force(pFFData->m_FileHandleList, idxTexInfo, pInfo);

	return pInfo;

}

bool i3VTFileFormatController_New::Defragment()
{
	i3VirtualTextureIO* pIO = GetVTex()->GetIO();

	vt_detail::WaitIOCompletion(pIO);


	if (vt_detail::CheckRefTextureFiles(GetVTex()) == false)
	{
		return false;
	}

	i3VirtualTextureCtx& Ctx = *GetVTex()->getContext();

	i3VirtualTextureCache_Allocator*	pAllocator = i3VirtualTextureCache_Allocator::new_object();
	pAllocator->Create(Ctx.m_cntTileTotal);
	pAllocator->SetSpace((REAL32)Ctx.m_Width, (REAL32)Ctx.m_Height);

	// 정렬 방식 새로운 방법으로 Date가 아니라, 텍스쳐의 전체크기를 우선순위로 가장 큰놈이 먼저 오고...
	// 그외에는 그냥 평소 저장된 그대로를 따르게끔 해준다...(날짜 무시...)
	i3VirtualTextureData* pVTData = GetVTex()->getData();

	i3::vector<I3VT_TEXINFO*> list;
	vt_detail::CopyAndSortRefTextures(pVTData->m_linearTexInfoList, list, false);

	pVTData->m_linearTexInfoList.clear();
	pVTData->m_str_refTexIdx_set.clear();	// 매핑도 새로 해야될것 같다...매핑구조 제거..

											// Subset 정보들을 모두 제거해둔다.
											// 새로운 Subset 정보들은 Ref. Texture들을 처리하면서 자동으로 추가된다.
	{
		vt_detail::ResetAllTileData(Ctx, pVTData->m_pTiles);

	

		{
			this->RemoveAllFileHandle();

			// 디렉토리 준비가 안되어 있으면 생성해야한다..
			{
				i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
				
				if (i3System::IsDirectory(strVTexDir.c_str()) == false)
					i3System::PrepareDirectory(strVTexDir.c_str());
			}

			vt_detail::RemoveAlli3VTexImageFiles();		
														
			this->CreateAllFileHandle();
		}

		// 여러 Virtual Texture 파일들의 위치를 초기화해둔다.
		this->ResetAllFileHandleOffset(2048);	// 딱히 베이스파일없이 모두 개별버추얼텍스쳐로 모두 2048로 통일.. 
	}

	i3UI::VirtualTexture_ProgressRange(list.size());

	// 5. list에 있는 Ref Texture들을 순서대로 읽어들여 재배치한다.
	for (size_t i = 0; i < list.size(); i++)
	{
		i3UI::VirtualTexture_ProgressPos(i);
		i3Texture * pTex;
		INT32 x, y;

		I3VT_TEXINFO* pInfo = list[i];

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

			pTex = file.Load(fullPath.c_str());				// 오리지날 이미지이다...이미지버퍼가 아닌 DirectX 서피스임..(이부분도 사실 문제이다..
													// DX 서피스가 명시적으로 요구될 이유는 사실 없다..프로세스 낭비...(메모리전용옵션을 주거나. 버퍼만 쓰는게 좋음)
			I3ASSERT(pTex != nullptr);

			i3::stack_string corrected_path;
			i3::get_case_sensitive_unix_filepath(fullPath, corrected_path);
			i3::stack_string corrected_uniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(corrected_path, corrected_uniquePath);

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

					I3VT_TEXINFO* newTexInfo = vt_detail::AddRefTexture(pVTData, pTex, x, y, pInfo->m_AddedDate); // 여기까지는 나름 공통..
													
					INT32 idxTexInfo = INT32(pVTData->m_linearTexInfoList.size() - 1);
					this->CreateFileHandle(idxTexInfo);
																											  // 셋팅된 TexInfo는 이제는 항상 리스트의 뒷쪽에 자리잡힌다..
					vt_detail::SettleTexture_ByVTexImage(GetVTex(), x, y, pTex, idxTexInfo, newTexInfo);
				}
			}

			I3_MUST_RELEASE(pTex);
		}

	}

	{
		this->TruncateAllFileHandle();
	}

	// AddRefTexture는 버전 소팅이 걸려서 m_linearTexInfoList를 예전방식순으로 돌려놓는다...
	// 원래는 그냥 영문자 순서대로 놓기로 했으므로 여기서 다시 소팅해준다..

	// struct comp
	// {
	//	bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
	//	{
	//		return	i3::generic_is_iless(lhs->m_strName, rhs->m_strName);
	//	}
	// };
	// i3::stable_sort(pVTData->m_linearTexInfoList.begin(), pVTData->m_linearTexInfoList.end(), comp());
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

bool	i3VTFileFormatController_New::PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY)
{
	i3VirtualTexture* pVTex = GetVTex();

	if (vt_detail::PlaceTextureImp_FindSameTextureInfo(pVTex, pTex, pX, pY))
		return true;

	bool bResult = vt_detail::PlaceTextureImp_FindNewPosition(pVTex, pTex, pX, pY);

	if (bResult)
	{
		i3VirtualTextureData* pVTData = pVTex->getData();
		I3VT_TEXINFO* newTexInfo = vt_detail::AddRefTexture(pVTData, pTex, *pX, *pY);

		INT32 idxTexInfo = INT32(pVTData->m_linearTexInfoList.size() - 1);
		this->CreateFileHandle(idxTexInfo);

		vt_detail::SettleTexture_ByVTexImage(pVTex, *pX, *pY, pTex, idxTexInfo, newTexInfo);
	}

	return bResult;
}
