#include "i3FrameworkPCH.h"

#include "i3VTFileFormatUtil.h"

#include "i3UIManager.h"
#include "i3VirtualTextureIO.h"

//#include "i3VTFileFormatController.h"

#include "i3VTFileFormat.h"

#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_is_iless.h"

#include "i3Base/string/system/is_filedate_less.h"
#include "i3Base/string/system/is_file.h"
#include "i3Base/string/system/collect_directoryname_and_filename.h"

#include "i3UIFilePathMgr.h"

namespace vt_detail
{
	I3VT_TILE* getTileByPixel(const i3VirtualTextureData* pData, INT32 px, INT32 py); // i3VirtualTexture.cpp
	I3VT_TILE * getTileByIndex(I3VT_TILE* TileTable, INT32 xidx, INT32 yidx);	// i3VirtualTexture.cpp


	// 아 이 함수 너무 거지 같네..고쳤는데도 여전이 더럽고 션찮다...
	// 이 함수의 존재 자체를 말살해야될듯 함..
	//0:UIRoot, 1:Nation 2:Image
	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx)
	{
		if (strDefault.empty())
		{
			res[0] = 0;
			return;
		}

		switch (idx)
		{
		case 0:  // UIRoot 사용하지 않음...그냥 빈 거 돌려줍니다.
			res[0] = 0;
			return;

		case 1: // 국가는 그냥 로케일 돌려줍니다. ( 순수 국가명만 나올거 같음)
		{
			i3::rc_string strLang = i3UIFilePathMgr::i()->GetCurrentLangaugeString();
			i3::safe_string_copy(res, strLang, MAX_PATH);
		}
			return;

		case 2: // 이미지 이름.. (????) .. 로케일이 포함되어 있으면 떼버려야함...(근데 뭔 구현이 이렇게 드러웠는지 몰것다..)..물론 지금도 더럽다...
		{
			// UIRe\Locale\(나라)\Image\xxxx
			i3::stack_string strRefined;
			i3UIFilePathMgr::i()->ObtainUniquePath(strDefault, strRefined);
			
			i3::safe_string_copy(res, strRefined, MAX_PATH);
			return;
		}
		

		default:
			res[0] = 0;
			break;
		}
	}
	
	bool FindWrongTexture(const char* szFull)
	{
		return i3System::IsFile(szFull);
	}



