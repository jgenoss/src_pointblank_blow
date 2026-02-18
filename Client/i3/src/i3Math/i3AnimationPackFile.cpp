#include "i3MathType.h"
#include "i3AnimationPackFile.h"
#include "i3Animation.h"
#include "i3Animation2.h"
#include "i3AnimationResManager.h"

//I3_CLASS_INSTANCE( i3AnimationPackFile, i3ElementBase);
I3_CLASS_INSTANCE(i3AnimationPackFile);

#define	TARRAY_EPS			0.0001f
#define RARRAY_EPS			0.0001f
#define SARRAY_EPS			0.0001f

i3Animation2 *	i3AnimationPackFile::_Convert2Anim2(i3Animation * pAnim)
{
	INT32 i, j, cnt, idx;
	i3Animation2 * pAnim2;

	bool b16Bits;

	pAnim2 = i3Animation2::new_object();

	i3TransformSequence * pSeq0 = (i3TransformSequence *)pAnim->GetTrack(0);

	if (pSeq0->GetKeyframeChannels() & i3TransformSequence::REAL16_FORMAT)
	{
		b16Bits = true;
	}
	else
		b16Bits = false;

	pAnim2->Create(pAnim);

	// 필요한 메모리의 할당은 끝나다.
	// 실질적인 값을 설정.
	for (i = 0; i < pAnim->GetTrackCount(); i++)
	{
		i3TransformSequence * pSeq = (i3TransformSequence *)pAnim->GetTrack(i);

		pAnim2->SetTrack(i, nullptr, pAnim->GetTrackBoneName(i));

		i3::pack::animation::TRACK * pDestSeq = pAnim2->getSequence(i);
		pDestSeq->m_KeyInterval = pSeq->GetKeyframeInterval();

		// Translation
		if (pSeq->HasTranslation())
		{
			VEC3D key;

			if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE)
				cnt = 1;
			else
				cnt = pSeq->GetKeyframeCount();

			if (b16Bits)
			{	// 16Bits
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetTranslation(j, &key);

					idx = m_TList16Bits.Find(&key, false, TARRAY_EPS);

					if (idx == -1)
					{
						idx = m_TList16Bits.Add(&key, TARRAY_EPS);
					}

					//I3ASSERT( idx < 0xFFFF);
					pDestSeq->m_pTIndex[j] = idx;

					m_TotalT16Count++;
				}
			}
			else
			{
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetTranslation(j, &key);

					idx = m_TList.Find(&key, false, TARRAY_EPS);

					if (idx == -1)
					{
						idx = m_TList.Add(&key, TARRAY_EPS);
					}

					pDestSeq->m_pTIndex[j] = idx;

					m_TotalTCount++;
				}
			}
		}

		// Rotation
		if (pSeq->HasRotation())
		{
			QUATERNION key;

			if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE)
				cnt = 1;
			else
				cnt = pSeq->GetKeyframeCount();

			if (b16Bits)
			{	// 16bits
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetRotation(j, &key);

					idx = m_RList16Bits.Find(&key, false, RARRAY_EPS);

					if (idx == -1)
					{
						idx = m_RList16Bits.Add(&key, RARRAY_EPS);
					}

					//I3ASSERT( idx < 0xFFFF);
					pDestSeq->m_pRIndex[j] = idx;

					m_TotalR16Count++;
				}
			}
			else
			{
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetRotation(j, &key);

					idx = m_RList.Find(&key, false, RARRAY_EPS);

					if (idx == -1)
					{
						idx = m_RList.Add(&key, RARRAY_EPS);
					}

					pDestSeq->m_pRIndex[j] = idx;

					m_TotalRCount++;
				}
			}
		}

		// Scale
		if (pSeq->HasScale())
		{
			VEC3D key;

			if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE)
				cnt = 1;
			else
				cnt = pSeq->GetKeyframeCount();

			if (b16Bits)
			{	// 16Bits
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetScale(j, &key);

					idx = m_SList16Bits.Find(&key, false, SARRAY_EPS);

					if (idx == -1)
					{
						idx = m_SList16Bits.Add(&key, SARRAY_EPS);
					}

					//I3ASSERT( idx < 0xFFFF);
					pDestSeq->m_pSIndex[j] = idx;

					m_TotalS16Count++;
				}
			}
			else
			{
				for (j = 0; j < cnt; j++)
				{
					pSeq->GetScale(j, &key);

					idx = m_SList.Find(&key, false, SARRAY_EPS);

					if (idx == -1)
					{
						idx = m_SList.Add(&key, SARRAY_EPS);
					}

					pDestSeq->m_pSIndex[j] = idx;

					m_TotalSCount++;
				}
			}
		}

		// Time
		if (pSeq->HasTime())
		{
			for (j = 0; j < (INT32)pSeq->GetKeyframeCount(); j++)
			{
				pDestSeq->m_pTime[j] = pSeq->GetTime(j);
			}
		}
	}

	return pAnim2;
}

