
#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#pragma comment (lib, "../ToolAutomation/debug/ToolAM.lib")
#else
#pragma comment (lib, "../ToolAutomation/distribute/ToolAM.lib")
#endif


#include "ToolAM.h"
#include "ToolAM_Def_CharTool.h"

using namespace ToolAM;
using namespace ToolAM::CharTool;
using namespace ToolAM::Util;

TOOLAM_CMDQ_DEF(ToolAM::CharTool::QNAME, &ToolAM::CharTool::ConvertCmdStr);
TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_NEW);
TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_ADDLOD_WITH_HERO);
TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_EXPORT_HERO_MASK);
TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_SAVE);

TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_EXPORT_CHARA);
TOOLAM_CMDFN_REGISTER(ToolAM::CharTool::QNAME, ECMD_CHARTOOL_MF_I3CHR_DELETE_AIDRIVERCPP);


TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_NEW)
{
	CMainFrame* m = (CMainFrame*)cb->m_prm;

	m->OnFileNew();
	m->m_PanelResource.AM_I3CHR_SetName(args[0].GetStr());

	return ECMD_PROC_RST_OK;
}

// 0: workdir, 1:subdir, 2:lodfilename pattern, 3:maskname, 4:heroname
TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_ADDLOD_WITH_HERO)
{
	CMainFrame* m = (CMainFrame*)cb->m_prm;

	std::string path = GeneratePath(args[0].GetStr(), args[1].GetStr());
	std::string file;

	for(INT32 i=0; i<3; i++)
	{
		char bufHeroName[MAX_PATH] = {0};
		char loadLODFilePath[MAX_PATH] = {0};

		sprintf_s(bufHeroName, MAX_PATH, "%s", args[4].GetStr());

		file = ToolAM::CharTool::EvaluateLODFileName(args[2].GetStr(), args[4].GetStr(), args[3].GetStr(), i);
		std::string resPath = path + '/' + args[3].GetStr() + '/' +"Resource" + '/' + file;

		strcpy_s(loadLODFilePath, MAX_PATH, resPath.c_str());
		bool rst = m->LoadResFile(resPath.c_str(), true);

		if(!rst)
		{
			// try old name
			sprintf_s(bufHeroName, MAX_PATH, "%s", ToolAM::GetOldHeroNameByName(args[4].GetStr()));

			file = ToolAM::CharTool::EvaluateLODFileName(args[2].GetStr(), bufHeroName, args[3].GetStr(), i);
			std::string resAlterPath = path + '/' + args[3].GetStr() + '/' +"Resource" + '/' + file;

			strcpy_s(loadLODFilePath, MAX_PATH, resAlterPath.c_str());
			rst = m->LoadResFile(resAlterPath.c_str(), true);
		}

		if(!rst) 
		{
			cb->Log(-1, "Failed to load lodfile (%s)", resPath.c_str());
			return ECMD_PROC_RST_FAIL;
		}

//		cb->Log(0, "Successfully loaded lodfile(%s)..", loadLODFilePath);
	}

	return ECMD_PROC_RST_OK;
}

// 0: workdir, 1:export i3chr path
TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_EXPORT_HERO_MASK)
{
	CMainFrame* m = (CMainFrame*)cb->m_prm;

	COptionInfo* expOpt = g_pOption->Clone();

	strcpy_s(expOpt->m_szWorkPath, MAX_PATH, args[0].GetStr());

	expOpt->m_bExportI3S = TRUE;
	expOpt->m_bExportI3CHR = TRUE;

	expOpt->m_bRotateAxis = FALSE;

	expOpt->m_bGenMipmap = TRUE;
	expOpt->m_bConvertDXT = TRUE;	expOpt->m_bDXTAlpha	= TRUE;
	expOpt->m_bExternTexture = TRUE;
 
	expOpt->m_bOptAnim = FALSE;		expOpt->m_bGetAnimPack = FALSE;

	expOpt->m_bGenC = TRUE;

	bool rst = m->Exporti3Chr(args[1].GetStr(), expOpt);

	delete expOpt;

	if(!rst)
	{
		cb->Log(-1, "Failed to export (%s)", args[1].GetStr());
		return ECMD_PROC_RST_FAIL;
	}

	return ECMD_PROC_RST_OK;
}


// 0: workdir, 1:export i3chr path, 2:save i3s  3:save i3chr
TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_EXPORT_CHARA)
{
	CMainFrame* m = (CMainFrame*)cb->m_prm;

	COptionInfo* expOpt = g_pOption->Clone();

	strcpy_s(expOpt->m_szWorkPath, MAX_PATH, args[0].GetStr());

	expOpt->m_bExportI3S = args[2].GetInt() ? TRUE : FALSE;
	expOpt->m_bExportI3CHR = args[3].GetInt() ? TRUE : FALSE;

	expOpt->m_bRotateAxis = TRUE; // Chara´Â Axis µ¹¸²

	expOpt->m_bGenMipmap = TRUE;
	expOpt->m_bConvertDXT = TRUE;	expOpt->m_bDXTAlpha	= TRUE;
	expOpt->m_bExternTexture = TRUE;
 
	expOpt->m_bOptAnim = FALSE;		expOpt->m_bGetAnimPack = FALSE;

	expOpt->m_bGenC = TRUE;

	bool rst = m->Exporti3Chr(args[1].GetStr(), expOpt);

	delete expOpt;

	if(!rst)
	{
		cb->Log(-1, "Failed to export (%s)", args[1].GetStr());
		return ECMD_PROC_RST_FAIL;
	}
	else
	{
		cb->Log(0, "OK! - Exported - (%s)", args[1].GetStr());
	}

	return ECMD_PROC_RST_OK;
}


TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_DELETE_AIDRIVERCPP)
{
	HANDLE h; 
	WIN32_FIND_DATA fd;

	char pathonly[MAX_PATH] = {0};
	char path[MAX_PATH] = {0};

	strcpy_s(pathonly, MAX_PATH, args[0].GetStr());

	sprintf_s(path, "%s\\*.*", pathonly);
	h = FindFirstFile(path, &fd);

	i3::vector<i3::string> delAry;

	while(h != INVALID_HANDLE_VALUE)
	{
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
		{
			if(!strnicmp(fd.cFileName, "AIDriverChara", strlen("AIDriverChara")))
			{
				char* p = strrchr(fd.cFileName, '.');
				if(p)
				{
					if( !stricmp(p, ".cpp") || !stricmp(p, ".h") )
					{
						i3::string dp;
						i3::sprintf(dp, "%s\\%s", pathonly, fd.cFileName);
						delAry.push_back(dp);
					}
				}
			}
		}
		
		if(!FindNextFile(h, &fd))
		{
			FindClose(h);
			break;
		}
	}

	for(size_t k=0; k<delAry.size(); k++)
		::DeleteFile(delAry[k].c_str());

	return ECMD_PROC_RST_OK;
}


TOOLAM_CMDFN(ECMD_CHARTOOL_MF_I3CHR_SAVE)
{
	CMainFrame* m = (CMainFrame*)cb->m_prm;

	m->OnSaveChara(args[0].GetStr());

	return ECMD_PROC_RST_OK;
}

//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////