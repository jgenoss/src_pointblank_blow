#include "pch.h"
#include "ExportThread.h"
#include "GlobalVariable.h"
#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/string/ext/extract_directoryname.h"

I3_CLASS_INSTANCE( CExportThread); //, i3Thread);

CExportThread::CExportThread(void)
{
	m_szDestPath[0] = 0;
	m_pDB = NULL;
}

CExportThread::~CExportThread(void)
{
}

void CExportThread::BuildUsedResList(void)
{
	::SendMessage( m_hwnd, WM_EXPORT_STATE, (WPARAM)"Finding used resources...", 0);
	
	m_ResList.clear();
	m_ElmList.clear();

	i3LevelElement3D * pElm;
	i3LevelRes * pRes;
	
	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS_SET, m_pScene->GetElementCount(), 0);

	// Element List 구축
	for(INT32 i = 0; i < m_pScene->GetElementCount(); i++)
	{
		pElm = m_pScene->GetElement( i);

		if( pElm->IsStyle( I3_LEVEL_STYLE_VOLATILE | I3_LEVEL_STYLE_CONTROL))
			continue;

		if( pElm->IsState( I3_LEVEL_STATE_DISABLE | I3_LEVEL_STATE_HIDE))
			continue;

		m_ElmList.push_back( pElm);

		::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, i + 1, 0);
	}

	for( size_t i = 0; i < m_ElmList.size(); i++)
	{
		pElm = m_ElmList[i];

		if( i3::kind_of<i3LevelPrimitive*>(pElm))
		{
			i3LevelPrimitive * pPrim = (i3LevelPrimitive*)pElm;

			if( pPrim->isTrigger())
			{
				i3LevelResEvent * pEvt = pPrim->getEventRes();

				if( pEvt != NULL)
				{
					pEvt->OnQueryDependancy( m_ResList);
				}
			}

			// Level Primitive Class들은 Resource를 저장할 필요가 없다.
		}
		else
		{
			pRes = pElm->getRes();

			if( pRes != NULL)
			{
				pRes->OnQueryDependancy( m_ResList);
			}
		}
	}

}

bool CExportThread::_IsNeedToUpdate( const char * pszPath, i3LevelRes * pRes)
{
	// 마지막 Update된 날짜를 확인한다.
	WIN32_FIND_DATA info;
	HANDLE hFind = FindFirstFile( pszPath, &info);

	if( pRes->isResState( I3LV_RES_STATE_MODIFIED))
	{
		// 변경된 Resource라면...
		return true;
	}

	if( hFind == INVALID_HANDLE_VALUE)
	{
		// 파일이 없다.
		// 반드시 Update해야 함.
		return true;
	}

	FindClose( hFind);

	INT64 date;

	date = ((INT64) info.ftLastWriteTime.dwHighDateTime << 32) | info.ftLastWriteTime.dwLowDateTime;
	if( date != pRes->getLastExportFileTime())
		return true;

	return false;
}

void CExportThread::ValidateResource(void)
{
	i3LevelRes * pRes;
	char conv[256], szPath[MAX_PATH];

	::SendMessage( m_hwnd, WM_EXPORT_STATE, (WPARAM)"Validating Resources...", 0);

	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS_SET, (INT32)m_ResList.size(), 0);

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList[i];

		sprintf( szPath, "%s/%s", m_szDestPath, pRes->getPath());

		if( _IsNeedToUpdate( szPath, pRes))
		{
			char szFull[MAX_PATH];

			// Validate Destination dirs..
			{
				char szPathOnly[MAX_PATH]; //, szName[MAX_PATH], * pszPathEnd;
				i3::extract_directoryname(pRes->getPath(), szPathOnly);

				sprintf( szFull, "%s/%s", g_pProfile->m_szWorkingDir, szPathOnly);
				i3System::PrepareDirectory( szFull);

				sprintf( conv, "· Resource validating...%s", pRes->getPath());
			}

			::SendMessage( m_hwnd, WM_EXPORT_LOG, (WPARAM) conv, 0);

			pRes->OnExport( m_pScene, szPath);
		}

		::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, i + 1, 0);
	}
}