void i3AnimationPackFile::_BuildArrays(i3::vector<i3Animation*>& AnimList)
{
	INT32 tcnt = 0, rcnt = 0, scnt = 0;
	i3Animation * pAnim;

	// 개수를 구한다.
	for (size_t i = 0; i < AnimList.size(); i++)
	{
		pAnim = AnimList[i];

		for (INT32 j = 0; j < pAnim->GetTrackCount(); j++)
		{
			i3TransformSequence * pSeq = pAnim->GetTrack(j);

			if (pSeq->HasTranslation())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE)
					tcnt++;
				else
					tcnt += pSeq->GetKeyframeCount();
			}

			if (pSeq->HasRotation())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE)
					rcnt++;
				else
					rcnt += pSeq->GetKeyframeCount();
			}

			if (pSeq->HasScale())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE)
					scnt++;
				else
					scnt += pSeq->GetKeyframeCount();
			}
		}
	}

	m_TList.setCount(tcnt);
	m_RList.setCount(rcnt);
	m_SList.setCount(scnt);

	// 실제 값을 추가.
	tcnt = rcnt = scnt = 0;

	for (size_t i = 0; i < AnimList.size(); i++)
	{
		pAnim = AnimList[i];

		INT32 cnt;

		for (INT32 j = 0; j < pAnim->GetTrackCount(); j++)
		{
			i3TransformSequence * pSeq = pAnim->GetTrack(j);

			if (pSeq->HasTranslation())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE)
					cnt = 1;
				else
					cnt = pSeq->GetKeyframeCount();

				VEC3D vec;

				for (INT32 k = 0; k < cnt; k++)
				{
					pSeq->GetTranslation(k, &vec);
					m_TList.SetVector(tcnt, &vec);
					tcnt++;
				}
			}

			if (pSeq->HasRotation())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE)
					cnt = 1;
				else
					cnt = pSeq->GetKeyframeCount();

				QUATERNION q;

				for (INT32 k = 0; k < cnt; k++)
				{
					pSeq->GetRotation(k, &q);
					m_RList.SetVector(rcnt, &q);
					rcnt++;
				}
			}

			if (pSeq->HasScale())
			{
				if (pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE)
					cnt = 1;
				else
					cnt = pSeq->GetKeyframeCount();

				VEC3D vec;

				for (INT32 k = 0; k < cnt; k++)
				{
					pSeq->GetScale(k, &vec);
					m_SList.SetVector(scnt, &vec);
					scnt++;
				}
			}
		}
	}

	// Sorting
	//m_TList.Sort( TARRAY_EPS);
	//m_RList.Sort( RARRAY_EPS);
	//m_SList.Sort( SARRAY_EPS);

	// 중복된 값의 제거.

	printf("Compacting translation array...(%d)\n", m_TList.getCount());
	m_TList.RemoveRedundant(TARRAY_EPS);
	printf("Compacting rotation array...(%d)\n", m_RList.getCount());
	m_RList.RemoveRedundant(RARRAY_EPS);
	printf("Compacting scale array...(%d)\n", m_SList.getCount());
	m_SList.RemoveRedundant(SARRAY_EPS);

	printf("T Array : %d -> %d\n", tcnt, m_TList.getCount());
	printf("R Array : %d -> %d\n", rcnt, m_RList.getCount());
	printf("S Array : %d -> %d\n", scnt, m_SList.getCount());
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ANIMPACKFILE_HEADER
	{
		UINT8		m_ID[4] = { 'A', 'P', 'F', '3' };

		UINT32		m_AnimCount = 0;
		UINT32		m_TArrayCount = 0;
		UINT32		m_RArrayCount = 0;
		UINT32		m_SArrayCount = 0;

		INT64		m_DataOffset = 0;
		INT64		m_DataSize = 0;

		UINT32		m_T16ArrayCount = 0;
		UINT32		m_R16ArrayCount = 0;
		UINT32		m_S16ArrayCount = 0;

		UINT32		pad[29] = { 0 };
	};

	struct ANIM_INFO
	{
		UINT32		m_Type = 0;		// 0 : i3Animation, 1:i3Animation2, 2:i3Animation2 & 16Bits 3:i3Animation2 TrackTable's 32bits
		UINT32		m_TrackCount = 0;
		REAL32		m_Duration = 0.0f;
		INT64		m_Offset = 0;
		char		m_szName[256] = { 0 };
		UINT32		pad[2] = { 0 };
	};
}
#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#define		REL_POS()			(strm.GetPosition() - header.m_DataOffset)

