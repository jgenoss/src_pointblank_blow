
#include "stdafx.h"
#include "dlgBatchRenAttrOpti.h"

#include "i3Opt/i3OptRenAttr.h"


//
// dlgBatchRenAttrOpti
//

BEGIN_MESSAGE_MAP(dlgBatchRenAttrOpti, CDialog)
	ON_BN_CLICKED(IDD_BRAO_BT_OPTIMIZE, &dlgBatchRenAttrOpti::OnStartOptimize)
	ON_BN_CLICKED(IDC_BRAO_BT_WRITE_RST, &dlgBatchRenAttrOpti::OnWriteResult)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


dlgBatchRenAttrOpti::dlgBatchRenAttrOpti() 
	: CDialog(dlgBatchRenAttrOpti::IDD)
{
	m_optimizer = i3OptRenAttr::new_object();
	m_continue = false;

	memset(m_oldWorkPath, 0, sizeof(m_oldWorkPath));
}

dlgBatchRenAttrOpti::~dlgBatchRenAttrOpti()
{
	I3_SAFE_RELEASE(m_optimizer);
	Clean();
}

BOOL dlgBatchRenAttrOpti::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	const char* oldWorkDir = i3ResourceFile::GetWorkingFolder();
	if(oldWorkDir)
		strcpy_s(m_oldWorkPath, MAX_PATH, oldWorkDir);
	else
		memset(m_oldWorkPath, 0, sizeof(m_oldWorkPath));

	i3ResourceFile::setSearchExternRefEnable(true);

	if(oldWorkDir)
		m_workPath.SetWindowText(oldWorkDir);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void dlgBatchRenAttrOpti::OnClose()
{
	if(m_oldWorkPath[0])
		i3ResourceFile::SetWorkingFolder(m_oldWorkPath);
	else
		i3ResourceFile::SetWorkingFolder(NULL);

	CDialog::OnClose();
}

void dlgBatchRenAttrOpti::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BRAO_EB_WORKPATH, m_workPath);
	DDX_Control(pDX, IDC_BRAO_EB_SUBPATH, m_subPath);
	DDX_Control(pDX, IDD_BRAO_BT_OPTIMIZE, m_btnOptimize);
	DDX_Control(pDX, IDC_BRAO_LB_LOG, m_log);
	DDX_Control(pDX, IDC_BRAO_CK_SAVE_I3S_OPTI_SUCCESS, m_saveI3SOnOptiOk);
}