void CExportThread::ExportResList(void)
{

	i3GameRes * pGameRes;
	i3GameSceneInfo * pSceneInfo;
	i3LevelRes * pRes;

	::SendMessage( m_hwnd, WM_EXPORT_STATE, (WPARAM)"Building Resource List Informations", 0);

	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS_SET, (INT32)m_ResList.size() + 2, 0);

	GameResourceListPtr pList = i3::make_shared<i3::vector<i3GameRes*> >();		//List::new_object();

	for( size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList[i];

		I3TRACE( "RES : %s\n", pRes->getPath());

		pGameRes = pRes->CreateGameRes();
		if( pGameRes == NULL)
			continue;
		pGameRes->SetName( pRes->getPath());
			
		pList->push_back( pGameRes);
			
		::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, i + 1, 0);
	}

	// Resource List
	{
		pSceneInfo = i3GameSceneInfo::new_object();

		pSceneInfo->setResourceList( pList);

		char szDest[MAX_PATH];

		sprintf( szDest, "%s/Scene/%s/%s.RSC", m_szDestPath, m_pScene->GetName(), m_pScene->GetName());

		{
			char szDir[MAX_PATH];

//			i3String::SplitPath( szDest, szDir);
			i3::extract_directoryname(szDest, szDir);

			i3System::PrepareDirectory( szDir);
		}

		pSceneInfo->SaveResource( szDest);
		::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, m_ResList.size() + 1, 0);
	}

	// Resource Removal
	for(size_t i = 0; i < pList->size(); i++)
	{
		pGameRes = (*pList)[i];

		pGameRes->Release();
	}

//	pList->Release();
	pSceneInfo->Release();

	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, m_ResList.size() + 2, 0);
}

