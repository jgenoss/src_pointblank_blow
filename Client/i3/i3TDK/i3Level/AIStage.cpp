#include "stdafx.h"
#include "AIStage.h"
#include <sys/stat.h>
#include <vector>
#include <algorithm>

CAIStage::CAIStage()
{
	//m_pVB = nullptr;
	m_pbRay = nullptr;
	m_pui8Ray = nullptr;

	m_bBuildNearInfo = FALSE;
	m_bBuildAngle = FALSE;
	m_bBuildDistance = FALSE;
}

void CAIStage::Save(CFile* pFile)
{
	BuildNearInfo();
	BuildAngle();
	BuildDistance();

	WCHAR wstrName[128];

#ifdef _UNICODE
	i3::safe_string_copy(wstrName, m_cstrName, 128);
#else
	i3::mb_to_utf16(m_cstrName, wstrName, 128);	// MultiByteToWideChar(CP_ACP, 0, m_cstrName.GetBuffer(), 128, wstrName, 128);
#endif

	//wcscpy_s(wstrName,128, m_cstrName.GetBuffer());
	//WideCharToMultiByte(CP_ACP, 0, wstrName, -1, m_cstrName.GetBuffer(), 128, nullptr, nullptr);
	pFile->Write(wstrName, sizeof(wstrName));	// 256바이트 (널문자2바이트도 포함) 254/2 = 127자까지 네이밍가능

	INT32 i32Count = m_vtMesh.size();
	pFile->Write(&i32Count,	sizeof(UINT32));	// 4바이트

	for(INT32 i = 0; i < i32Count; i++)
	{
		D3DXVECTOR3 dxv3Tmp[3];
		memcpy(dxv3Tmp, m_vtMesh[i].m_dxv3Pt, sizeof(D3DXVECTOR3)*3);
		dxv3Tmp[0].x = -dxv3Tmp[0].x;
		dxv3Tmp[1].x = -dxv3Tmp[1].x;
		dxv3Tmp[2].x = -dxv3Tmp[2].x;
		pFile->Write(dxv3Tmp, sizeof(D3DXVECTOR3)*3);					// 36바이트
		pFile->Write(m_vtMesh[i].m_r32Distance, sizeof(REAL32)*3);		// 12바이트
		pFile->Write(m_vtMesh[i].m_ui16NeighborIdx, sizeof(UINT16)*3);	//  6바이트
		pFile->Write(m_vtMesh[i].m_i8NeighborByun, sizeof(INT8)*3);		//  3바이트
		pFile->Write(&m_vtMesh[i].m_ui8Angle, sizeof(UINT8));			//  1바이트
	} // 폴리곤 한개당 58바이트
}

INT32 CAIStage::CheckVisibleTest(INT32 i32IdxBegin, INT32 i32IdxEnd)
{
	if (i32IdxBegin == i32IdxEnd) return 9;

	DS_LINE lnAB;
	DS_LINE lnCD;
	INT32 i32Success = 0;
	D3DXVECTOR3 dxv3Tmp;
	D3DXVECTOR3 dxv3Tmp2;

	for(INT32 i = 0 ; i < 3; i++)
	{
		D3DXVec3Lerp(&dxv3Tmp, &m_vtMesh[i32IdxBegin].m_dxv3Pt[i], &m_vtMesh[i32IdxBegin].m_dxv3Center, 0.03f);
		lnAB.SetBegin(&dxv3Tmp);
		for(INT32 j = 0; j < 3; j++)
		{
			D3DXVec3Lerp(&dxv3Tmp2, &m_vtMesh[i32IdxEnd].m_dxv3Pt[j], &m_vtMesh[i32IdxEnd].m_dxv3Center, 0.03f);
			lnAB.SetEnd(&dxv3Tmp2);

			INT32 i32OldIdx = -1;
			INT32 i32Idx = i32IdxBegin;
			for(INT32 l = 0; l < 512; l++)	// 메쉬를 512번이나 통과했는데도 골에 닿지 못했으면 실패로 간주
			{
				if (i32Idx == i32IdxEnd)	// 끝Idx까지 왔으면 성공 
				{
					i32Success++;
					break;
				}
				if (i32Idx == 0xFFFF)		// 가던 도중에 이웃메시가 없으면 실패
				{
					break;
				}

				for(INT32 k = 0; k < 3; k++)
				{
					if (m_vtMesh[i32Idx].m_ui16NeighborIdx[k] == i32OldIdx) continue;	// 세 변중 되돌아가는 변은 검사안합니다.

					m_vtMesh[i32Idx].Get2DLine(k, &lnCD);
					if (g_GetIntersectPoint(&lnAB, &lnCD))
					{
						i32OldIdx = i32Idx;
						i32Idx = m_vtMesh[i32Idx].m_ui16NeighborIdx[k];
						break;
					}
				}
				if (i32Idx == i32OldIdx)
				{
					break;
				}
			}
		}
	}
	return i32Success;
}