void dlgBatchRenAttrOpti::OnStartOptimize()
{
	if(m_continue)
	{
		m_btnOptimize.SetWindowText("Optimize");
		m_continue = false;
	}
	else
	{
		Clean();

		CString strWorkPath;
		CString strSubPath;

		m_workPath.GetWindowText(strWorkPath);
		m_subPath.GetWindowText(strSubPath);

		if(strWorkPath.IsEmpty() || strSubPath.IsEmpty())
		{
			AfxMessageBox("ERR! - Empty path info founded..", MB_ICONERROR);
			return;
		}

		m_btnOptimize.SetWindowText("Cancel..");

		i3ResourceFile::SetWorkingFolder((char*)strWorkPath.GetString());

		char scanPath[MAX_PATH] = {0};
		sprintf_s(scanPath, MAX_PATH, "%s/%s/", strWorkPath.GetString(), strSubPath.GetString());

		Scani3S(scanPath);
		
		SetProgressRange((INT32)m_row.size());
		SetProgressPos(0);

		m_continue = true;

		INT32 numProc = 0;
		for(OptiRstRow::iterator x = m_row.begin(); x != m_row.end() && m_continue; 
			x = m_row.upper_bound((*x).first))
		{
			OptiRstRow::_Pairii range = m_row.equal_range((*x).first);
			for(OptiRstRow::iterator rngIter = range.first; 
				rngIter != range.second && m_continue; rngIter++)
			{
				OptiRst* curRst = (*rngIter).second;

				Log("[INFO] optimizing [%s/%s]", curRst->m_basePath.c_str(), curRst->m_filename.c_str());

				Optimize(curRst);
				SetProgressPos(numProc++);

				const char* stateDescPrefix = NULL;

				curRst->m_attrSetOptiRatio = 0.0f;
				curRst->m_renAttrOptiRatio = 0.0f;

				if(curRst->m_state == OptiRst::ESTATE_OPTIMIZED)
				{
					float attrSetPer = 0.0f;
					float renAttrPer = 0.0f;

					if(curRst->m_statitics[0].m_numAttrSet != 0)
						attrSetPer = ((float)curRst->m_statitics[1].m_numAttrSet / curRst->m_statitics[0].m_numAttrSet) * 100.0f;

					if(curRst->m_statitics[0].m_numRenAttr != 0)
						renAttrPer = ((float)curRst->m_statitics[1].m_numRenAttr / curRst->m_statitics[0].m_numRenAttr) * 100.0f;

					const char* rTxt = "optimized";

					if(renAttrPer == 100.0f)
						rTxt = "same to previous, no optimizable attrset is there..";

					Log("[INFO] [%s] is %s [attrSet=%3.2f%%][%d->%d] [renAttr=%3.2f%%][%d->%d] [cancel=%d]",
						curRst->m_filename.c_str(), rTxt, 
						attrSetPer, curRst->m_statitics[0].m_numAttrSet, curRst->m_statitics[1].m_numAttrSet,
						renAttrPer, curRst->m_statitics[0].m_numRenAttr, curRst->m_statitics[1].m_numRenAttr,
						curRst->m_statitics[0].m_numCancel);

					stateDescPrefix = "   [INFO-DESC]";
					
					curRst->m_attrSetOptiRatio = attrSetPer;
					curRst->m_renAttrOptiRatio = renAttrPer;
				}
				else
				{
					Log("[FAIL] [%s] is failed to optimize code=%d", curRst->m_filename.c_str(), curRst->m_errCode);
					stateDescPrefix = "   [FAIL-DESC]";
				}

				OptiRst::StateDesc& desc = curRst->m_stateDesc;
				if(!desc.empty())
				{
					OptiRst::StateDesc::iterator x2 = desc.begin();
					while(x2 != desc.end())
					{
						Log("%s [%s] ", stateDescPrefix, (*x2).c_str());
						x2++;
					}							
				}

				PumpMessage();
			}		
		}

		m_continue = false;
		m_btnOptimize.SetWindowText("Optimize");
	}

}