#define		CHKGOTO( rc)		if( rc == STREAM_ERR)	goto ExitPart

UINT32 i3AnimationPackFile::SaveToFile(char * pszPath, const i3::vector<i3Animation*>& AnimList,
	i3AniPackFileProgress* progressCallback)
{
	UINT32 Rc;
	i3FileStream strm;
	pack::ANIMPACKFILE_HEADER header;
	pack::ANIM_INFO * pAnimTable = nullptr;
	UINT32	AnimTableOffset;
	i3::vector<i3Animation2*>	Anim2List;
	i3Animation2 *		pAnim2;
	i3Animation *		pAnim;

	m_TList.Reset();
	m_RList.Reset();
	m_SList.Reset();

	m_TList16Bits.Reset();
	m_RList16Bits.Reset();
	m_SList16Bits.Reset();

	m_TotalTCount = m_TotalRCount = m_TotalSCount = 0;
	m_TotalT16Count = m_TotalR16Count = m_TotalS16Count = 0;

	// 
	if (strm.Create(pszPath, STREAM_WRITE) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not create %s file.", pszPath);
		return STREAM_ERR;
	}

	// Header
	{
		header.m_AnimCount = AnimList.size();

		Rc = strm.Write(&header, sizeof(header));
		I3_CHKIO(Rc);
	}

	//_BuildArrays( pAnimList);

	if (progressCallback)
	{
		progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_START;
		progressCallback->m_arg0 = i3AniPackFileProgress::EPHASE_SAVE_START;
		progressCallback->m_arg1 = i3AniPackFileProgress::EPHASE_SAVE_END;
		if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_BEGIN))
			return STREAM_ERR;

		progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_CONV_I3ANIM_TO_I3ANIM2;
		progressCallback->m_arg0 = AnimList.size();
		if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_BEGIN))
			return STREAM_ERR;
	}

	// i3Animation -> i3Animation2 변환
	for (size_t i = 0; i < AnimList.size(); i++)
	{
		pAnim = AnimList[i];

		if (progressCallback)
		{
			progressCallback->m_argText0 = pAnim->GetName();
			progressCallback->m_arg1 = i;
			if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_UPDATE))
				return STREAM_ERR;
		}

		printf("ANIM->ANIM2 [%d/%d] : %s\n", i + 1, AnimList.size(), pAnim->GetName());
		pAnim2 = _Convert2Anim2(pAnim);

		Anim2List.push_back(pAnim2);
	}

	if (m_TotalTCount > 0)
		printf("Compress Result T : %d/%d (%.1f%%)\n", m_TList.getCount(), m_TotalTCount, (REAL32)m_TList.getCount() / m_TotalTCount * 100.0f);

	if (m_TotalT16Count > 0)
		printf("Compress Result T(16Bits): %d/%d (%.1f%%)\n", m_TList16Bits.getCount(), m_TotalT16Count, (REAL32)m_TList16Bits.getCount() / m_TotalT16Count * 100.0f);

	if (m_TotalRCount > 0)
		printf("Compress Result R : %d/%d (%.1f%%)\n", m_RList.getCount(), m_TotalRCount, (REAL32)m_RList.getCount() / m_TotalRCount * 100.0f);

	if (m_TotalR16Count > 0)
		printf("Compress Result R(16Bits): %d/%d (%.1f%%)\n", m_RList16Bits.getCount(), m_TotalR16Count, (REAL32)m_RList16Bits.getCount() / m_TotalR16Count * 100.0f);

	if (m_TotalSCount > 0)
		printf("Compress Result S : %d/%d (%.1f%%)\n", m_SList.getCount(), m_TotalSCount, (REAL32)m_SList.getCount() / m_TotalSCount * 100.0f);

	if (m_TotalS16Count > 0)
		printf("Compress Result S(16Bits): %d/%d (%.1f%%)\n", m_SList16Bits.getCount(), m_TotalS16Count, (REAL32)m_SList16Bits.getCount() / m_TotalS16Count * 100.0f);

	// Animation Table
	{
		pAnimTable = (pack::ANIM_INFO *)i3MemAlloc(sizeof(pack::ANIM_INFO) * Anim2List.size());
		I3ASSERT(pAnimTable != nullptr);

		memset(pAnimTable, 0, sizeof(pack::ANIM_INFO) * Anim2List.size());

		for (size_t i = 0; i < Anim2List.size(); i++)
		{
			i3Animation2 * pAnim_InLoop = Anim2List[i];

			if (pAnim_InLoop->is16Bits())
			{
				pAnimTable[i].m_Type = 3;
			}
			else
				pAnimTable[i].m_Type = 1;

			pAnimTable[i].m_TrackCount = pAnim_InLoop->GetTrackCount();
			pAnimTable[i].m_Duration = pAnim_InLoop->GetDuration();
			i3::safe_string_copy(pAnimTable[i].m_szName, pAnim_InLoop->GetNameString(), _countof(pAnimTable[i].m_szName));

			if (progressCallback)
			{
				progressCallback->m_argText0 = pAnim_InLoop->GetName();
				progressCallback->m_arg1 = i;
			}
		}

		AnimTableOffset = strm.GetPosition();

		if (progressCallback)
		{
			progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_I3ANIM2_TABLE;
			progressCallback->m_arg0 = sizeof(pack::ANIM_INFO) * Anim2List.size();
			if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_BEGIN))
				return STREAM_ERR;
		}

		Rc = strm.Write(pAnimTable, sizeof(pack::ANIM_INFO) * Anim2List.size());
		if (Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not save the animation pack file.");
			goto ExitPart;
		}
	}

	// 전체 Data를 저장할 메모리 버퍼의 크기를 계산.
	{
		INT32 size = 0;

		{
			// Translate Array Size
			size += sizeof(VEC3D) * m_TList.getCount();
			header.m_TArrayCount = m_TList.getCount();

			// Rotate Array Size
			size += sizeof(QUATERNION) * m_RList.getCount();
			header.m_RArrayCount = m_RList.getCount();

			// Scale Array Size
			size += sizeof(VEC3D) * m_SList.getCount();
			header.m_SArrayCount = m_SList.getCount();
		}

		{
			// Translate Array Size
			size += m_TList16Bits.getDataSize();
			header.m_T16ArrayCount = m_TList16Bits.getCount();

			// Rotate Array Size
			size += m_RList16Bits.getDataSize();
			header.m_R16ArrayCount = m_RList16Bits.getCount();

			// Scale Array Size
			size += m_SList16Bits.getDataSize();
			header.m_S16ArrayCount = m_SList16Bits.getCount();
		}

		// 개별 animation의 크기
		for (size_t i = 0; i < Anim2List.size(); i++)
		{
			pAnimTable[i].m_Offset = size;

			pAnim2 = Anim2List[i];

			size += pAnim2->CalcBufferSize();
		}

		header.m_DataSize = size;
	}

	/////////////////////////////////
	// Data Save
	{

		header.m_DataOffset = strm.GetPosition();

		{
			// Translate Array
			for (INT32 i = 0; i < m_TList.getCount(); i++)
			{
				Rc = strm.Write(m_TList.GetVector(i), sizeof(VEC3D));
				CHKGOTO(Rc);
			}

			// Rotate Array
			for (INT32 i = 0; i < m_RList.getCount(); i++)
			{
				Rc = strm.Write(m_RList.GetVector(i), sizeof(QUATERNION));
				CHKGOTO(Rc);
			}

			// Scale Array
			for (INT32 i = 0; i < m_SList.getCount(); i++)
			{
				Rc = strm.Write(m_SList.GetVector(i), sizeof(VEC3D));
				CHKGOTO(Rc);
			}
		}

		{
			// Translate Array
			if (m_TList16Bits.getCount() > 0)
			{
				Rc = strm.Write(m_TList16Bits.getData(), m_TList16Bits.getDataSize());
				CHKGOTO(Rc);
			}

			// Rotate Array
			if (m_RList16Bits.getCount() > 0)
			{
				Rc = strm.Write(m_RList16Bits.getData(), m_RList16Bits.getDataSize());
				CHKGOTO(Rc);
			}

			// Scale Array
			if (m_SList16Bits.getCount() > 0)
			{
				Rc = strm.Write(m_SList16Bits.getData(), m_SList16Bits.getDataSize());
				CHKGOTO(Rc);
			}
		}


		if (progressCallback)
		{
			progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_I3ANIM2;
			progressCallback->m_arg0 = Anim2List.size();
			if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_BEGIN))
			{
				I3MEM_SAFE_FREE(pAnimTable);
				return STREAM_ERR;
			}
		}

		// 개별 Animation의 Data 저장
		for (size_t i = 0; i < Anim2List.size(); i++)
		{
			pAnim2 = Anim2List[i];

			Rc = pAnim2->StoreData(&strm);
			CHKGOTO(Rc);

			if (progressCallback)
			{
				progressCallback->m_arg1 = i;
				if (!progressCallback->OnReport(i3AniPackFileProgress::EACT_UPDATE))
				{
					I3MEM_SAFE_FREE(pAnimTable);
					return STREAM_ERR;
				}
			}
		}
	}

	// 
	if (progressCallback)
	{
		progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_I3ANIM2_HEADER_AND_TABLE;
		if (!progressCallback->OnReport())
		{
			I3MEM_SAFE_FREE(pAnimTable);
			return STREAM_ERR;
		}
	}

	//////////////////////////////////
	// Header Update
	{
		strm.SetPosition(0, STREAM_BEGIN);

		Rc = strm.Write(&header, sizeof(header));
		CHKGOTO(Rc);
	}

	// Animation Table Update
	{
		strm.SetPosition(AnimTableOffset, STREAM_BEGIN);

		Rc = strm.Write(pAnimTable, sizeof(pack::ANIM_INFO) * AnimList.size());
		CHKGOTO(Rc);
	}