#define MAXRETRIES 100
#define RETRYDELAY 100

	bool TryOpenFileStream(const char* pszPath, i3FileStream& outFile)
	{
		INT32 Retries = 0;
		DWORD acc = STREAM_READ | STREAM_SHAREREAD | STREAM_SHAREWRITE;

		do
		{
			if (outFile.Open(pszPath, acc) == false)
			{
				DWORD dwErr = GetLastError();

				if (ERROR_SHARING_VIOLATION == dwErr)
				{
					Retries += 1;
					Sleep(RETRYDELAY);
					continue;
				}
				else
				{
					I3PRINTLOG(I3LOG_WARN, "Could not open %s virtual texture file.", pszPath);
					return false;
				}
			}
			else
				break;

		} while (Retries < MAXRETRIES);

		return (Retries < MAXRETRIES);
	}
	
	void WaitIOCompletion(i3VirtualTextureIO* io)
	{
		while (!io->IsEmpty())
		{
			Sleep(1);
		}
	}


	__HANDLE_TYPE OpenVTexFile(const char * pszPath, bool bCreate)
	{
		__HANDLE_TYPE		hFile;

#if defined( I3_WINDOWS)
		/*hFile = ::CreateFile( pszPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING,
		NULL);*/

		DWORD dwRetries = 0;

		DWORD FileAccess = GENERIC_READ;
		DWORD FileAttribute = FILE_SHARE_READ | FILE_SHARE_WRITE;
		DWORD createFlag;

		if (i3UI::getToolDevMode())
		{
			FileAccess |= GENERIC_WRITE;
			//FileAttribute |= FILE_SHARE_WRITE;
		}

		if (bCreate)
			createFlag = CREATE_ALWAYS;
		else
			createFlag = OPEN_EXISTING;

		do
		{
			hFile = ::CreateFile(pszPath, FileAccess,
				FileAttribute,
				NULL,
				createFlag,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING,
				NULL);

			// 다른 Process에서 사용중이면 다시 시도하도록 한다.
			if (INVALID_HANDLE_VALUE == hFile)
			{
				DWORD dwErr = GetLastError();

				if (ERROR_SHARING_VIOLATION == dwErr)
				{
					dwRetries += 1;
					Sleep(RETRYDELAY);
					continue;
				}
				else
				{
					// An error occurred.
					I3TRACE("Open fail VT %s file.\n", pszPath);
					//open 실패하면 다시 시도하게 해야지..
					dwRetries += 1;
					Sleep(RETRYDELAY);
					continue;
				}
			}

			break;
		} while (dwRetries < MAXRETRIES);

#endif

		return hFile;
	}



	UINT64 GetVTexFileSize(__HANDLE_TYPE hFile)
	{
#if defined( I3_WINDOWS)
		DWORD hi, lo;

		lo = ::GetFileSize(hFile, &hi);

		return ((UINT64)hi << 32) | lo;
#endif
	}

	bool CheckRefTextureFiles(const i3VirtualTexture* pVTex)
	{

		INT32 i;
		I3VT_TEXINFO * pInfo;

		//2015-01-13 PB_ENG 김대영 국가를 지정하고 어드저스트 할 경우 해당 국가 폴더의 이미지 파일들 대상으로 작업하도록 변경합니다. (수정전 : 국가 상관없이 UIRE 폴더 대상으로 작업 )
		const INT32 numRefTexture = pVTex->getRefTextureCount();

		for (i = 0; i < numRefTexture; i++)
		{
			pInfo = pVTex->getRefTexture(i);

			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pInfo->m_strName, strUniquePath);

			i3::rc_string strLocalePath = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);

			i3::stack_string strFullPath = strLocalePath;
			strFullPath += '/';
			strFullPath += strUniquePath;

			if (i3System::IsFile(strFullPath.c_str()) == false)
			{
				if (i3UI::getToolDevMode())
				{
					i3::stack_string msg;
					i3::sprintf(msg, "%s file is not exist!.", pInfo->m_strName);
					MessageBox(nullptr, msg.c_str(), "Error", MB_OK);
				}

				I3PRINTLOG(I3LOG_WARN, "%s file is not exist!.", pInfo->m_strName);
				return false;
			}
		}

		return true;
	}

	// 사이즈 외엔 걍 알파벳 정렬 순서로다...
	template<bool isOld>
	struct sortRefTexture
	{
		static void call(i3::vector<I3VT_TEXINFO*>& destList)
		{
			struct comp
			{
				bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
				{
					if (lhs->m_AddedDate < rhs->m_AddedDate)
						return true;
					return lhs->m_CODE < rhs->m_CODE;
				}
			};
			i3::stable_sort(destList.begin(), destList.end(), comp());
		}
	};

	template<>
	struct sortRefTexture<false>		// 신버전 정렬...
	{
		static void call(i3::vector<I3VT_TEXINFO*>& destList)
		{
			struct comp
			{
				bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
				{
					INT32 lhsSize = lhs->m_Width * lhs->m_Height;
					INT32 rhsSize = rhs->m_Width * rhs->m_Height;

					if (lhsSize > rhsSize)				// 사이즈가 큰게 우선이어야 하므로 평소의 반대로 뒤집음..
						return true;

					if (lhsSize == rhsSize)
						return i3::generic_is_iless(lhs->m_strName, rhs->m_strName);

					return false;
				}
			};
			i3::stable_sort(destList.begin(), destList.end(), comp());
		}
	};

	void  CopyAndSortRefTextures(const i3::vector<I3VT_TEXINFO*>& srcList, i3::vector<I3VT_TEXINFO*>& destList, bool bOldVersion)
	{
		// 정렬 타입 리스트의 값을 멋대로 바꾸지 말것..
		// 다른 컨테이너에 카피하고 그것으로 처리..

		destList.reserve(srcList.size());
		destList = srcList;

		if (bOldVersion)
			sortRefTexture<true>::call(destList);
		else
			sortRefTexture<false>::call(destList);
	}
	
	void ResetAllTileData(const i3VirtualTextureCtx& Ctx, I3VT_TILE* inoutTileTable)
	{

		for (INT32 y = 0; y < Ctx.m_cntTileY; y++)
		{
			for (INT32 x = 0; x < Ctx.m_cntTileX; x++)
			{
				I3VT_TILE* pTile = getTileByIndex(inoutTileTable, x, y);

				pTile->setState(0);
				pTile->setTexInfo(nullptr);
				pTile->setSubset(0);
				pTile->setFileOffset(0);
				pTile->clearRTile();
				pTile->clearVTile();
			}
		}
	}

	void CreateTileTable(const i3VirtualTextureCtx& Ctx, I3VT_TILE*& inoutTileTable)
	{
		I3MEM_SAFE_FREE(inoutTileTable);

		UINT32 szTileInfo = sizeof(I3VT_TILE) * Ctx.m_cntTileTotal;

		inoutTileTable = (I3VT_TILE *)i3MemAlloc(szTileInfo);

		ResetAllTileData(Ctx, inoutTileTable);
	}

	INT32 AdjustUnit(const i3VirtualTextureCtx& Ctx, INT32 x, bool bRoundOff)
	{
		INT32 newX;

		newX = (x / Ctx.m_TileSize) * Ctx.m_TileSize;

		if (x % Ctx.m_TileSize)
			newX += Ctx.m_TileSize;

		return newX;
	}

	I3VT_TEXINFO* AddRefTexture(i3VirtualTextureData* pData, i3Texture * pTex, INT32 x, INT32 y, UINT64 date = 0L)
	{
	
		i3::stack_string uniquePath;

		i3UIFilePathMgr::i()->ObtainUniquePath(pTex->GetNameString(), uniquePath);

		i3::make_unix_path(uniquePath);

		typedef I3VT_TEXINFO_INDEX_SET::iterator set_itearator;

		I3VT_TEXINFO_INDEX_SET::insert_commit_data		commit_data;

		i3::pair<set_itearator, bool> pr = pData->m_str_refTexIdx_set.insert_check_other(uniquePath, commit_data);

		if (pr.second == false)	// 이미 존재..
		{
			return *pr.first;						// AddRef없이 그냥 넘기면 됨..리턴값은 참조용임...
		}

		// 없을때만 생성하고 삽입한다...없을때만 생성하자..생성했다가 있으니까..그때 삭제하는건 비합리적인 일임...

		I3VT_TEXINFO * pInfo = new I3VT_TEXINFO;
		NetworkDump_Malloc(pInfo, sizeof(I3VT_TEXINFO), __FILE__, __LINE__);

		pInfo->m_strName = uniquePath;

		{
			char code[16];
			UINT128 * pTemp = (UINT128 *)code;
			i3::stack_string strUpper;

			i3::safe_string_copy(strUpper, pInfo->m_strName, MAX_PATH);
			i3::to_upper(strUpper);
			i3::make_unix_path(strUpper);

			i3MD5::Gen(const_cast<char*>(strUpper.c_str()), strUpper.size(), code);		// 아 const문제 거지같네..

			pInfo->m_CODE = *pTemp;		// m_CODE 따위 이제 쓰지 않고 싶지만 호환성 때문에...-_-;;;;;;;;;;;;
		}

		pInfo->m_Width = pTex->GetWidth();
		pInfo->m_Height = pTex->GetHeight();
		pInfo->m_X = x;
		pInfo->m_Y = y;

		if (date == 0L)
		{
			SYSTEMTIME sysTm;

			::GetSystemTime(&sysTm);
			::SystemTimeToFileTime(&sysTm, (LPFILETIME)& pInfo->m_AddedDate);
		}
		else
		{
			pInfo->m_AddedDate = date;	// 이것도 다 호환성 때문에...이런거..AdjustVirtualTexture에서 
		}								// 거대 메모리 공간에서 차지할 타일 위치의 선점 우선순위가 날짜로 결정된다...거지 같음...


										// 경로의 소문자화 루틴은 삭제했다!!! 이런거 안할수 있는 방법이 생기면 안하는게 좋음..

		pData->m_linearTexInfoList.push_back(pInfo);		// 뒤에다가 일단 넣는다...실시간 소팅이 안되면 문제가 되는지 안되는지를 모르겠다..
															// 이게 문제가 된다면 AddRefTexture를 단독으로 쓰면 안된다...

		pData->m_str_refTexIdx_set.insert_commit(pInfo, commit_data);

		return	pInfo;
	}


	void    SettleTextureImp(i3VirtualTexture* pVTex, INT32 x, INT32 y, i3Texture* pTex, I3VT_TEXINFO* pVTexInfo, INT32 subsetIdx)
	{
		I3VT_REQ_HNDL req;
		HRESULT hRv;

		i3VTFileFormat* pFF = pVTex->GetFileFormat();
		I3VT_IOHANDLE * pHandle = pFF->getFileHandle(subsetIdx);
		I3ASSERT(pHandle != nullptr);

		i3VirtualTextureIO* pIO = pVTex->GetIO();
		i3VirtualTextureCache* pVTCache = pVTex->getCache();


		i3VirtualTextureCtx& Ctx = *pVTex->getContext();

		// Surface를 복사에 사용될 임시 Texture 생성.
		i3Texture * pTileTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		pTileTex->SetName("VirtualTile");
#endif
		pTileTex->Create(I3VT_TILE_SIZE, I3VT_TILE_SIZE, 1, Ctx.m_Format, 0);

		i3Surface* pDestSurface = pTileTex->getSurface(0);
		i3Surface* pSrcSurface = pTex->getSurface(0);

		INT32 cx, cy, accmCX, accmCY;
		accmCX = accmCY = cx = cy = 0;

		RECT rcDest, rcSrc;

		// Texture들을 Tile 영역별로 나누어 
		for (INT32 sy = y; sy < (y + (INT32)pTex->GetHeight());)
		{
			INT32 ey = ((sy + I3VT_TILE_SIZE) / I3VT_TILE_SIZE) * I3VT_TILE_SIZE;
			cy = ey - sy;

			if ((accmCY + cy) > (INT32) pTex->GetHeight())
				cy = pTex->GetHeight() - accmCY;

			accmCX = 0;

			for (INT32 sx = x; sx < (x + (INT32)pTex->GetWidth());)
			{
				INT32 ex = ((sx + I3VT_TILE_SIZE) / I3VT_TILE_SIZE) * I3VT_TILE_SIZE;
				cx = ex - sx;

				if ((accmCX + cx) > (INT32) pTex->GetWidth())
					cx = pTex->GetWidth() - accmCX;

				rcDest.left = sx % I3VT_TILE_SIZE;
				rcDest.top = sy % I3VT_TILE_SIZE;
				rcDest.right = rcDest.left + cx;
				rcDest.bottom = rcDest.top + cy;

				rcSrc.left = sx - x;
				rcSrc.top = sy - y;
				rcSrc.right = rcSrc.left + cx;
				rcSrc.bottom = rcSrc.top + cy;

				I3VT_TILE * pTile = vt_detail::getTileByPixel(pVTex->getData(), sx, sy);

				// Subset 정보를 갱신
				//	pTile->setRefTexture( idxRef);
				pTile->setTexInfo(pVTexInfo);
				pTile->setSubset(subsetIdx);

				// 만약 이전 Pixel Data가 있다면 일단 읽어 온다.

				if (pTile->hasData())
				{
					pTileTex->Lock(0);

					req = pIO->ReqLoad(pHandle, pTile->getFileOffset(), pTileTex->GetDataSize(), pTileTex->GetLockedBuffer(0), 0);
					pIO->Wait(req);

					pTileTex->Unlock(0);
				}


#if defined( I3G_DX)
				{
					hRv = D3DXLoadSurfaceFromSurface(pDestSurface->getSurface(), nullptr, &rcDest,
						pSrcSurface->getSurface(), nullptr, &rcSrc,
						D3DX_FILTER_POINT, 0);
				}
#else
#error "You have to implement the function for this platform!"
#endif

				if (FAILED(hRv))
				{
					I3PRINTLOG(I3LOG_WARN, "Could not copy pixel data from the original texture.");
					goto ErrHandle;
				}

				// 복사된 Pixel Data의 저장.
				{
					UINT32 sz = i3Gfx::CalcRasterBufferSize(I3VT_TILE_SIZE, I3VT_TILE_SIZE, 1, Ctx.m_Format);

					pTileTex->Lock(0, I3G_LOCK_READONLY);

					if (pTile->hasData() == false)
					{
						// 기존의 데이터가 없다. 파일의 끝에 배치.
						pTile->setFileOffset(pHandle->m_LastDataOffset);

						pHandle->m_LastDataOffset += sz;
						pHandle->m_LastDataOffset = _ALIGN_(pHandle->m_LastDataOffset, 2048);
					}
					else
					{
						I3TRACE("*** Non-Empty Tile!!!!\n");
					}

					req = pIO->ReqWrite(pHandle, pTile->getFileOffset(), sz, pTileTex->GetLockedBuffer(0));
					pIO->Wait(req);


					if (pVTCache != nullptr)
					{
						// 변경된 tile의 내용을 각 Cache에 Update한다.
						pVTCache->RefreshTile(pVTex->getTileIndex(pTile), pTileTex->GetLockedBuffer(0));
					}

					pTileTex->Unlock(0);
				}

				accmCX += cx;
				sx = ex;
			}

			accmCY += cy;

			sy = ey;
		}

	ErrHandle:

		I3_SAFE_RELEASE(pTileTex);

	}
	
	I3VT_TEXINFO* FindRefTextureByRegion(i3VirtualTexture* pVTex, INT32 x, INT32 y, INT32 w, INT32 h)
	{
		INT32 i;
		I3VT_TEXINFO * pInfo;
		i3::pack::RECT s, d;

		d.left = (REAL32)x;
		d.top = (REAL32)y;
		d.right = (REAL32)(x + w - 1);
		d.bottom = (REAL32)(y + h - 1);

		for (i = 0; i < pVTex->getRefTextureCount(); i++)
		{
			pInfo = pVTex->getRefTexture(i);

			s.left = (REAL32)pInfo->m_X;
			s.top = (REAL32)pInfo->m_Y;
			s.right = s.left + pInfo->m_Width - 1;
			s.bottom = s.top + pInfo->m_Height - 1;

			if (i3Math::RectInRect(&s, &d) != TOUCH_NONE)
			{
				return pInfo;
			}
		}

		return nullptr;
	}
	
	bool	PlaceTextureImp_FindSameTextureInfo(i3VirtualTexture* pVTex, i3Texture * pTex, INT32 * pX, INT32 * pY)
	{
		I3VT_TEXINFO * pInfo = nullptr;
		// 기존의 Ref Texture 중에 동일한 것이 존재하는지 확인
		
		i3::stack_string strUniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(pTex->getResourceLoadPath(), strUniquePath);

		pInfo = pVTex->FindRefTexture(strUniquePath.c_str());
		
		if (pInfo != nullptr)
		{
			*pX = pInfo->m_X;
			*pY = pInfo->m_Y;
			return true;
		}

		return false;
	}

	bool	PlaceTextureImp_FindNewPosition(i3VirtualTexture* pVTex, i3Texture * pTex, INT32 * pX, INT32 * pY)
	{
		i3VirtualTextureCtx& Ctx = *pVTex->getContext();

		INT32 x = 0, y = 0;
		I3VT_TEXINFO * pInfo = nullptr;

		// 현재 등록되어 있는 Reference Texture들을 모두 비교하여 배치할 영역을 찾는다.
		if (pVTex->getRefTextureCount() == 0)
		{
			*pX = x;	*pY = y;

			return true;
		}

		for (INT32 i = 0; i < pVTex->getRefTextureCount(); i++)
		{
			pInfo = pVTex->getRefTexture(i);

			// 1. 현재 Texture의 오른쪽에 배치 시도
			{
				x = vt_detail::AdjustUnit(Ctx, pInfo->m_X + pInfo->m_Width, true);
				y = vt_detail::AdjustUnit(Ctx, pInfo->m_Y, false);

				if (x + INT32(pTex->GetWidth()) < I3VT_VTEX_SIZE)
				{
					I3VT_TEXINFO * pFoundInfo = vt_detail::FindRefTextureByRegion(pVTex, x, y, pTex->GetWidth(), pTex->GetHeight());
					if (pFoundInfo == nullptr)
					{
						// 찾았다!
						*pX = x;	*pY = y;
						return true;
					}
				}
			}

			// 2. 현재 Texture의 아래쪽에 배치 시도
			{
				x = vt_detail::AdjustUnit(Ctx, pInfo->m_X, false);
				y = vt_detail::AdjustUnit(Ctx, pInfo->m_Y + pInfo->m_Height, true);

				if (y + INT32(pTex->GetHeight()) < I3VT_VTEX_SIZE)
				{
					I3VT_TEXINFO * pFoundInfo = vt_detail::FindRefTextureByRegion(pVTex, x, y, pTex->GetWidth(), pTex->GetHeight());
					if (pFoundInfo == nullptr)
					{
						// 찾았다!
						*pX = x;	*pY = y;
						return true;
					}
				}
			}
		}

		return false;

	}


}