BOOL CAIStage::LoadNM(CFile* pFile)
{
	WCHAR wstrName[128];
	pFile->Read(wstrName, sizeof(wstrName));
	m_cstrName = wstrName;

	INT32 i32Count;
	pFile->Read(&i32Count, sizeof(INT32));

	D3DXVECTOR3	dxv3Pt[3];
	INT8 i8Byun[3];
	UINT16 ui16Index[3];
	UINT8 ui8Angle;
	REAL32 r32Distance[3];

	for(INT32 i = 0; i < i32Count; i++)
	{
		pFile->Read(dxv3Pt, sizeof(D3DXVECTOR3)*3);	// 36바이트
		dxv3Pt[0].x = -dxv3Pt[0].x;
		dxv3Pt[1].x = -dxv3Pt[1].x;
		dxv3Pt[2].x = -dxv3Pt[2].x;

		pFile->Read(r32Distance, sizeof(REAL32)*3);	// 12바이트
		pFile->Read(ui16Index, sizeof(UINT16)*3);	//  6바이트 - 읽지만 별다르게 사용하진 않습니다.
		pFile->Read(i8Byun, sizeof(INT8)*3);		//  3바이트 - 읽지만 별다르게 사용하진 않습니다.
		pFile->Read(&ui8Angle, sizeof(INT8));		//  1바이트 - 읽지만 별다르게 사용하진 않습니다.

		CAIMesh	Mesh;
		Mesh.Create(dxv3Pt, ui8Angle);
		m_vtMesh.push_back(Mesh);
	}

	return TRUE;
}

BOOL CAIStage::LoadNMC(CFile* pFile)
{
	m_cstrName = "NoName";

	INT32 i32Count;
	pFile->Read(&i32Count, sizeof(INT32));

	D3DXVECTOR3	Pt[3];
	for(INT32 i = 0; i < i32Count; i++)
	{
		pFile->Read(Pt, sizeof(D3DXVECTOR3)*3);

		CAIMesh	Mesh;
		Mesh.Create(Pt, 0);
		m_vtMesh.push_back(Mesh);
	}

	return TRUE;
}

struct TmpStruct {
	CAIStage* pStage = nullptr;
	INT32 i32BeginIdx = 0;
};
#define THREAD_COUNT_BY_BUILD_RAY_FILE	7

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	TmpStruct* pStruct = (TmpStruct*)lpParameter;
	char szFileName[32];
	sprintf(szFileName, "%04d.txt", pStruct->i32BeginIdx);
	FILE* fp = fopen(szFileName, "wt");
	if (fp)
	{
		pStruct->pStage->Thread(pStruct->i32BeginIdx, fp);
		fclose(fp);
	}
	delete pStruct;
	return 0;
}

void CAIStage::BuildRAY()
{
	BuildNearInfo();

	INT32 i32Cnt = (INT32)m_vtMesh.size();
	INT32 i32CntCnt = i32Cnt*i32Cnt;
	HANDLE hThread[THREAD_COUNT_BY_BUILD_RAY_FILE];
	DWORD dwThreadID[THREAD_COUNT_BY_BUILD_RAY_FILE];
	m_pui8Result = new UINT8 [i32CntCnt];
	char szOutName[256];
	i3::snprintf(szOutName, 256, "%s.ray.v2", (const char*)m_cstrName);

	for(INT32 i = 0; i < THREAD_COUNT_BY_BUILD_RAY_FILE; i++)
	{
		TmpStruct* pTmp = new TmpStruct;
		pTmp->pStage = this;
		pTmp->i32BeginIdx = i;
		hThread[i] = CreateThread(nullptr, 0, ThreadProc, pTmp, 0, &dwThreadID[i]);
	}

	WaitForMultipleObjects(THREAD_COUNT_BY_BUILD_RAY_FILE, hThread, TRUE, INFINITE);

	UINT8* pui8ResultFile = new UINT8 [i32CntCnt/2];
	ZeroMemory(pui8ResultFile, sizeof(UINT8)*i32CntCnt/2);
	
	for(INT32 i = 0; i < i32CntCnt; i += 2)
	{
		pui8ResultFile[i/2] = m_pui8Result[i] | m_pui8Result[i + 1] << 4;
	}
	delete[] m_pui8Result;

	CFile file;
	file.Open(m_cstrName + ".ray.v3", CFile::modeCreate | CFile::modeWrite);
	file.Write(&i32Cnt, sizeof(INT32));
	file.Write(pui8ResultFile, i32CntCnt/2);
	file.Close();

	delete[] pui8ResultFile;
}