void dlgBatchRenAttrOpti::OnWriteResult()
{
	char path[MAX_PATH] = {0};
	
	OPENFILENAME ofn;
	{
		memset(&ofn, 0, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner	= GetSafeHwnd();
		ofn.lpstrFilter = "excel csv(*.csv)\0*.csv\0csv file(*.csv)\0\0";
		ofn.lpstrFile   = path;
		ofn.lpstrTitle  = "Select directory to save result file..";
		ofn.nMaxFile	= 1024;
		ofn.Flags = OFN_EXPLORER;
	}

	if(GetSaveFileName(&ofn))
	{
		i3FileStream outStream;

		if(!outStream.Create(path, STREAM_WRITE))
			return;

		char buf[2048] = {0};
		UINT32 no = 0;

		sprintf_s(buf, 2048, "no,state,path,i3AttrsetPrev,i3AttrsetAfter,i3AttrOptiRstRatio,i3RenattrPrev,i3RenattrAfter,i3RenattrOptiRatio,Desc\n\r");
		outStream.WriteLine(buf);

		UINT32 totalStatitics			= 0;

		UINT32 totalOptimized           = 0;
		UINT32 totalNothingOptimize     = 0;
		UINT32 totalOptimizedAndSaved	= 0;
		UINT32 totalErrDuppRenAttr      = 0;
		UINT32 totalErrInternal			= 0;

		UINT32 totalAttrSetPrev			= 0;
		UINT32 totalAttrSetAfter		= 0;
		float  totalAttrSetOptiRatio	= 0.0f;
		UINT32 totalRenAttrPrev			= 0;
		UINT32 totalRenAttrAfter		= 0;
		float  totalRenAttrOptiRatio    = 0.0f;


		for(OptiRstRow::iterator x = m_row.begin(); x != m_row.end();
			x = m_row.upper_bound((*x).first))
		{
			OptiRstRow::_Pairii rangeIter = m_row.equal_range((*x).first);
			for(OptiRstRow::iterator y = rangeIter.first; 
				y != rangeIter.second; y++)
			{
				//(*y).first
				OptiRst* curOptiRst = (*y).second;

				std::string path2 = ((*y).first).c_str();
				path2 += curOptiRst->m_filename;

				std::string stateStr;
				DWORD errCode = curOptiRst->m_errCode;
				bool addStatitics = false;

				if((i3OptRenAttr::OptiRst::ORF_ERR_DUPPLICATED_RENATTR & errCode) == i3OptRenAttr::OptiRst::ORF_ERR_DUPPLICATED_RENATTR)
				{
					stateStr = "ERR_DUPP_RENATTR";
					totalErrDuppRenAttr++;
				}
				else if((i3OptRenAttr::OptiRst::ORF_ERR_INTERNAL_OPERATION & errCode) == i3OptRenAttr::OptiRst::ORF_ERR_INTERNAL_OPERATION)
				{
					stateStr = "ERR_INTERNAL_OPER";
					totalErrInternal++;
				}
				else
				{
					if((i3OptRenAttr::OptiRst::ORF_NOTHING_IS_OPTIMIZED & errCode) == i3OptRenAttr::OptiRst::ORF_NOTHING_IS_OPTIMIZED)
					{
						stateStr = "NOTHING_OPTI";						
						totalNothingOptimize++;
					}
					else
					{
						stateStr = "OPTIMIZED";
						totalOptimized++;

						if((i3OptRenAttr::OptiRst::ORF_OPTIMIZED_AND_SAVED & errCode) == i3OptRenAttr::OptiRst::ORF_OPTIMIZED_AND_SAVED)
						{
							stateStr = "OPTIMIZED_AND_SAVED";
							totalOptimizedAndSaved++;
						}
					}

					addStatitics = true;
				}

				std::string stateDesc = " ";

				OptiRst::StateDesc::iterator sdIter = curOptiRst->m_stateDesc.begin();
				while(sdIter != curOptiRst->m_stateDesc.end())
				{
					std::string& cs = (*sdIter);

					if(cs.c_str())
						stateDesc = stateDesc + cs.c_str() +" | ";

					sdIter++;
				}

				sprintf_s(buf, 2048, "%d,%s,%s,%d,%d,%3.2f,%d,%d,%3.2f,%s\n\r", 
					no, 
					stateStr.c_str(),
					path2.c_str(),
					curOptiRst->m_statitics[0].m_numAttrSet,
					curOptiRst->m_statitics[1].m_numAttrSet,
					curOptiRst->m_attrSetOptiRatio,
					curOptiRst->m_statitics[0].m_numRenAttr,
					curOptiRst->m_statitics[1].m_numRenAttr,
					curOptiRst->m_renAttrOptiRatio,
					stateDesc.c_str() ? stateDesc.c_str() : " "
				);				
				outStream.WriteLine(buf);

				if(addStatitics)
				{
					totalStatitics++;

					totalAttrSetPrev		+= curOptiRst->m_statitics[0].m_numAttrSet;
					totalAttrSetAfter		+= curOptiRst->m_statitics[1].m_numAttrSet;
					totalAttrSetOptiRatio	+= curOptiRst->m_attrSetOptiRatio;
					totalRenAttrPrev		+= curOptiRst->m_statitics[0].m_numRenAttr;
					totalRenAttrAfter		+= curOptiRst->m_statitics[1].m_numRenAttr;
					totalRenAttrOptiRatio   += curOptiRst->m_renAttrOptiRatio;
				}

				no++;
			}			
		}
		
		// add statitics

		char statitics[1024] = {0};
		sprintf_s(statitics, 1024, "optimized=%d / nothing-optimize=%d / optimized and saved=%d / err-dupp renattr=%d / err-internal=%d ",
			totalOptimized, totalNothingOptimize, totalOptimizedAndSaved, totalErrDuppRenAttr, totalErrInternal);

		sprintf_s(buf, 2048, "99999,STATICS-OPTI,%s,%d,%d,%3.2f,%d,%d,%3.2f,%s\n\r",
					statitics,
					totalAttrSetPrev,	
					totalAttrSetAfter,	
					((float)totalAttrSetAfter/(float)totalAttrSetPrev) * 100.0f,
					totalRenAttrPrev,	
					totalRenAttrAfter,	
					((float)totalRenAttrAfter/(float)totalRenAttrPrev) * 100.0f,
					" ");
		outStream.WriteLine(buf);
		outStream.Close();
	}
}

void dlgBatchRenAttrOpti::Clean()
{
	OptiRstRow::iterator x = m_row.begin();
	for(;x != m_row.end(); x++)
		delete (*x).second;

	m_row.clear();

	if(m_log.GetSafeHwnd())
		m_log.ResetContent();
}

INT32 dlgBatchRenAttrOpti::Scani3S(const char* path)
{
	if(!path)
		return 0;

	FoundFileInserter inserter(m_row);

	return Collecti3sFile(inserter, path, true);
}

INT32 dlgBatchRenAttrOpti::Collecti3sFile(FoundFileInserter& inserter, const char* path, 
										  bool recSubDirScan)
{
	std::vector<std::string>	curCollectedSubDir;
	WIN32_FIND_DATAA		findData;
	char curBasePath[MAX_PATH] = {0};
	char findFirstPath[MAX_PATH] = {0};
	UINT numCollected = 0;
	
	bool cont = true;

	strcpy_s(curBasePath, MAX_PATH, path);

	sprintf_s(findFirstPath, MAX_PATH, "%s*", curBasePath);	

	HANDLE hFind = FindFirstFileA(findFirstPath, &findData);
	while(hFind != INVALID_HANDLE_VALUE && cont)
	{
		if(findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!inserter.isSkipDir(findData.cFileName))
			{
				// Directory
				char subDirPath[MAX_PATH] = {0};

				strcat_s(subDirPath, MAX_PATH, curBasePath);
				strcat_s(subDirPath, MAX_PATH, findData.cFileName);
				strcat_s(subDirPath, MAX_PATH, "/");

				curCollectedSubDir.push_back(subDirPath);
			}
		}
		else
		{
			if( (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || // ex .svn
				(findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ||
				(findData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) ||
				(findData.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ||
				(findData.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE)
				)
			{
				;
			}
			else
			{
				// File
				INT32 siz = strlen(findData.cFileName);
				if(siz > 3)
				{
					if(inserter.TestFilter(findData.cFileName))
					{
						inserter.AddFilePath(curBasePath, findData.cFileName);
						numCollected++;
					}
				}				
			}
		}

		cont = FindNextFileA(hFind, &findData) ? true : false;
	}

	if(hFind)
	{
		FindClose(hFind);
		hFind = NULL;
	}

	if(recSubDirScan)
	{
		for(INT32 i=0; i<(INT32)curCollectedSubDir.size(); i++)
			numCollected += Collecti3sFile(inserter, curCollectedSubDir[i].c_str(), recSubDirScan);
	}

	return numCollected;
}


//
// dlgBatchRenAttrOpti::FoundFileInserter
//
dlgBatchRenAttrOpti::FoundFileInserter::FoundFileInserter(OptiRstRow& out)
	: m_out(out)
{	
}

bool dlgBatchRenAttrOpti::FoundFileInserter::isSkipDir(const char* path)
{
	if(!path)
		return true;

	// skip ., ..
	if( (strlen(path) == 1 && !stricmp(path, ".")) ||
		 (strlen(path) == 2 && !stricmp(path, "..")) )
		 return true;

	return false;
}

bool dlgBatchRenAttrOpti::FoundFileInserter::TestFilter(const char* filename)
{
	size_t siz = strlen(filename);

	if(siz < 5)
		return false;

	const char* p = strchr(filename, '.');
	if(!p)
		return false;

	if(stricmp(p+1, "i3s"))
		return false;

	return true;		
}

void dlgBatchRenAttrOpti::FoundFileInserter::AddFilePath(const char* basePath, const char* fileName)
{
	char lwrBasePath[MAX_PATH] = {0};
	char lwrFileName[MAX_PATH] = {0};
	char lwrFullpath[MAX_PATH] = {0};

	strcpy_s(lwrBasePath, MAX_PATH, basePath); strlwr(lwrBasePath);
	strcpy_s(lwrFileName, MAX_PATH, fileName); strlwr(lwrFileName);	
	sprintf_s(lwrFullpath, MAX_PATH, "%s%s", lwrBasePath, lwrFileName);  strlwr(lwrFullpath);

	bool firstInsert = false;

	OptiRstRow::iterator x = m_out.find(lwrBasePath);
	if(x == m_out.end())
	{
		OptiRst* newOptiRst = new OptiRst;

		newOptiRst->m_basePath = lwrBasePath;
		newOptiRst->m_filename = lwrFileName;

		OptiRstRow::value_type val(lwrBasePath, newOptiRst);
		x = m_out.insert(val);
		firstInsert = true;
	}

	if(!firstInsert)
	{
		bool foundDupplicate = false;
		OptiRstRow::_Pairii range = m_out.equal_range((*x).first);
		for(OptiRstRow::iterator rngIter = range.first;	rngIter != range.second; rngIter++)
		{
			OptiRst& src = *((*rngIter).second);

			if(!stricmp(src.m_filename.c_str(), fileName))
			{
				foundDupplicate = true;
				break;
			}
		}

		if(foundDupplicate)
			return;

		OptiRst* newOptiRst = new OptiRst;

		newOptiRst->m_basePath = lwrBasePath;
		newOptiRst->m_filename = lwrFileName;

		OptiRstRow::value_type val(lwrBasePath, newOptiRst);
		m_out.insert(val);
	}
}

dlgBatchRenAttrOpti::OptiRst::OptiRst()
{
	m_state = ESTATE_READY_FOR_OPTIMIZE;	
	memset(m_statitics, 0, sizeof(m_statitics));

	m_attrSetOptiRatio = 0.0f;
	m_renAttrOptiRatio = 0.0f;

	m_errCode = 0;
}

struct OptRstErrOutputAgent
{
	OptRstErrOutputAgent(dlgBatchRenAttrOpti::OptiRst::StateDesc& out)
		: m_out(out) { }

	static void output(const char* in)
	{
		if(in && ms_curInst)
			ms_curInst->m_out.push_back(std::string(in));
	}

	dlgBatchRenAttrOpti::OptiRst::StateDesc&	m_out;
	static OptRstErrOutputAgent*				ms_curInst;
};

OptRstErrOutputAgent* OptRstErrOutputAgent::ms_curInst = NULL;


void dlgBatchRenAttrOpti::Optimize(OptiRst* outRst)
{
	if(!outRst)
		return;

	char filepath[MAX_PATH] = {0};
	sprintf_s(filepath, MAX_PATH, "%s%s", outRst->m_basePath.c_str(),
		outRst->m_filename.c_str());

	OptRstErrOutputAgent errOutAgent(outRst->m_stateDesc);
	OptRstErrOutputAgent::ms_curInst = &errOutAgent;

	i3OptRenAttr::OptiRst optimizerResult;	
	optimizerResult.m_errOutput = OptRstErrOutputAgent::output;

	bool saveI3SOnOptiOk = m_saveI3SOnOptiOk.GetCheck() ? true : false;

	if(m_optimizer->DoOptimize(filepath, &optimizerResult, saveI3SOnOptiOk))
	{
		// ok
		outRst->m_state = OptiRst::ESTATE_OPTIMIZED;

		outRst->m_statitics[0].m_numAttrSet = optimizerResult.m_numAttrSet[0];
		outRst->m_statitics[0].m_numRenAttr = optimizerResult.m_numRenAttr[0];
		(outRst->m_statitics[0]).m_numCancel  = optimizerResult.m_numCancelOper;
		
		outRst->m_statitics[1].m_numAttrSet = optimizerResult.m_numAttrSet[1];
		outRst->m_statitics[1].m_numRenAttr = optimizerResult.m_numRenAttr[1];
		(outRst->m_statitics[1]).m_numCancel  = optimizerResult.m_numCancelOper;
	}
	else
	{
		// fail
		outRst->m_state = OptiRst::ESTATE_OPTIMIZE_FAILED;
	}

	outRst->m_errCode = optimizerResult.m_operRstFlag;

	PumpMessage();
}

void dlgBatchRenAttrOpti::PumpMessage()
{
	HWND dlg = GetSafeHwnd();

	MSG msg;
	while(::PeekMessage(&msg, dlg, 0, 0, PM_NOREMOVE))
	{
		if(::GetMessage(&msg, dlg, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void dlgBatchRenAttrOpti::SetProgressRange(INT32 rangeMax)
{
	HWND ctlProgress = NULL;
	GetDlgItem(IDC_BRAO_PB_PROGRESS, &ctlProgress);
	::SendMessage(ctlProgress, PBM_SETRANGE, 0, MAKELONG(0, rangeMax));
}

void dlgBatchRenAttrOpti::SetProgressPos(INT32 pos)
{
	HWND ctlProgress = NULL;
	GetDlgItem(IDC_BRAO_PB_PROGRESS, &ctlProgress);
	::SendMessage(ctlProgress, PBM_SETPOS, pos, 0);
}

void dlgBatchRenAttrOpti::Log(const char* fmt, ...)
{
	va_list va;
	char buf[1024] = {0};

	va_start(va, fmt);
	vsprintf(buf, fmt, va);
	va_end(va);

	m_log.AddString(buf);
	m_log.SetCurSel(m_log.GetCount()-1);
}