ExitPart:

	I3MEM_SAFE_FREE(pAnimTable);

	if (progressCallback)
	{
		progressCallback->m_phase = i3AniPackFileProgress::EPHASE_SAVE_END;
		if (!progressCallback->OnReport())
			return STREAM_ERR;
	}

	// 에라이 썅....그지 같이 릭도 제대로 처리 안하고...(2016.12.09.수빈)
	i3::cu::for_each_SafeRelease(Anim2List);
	
	return Rc;
}

UINT32 i3AnimationPackFile::LoadFromFile(char * pszPath)
{
	UINT32 Rc;
	i3FileStream strm;
	pack::ANIMPACKFILE_HEADER header;
	INT32 i;
	i3AnimationResManager * pManager;

	i3MemoryBuffer *	pBuff = nullptr;

	VEC3D *			pTArray = nullptr;
	QUATERNION *	pRArray = nullptr;
	VEC3D *			pSArray = nullptr;
	REAL16 *		pTArray16Bits = nullptr;
	REAL16 *		pRArray16Bits = nullptr;
	REAL16 *		pSArray16Bits = nullptr;

	bool b16Bits = false;

	pManager = (i3AnimationResManager *)i3NamedResourceManager::FindByMeta(i3AnimationResManager::static_meta());
	I3ASSERT(pManager != nullptr);

	// 
	if (strm.Open(pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		//I3PRINTLOG(I3LOG_WARN,  "Could not create %s file.\n", pszPath);	// 로그 삭제
		return STREAM_ERR;
	}

	// Header
	{
		Rc = strm.Read(&header, sizeof(header));
		I3_CHKIO(Rc);

		if (memcmp(header.m_ID, "APF2", 4) == 0)
			b16Bits = true;
	}

	// Animation Data Load
	{
		pBuff = i3MemoryBuffer::new_object_ref();

		pBuff->Create((UINT32)header.m_DataSize);

		strm.SetPosition((INT32)header.m_DataOffset, STREAM_BEGIN);
		Rc = strm.Read(pBuff->getBuffer(), (INT32)header.m_DataSize);
		I3_CHKIO(Rc);

		UINT32	iIndexer = 0;

		// Translate Array
		if (header.m_TArrayCount > 0)
		{
			pTArray = (VEC3D *)pBuff->getBuffer();
			iIndexer += sizeof(VEC3D) * header.m_TArrayCount;
		}

		// Rotate Array
		if (header.m_RArrayCount > 0)
		{
			pRArray = (QUATERNION *)((char*)pBuff->getBuffer() + iIndexer);
			iIndexer += sizeof(QUATERNION) * header.m_RArrayCount;
		}

		// Scale Array
		if (header.m_SArrayCount > 0)
		{
			pSArray = (VEC3D *)((char*)pBuff->getBuffer() + iIndexer);
			iIndexer += sizeof(VEC3D) * header.m_SArrayCount;
		}

		if (header.m_T16ArrayCount > 0)
		{
			pTArray16Bits = (REAL16*)((char*)pBuff->getBuffer() + iIndexer);
			iIndexer += sizeof(REAL16) * header.m_T16ArrayCount * 3;
		}

		if (header.m_R16ArrayCount > 0)
		{
			pRArray16Bits = (REAL16*)((char*)pBuff->getBuffer() + iIndexer);
			iIndexer += sizeof(REAL16) * header.m_R16ArrayCount * 4;
		}

		if (header.m_S16ArrayCount > 0)
		{
			pSArray16Bits = (REAL16*)((char*)pBuff->getBuffer() + iIndexer);
		}

		//pAnimDataStart = (char *) pBuff->getBuffer();
	}

	strm.SetPosition(sizeof(header), STREAM_BEGIN);

	// Animation Table
	{
		pack::ANIM_INFO info;

		for (i = 0; i < (INT32)header.m_AnimCount; i++)
		{
			i3Animation2 * pAnim = i3Animation2::new_object_ref();

			Rc = strm.Read(&info, sizeof(info));
			if (Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL, pszPath);
				return STREAM_ERR;
			}
			//I3_CHKIO( Rc);

#if defined( I3_DEBUG)
			pAnim->SetName(info.m_szName);
#endif

			if (info.m_Type >= 2)
			{
				if (info.m_Type == 3)
					pAnim->LoadData16Bits(pBuff, (INT32)info.m_Offset, info.m_TrackCount, info.m_Duration, 1);
				else
					pAnim->LoadData16Bits(pBuff, (INT32)info.m_Offset, info.m_TrackCount, info.m_Duration);

				pAnim->setArrays(pTArray16Bits, pRArray16Bits, pSArray16Bits);
			}
			else
			{
				pAnim->LoadData(pBuff, (INT32)info.m_Offset, info.m_TrackCount, info.m_Duration);
				pAnim->setArrays(pTArray, pRArray, pSArray);
			}

			pManager->AddResource(pAnim, info.m_szName);
		}
	}

	return Rc;
}


UINT32 i3AnimationPackFile::LoadAnimationList(const char * pszPath)
{
	UINT32 Rc;
	i3FileStream strm;
	pack::ANIMPACKFILE_HEADER header;
	i3AnimationResManager * pManager;

	pManager = (i3AnimationResManager *)i3NamedResourceManager::FindByMeta(i3AnimationResManager::static_meta());
	I3ASSERT(pManager != nullptr);

	// 
	if (strm.Open(pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not create %s file.", pszPath);
		return STREAM_ERR;
	}

	// Header
	{
		Rc = strm.Read(&header, sizeof(header));
		I3_CHKIO(Rc);
	}

	strm.SetPosition(sizeof(header), STREAM_BEGIN);

	// Animation Table
	{
		pack::ANIM_INFO info;
		I3ASSERT(header.m_AnimCount < 65535);

		for (UINT32 i = 0; i < header.m_AnimCount; i++)
		{
			Rc = strm.Read(&info, sizeof(info));
			if (Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL, pszPath);
				return STREAM_ERR;
			}

			pManager->AddResourcePack(info.m_szName, pszPath);
		}
	}

	return Rc;
}