void CAIStage::Thread(INT32 i32BeginIdx, FILE* pFile)
{
	INT32 i32Cnt = (INT32)m_vtMesh.size();

	for(INT32 i = i32BeginIdx; i < i32Cnt; i += THREAD_COUNT_BY_BUILD_RAY_FILE)
	{
		INT32 i32Visible = 0;
		INT32 i32Invisible = 0;

		for(INT32 j = 0; j < i32Cnt; j++)
		{
			switch(CheckVisibleTest(i, j))
			{
			case 0:
				i32Invisible++;
				m_pui8Result[i*i32Cnt + j] = 0;
				break;
			case 9:
				i32Visible++;
				m_pui8Result[i*i32Cnt + j] = 2;
				break;
			default:
				m_pui8Result[i*i32Cnt + j] = 1;
				break;
			}
		}
		
		REAL32 r32RatioV = (REAL32)i32Visible/i32Cnt*100.f; 
		REAL32 r32RatioI = (REAL32)i32Invisible/i32Cnt*100.f;
		fprintf(pFile, "%d/%d(%.2f%%) Visible:%d(%.2f%%), Normal:%d(%.2f%%), Invisible:%d(%.2f%%)\n",
			i, i32Cnt, (REAL32)i/i32Cnt*100.f,
			i32Visible, r32RatioV,
			i32Cnt - i32Visible - i32Invisible, 100.f - r32RatioV - r32RatioI,
			i32Invisible, r32RatioI);
		fflush(pFile);
	}
}

void CAIStage::LoadRayV2(CFile* pFile)
{
	UINT32 u32Cnt;
	pFile->Read(&u32Cnt, sizeof(UINT32));

	if (m_vtMesh.size() != u32Cnt)
	{
		CString cstrMsg;
		cstrMsg.Format(_T("DataCount is not same.(DataCount:%d, RayFileDataCount:%d)"), m_vtMesh.size(), u32Cnt);
		AfxMessageBox(cstrMsg);
		return;
	}
	if (m_pui8Ray) delete[] m_pui8Ray;

	m_pui8Ray = new UINT8 [u32Cnt*u32Cnt/2];
	pFile->Read(m_pui8Ray, u32Cnt*u32Cnt/2*sizeof(UINT8));
}

void CAIStage::LoadRay(CFile* pFile)
{
	UINT32 u32Cnt;
	pFile->Read(&u32Cnt, sizeof(UINT32));

	if (m_vtMesh.size() != u32Cnt)
	{
		CString cstrMsg;
		cstrMsg.Format(_T("DataCount is not same.(DataCount:%d, RayFileDataCount:%d)"), m_vtMesh.size(), u32Cnt);
		AfxMessageBox(cstrMsg);
		return;
	}

	if (m_pbRay) delete[] m_pbRay;

	INT32 i32CntSq = u32Cnt*u32Cnt;
	m_pbRay = new bool [i32CntSq];
	ZeroMemory(m_pbRay, sizeof(bool)*i32CntSq);

	INT32 i32Size = i32CntSq/8;
	UINT8* pui8Ray = new UINT8 [i32Size];
	pFile->Read(pui8Ray, sizeof(UINT8)*i32Size);
	INT32 j = 0;
	INT32 i32TrueCnt = 0;
	for(INT32 i = 0; i < i32Size; i++)
	{
		for(INT32 k = 0; k < 8; k++,j++)
		{
			if (pui8Ray[i] & (1 << k))
			{
				m_pbRay[j] = true;
				i32TrueCnt++;
			}
		}
	}
	delete[] pui8Ray;

	REAL32 r32Ratio = (REAL32)i32TrueCnt/i32CntSq*100.f;
	CString cstrMsg;
	cstrMsg.Format(_T("Total:%d, True:%d(%.2f%%), False:%d(%.2f%%)"), i32CntSq, i32TrueCnt, r32Ratio, i32CntSq - i32TrueCnt, 100.f - r32Ratio);
	AfxMessageBox(cstrMsg);
}

void g_SwapREAL32(REAL32* pr32A, REAL32* pr32B)
{
	REAL32 r32Tmp;
	r32Tmp = *pr32A;
	*pr32A = *pr32B;
	*pr32B = r32Tmp;
}