namespace vt_util
{
	void GetVariousUIFileName(char* res, const i3::literal_range& strDefault, int idx)
	{
		vt_detail::GetVariousUIFileName(res, strDefault, idx);
	}

	bool IsAnyUilFileExist_InVTexDirectory(const i3::literal_range& path)
	{
		i3::stack_string uniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(path, uniquePath);

		i3::rc_string strNewFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
		i3::stack_string fullPathNew = strNewFormatPath;
		fullPathNew += '/';
		fullPathNew += uniquePath;

		if (i3System::IsFile(fullPathNew.c_str()))
			return true;

		i3::rc_string strOldFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);
		i3::stack_string fullPathOld = strOldFormatPath;
		fullPathOld += '/';
		fullPathOld += uniquePath;

		if (i3System::IsFile(fullPathOld.c_str()))
			return true;

		return false;
	}

	bool IsVirtualTextureFileInHardDisk(const i3::literal_range& strFullPath, bool isVTexListFormat)
	{
		i3::stack_string strModPath;

		if (isVTexListFormat)
			i3::change_file_ext_copy(strModPath, strFullPath, "i3VTexList");
		else
			i3::change_file_ext_copy(strModPath, strFullPath, "i3VTex");

		if (i3System::IsFile(strModPath.c_str()))
			return true;

		return false;
	}


	bool SelectUilFileByDate_InVTexDirectory(const i3::literal_range& path,
		i3::stack_string& outFullPath, bool& outIsVTexListFormat)
	{

		i3::stack_string uniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(path, uniquePath);

		i3::rc_string strNewFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
		i3::stack_string fullPathNew = strNewFormatPath;
		fullPathNew += '/';
		fullPathNew += uniquePath;

		i3::rc_string strOldFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);
		i3::stack_string fullPathOld = strOldFormatPath;
		fullPathOld += '/';
		fullPathOld += uniquePath;

		bool bNewFormatExist = i3System::IsFile(fullPathNew.c_str());
		bool bOldFormatExist = i3System::IsFile(fullPathOld.c_str());

		if (!bNewFormatExist && !bOldFormatExist)		// 모두 존재 안하면 망...
		{
			outFullPath = path;
			outIsVTexListFormat = false;
			return false;
		}

		if (!bOldFormatExist)		// 신규포맷만 존재..
		{
			outFullPath = fullPathNew;
			outIsVTexListFormat = true;
			return true;
		}
		if (!bNewFormatExist)		// 구포맷만 존재..
		{
			outFullPath = fullPathOld;
			outIsVTexListFormat = false;
			return true;
		}

		//// 둘다 존재하면 갱신 날짜 비교...
		UINT64 addedDateNew = 0;
		UINT64 addedDateOld = 0;
		LPFILETIME fileTimeNew = (LPFILETIME)&addedDateNew;
		LPFILETIME fileTimeOld = (LPFILETIME)&addedDateOld;

		HANDLE h_fileNew = ::CreateFile(fullPathNew.c_str(),
			GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		::GetFileTime(h_fileNew, NULL, NULL, fileTimeNew);
		::CloseHandle(h_fileNew);

		HANDLE h_fileOld = ::CreateFile(fullPathOld.c_str(),
			GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		::GetFileTime(h_fileOld, NULL, NULL, fileTimeOld);
		::CloseHandle(h_fileOld);

		// 숫자가 큰 놈이 아무래도 최신??
		if (addedDateOld <= addedDateNew)
		{
			outIsVTexListFormat = true;
			outFullPath = fullPathNew;
		}
		else
		{
			outIsVTexListFormat = false;
			outFullPath = fullPathOld;
		}

		return true;
	}

	namespace detail
	{
		bool SelectUilOri3UILibHdr(const i3::stack_string& inputFullPath, bool isVTexListFolder, i3::stack_string& outFullPath, bool& outIsUILibHdr)
		{
			i3::stack_string FullPath_Uil;
			i3::stack_string FullPath_UILibHdr;

			i3::change_file_ext_copy(FullPath_Uil, inputFullPath, "i3Uil");
			i3::change_file_ext_copy(FullPath_UILibHdr, inputFullPath, "i3UILibHdr");

			bool uil_exist = i3::is_file(FullPath_Uil);
			bool uilibhdr_exist = i3::is_file(FullPath_UILibHdr);

			if (!uil_exist && !uilibhdr_exist)
			{
				outFullPath.clear();
				outIsUILibHdr = false;
				return false;
			}

			if (!uil_exist)
			{
				outFullPath = FullPath_UILibHdr;	outIsUILibHdr = true;	return true;
			}
			if (!uilibhdr_exist)
			{
				outFullPath = FullPath_Uil;		outIsUILibHdr = false;	return true;
			}

			if (isVTexListFolder)
			{
				outFullPath = FullPath_UILibHdr;	outIsUILibHdr = true;
			}
			else
			{
				outFullPath = FullPath_Uil;		outIsUILibHdr = false;
			}

			/*   같은 버추얼텍스쳐 폴더 안에서 uil 과 uilibhdr파일이 동시 존재하는 경우, 날짜 비교대신에
			     텍스쳐 폴더가 구버전,신버전 폴더인지 여부로 outUiLibHdr 불값을 결정하도록 버그 수정한다.
			if (i3::is_modified_filedate_less(FullPath_Uil, FullPath_UILibHdr))
			{
				outFullPath = FullPath_UILibHdr;	outIsUILibHdr = true; 
			}
			else
			{
				outFullPath = FullPath_Uil;		outIsUILibHdr = false;
			}
			*/
			return true;
		}
	}
	

	bool SelectUILibraryFileByDate_InVTexDirectory(const i3::literal_range& path, i3::stack_string& outFullPath,
		bool& outIsUILibHdrFormat, bool& outIsVTexListFormat)
	{
		i3::stack_string uniquePath;
		i3UIFilePathMgr::i()->ObtainUniquePath(path, uniquePath);

		i3::rc_string strNewVTexFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);

		i3::stack_string NewVTexFormatFullPath = strNewVTexFormatPath;
		NewVTexFormatFullPath += '/';
		NewVTexFormatFullPath += uniquePath;

		bool bIs_NewVTex_i3UILibHdr;
		bool bExist_NewVTex = detail::SelectUilOri3UILibHdr(NewVTexFormatFullPath, true  ,NewVTexFormatFullPath, bIs_NewVTex_i3UILibHdr);
		
		i3::rc_string strOldVTexFormatPath = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);
		i3::stack_string OldVTexFormatFullPath = strOldVTexFormatPath;
		OldVTexFormatFullPath += '/';
		OldVTexFormatFullPath += uniquePath;
		
		bool bIs_OldVTex_i3UILibHdr;
		bool bExist_OldVTex = detail::SelectUilOri3UILibHdr(OldVTexFormatFullPath, false, OldVTexFormatFullPath, bIs_OldVTex_i3UILibHdr);
					
		if (!bExist_NewVTex && !bExist_OldVTex)
		{
			outFullPath.clear();	outIsUILibHdrFormat = false;	outIsVTexListFormat = false;
			return false;
		}

		if (!bExist_NewVTex)
		{
			outFullPath = OldVTexFormatFullPath;	outIsUILibHdrFormat = bIs_OldVTex_i3UILibHdr;
			outIsVTexListFormat = false;
			return true;
		}

		if (!bExist_OldVTex)
		{
			outFullPath = NewVTexFormatFullPath;	outIsUILibHdrFormat = bIs_NewVTex_i3UILibHdr;
			outIsVTexListFormat = true;
			return true;
		}

		if (i3::is_modified_filedate_less(OldVTexFormatFullPath, NewVTexFormatFullPath))
		{
			outFullPath = NewVTexFormatFullPath;	outIsUILibHdrFormat = bIs_NewVTex_i3UILibHdr;
			outIsVTexListFormat = true;
		}
		else
		{
			outFullPath = OldVTexFormatFullPath;	outIsUILibHdrFormat = bIs_OldVTex_i3UILibHdr;
			outIsVTexListFormat = false;
		}
				
		return true;
	}
	
	bool CollectSubDirectoryAndFilename(const i3::literal_range& strDir,
			i3::vector<i3::rc_string>& vecSubDir,
			i3::vector<i3::rc_string>& vecFileName, bool& outIsInPackFile)
	{
		outIsInPackFile = false;
		const bool isToolMode = i3UI::getToolDevMode();		// UIEditor에서는 팩파일 검색을 해서는 안된다.
		
		const char* pszWorkingDir = i3ResourceFile::GetWorkingFolder();
		i3::stack_string input_path;
		if (pszWorkingDir[0] != '\0')
		{
			i3::make_relative_path(pszWorkingDir, strDir, input_path);
		}
		else
		{
			input_path = strDir;
		}

		if (!isToolMode)		// 팩파일 검색 시도..
		{
			i3PackNode* pDirNode = i3Pack::FindDirectoryPackNode(input_path.c_str());

			if (pDirNode)		// 팩파일에 포함되었으면 이것만 고려하고 실제 폴더는 더 이상 고려하지 않는다!!...
			{
				i3::stack_string strExt;

				const INT32 numFile =	pDirNode->getFileCount();
				vecFileName.reserve(numFile);
				
				for (INT32 i = 0; i < numFile; ++i)
				{
					I3_PACK_FILE_REF* pFileRef = pDirNode->getFile(i);

					vecFileName.push_back(pFileRef->m_szName);
				}
				
				const INT32 numChild = pDirNode->getChildCount();
				vecSubDir.reserve(numChild);

				for (INT32 i = 0; i < numChild; ++i)
				{
					i3PackNode* pChildNode = static_cast<i3PackNode*>(pDirNode->getChild(i));
					vecSubDir.push_back(pChildNode->GetNameString());
				}

				outIsInPackFile = true;
				return true;
			}
		}

		// 툴이거나, PACK파일 없는 경우에 실제 폴더 검색한다..
	
		if (pszWorkingDir[0] != '\0')
		{
			i3::stack_string tmp_inputPath = input_path;
			input_path = pszWorkingDir;	input_path += '/';	input_path += tmp_inputPath;
		}
		
		return i3::collect_directoryname_and_filename(input_path, vecSubDir, vecFileName);
	
	}
	
	/*
	bool SelectVirtualTextureFileByDate(const i3::literal_range& inputPath,
		i3::rc_string& outResultPath, bool& outIsNewFormat)
	{
		bool bNewFormatExist = false;
		bool bOldFormatExist = false;
		i3::stack_string strModPathNew, strModPathOld;

		i3::change_file_ext_copy(strModPathNew, inputPath, "i3VTexList");

		if (i3System::IsFile(strModPathNew.c_str()))
		{
			bNewFormatExist = true;
		}
		
		i3::change_file_ext_copy(strModPathOld, inputPath, "i3VTex");

		if (i3System::IsFile(strModPathOld.c_str()))
		{
			bOldFormatExist = true;
		}

		if (bNewFormatExist == false && bOldFormatExist == false)		// 아예 없다.
			return false;

		if (bNewFormatExist && bOldFormatExist)
		{
			// 둘다 존재하면 최신 시간인것을 선택....시간마저 같다면 신규포맷으로 결정..
		
			UINT64 addedDateNew = 0;
			UINT64 addedDateOld = 0;
			LPFILETIME fileTimeNew = (LPFILETIME)&addedDateNew;
			LPFILETIME fileTimeOld = (LPFILETIME)&addedDateOld;

			HANDLE h_fileNew = ::CreateFile(strModPathNew.c_str(), 
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			::GetFileTime(h_fileNew, NULL, NULL, fileTimeNew);
			::CloseHandle(h_fileNew);

			HANDLE h_fileOld = ::CreateFile(strModPathOld.c_str(),
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			::GetFileTime(h_fileOld, NULL, NULL, fileTimeOld);
			::CloseHandle(h_fileOld);

			// 숫자가 큰 놈이 아무래도 최신??
			if (addedDateOld < addedDateNew)
			{
				outIsNewFormat = true;
				outResultPath = strModPathNew;
			}
			else
			{
				outIsNewFormat = false;
				outResultPath = strModPathOld;
			}
		}
		else
		{
			outIsNewFormat = bNewFormatExist;
			outResultPath = (bNewFormatExist) ? strModPathNew : strModPathOld;
		}
		
		return true;
	}

	bool IsAnyVirtualTextureFileInHardDisk(const i3::literal_range& strPath)
	{
		i3::stack_string strModPath;

		i3::change_file_ext_copy(strModPath, strPath, "i3VTexList");

		if (i3System::IsFile(strModPath.c_str()))
			return true;

		i3::change_file_ext_copy(strModPath, strPath, "i3VTex");

		if (i3System::IsFile(strModPath.c_str()))
			return true;

		return false;
	}
	*/

}