void CExportThread::ExportInstanceList(void)
{
	::SendMessage( m_hwnd, WM_EXPORT_STATE, (WPARAM)"Building Element Instance List Informations", 0);

	if( m_pScene->GetElementCount() == 0)
		return;

	
	UINT32 rc;
	i3LevelElement3D * pElm;
	i3::pack::GAME_INSTANCE_PACK header;
	char szPath[ MAX_PATH];
	i3FileStream strm;
	i3GameSceneInfo * pGS;
//	List ElmList;					// 쓰이지 않는듯 해서 주석..

	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS_SET, (INT32)m_ElmList.size() + 1, 0);

	// Instance List File
	{
		sprintf( szPath, "%s/Scene/%s", m_szDestPath, m_pScene->GetName());
		i3System::PrepareDirectory( szPath);

		sprintf( szPath, "%s/Scene/%s/%s.i3INS", m_szDestPath, m_pScene->GetName(), m_pScene->GetName());

		if( strm.Create( szPath, STREAM_WRITE) == FALSE)
		{
			::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Instance List 파일을 생성할 수 없습니다.", 0);
			::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
			return;
		}
	}

	{
		header.m_nInstanceCount = m_ElmList.size();
		header.m_LayerCount =		m_pScene->getLayerCount();
		
		rc = strm.Write( &header, sizeof(header));
		if( rc == STREAM_ERR)
		{
			::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Instance List 파일을 생성할 수 없습니다.", 0);
			::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
		}
	}

	// Layer 정보 저장
	if( m_pScene->getLayerCount() > 0)
	{
		INT32 cnt = m_pScene->getLayerCount();
		i3::pack::GAME_LAYER * pLayers = (i3::pack::GAME_LAYER *) i3MemAlloc( sizeof(i3::pack::GAME_LAYER) * cnt);
		INT32 j;

		i3mem::FillZero( pLayers, sizeof( i3::pack::GAME_LAYER) * cnt);

		for( j = 0; j < cnt; j++)
		{
			i3LevelLayer * pLayer = m_pScene->getLayer( j);

			i3::safe_string_copy( pLayers[j].m_szName, pLayer->GetName(), _countof( pLayers[j].m_szName));
		}

		rc = strm.Write( pLayers, sizeof( i3::pack::GAME_LAYER) * cnt);
		if( rc == STREAM_ERR)
		{
			::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Layer 정보를 저장할 수 없습니다. .", 0);
			::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
		}

		i3MemFree( pLayers);
	}

	{
		i3::pack::GAME_INSTANCE_2 info;
		i3GameObj * pObj;
		void * pData;

		// i3GameSceneInfo를 준비한다.
		{
			pGS = i3GameSceneInfo::new_object();
			pGS->Create( NULL);

			for(size_t i = 0; i < m_ElmList.size(); i++)
			{
				pElm = m_ElmList[i];

				pObj = pElm->CreateGameInstance();

				pGS->AddInstance( pObj);

				I3_SAFE_RELEASE( pObj);
			}
		}

		for(size_t i = 0; i < m_ElmList.size(); i++)
		{
			pElm = m_ElmList[i];

			i3mem::FillZero( &info, sizeof(info));

			pElm->OnSetInstanceInfo( &info);

			// Layer 정보 갱신
			if( pElm->getLayer() != NULL)
			{
				i3LevelLayer * pLayer = pElm->getLayer();

				info.m_Layer = m_pScene->FindLayerByName( pLayer->GetName());
				I3ASSERT( info.m_Layer != -1);
			}

			{
				pData = NULL;
				pObj = pElm->CreateGameInstance();

				if( pObj != NULL)
				{
					info.nGameDataSize = pObj->OnQueryGameData( pGS, pElm, & pData);

					I3_SAFE_RELEASE( pObj);
				}
			}

			rc = strm.Write( &info, sizeof(info));
			if( rc == STREAM_ERR)
			{
				::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Instance List 파일을 생성할 수 없습니다.", 0);
				::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
			}

			// 추가 Game Data의 저장
			{
				if( info.nGameDataSize > 0)
				{
					rc = strm.Write( pData, info.nGameDataSize);
					if( rc == STREAM_ERR)
					{
						::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Instance List 파일을 생성할 수 없습니다.", 0);
						::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
					}

					i3MemFree( pData);
				}

				if( info.nNameSize > 0)
				{
					rc = strm.Write( (void*)pElm->GetName(), info.nNameSize);
					if( rc == STREAM_ERR)
					{
						::SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Element의 이름을 저장할수 없습니다. .", 0);
						::SendMessage( m_hwnd, WM_EXPORT_CANCEL, 0, 0);
					}
				}
			}

			::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, i + 1, 0);
		}

		strm.Close();

		// i3GameSceneInfo의 Destructor에서 Instance들을 Sg.에서 분리, 해제하는 처리를 한다.
		// 결과로 Level Tool에서 보이지 않는 현상이 발생하기 때문에
		// i3GameSceneInfo의 Instance List를 교체하여 해제하지 못하도록 Trick 처리...
		{
		//	List * pList = List::new_object_ref();
			InstanceListPtr pList = i3::make_shared<i3::vector<i3GameObj*> >();
			pGS->setInstanceList( pList);
		}

		pGS->Release();

		//::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, i + 2, 0);

		if( rc == STREAM_ERR)
		{
			SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Instance List 파일을 저장할 수 없습니다.", 0);
		}
	}
}

void CExportThread::ExportPostProcessList()
{
	::SendMessage( m_hwnd, WM_EXPORT_STATE, (WPARAM)"Save Post Process Informations", 0);
	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS_SET, 1, 0);
		
	char conv[MAX_PATH];
	sprintf( conv, "%s/World/%s/%s", m_szDestPath, m_pScene->GetName(), m_pScene->GetName());
	i3::generic_string_cat(conv, "_PostProcess.xml");

	i3XMLFile xmlFile;

	i3PostProcessManager * pManager = g_pViewerCtrl->getViewer()->GetPostProcessManager();
	pManager->SavePostProcessToXML(&xmlFile);

	bool bResult = xmlFile.Save(conv);

	if(!bResult)
	{
		SendMessage( m_hwnd, WM_EXPORT_ERROR, (WPARAM) "Post Process 파일을 저장할 수 없습니다.", 0);
	}

	::SendMessage( m_hwnd, WM_EXPORT_PROGRESS, 1, 0);
}


UINT32 CExportThread::OnRunning( void * pUserData)
{
	BuildUsedResList();

	ValidateResource();

	ExportResList();

	ExportInstanceList();

	ExportPostProcessList();

	::SendMessage( m_hwnd, WM_EXPORT_END, 0, 0);

	return 0;
}