BOOL CAIStage::SetNmData()
{

	return 1;
}
BOOL CAIStage::LoadASE(CFile* pFile)
{
	m_cstrName = pFile->GetFileName();

	char* pszMemory = new char [(UINT32)pFile->GetLength() + 1];
	pFile->Read(pszMemory, (UINT32)pFile->GetLength());
	pszMemory[pFile->GetLength()] = '\0';

	CStringA cstrASEFile = pszMemory;

	delete[] pszMemory;

	INT32 i32MeshNumVertex;
	INT32 i32MeshNumFaces;
	const char* pwstrMeshNumVertex	= "*MESH_NUMVERTEX";
	const char* pwstrMeshNumFaces	= "*MESH_NUMFACES";
	const char* pwstrMeshVertexList	= "*MESH_VERTEX_LIST";
	const char* pwstrMeshVertex		= "*MESH_VERTEX";
	const char* pwstrMeshFaceList	= "*MESH_FACE_LIST";
	const char* pwstrMeshFace		= "*MESH_FACE";
	INT32 i32addr = 0;
	INT32 i32add_len;
	
	i32addr = cstrASEFile.Find(pwstrMeshNumVertex, i32addr);
	i32addr += strlen(pwstrMeshNumVertex) + 1;
	//scanf_s(cstrASEFile.Mid(i32addr, 20), "%d", &i32MeshNumVertex);

	i32MeshNumVertex = atoi( cstrASEFile.Mid(i32addr, 4) );
	
	i32addr = cstrASEFile.Find(pwstrMeshNumFaces, i32addr);
	i32addr += strlen(pwstrMeshNumFaces) + 1;		

	//scanf_s(cstrASEFile.Mid(i32addr, 20), "%d", &i32MeshNumFaces);

	i32MeshNumFaces = atoi( cstrASEFile.Mid(i32addr, 4) );

	i32addr = cstrASEFile.Find(pwstrMeshVertexList, i32addr);
	i32addr += strlen(pwstrMeshVertexList) + 1;
	D3DXVECTOR3* pdxv3Vertex = new D3DXVECTOR3 [i32MeshNumVertex];
	i32add_len = strlen(pwstrMeshVertex) + 1;
	for(INT32 i = 0; i < i32MeshNumVertex; i++)
	{
		INT32 Temp;
		i32addr = cstrASEFile.Find(pwstrMeshVertex, i32addr);
		i32addr += (i32add_len +1);
		sscanf_s(cstrASEFile.Mid(i32addr, 60), "%d\t%f\t%f\t%f", &Temp, &pdxv3Vertex[i].x, &pdxv3Vertex[i].y, &pdxv3Vertex[i].z);
	}

	i32addr = cstrASEFile.Find(pwstrMeshFaceList, i32addr);
	i32addr += strlen(pwstrMeshFaceList) + 1;
	INT32* pi32Face = new INT32 [i32MeshNumFaces*3];
	i32add_len = strlen(pwstrMeshFace) + 1;
	for(INT32 i = 0; i < i32MeshNumFaces*3; i+=3)
	{
		INT32 Temp;
		i32addr = cstrASEFile.Find(pwstrMeshFace, i32addr);
		i32addr += i32add_len;
		sscanf_s(cstrASEFile.Mid(i32addr, 40), "%d:    A:%d B:%d C:%d", &Temp, &pi32Face[i], &pi32Face[i+1], &pi32Face[i+2]);
	}
	cstrASEFile.Empty();

	for(INT32 i = 0; i < i32MeshNumFaces*3; i+=3)
	{
		D3DXVECTOR3	dxv3Pt[3];

		for(INT32 j = 0; j < 3; j++)
		{
			memcpy(dxv3Pt[j], pdxv3Vertex[pi32Face[i + j]], sizeof(D3DXVECTOR3));
			g_SwapREAL32(&dxv3Pt[j].y, &dxv3Pt[j].z);
		}

		CAIMesh	Mesh;
		Mesh.Create(dxv3Pt, 0);
		m_vtMesh.push_back(Mesh);
	}

	delete[] pdxv3Vertex;
	delete[] pi32Face;

	return TRUE;
}

void CAIStage::BuildAngle()
{
	if (m_bBuildAngle == FALSE)
	{
		D3DXPLANE dxplTmp;
		D3DXVECTOR3 dxv3Normal;
		REAL32 r32Angle;

		for(UINT32 i = 0; i < m_vtMesh.size(); i++)
		{
			D3DXPlaneFromPoints(&dxplTmp, &m_vtMesh[i].m_dxv3Pt[0], &m_vtMesh[i].m_dxv3Pt[1], &m_vtMesh[i].m_dxv3Pt[2]);
			dxv3Normal.x = dxplTmp.a;
			dxv3Normal.y = dxplTmp.b;
			dxv3Normal.z = dxplTmp.c;
			D3DXVec3Normalize(&dxv3Normal, &dxv3Normal);
			r32Angle = acosf(dxv3Normal.y);
			m_vtMesh[i].m_ui8Angle = (UINT8)(D3DXToDegree(r32Angle) + 0.5f);
		}

		m_bBuildAngle = TRUE;
	}
}

