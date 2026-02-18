// Dlg_Merge_1PV_3PV.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "Dlg_Merge_1PV_3PV.h"
#include "AI.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/generic_string_replace_all.h"

// CDlg_Merge_1PV_3PV dialog

IMPLEMENT_DYNAMIC(CDlg_Merge_1PV_3PV, CDialog)

CDlg_Merge_1PV_3PV::CDlg_Merge_1PV_3PV(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Merge_1PV_3PV::IDD, pParent)
{

}

CDlg_Merge_1PV_3PV::~CDlg_Merge_1PV_3PV()
{
}

void CDlg_Merge_1PV_3PV::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_Merge_1PV_3PV, CDialog)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDlg_Merge_1PV_3PV message handlers


void CDlg_Merge_1PV_3PV::BuildGroups( i3::vector<i3::rc_string>& List)
{
	char szFileName[MAX_PATH];
	char szPath[ MAX_PATH], sz1PV[ MAX_PATH], sz3PV[ MAX_PATH];
	INT32 idx0, idx1;

	while( List.size() > 0)
	{
	//	i3String::SplitFileName( List[0].c_str(), szFileName, FALSE);
		i3::extract_filetitle(List[0], szFileName);

		i3::generic_string_ireplace_all( szFileName, "_1PV", "");
		i3::generic_string_ireplace_all( szFileName, "_3PV", "");

//		i3String::SplitPath(  List[0].c_str(), szPath);
		i3::extract_directoryname(List[0], szPath);

		i3::snprintf( sz1PV, sizeof(sz1PV), "%s\\%s_1PV.i3AI", szPath, szFileName);
		idx0 =	i3::vu::int_index_of(List, sz1PV);		//		pList->IndexOf( sz1PV);

		i3::snprintf( sz3PV, sizeof(sz3PV), "%s\\%s_3PV.i3AI", szPath, szFileName);
		idx1 =	i3::vu::int_index_of(List, sz3PV);		//		pList->IndexOf( sz3PV);

		if((idx0 != -1) && (idx1 != -1))
		{
			GROUP_INFO * pInfo = new GROUP_INFO;

			i3::safe_string_copy( pInfo->m_1PV, List[idx0].c_str(), MAX_PATH );
			i3::safe_string_copy( pInfo->m_3PV, List[idx1].c_str(), MAX_PATH );
			i3::safe_string_copy( pInfo->m_szName, szFileName, 128 );
			i3::safe_string_copy( pInfo->m_szPath, szPath, MAX_PATH );

			I3TRACE( "%s + %s -> %s\n", pInfo->m_1PV, pInfo->m_3PV, pInfo->m_szName);

			m_GroupList.push_back( pInfo);

			i3::vu::erase_index(List, idx0);		//	pList->Delete( idx0);

			idx1 =	i3::vu::int_index_of(List, sz3PV);		//	pList->IndexOf( sz3PV);
			i3::vu::erase_index(List, idx1);		//	pList->Delete( idx1);
		}
		else
		{
			if( idx0 == -1)
			{
				//CharaEditorNotifyBox( m_hWnd, "%s 파일은 못찾았음!", sz1PV);
				I3TRACE( "ERR : %s\n", sz1PV);
			}
			else
			{
				//CharaEditorNotifyBox( m_hWnd, "%s 파일을 못찾음!", sz3PV);
				I3TRACE( "ERR : %s\n", sz3PV);
			}

			i3::vu::erase_index(List, 0);		// pList->Delete( 0);
		}
		
	}
}

void CDlg_Merge_1PV_3PV::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];
	i3::vector<i3::rc_string>	list;

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		I3TRACE( "FILE : %s\n", TempName);
		list.push_back( TempName);
	}

	DragFinish( hDropInfo);

	BuildGroups( list);

	MergeAI();
}

static void Scan_AI( i3ResourceFile * pFile, i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < pFile->GetObjectCount(); i++)
	{
		i3PersistantElement * pObj = pFile->GetObject( i);

		if( i3::same_of<AI*>(pObj)) //->IsExactTypeOf( AI::static_meta()))
		{
			AI * pAI = (AI *) pObj;

			pAI->ConvertToAnimTemplate( true);

			List.push_back( pObj);
		}
	}
}

void CDlg_Merge_1PV_3PV::Merge( GROUP_INFO * pInfo)
{
	AI * pAI_1PV, * pAI_3PV;
	AI * pRoot;
	i3::vector<i3PersistantElement*> list;

	{
		pRoot = AI::new_object();
		pRoot->SetName( "CharaAI");

		i3AI * pAI = i3AI::new_object_ref();
		pAI->SetName( "CharaAI");

		pRoot->SetAI( pAI);

		list.push_back( pRoot);
	}

	{
		i3ResourceFile file;

		if( file.Load( pInfo->m_1PV) == FALSE)
		{
			CharaEditorNotifyBox( m_hWnd, "%s 파일을 읽어 들일 수 없습니다.", pInfo->m_1PV);
			return;
		}

		pAI_1PV = (AI *) file.getKeyObject();
		pAI_1PV->AddRef();

		pAI_1PV->SetName( "1PV");
		pAI_1PV->getAI()->SetName( "1PV");

		Scan_AI( &file, list);
	}

	{
		i3ResourceFile file;

		if( file.Load( pInfo->m_3PV) == FALSE)
		{
			CharaEditorNotifyBox( m_hWnd, "%s 파일을 읽어 들일 수 없습니다.", pInfo->m_3PV);
			return;
		}

		pAI_3PV = (AI *) file.getKeyObject();
		pAI_3PV->AddRef();

		pAI_3PV->SetName( "3PV");
		pAI_3PV->getAI()->SetName( "3PV");

		Scan_AI( &file, list);
	}

	{
		i3AIContext * pRootCtx = pRoot->getAIContext();

		pRootCtx->AddChild( pAI_1PV->getAIContext());
		pRootCtx->AddChild( pAI_3PV->getAIContext());
	}

	{
		i3ResourceFile file;
		char szFull[ MAX_PATH];
		

		i3::snprintf( szFull, sizeof(szFull), "%s/%s.i3AI", pInfo->m_szPath, pInfo->m_szName);

		file.setKeyObject( pRoot);
		file.SetObjects( list);

		file.Save( szFull);
	}

	I3_SAFE_RELEASE( pRoot);
	I3_SAFE_RELEASE( pAI_1PV);
	I3_SAFE_RELEASE( pAI_3PV);
}

void CDlg_Merge_1PV_3PV::MergeAI(void)
{
	
	GROUP_INFO * pInfo;

	for(size_t i = 0; i < m_GroupList.size(); i++)
	{
		pInfo = m_GroupList[i];

		Merge( pInfo);

		delete pInfo;
	}
	
	m_GroupList.clear();
}