void CAIStage::BuildDistance()
{
	if (m_bBuildDistance == FALSE)
	{
		INT32 i32MeshCount = m_vtMesh.size();
		D3DXVECTOR3 dxv3Tmp;
		for(INT32 i = 0; i < i32MeshCount; i++)
		{
			if (m_vtMesh[i].m_pNeighbor[0] != nullptr)
			{
				dxv3Tmp = m_vtMesh[i].m_dxv3Center - m_vtMesh[i].m_pNeighbor[0]->m_dxv3Center;
				m_vtMesh[i].m_r32Distance[0] = D3DXVec3Length(&dxv3Tmp);
			}
			else
			{
				m_vtMesh[i].m_r32Distance[0] = 0.f;
			}
			
			if (m_vtMesh[i].m_pNeighbor[1] != nullptr)
			{
				dxv3Tmp = m_vtMesh[i].m_dxv3Center - m_vtMesh[i].m_pNeighbor[1]->m_dxv3Center;
				m_vtMesh[i].m_r32Distance[1] = D3DXVec3Length(&dxv3Tmp);
			}
			else
			{
				m_vtMesh[i].m_r32Distance[1] = 0.f;
			}

			if (m_vtMesh[i].m_pNeighbor[2] != nullptr)
			{
				dxv3Tmp = m_vtMesh[i].m_dxv3Center - m_vtMesh[i].m_pNeighbor[2]->m_dxv3Center;
				m_vtMesh[i].m_r32Distance[2] = D3DXVec3Length(&dxv3Tmp);
			}
			else
			{
				m_vtMesh[i].m_r32Distance[2] = 0.f;
			}
		}
		m_bBuildDistance = TRUE;
	}
}

void CAIStage::BuildNearInfo()
{
	if (m_bBuildNearInfo == FALSE)
	{
		INT32 i32MeshCount = m_vtMesh.size();

		for(INT32 i = 0; i < i32MeshCount; i++)
		{
			UINT16* pui16Address = &m_vtMesh[i].m_ui16NeighborIdx[0];
			*pui16Address = 0xFFFF;
			pui16Address++;
			*pui16Address = 0xFFFF;
			pui16Address++;
			*pui16Address = 0xFFFF;
		}

		for(INT32 i = 0; i < i32MeshCount; i++)
		{
			for(INT32 j = 0; j < i32MeshCount; j++)
			{
				m_vtMesh[i].CheckNeighbor(j, &m_vtMesh[j]);
			}
		}

		m_bBuildNearInfo = TRUE;
	}
}

BOOL CAIStage::RebuildVB()
{
	//if (m_pVB)
	//	m_pVB->Release();

	//// 버텍스 버퍼 생성
	//HRESULT hr = g_pd3dDevice->CreateVertexBuffer(m_vtMesh.size()*3*sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pVB, nullptr);
	//if (FAILED(hr)) return FALSE;

	//CUSTOMVERTEX* pV;
	//hr = m_pVB->Lock(0, 0, (void**)&pV, 0);
	//if (FAILED(hr)) return FALSE;

	//INT32 i32MeshCnt = m_vtMesh.size();
	//INT32 i32IdxPicked = -1;
	//if (m_vtIdxPicked.size() == 1) i32IdxPicked = m_vtIdxPicked[0];
	//
	//for(UINT32 i = 0; i < m_vtMesh.size(); i++)
	//{		
	//	CUSTOMVERTEX* pcustom = &pV[i*3 + 0];
	//	D3DCOLOR col;

	//	switch(g_i8Mode)
	//	{
	//	case MODE_EDIT:	// 일반 편집 모드
	//		{
	//			if (m_vtMesh[i].m_ui8Angle < 60)
	//			{
	//				col = D3DXCOLOR(0.f, ((REAL32)(rand()%200 + 800))/1000.f, 0.f, 1.f);
	//			}
	//			else
	//			{
	//				col = D3DXCOLOR(0.f, 0.f, ((REAL32)(rand()%200 + 800))/1000.f, 1.f);
	//			}
	//		}
	//		break;
	//	case MODE_RAY: // 레이 파일 적용 모드
	//		{
	//			if (i32IdxPicked != -1)
	//			{
	//				/*
	//				if (m_pbRay[i + i32IdxPicked*i32MeshCnt] == true)	col = RGB(255,   0, 255);
	//				else												col = RGB(255, 255, 255);*/
	//				UINT8 ui8Result = SendRayV2(i32IdxPicked, i);
	//				switch(ui8Result)
	//				{
	//				case 0:
	//					col = D3DXCOLOR(0.f, ((REAL32)(rand()%200 + 800))/1000.f, 0.f, 1.f);
	//					break;
	//				case 1:
	//					col = RGB(107 + rand()%40, 0, 0);
	//					break;
	//				case 2:
	//					col = RGB(215 + rand()%40, 215 + rand()%40, 0);
	//					break;
	//				default:
	//					assert(FALSE);
	//					break;
	//				}
	//			}
	//			else col = RGB(255, 255, 255);
	//		}
	//		break;
	//	case MODE_PATHFIND:
	//		{
	//			if (m_Path.Contain(i))	col = D3DXCOLOR(0.6f, 0.6f, ((REAL32)(rand()%200 + 800))/1000.f, 1.f);
	//			else					col = D3DXCOLOR(0.f, ((REAL32)(rand()%200 + 800))/1000.f, 0.f, 1.f);
	//		}
	//		break;
	//	}
	//	pcustom->position	= m_vtMesh[i].m_dxv3Pt[0];
	//	pcustom->color		= col;
	//	pcustom++;
	//	pcustom->position	= m_vtMesh[i].m_dxv3Pt[1];
	//	pcustom->color		= col;
	//	pcustom++;
	//	pcustom->position	= m_vtMesh[i].m_dxv3Pt[2];
	//	pcustom->color		= col;
	//}

	//for(UINT32 i = 0; i < m_vtIdxPicked.size(); i++)
	//{
	//	CUSTOMVERTEX* pcustom = &pV[m_vtIdxPicked[i]*3 + 0];
	//	D3DCOLOR col = RGB(0, 0, (INT32)((pcustom->position.x*pcustom->position.y*pcustom->position.z)*20000.f)%36+220);
	//	pcustom->color		= col;
	//	pcustom++;
	//	pcustom->color		= col;
	//	pcustom++;
	//	pcustom->color		= col;
	//}

	//m_pVB->Unlock();
	return TRUE;
}

void CAIStage::Delete()
{
	std::vector<CAIMesh> vtNew;
	int i = 0;

	for(std::vector<CAIMesh>::iterator it2 = m_vtMesh.begin(); it2 != m_vtMesh.end(); it2++, i++)
	{
		bool bInclude = false;
		for(std::vector<int>::iterator it = m_vtIdxPicked.begin(); it != m_vtIdxPicked.end(); it++)
			if (*it == i) { bInclude = true; break; }
		
		if (!bInclude) vtNew.push_back(*it2);
	}
	m_vtMesh.clear();
	m_vtMesh = vtNew;
	vtNew.clear();
	m_vtIdxPicked.clear();

	m_bBuildNearInfo = FALSE;

	RebuildVB();
}

void CAIStage::Picking(CRect* pRect)
{
	//INT32 i32a = pRect->left;
	//INT32 i32b = pRect->right;
	//INT32 i32c = pRect->top;
	//INT32 i32d = pRect->bottom;

	//vector<INT32> vti32IdxPicked;
	//D3DXVECTOR3 dxv3ClickPos;
	//INT32 i32idx;

	//if (i32a == i32b && i32c == i32d)
	//{
	//	D3DXMATRIX dxmtProj;
	//	CPoint ptCursor;
	//	D3DXVECTOR3 dxv3v;

	//	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &dxmtProj);

	//	D3DXMATRIX dxmtView, dxmtm;
	//	D3DXVECTOR3 vPickRayDir, vPickRayOrig;
	//	g_pd3dDevice->GetTransform(D3DTS_VIEW, &dxmtView);
	//	D3DXMatrixInverse(&dxmtm, nullptr, &dxmtView);

	//	dxv3v.x = (( ((i32a*2.f/g_d3dpp.BackBufferWidth) - 1.f)) - dxmtProj._31)/dxmtProj._11;
	//	dxv3v.y = ((-((i32c*2.f/g_d3dpp.BackBufferHeight) - 1.f)) - dxmtProj._32)/dxmtProj._22;
	//	dxv3v.z = 1.f;

	//	vPickRayOrig.x = dxmtm._41;
	//	vPickRayOrig.y = dxmtm._42;
	//	vPickRayOrig.z = dxmtm._43;

	//	vPickRayDir.x  = dxv3v.x*dxmtm._11 + dxv3v.y*dxmtm._21 + dxv3v.z*dxmtm._31;
	//	vPickRayDir.y  = dxv3v.x*dxmtm._12 + dxv3v.y*dxmtm._22 + dxv3v.z*dxmtm._32;
	//	vPickRayDir.z  = dxv3v.x*dxmtm._13 + dxv3v.y*dxmtm._23 + dxv3v.z*dxmtm._33;

	//	i32idx = PickByRay(&vPickRayOrig, &vPickRayDir, &dxv3ClickPos);
	//	if (i32idx != -1)
	//	{
	//		vti32IdxPicked.push_back(i32idx);
	//	}
	//}
	//else
	//{
	//	D3DXMATRIXA16 matResult, matView, matWorld, matProj;
	//	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	//	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	//	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//	matResult = matWorld*matView*matProj;

	//	INT32 i = 0;
	//	vector<CAIMesh>::iterator it;
	//	for(it = m_vtMesh.begin(); it != m_vtMesh.end(); it++, i++)
	//	{
	//		D3DXVECTOR3 dxv3Tmp;
	//		INT32 i32x;
	//		INT32 i32y;
	//		D3DXVec3TransformCoord(&dxv3Tmp, &it->m_dxv3Pt[0], &matResult);
	//		i32x = (INT32)(((dxv3Tmp.x + 1.f)*g_d3dpp.BackBufferWidth)/2.f);
	//		i32y = (INT32)(((dxv3Tmp.y - 1.f)*g_d3dpp.BackBufferHeight)/-2.f);

	//		if (i32a <= i32x && i32x <= i32b && i32c <= i32y && i32y <= i32d)
	//		{
	//			vti32IdxPicked.push_back(i);
	//			continue;
	//		}

	//		D3DXVec3TransformCoord(&dxv3Tmp, &it->m_dxv3Pt[1], &matResult);
	//		i32x = (INT32)(((dxv3Tmp.x + 1.f)*g_d3dpp.BackBufferWidth)/2.f);
	//		i32y = (INT32)(((dxv3Tmp.y - 1.f)*g_d3dpp.BackBufferHeight)/-2.f);

	//		if (i32a <= i32x && i32x <= i32b && i32c <= i32y && i32y <= i32d)
	//		{
	//			vti32IdxPicked.push_back(i);
	//			continue;
	//		}

	//		D3DXVec3TransformCoord(&dxv3Tmp, &it->m_dxv3Pt[2], &matResult);
	//		i32x = (INT32)(((dxv3Tmp.x + 1.f)*g_d3dpp.BackBufferWidth)/2.f);
	//		i32y = (INT32)(((dxv3Tmp.y - 1.f)*g_d3dpp.BackBufferHeight)/-2.f);

	//		if (i32a <= i32x && i32x <= i32b && i32c <= i32y && i32y <= i32d)
	//		{
	//			vti32IdxPicked.push_back(i);
	//		}
	//	}
	//}

	//if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	//{
	//	vector<int>::iterator it;
	//	for(it = vti32IdxPicked.begin(); it != vti32IdxPicked.end(); it++)
	//		remove(m_vtIdxPicked.begin(), m_vtIdxPicked.end(), *it);
	//}
	//else
	//{
	//	if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000))
	//	{
	//		m_vtIdxPicked.clear();
	//	}

	//	m_vtIdxPicked.insert(m_vtIdxPicked.end(), vti32IdxPicked.begin(), vti32IdxPicked.end());
	//}

	//// 중복제거
	//sort(m_vtIdxPicked.begin(), m_vtIdxPicked.end());
	//m_vtIdxPicked.erase(unique(m_vtIdxPicked.begin(), m_vtIdxPicked.end()), m_vtIdxPicked.end());

	//// 드래그가 아니고 원클릭으로 했을경우 피킹한 폴리곤의 좌표를 2개까지 기억합니다.
	//if (i32a == i32b && i32c == i32d)
	//{
	//	if (m_vtIdxPicked.size() == 1)
	//	{
	//		m_i32ClickIdx[0] = i32idx;
	//		memcpy(&m_dxv3ClickPos[0], &dxv3ClickPos, sizeof(D3DXVECTOR3));
	//	}
	//	else if (m_vtIdxPicked.size() == 2)
	//	{
	//		m_i32ClickIdx[1] = i32idx;
	//		memcpy(&m_dxv3ClickPos[1], &dxv3ClickPos, sizeof(D3DXVECTOR3));
	//	}
	//}
}

void CAIStage::Destroy()
{
	m_vtMesh.clear();
	m_vtIdxPicked.clear();
	//m_pVB->Release();

	if (m_pbRay) delete[] m_pbRay;
	if (m_pui8Ray) delete[] m_pui8Ray;
}

void CAIStage::DrawBeginEndPos()
{
	DS_LINE lnData;

	lnData.SetCross(m_dxv3ClickPos[0], TRUE);
	lnData.Draw();
	lnData.SetCross(m_dxv3ClickPos[0], FALSE);
	lnData.Draw();

	lnData.SetCross(m_dxv3ClickPos[1], TRUE);
	lnData.Draw();
	lnData.SetCross(m_dxv3ClickPos[1], FALSE);
	lnData.Draw();
}

void CAIStage::Render()
{
	//g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_vtMesh.size());
}

INT32 CAIStage::PickByRay(D3DXVECTOR3* pdxv3RayPos, D3DXVECTOR3* pdxv3RayDir, D3DXVECTOR3* pdxv3Out)
{
	REAL32 u, v, r32Distance;
	REAL32 r32MinDistance = FLT_MAX;
	INT32 i32PickedIdx = -1;

	u = 1.f;
	v = 1.f;

	std::vector<CAIMesh>::iterator it;
	INT32 i = 0;
	for(it = m_vtMesh.begin(); it != m_vtMesh.end(); it++, i++)
	{
		if (D3DXIntersectTri(&it->m_dxv3Pt[0], &it->m_dxv3Pt[1], &it->m_dxv3Pt[2], pdxv3RayPos, pdxv3RayDir, &u, &v, &r32Distance))
		{
			if (r32Distance < r32MinDistance)
			{
				r32MinDistance = r32Distance;
				i32PickedIdx = i;
			}
		}
	}

	if (i32PickedIdx != -1)
	{
		*pdxv3Out = 
			m_vtMesh[i32PickedIdx].m_dxv3Pt[0] + 
			u*(m_vtMesh[i32PickedIdx].m_dxv3Pt[1]-m_vtMesh[i32PickedIdx].m_dxv3Pt[0]) +
			v*(m_vtMesh[i32PickedIdx].m_dxv3Pt[2]-m_vtMesh[i32PickedIdx].m_dxv3Pt[0]);
	}
	return i32PickedIdx;
}

void CAIStageArray::Save(CFile* pFile)
{
	char FileHeader[] = { 19, 82, 5, 14 };
	assert( m_vtpChild.size() < 255);
	UINT8 ui8Num = static_cast<UINT8>(m_vtpChild.size());

	pFile->Write(FileHeader, sizeof(FileHeader));
	pFile->Write(&ui8Num, sizeof(UINT8));
	
	for(UINT32 i = 0; i < m_vtpChild.size(); i++)
	{
		m_vtpChild[i]->Save(pFile);
	}
}

BOOL CAIStageArray::InsertASE(CFile* pFile)
{
	CAIStage* pStage = new CAIStage;
	char szFileHeaderASEFormat[] = { '*', '3', 'D', 'S' };
	char szFileHeaderNMCFormat[] = { 20, 12, 2, 25 };
	char readData[4];

	pFile->Read(readData, sizeof(readData));

	if (0 == memcmp(szFileHeaderASEFormat, readData, sizeof(szFileHeaderASEFormat)))
	{
		pStage->LoadASE(pFile);
	}
	else if (0 == memcmp(szFileHeaderNMCFormat, readData, sizeof(szFileHeaderNMCFormat)))
	{
		pStage->LoadNMC(pFile);
	}
	else
	{
		delete pStage;
		return FALSE;
	}

	pStage->RebuildVB();
	m_vtpChild.push_back(pStage);
//	((CMainFrame*)AfxGetMainWnd())->Add(&pStage->m_cstrName);

	m_pSelected = m_vtpChild[m_vtpChild.size() - 1];
	return TRUE;
}

BOOL CAIStageArray::Load(CFile* pFile)
{
	char szFileHeaderNMFormat[] = { 19, 82, 5, 14 };
	char readData[4];
	UINT8 ui8Num;

	pFile->Read(readData, sizeof(readData));

	if (0 != memcmp(szFileHeaderNMFormat, readData, sizeof(szFileHeaderNMFormat))) return FALSE;
	
	m_vtpChild.clear();
	pFile->Read(&ui8Num, sizeof(UINT8));
	for(int i = 0; i < ui8Num; i++)
	{
		CAIStage* pStage = new CAIStage;
		pStage->LoadNM(pFile);
		pStage->RebuildVB();
		m_vtpChild.push_back(pStage);
//		((CMainFrame*)AfxGetMainWnd())->Add(&pStage->m_cstrName);
	}

	m_pSelected = m_vtpChild[0];
	return TRUE;	
}