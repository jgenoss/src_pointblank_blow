
#include "stdafx.h"

#include "resource.h"

#include "i3TDKAgent.h"
#include "process.h"
#include "DxRenderContext.h"
#include "OptiExp_Alli3OptSceneOptimizers.h"

#include "OptiExpTask.h"
#include "../OptiExpPlugService.h"
#include "i3SceneUtil/i3SceneUtil.h"

#include "i3Base/string/ext/resolve_path_from_environment_variable.h"

//
// OptiExpTaskReporter
//

OptiExpTaskShareInfo::OptiExpTaskShareInfo()
{
	m_prevTask = NULL;
	m_lastTaskRst = false;
	m_sgInfo = NULL;
	m_expCallBack = NULL;
	m_ownerWnd = NULL;

	SetFilePathName(NULL, NULL);

	m_deferredLog = false;
}

OptiExpTaskShareInfo::~OptiExpTaskShareInfo()
{
	Reset();
}

void OptiExpTaskShareInfo::Reset()
{
	FlushLog();

	m_prevTask = NULL;
	m_lastTaskRst = false;
	
	if(m_sgInfo)
	{
		I3ASSERT(m_sgInfo->GetRefCount()==1);

		if(m_sgInfo->getInstanceSg())
		{			
			I3ASSERT(m_sgInfo->getInstanceSg()->GetRefCount()==1);
		}
	}

	I3_SAFE_RELEASE(m_sgInfo);
}

void OptiExpTaskShareInfo::GenerateSceneGraphInfoFrom(i3Node* node)
{
	I3_SAFE_RELEASE(m_sgInfo);

	m_sgInfo = i3SceneGraphInfo::new_object(); // ref+1
	m_sgInfo->setInstanceSg(node);
}

void OptiExpTaskShareInfo::Log(const char* fmt, ...)
{
	static char tempstr[1024];

	va_list marker;
	va_start(marker, fmt);

	strcpy(tempstr, "[OptiExp] ");
	char* st = tempstr;

	while(*st != NULL)
		st++;

	vsprintf(st, fmt, marker);
	va_end( marker);

	if(m_deferredLog)
		m_deferredLogVec.push_back(tempstr);
	else
		m_expCallBack->OEPC_Log(tempstr);
}

void OptiExpTaskShareInfo::SetDeferredLogging(bool deferred)
{
	FlushLog();
	m_deferredLog = deferred;
}

void OptiExpTaskShareInfo::FlushLog()
{
	if(GetExpCallBack())
	{
		for(i3::vector<i3::string>::size_type i=0; i<m_deferredLogVec.size(); i++)
			GetExpCallBack()->OEPC_Log((char*)m_deferredLogVec[i].c_str());
	}

	m_deferredLogVec.clear();
}

void OptiExpTaskShareInfo::SetFilePathName(const char* path, const char* name)
{
	if(path)
		strcpy(m_expFilePath, path);
	else
		m_expFilePath[0] = NULL;

	if(name)
		strcpy(m_expFileName, name);
	else
		m_expFileName[0] = NULL;
}

//
// OptiExpTaskProgressWnd
//

OptiExpProgressWnd::OptiExpProgressWnd(CallBack* callback, HWND parent, bool show)
{
	m_callback = callback;
	m_parent = parent;
	CreateDialogParam(OptiExpPlug_HInstance, MAKEINTRESOURCE(IDD_DLG_OPTIEXP_TASKPROGRESS),
		m_parent, OptiExpProgressWnd::DlgProc, (LPARAM)this);

	if(m_dlg && show)
	{
		ShowWindow(m_dlg, SW_SHOW);
		::SetFocus(m_dlg);
	}
}

OptiExpProgressWnd::~OptiExpProgressWnd()
{
	OnDestroyWnd();
}

void OptiExpProgressWnd::Show(bool show)
{
	if(m_dlg)
	{
		ShowWindow(m_dlg, SW_SHOW);
		::SetFocus(m_dlg);
	}
}

bool OptiExpProgressWnd::PumpMsg()
{
	if(!m_dlg)
		return false;

	bool exitWnd = false;
	MSG msg;
	while(PeekMessage(&msg, m_dlg, 0, 0, PM_NOREMOVE))
	{
		if(!GetMessage(&msg, m_dlg, 0, 0))
		{
			// exit
			exitWnd = true;
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return !exitWnd;
}

void OptiExpProgressWnd::SetText(ECTL item, const char* fmt, ...)
{
	char tempstr[2048];

	if(item < ECTL_ST_00 || item >= ECTL_ST_MAX)
		return;

	va_list marker;
	va_start(marker, fmt);

	vsprintf(tempstr, fmt, marker);
	va_end(marker);

	SetWindowText(m_ctl[item], tempstr);
}

void OptiExpProgressWnd::SetProgressInfo(INT32 minVal, INT32 maxVal)
{
	SendMessage(m_ctl[ECTL_PB_00], PBM_SETRANGE, 0, MAKELONG(minVal, maxVal));
}

void OptiExpProgressWnd::SetProgress(INT32 val)
{
	SendMessage(m_ctl[ECTL_PB_00], PBM_SETPOS, val, 0);
}

bool OptiExpProgressWnd::OnCancel()
{
	if(m_callback)
		return m_callback->OptiExpProgressWnd_OnCancel();

	return true;
}

BOOL CALLBACK OptiExpProgressWnd::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OptiExpProgressWnd* inst = (OptiExpProgressWnd*)GetWindowLongPtr(hWnd, DWL_USER);

	switch(msg) 
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr(hWnd, DWL_USER, (LONG_PTR)lParam);
			inst = (OptiExpProgressWnd*)lParam;
			inst->OnInitDialog(hWnd);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				{					
					if(inst)
					{
						inst->OnCancel();
						inst->OnDestroyWnd();
					}
					else
						DestroyWindow(hWnd);
				}
				return TRUE;

			default:
				return FALSE;
			}
		}
		break;

	case WM_DESTROY:
		{
			if(inst)
				inst->OnDestroyWnd();
		}
		break;
	}

	return FALSE;	
}

void OptiExpProgressWnd::OnInitDialog(HWND hWnd)
{
	m_dlg = hWnd;

	m_ctl[ECTL_ST_00] = GetDlgItem(m_dlg, IDC_OPTIEXPTP_ST_00);
	m_ctl[ECTL_ST_01] = GetDlgItem(m_dlg, IDC_OPTIEXPTP_ST_01);

	m_ctl[ECTL_PB_00] = GetDlgItem(m_dlg, IDC_OPTIEXPTP_PB_00);

	OptiExpPlug_CenterWindow(m_dlg, GetParent(m_dlg));
}

void OptiExpProgressWnd::OnDestroyWnd()
{
	if(m_dlg)
	{
		SetWindowLongPtr(m_dlg, DWL_USER, NULL);
		DestroyWindow(m_dlg);
		m_dlg = NULL;
	}
}

//
// OptiExpTaskProcessor
//

bool OptiExpTaskProcessor::DoWithStatus(OptiExpTaskShareInfo* taskShInfo, 
	OptiExpTaskVec& tasks, HWND parentWnd)
{
	bool errInTask = false;
	INT32 numTask = tasks.size();

	m_quitProcessing = false;

	OptiExpProgressWnd pw(this, parentWnd, true);
	pw.SetProgressInfo(0, numTask);
	pw.SetText(OptiExpProgressWnd::ECTL_ST_00, "outputfile : %s/%s", 
		taskShInfo->m_expFilePath, taskShInfo->m_expFileName);

	for(OptiExpTaskVec::size_type i=0; i<tasks.size(); i++)
	{
		OptiExpTask* task = tasks[i];

		pw.SetText(OptiExpProgressWnd::ECTL_ST_01, "[%02d/%02d] %s", i, numTask, task->GetDesc());
		pw.SetProgress(i);		
		pw.PumpMsg();

		if(!tasks[i]->Do(taskShInfo))
		{
			errInTask = true;
			break;
		}

		pw.PumpMsg();

		if(m_quitProcessing)
			break;
	}

	if(m_quitProcessing)
	{
		taskShInfo->Log("INFO! - Task operation was canceled by user ! ");
		// 취소되었음
		errInTask = true;
	}

	return !errInTask;
}

bool OptiExpTaskProcessor::OptiExpProgressWnd_OnCancel()
{
	m_quitProcessing = true;
	return true;
}

//
// OptiExpTask
//

OptiExpTask::OptiExpTask()
{
	m_taskShInfo = NULL;
	m_worker = NULL;
	m_enable = true;
}

OptiExpTask::~OptiExpTask()
{
	I3_SAFE_RELEASE(m_worker);
}

bool OptiExpTask::Do(OptiExpTaskShareInfo* taskShInfo)
{
	bool rst = true;

	m_taskShInfo = taskShInfo;

	if(GetEnable())
		rst = OnWork();

	if(m_taskShInfo)
	{
		m_taskShInfo->m_prevTask = this;
		m_taskShInfo->m_lastTaskRst = rst;
	}

	m_taskShInfo = NULL;

	return rst;
}

bool OptiExpTask::ShowSetupBox(HWND hWndParent, OptiExpTaskShareInfo* taskShInfo)
{
	if(!i3TDK_SceneOptimizer_DoModalOption(GetSceneOptimizer()))
		return false;

	return true;
}

bool OptiExpTask::OnWork()
{
	return true;
}


//
//	OptiExpTaskDx9RenCtxContainer
//

OptiExpTaskDx9RenCtxContainer::OptiExpTaskDx9RenCtxContainer()
{
	OnReset();
}

OptiExpTaskDx9RenCtxContainer::~OptiExpTaskDx9RenCtxContainer()
{
	for(OptiExpTaskVec::size_type i=0; i<m_subTask.size(); i++)
		delete m_subTask[i];

	m_subTask.clear();
}

const char* OptiExpTaskDx9RenCtxContainer::GetDesc()
{
	return "i3DRenderContext Container for Dx9Device";
}

void OptiExpTaskDx9RenCtxContainer::OnReset()
{
	m_subTaskOperCanceled = false;
	OnSetupDefaultParam();
}

OptiExpTaskVec::size_type OptiExpTaskDx9RenCtxContainer::GetNumSubTask()
{
	return m_subTask.size();
}

OptiExpTask* OptiExpTaskDx9RenCtxContainer::GetSubTask(OptiExpTaskVec::size_type index)
{
	if(index >= m_subTask.size())
		return NULL;

	return m_subTask[index];
}

void OptiExpTaskDx9RenCtxContainer::AddSubTask(OptiExpTask* task)
{
	if(!task)
		return;

	m_subTask.push_back(task);
}

void OptiExpTaskDx9RenCtxContainer::OnSetupDefaultParam()
{
}

bool OptiExpTaskDx9RenCtxContainer::DoOperationInDxRenContext()
{
	// DxRenderContext 를 다른 Thread에서 생성해서 사용, Dx9Device를 맥스와 같은스레드에서 생성할경우
	// 맥스의 리본등 툴팁이나오는곳에 마우스오버하면 TimeSpan period must be less than or equal to Int32.MaxValue.
	// 오류가발생한다. 이런관계로 따로 쓰레드생성하여 사용한다.
	HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, RenCtxCntrThread, (void*)this, 0, 0);

	if(threadHandle == INVALID_HANDLE_VALUE)
	{
		GetTaskShareInfo()->Log("[OptiExpTaskDx9RenCtxContainer] Err! - failed to create thread..");
		return false;
	}

	bool workRst = false;	
	bool subTaskEnd = false;
	m_subTaskOperCanceled = false;

	do 
	{
		DWORD thRst = WaitForSingleObject(threadHandle, 0);

		switch(thRst)
		{
		case WAIT_TIMEOUT :
			{
				Sleep(1);
				// continue;
			}
			break;

		case WAIT_OBJECT_0:
			{
				DWORD threadExitCode = -1;
				GetExitCodeThread(threadHandle, &threadExitCode);

				if(threadExitCode == 1)
					workRst = true;

				subTaskEnd = true;
			}
			break;

		case WAIT_FAILED:
			{
				GetTaskShareInfo()->Log("[OptiExpTaskDx9RenCtxContainer] Err! - thread err(wait_fail)..");
				subTaskEnd = true;

				return false;
			}
		default:
			break;
		}

	} while(!subTaskEnd);

	if(m_subTaskOperCanceled)
	{
		GetTaskShareInfo()->Log("[OptiExpTaskDx9RenCtxContainer] Warn! - Subtask operation canceled..");
		return false;
	}

	return workRst;
}

bool OptiExpTaskDx9RenCtxContainer::OnWork()
{
	OptiExpPlugCallBack* expCB = GetTaskShareInfo()->GetExpCallBack();

	GetTaskShareInfo()->Log("[OptiExpTaskDx9RenCtxContainer] starting..");

/*   2013.9.4 일 기준
	i3Export2의 I3_EXPORT_EXP int LibInitialize(void) 함수에보면
	i3TextureMem::static_meta()->set_class_meta_style_concrete(i3Texture::static_meta());
	i3TextureCubeMem::static_meta()->set_class_meta_style_concrete(i3TextureCube::static_meta());
	i3VertexArrayMem::static_meta()->set_class_meta_style_concrete(i3VertexArray::static_meta());
	i3IndexArrayMem::static_meta()->set_class_meta_style_concrete(i3IndexArray::static_meta());

    이존재함 , DXResource 생성 없이 처리하기 위한것 같음, 이로인해 DX Device사용을해야하는
    옵티마이저쪽에서 문제가있어서 사용시에만 위의클래스를 DxResource를 사용하는 클래스로 되돌리는 코드 삽입
*/
	// I3TextureDX계열로 전환
	GetTaskShareInfo()->GetExpCallBack()->OEPC_Changei3DxResourceClassMode(false);

	bool workRst = false;
	GetTaskShareInfo()->SetDeferredLogging(true);
	workRst = DoOperationInDxRenContext();
	GetTaskShareInfo()->SetDeferredLogging(false);

	// i3TextureMem계열로 복귀
	GetTaskShareInfo()->GetExpCallBack()->OEPC_Changei3DxResourceClassMode(true);

	GetTaskShareInfo()->Log("[OptiExpTaskDx9RenCtxContainer] finished..");

	return workRst;
}

unsigned __stdcall OptiExpTaskDx9RenCtxContainer::RenCtxCntrThread(void* param)
{
	OptiExpTaskDx9RenCtxContainer* owner = (OptiExpTaskDx9RenCtxContainer*)param;

	DxRenderContext* renCtx = new DxRenderContext;

	if(renCtx->GetErr())
	{
		delete renCtx;
		return 0;
	}

	OptiExpTaskShareInfo* tsi = owner->GetTaskShareInfo();

	const char* findShaderName = NULL;

#if defined( I3_DEBUG)
	findShaderName = "i3SuperShader.hlsl";
#else
	findShaderName = "DefaultSHGL";	
#endif

	tsi->Log("[OptiExpTaskDx9RenCtxContainer] listing search path of shader [%s]..", findShaderName);
	bool shaderPathErr = true;

	for(INT32 i=0; i<i3ShaderCache::GetShaderSearchPathCount(); i++)
	{
		i3::stack_string shaderPath;
		i3::resolve_path_from_environment_variable(i3ShaderCache::GetShaderSearchPath(i), shaderPath);

		char out[MAX_PATH];
		sprintf_s(out, MAX_PATH, "[ShaderLookupPath %d] %s ", i, shaderPath.c_str());
		tsi->Log(out);

		if(shaderPathErr)
		{
			char findPath[MAX_PATH];
			sprintf_s(findPath, MAX_PATH, "%s/%s", shaderPath.c_str(), findShaderName);

			if(i3FileStream::isFile(findPath))
			{
				tsi->Log("[OptiExpTaskDx9RenCtxContainer] found shader[%s]", findPath);
				shaderPathErr = false;
				break;
			}
		}
	}

	if(shaderPathErr)
		tsi->Log("[OptiExpTaskDx9RenCtxContainer] ERR! - Failed searching shader path..");

	// cancel callback 처리들어가야함
	OptiExpTaskProcessor taskProcessor;
	taskProcessor.DoWithStatus(tsi, *(owner->GetSubTasks()), NULL);

	if(taskProcessor.IsQuitProcessing())
		owner->m_subTaskOperCanceled = true;

	delete renCtx;

	return 1;
}

//
//	OptiExpTaskI3Exporter
//

OptiExpTaskI3Exporter::OptiExpTaskI3Exporter()
{
	m_option = new i3tool::exporter::pack::OPTION_INFO;
	OnReset();
}

OptiExpTaskI3Exporter::~OptiExpTaskI3Exporter()
{	
	if(m_option)
		delete m_option;
}

bool OptiExpTaskI3Exporter::ShowSetupBox(HWND hWndParent, 
	OptiExpTaskShareInfo* taskShInfo)
{
	i3tool::exporter::pack::OPTION_INFO oldExporterOption;
	OptiExpPlugCallBack* expCB = taskShInfo->GetExpCallBack();
	
	// save previous
	I3EXP_OPTION_INFO_Copy(&oldExporterOption, expCB->OEPC_GetExporterOpt());
	I3EXP_OPTION_INFO_Copy(expCB->OEPC_GetExporterOpt(), GetExporterOption());

	if(expCB->OEPC_DoExporterOptDialog())
	{
		I3EXP_OPTION_INFO_Copy(GetExporterOption(), expCB->OEPC_GetExporterOpt());
	}

	// restore	
	I3EXP_OPTION_INFO_Copy(expCB->OEPC_GetExporterOpt(), &oldExporterOption);

	return true;
}

const char* OptiExpTaskI3Exporter::GetDesc()
{
	return "i3 Scene Exporter Setup";
}

void OptiExpTaskI3Exporter::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = NULL;

	OnSetupDefaultParam();
}

void OptiExpTaskI3Exporter::OnSetupDefaultParam()
{
	I3EXP_OPTION_INFO_SetDefault(m_option);

	// basic	
	m_option->m_bExportI3S = false; // OptiExpTaskSaveI3S에 위임
	m_option->m_bRotateAxis = false;
	m_option->m_bExportSelectedOnly = false;
	m_option->m_bReportError = true;
	m_option->m_bTreatControlAsBone = false;
	m_option->m_bUseMaterialName = false;

	// object type
	m_option->m_bExportMesh = false;
	m_option->m_bExportLight = false;
	m_option->m_bExportCamera = false;

	m_option->m_bViewAfterExport = false;

	// exception
	memset(m_option->m_szExcludeNodeName, 0, sizeof(m_option->m_szExcludeNodeName));

	// animation
	m_option->m_bExportAnim = false;
	m_option->m_bForceExportAnim = false;
	m_option->m_KeySampleType = i3tool::exporter::KEYSAMPLE_SAMPLE;
	m_option->m_SamplingInterval = 10;

	// physics
	m_option->m_bExportPhysicsGeometry = false;
	m_option->m_bExportShapeAsConvex = false;
	m_option->m_bCreateRappingConvex = false;
	m_option->m_bCreateForSimulate = false;

	// skinwidth
	m_option->m_SkinWidth = 0.01f;
}

bool OptiExpTaskI3Exporter::OnWork()
{
	OptiExpPlugCallBack* expCB = GetTaskShareInfo()->GetExpCallBack();

	GetTaskShareInfo()->Reset();
	GetTaskShareInfo()->Log("[OptiExpTaskI3Exporter] starting..");

	if(!strlen(GetTaskShareInfo()->m_expFileName))
	{
		GetTaskShareInfo()->Log("[OptiExpTaskI3Exporter] ERR! - Invalid filepath..null! ");
		return false;
	}

	strcpy(m_option->m_szLastPath, GetTaskShareInfo()->m_expFilePath);
	strcpy(m_option->m_szLastName, GetTaskShareInfo()->m_expFileName);

	// preserve previous exporter option
	i3tool::exporter::pack::OPTION_INFO oldExporterOption;
	I3EXP_OPTION_INFO_Copy(&oldExporterOption, expCB->OEPC_GetExporterOpt());

	bool expRst = false;
	i3Node* rstExpNode = NULL;

	I3EXP_OPTION_INFO_Copy(expCB->OEPC_GetExporterOpt(), m_option); // apply

	expRst = expCB->OEPC_DoExport(m_option->m_szLastPath, m_option->m_szLastName, &rstExpNode);
	I3EXP_OPTION_INFO_Copy(expCB->OEPC_GetExporterOpt(), &oldExporterOption); // restore

	if(!expRst || !rstExpNode) 
	{
		GetTaskShareInfo()->Log("[OptiExpTaskI3Exporter] ERR! - Failed to export ..");
		return false;
	}

	if(m_option->m_bExportI3S)
	{
		GetTaskShareInfo()->Log("[OptiExpTaskI3Exporter] i3s file is writed [%s/%s]..",
			m_option->m_szLastPath, m_option->m_szLastName);
	}

	I3ASSERT(rstExpNode->GetRefCount() == 1);
	GetTaskShareInfo()->GenerateSceneGraphInfoFrom(rstExpNode);

	I3ASSERT(rstExpNode->GetRefCount() == 2);
	rstExpNode->Release();

	GetTaskShareInfo()->Log("[OptiExpTaskI3Exporter] Exporting is successfully completed..");

	return true;
}

void OptiExpTaskI3Exporter::ApplyAnimBoneListOpt(OptiExpAnimBoneListManager* boneList)
{
	
}


//
// OptiExpTaskMipmapGen
//

const char* OptiExpTaskMipmapGen::GetDesc()
{
	return "generate texture Mipmap";
}

void OptiExpTaskMipmapGen::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptMipmapGen::static_meta()->create_instance();	
	m_worker->AddRef();

	OnSetupDefaultParam();
}
 
void OptiExpTaskMipmapGen::OnSetupDefaultParam()
{
	i3OptMipmapGen* mipmapGen = (i3OptMipmapGen*)GetWorker();

	mipmapGen->setLimitSizeState(true);
	mipmapGen->setMinWidth(32);
	mipmapGen->setMinHeight(32);

	mipmapGen->setLimitLevelState(true);
	mipmapGen->setLimitMaxLevel(4);

	mipmapGen->setFilter(i3OptMipmapGen::FILTER_D3DX_TRIANGLE);

	mipmapGen->setDitherState(false);
	mipmapGen->setSRGBInState(false);
	mipmapGen->setSRGBOutState(false);

	mipmapGen->setMirrorUState(true);
	mipmapGen->setMirrorVState(true);
	mipmapGen->setMirrorWState(true);
}

bool OptiExpTaskMipmapGen::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskMipmapGen] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);

	return true;
}


//
// OptiExpTaskCvtTextureToDDS
//

const char* OptiExpTaskCvtTextureToDDS::GetDesc()
{
	return "i3OptConvertDXTexture Convert all textures in scene  to DDS texture..";
}

void OptiExpTaskCvtTextureToDDS::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptConvertDXTTexture::static_meta()->create_instance();
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskCvtTextureToDDS::OnSetupDefaultParam()
{
	i3OptConvertDXTTexture* optCvtDxt = (i3OptConvertDXTTexture*)GetSceneOptimizer();

	optCvtDxt->setTargetImageFormat(I3G_IMAGE_FORMAT_DXT1);
	optCvtDxt->setQuality(i3OptConvertDXTTexture::QUALITY_HIGHEST);
	optCvtDxt->setBinaryAlphaEnable(false);
	optCvtDxt->setAlphaRef(127);
	optCvtDxt->setDitherEnable(true);
}

bool OptiExpTaskCvtTextureToDDS::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskCvtTextureToDDS] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);

	return true;
}


//
// OptiExpTaskNodeOpti
//

const char* OptiExpTaskNodeOpti::GetDesc()
{
	return "i3OptNodeFlag Optimize node, AddMask - Static, Disable Shadow Casting / Remove mask : Disable Shadow receiving..";
}

void OptiExpTaskNodeOpti::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptNodeFlag::static_meta()->create_instance();	
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskNodeOpti::OnSetupDefaultParam()
{
	i3OptNodeFlag* optNodeFlag = (i3OptNodeFlag*)GetSceneOptimizer();

	// flags are defined at i3node.h
	optNodeFlag->setAddFlag(I3_NODEFLAG_STATIC | I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	optNodeFlag->setRemoveFlag(I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
}

bool OptiExpTaskNodeOpti::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskNodeOpti] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);
	return true;
}


//
// OptiExpTaskCreateBoundBox
//

const char* OptiExpTaskCreateBoundBox::GetDesc()
{
//	return GetWorker()->getDesc();
	return "i3OptCreateBoundBox";
}

void OptiExpTaskCreateBoundBox::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptCreateBoundBox::static_meta()->create_instance();	
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskCreateBoundBox::OnSetupDefaultParam()
{
	// no options to set
}

bool OptiExpTaskCreateBoundBox::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskCreateBoundBox] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);
	return true;
}


//
// OptiExpTaskRemoveVtxComponent
//

const char* OptiExpTaskRemoveVtxComponent::GetDesc()
{
	return "i3OptRemoveVertexComponent";
}

void OptiExpTaskRemoveVtxComponent::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptRemoveVertexComponent::static_meta()->create_instance();	
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskRemoveVtxComponent::OnSetupDefaultParam()
{
	i3OptRemoveVertexComponent* optRVC 
		= (i3OptRemoveVertexComponent*)GetSceneOptimizer();	

	optRVC->SetRemoveColor(TRUE);
	optRVC->SetRemoveNormal(FALSE);
	optRVC->SetRemoveTexcoord(FALSE);
	optRVC->SetRemoveTangent(FALSE);
}

bool OptiExpTaskRemoveVtxComponent::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskRemoveVtxComponent] starting..");
	
	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);
	return true;
}

//
// OptiExpTaskRemoveVtxComponent_occluder
// 


const char* OptiExpTaskRemoveVtxComponent_occluder::GetDesc()
{
	return "i3OptRemoveVertexComponent_occluder";
}

void OptiExpTaskRemoveVtxComponent_occluder::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptRemoveVertexComponent::static_meta()->create_instance();
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskRemoveVtxComponent_occluder::OnSetupDefaultParam()
{
	i3OptRemoveVertexComponent* optRVC
		= (i3OptRemoveVertexComponent*)GetSceneOptimizer();

	optRVC->SetRemoveColor(TRUE);
	optRVC->SetRemoveNormal(FALSE);
	optRVC->SetRemoveAllTexcoord(TRUE);
	optRVC->SetRemoveTangent(TRUE);
}

bool OptiExpTaskRemoveVtxComponent_occluder::OnWork()
{
	GetTaskShareInfo()->Log("[i3OptRemoveVertexComponent_occluder] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);
	return true;
}

//
// OptiExpTaskRemoveTextureBindAttr_occluder
//


const char* OptiExpTaskRemoveAttr_occluder::GetDesc()
{
	return "OptiExpTaskRemoveAttr_occluder";
}

void OptiExpTaskRemoveAttr_occluder::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptRemoveAttribute::static_meta()->create_instance();
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskRemoveAttr_occluder::OnSetupDefaultParam()
{
	i3OptRemoveAttribute* opt = (i3OptRemoveAttribute*)GetSceneOptimizer();
	opt->setClassMeta(i3RenderAttr::static_meta());
}

bool OptiExpTaskRemoveAttr_occluder::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskRemoveAttr_occluder] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());

	GetSceneOptimizer()->setSg(NULL);
	return true;
}



//
// OptiExpTaskPhsXSetBeforeExport
//
OptiExpTaskPhsXSetBeforeExport::OptiExpTaskPhsXSetBeforeExport()
{
	m_dynTypeToStr.push_back("-------");
	m_dynTypeToStr.push_back("dynamic");
	m_dynTypeToStr.push_back("kinematic");
	m_dynTypeToStr.push_back("static");
	m_dynTypeToStr.push_back("(none)");

	m_excludeNode[0] = NULL;
	m_includeNode[0] = NULL;
	m_includeOnly = false;
	OnReset();
}

OptiExpTaskPhsXSetBeforeExport::~OptiExpTaskPhsXSetBeforeExport()
{
}

void OptiExpTaskPhsXSetBeforeExport::SetExcludeNode(const char* excludeNode)
{
	if(excludeNode)
		strcpy_s(m_excludeNode, 1024, excludeNode);
}

void OptiExpTaskPhsXSetBeforeExport::SetIncludeNode(const char* includeNode)
{
	if(includeNode)
		strcpy_s(m_includeNode, 1024, includeNode);
}

void OptiExpTaskPhsXSetBeforeExport::SetIncludeNodeOnly(bool includeOnly)
{
	m_includeOnly = includeOnly;
}

bool OptiExpTaskPhsXSetBeforeExport::ShowSetupBox(HWND hWndParent, 
	OptiExpTaskShareInfo* taskShInfo)
{
	BOOL rst = OPTIEXPTASK_SETUP_DLG_DOMODAL(OptiExpTaskPhsXSetBeforeExport, hWndParent);
	return rst ? true : false;
} 
 
const char* OptiExpTaskPhsXSetBeforeExport::GetDesc()
{
	return "OptiExpTaskPhsXSetBeforeExport";
}

void OptiExpTaskPhsXSetBeforeExport::OnReset()
{
	OnSetupDefaultParam();
}

void OptiExpTaskPhsXSetBeforeExport::OnSetupDefaultParam()
{
	m_dynType = EPHY_DYT_NONE;
}

bool OptiExpTaskPhsXSetBeforeExport::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskPhsXSetBeforeExport] setup all-node.. \
							physX-Opt=%s", GetDynTypeStr(m_dynType));
	
	INT32 numSetted = GetTaskShareInfo()->GetExpCallBack()->OEPC_SetPhysXSettingToNode(
		m_dynType, m_includeOnly, m_includeNode, m_excludeNode);

	GetTaskShareInfo()->Log("[OptiExpTaskPhsXSetBeforeExport] count of setted = %d.. \
							physX-Opt=%s", numSetted, GetDynTypeStr(m_dynType));
	
	return true;
}

const char* OptiExpTaskPhsXSetBeforeExport::GetDynTypeStr(
	EPHYSX_DYNAMIC_TYPE type)
{
	return m_dynTypeToStr[type].c_str();
}

OPTIEXPTASK_SETUP_DLG_DEF_IMPL(OptiExpTaskPhsXSetBeforeExport)

void OptiExpTaskPhsXSetBeforeExport::OnSetupDlgInit(HWND hWnd)
{
	INT32 top = 10, left = 10;
	INT32 curX=left, curY = top;
	HINSTANCE hInst = OptiExpPlug_HInstance;

	m_setupDlg = hWnd;

	HWND desc = CreateWindow("static", "PhysX DynType", WS_CHILD | WS_VISIBLE,
		curX, curY, 100, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX += 110;

	m_cbDyn = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		curX, curY, 100, 20, hWnd, (HMENU)0x0000fa00, hInst, NULL);

	for(INT32 i=0; i<EPHY_DYT_MAX; i++)
		SendMessage(m_cbDyn, CB_ADDSTRING, 0, (LPARAM)m_dynTypeToStr[i].c_str());

	SendMessage(m_cbDyn, CB_SETCURSEL, m_dynType, 0);

	curX = left;
	curY += 30;
	CreateWindow("static", "Include Node:", WS_CHILD | WS_VISIBLE,
		curX, curY, 100, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX = left + 110;
	m_ebInclude = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
		curX, curY, 400, 20, hWnd, 0, hInst, NULL);
	SetWindowText(m_ebInclude, m_includeNode);

	curY += 30;
	m_ckIncludeOnly = CreateWindow("button", "process include node only", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
		curX, curY,	400, 20, hWnd, (HMENU)0x0, hInst, NULL);

	Opti_Button_SetCheck(m_ckIncludeOnly, m_includeOnly);

	curX = left;
	curY += 30;
	CreateWindow("static", "Exclude Node:", WS_CHILD | WS_VISIBLE,
		curX, curY, 100, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX = left + 110;
	m_ebExclude = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
		curX, curY, 400, 20, hWnd, 0, hInst, NULL);
	SetWindowText(m_ebExclude, m_excludeNode);

	curX = left + 120;
	curY += 30;
	CreateWindow("static", "ex:) Scene Root;AAANode;BBBNode", WS_CHILD | WS_VISIBLE,
		curX, curY, 400, 20, hWnd, (HMENU)-1, hInst, NULL);
	curY += 30;
	CreateWindow("static", "Notice! : 'Scene Root' is root of 3dsmax scene", WS_CHILD | WS_VISIBLE,
		curX, curY, 400, 20, hWnd, (HMENU)-1, hInst, NULL);
}

BOOL OptiExpTaskPhsXSetBeforeExport::OnSetupDlgCmd(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD evt = HIWORD(wParam);

	if(id == IDOK || id == IDCANCEL)
	{
		if(id == IDOK)
		{
			m_dynType = (EPHYSX_DYNAMIC_TYPE)SendMessage(m_cbDyn, CB_GETCURSEL, 0, 0);

			if(m_dynType < 1 || m_dynType > EPHY_DYT_MAX)
				m_dynType = EPHY_DYT_NONE;

			GetWindowText(m_ebExclude, m_excludeNode, 1024);
			GetWindowText(m_ebInclude, m_includeNode, 1024);
			m_includeOnly = Opti_Button_GetCheck(m_ckIncludeOnly) ? true : false;
		}

		EndDialog(m_setupDlg, id == IDOK ? 1 : 0);
		return TRUE;
	}

	return FALSE;
}

//
// OptiExpTaskSaveI3S
//

const char* OptiExpTaskSaveI3S::GetDesc()
{
	return "OptiExpTaskSaveI3S";
}

void OptiExpTaskSaveI3S::OnReset()
{
	OnSetupDefaultParam();
}

void OptiExpTaskSaveI3S::OnSetupDefaultParam()
{
}

bool OptiExpTaskSaveI3S::OnWork()
{
	OptiExpTaskShareInfo* tsh = GetTaskShareInfo();

	if(!tsh->GetExpSceneGraph())
	{
		tsh->Log("[OptiExpTaskSaveI3S] ERR! - writing is failed, invalid export scene..");
		return false;
	}

	tsh->Log("[OptiExpTaskSaveI3S] start writing i3s file.. = [%s/%s].. ", 
		tsh->m_expFilePath, tsh->m_expFileName);

	{
		char path[MAX_PATH] = {0};
		sprintf(path, "%s\\%s", tsh->m_expFilePath, tsh->m_expFileName);

		i3SceneFile i3sFile;
		i3sFile.SetSceneGraph(tsh->GetExpSceneGraph()->getInstanceSg());
	
		if(i3sFile.Save(path) == STREAM_ERR)
		{
			tsh->Log("[OptiExpTaskSaveI3S] ERR! - writing i3s file is failed..");
			return false;
		}
	}	

	tsh->Log("[OptiExpTaskSaveI3S] writing i3s file is succeed..", 
		tsh->m_expFilePath, tsh->m_expFileName);
	
	return true;
}

//
//  OptiExpTaskSetAniTimeRange
//

OptiExpTaskSetAniTimeRange::OptiExpTaskSetAniTimeRange(INT32 startTime, INT32 endTime)
{
	m_action = 0;
	m_restoreTo = NULL;

	OnReset();

	m_startTime = startTime;
	m_endTime = endTime;
}

OptiExpTaskSetAniTimeRange::OptiExpTaskSetAniTimeRange(OptiExpTaskSetAniTimeRange* restoreTo)
{
	m_action = 1;
	m_restoreTo = restoreTo;

	OnReset();
}

OptiExpTaskSetAniTimeRange::~OptiExpTaskSetAniTimeRange()
{
}
 
const char* OptiExpTaskSetAniTimeRange::GetDesc()
{
	return "OptiExpTaskSetAniTimeRange";
}

void OptiExpTaskSetAniTimeRange::OnReset()
{
	OnSetupDefaultParam();
}

void OptiExpTaskSetAniTimeRange::OnSetupDefaultParam()
{
	m_startTime = -1;
	m_endTime = 0;

	m_prevStartTime = -1;
	m_prevEndTime = 0;
}

bool OptiExpTaskSetAniTimeRange::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskSetAniTimeRange] executing..");

	OptiExpPlugCallBack* cb = GetTaskShareInfo()->GetExpCallBack();

	if(m_action == 0)
	{
		// set
		GetTaskShareInfo()->Log("[OptiExpTaskSetAniTimeRange] set " \
			" anitime [%d],[%d]..", m_startTime, m_endTime);

		cb->OEPC_GetAniTimeRange(m_prevStartTime, m_prevEndTime); //save previous
		cb->OEPC_SetAniTimeRange(m_startTime, m_endTime);
	}
	else if(m_action == 1)
	{
		// restore
		GetTaskShareInfo()->Log("[OptiExpTaskSetAniTimeRange] restore " \
			"to previus anitime [%d],[%d]..", m_prevStartTime, m_prevEndTime);

		if(m_restoreTo)
		{
			cb->OEPC_SetAniTimeRange(
				m_restoreTo->m_prevStartTime, m_restoreTo->m_prevEndTime);
		}
	}
	
	return true;
}

//
// OptiExpTaskSetupThickness
//

struct ThicknessRow
{
	char			m_name[64];
	DWORD			m_i3TerType;
	INT32			m_thickness;
	INT32			m_thickness_ori;
};

static ThicknessRow					gs_thickness[] =
{
	{	"TH_MET_",			I3_TERRAIN_STEEL_THIN		, 90 , 90	},
	{	"TH_MARB_",			I3_TERRAIN_MARBLE_THIN		, 90 , 90	},

	{	"TH_WOOD_",			I3_TERRAIN_WOOD_THIN		, 50 , 50	},
	{	"TH_CCR_",			I3_TERRAIN_CONCRETE_THIN	, 50 , 50	},
	{	"TH_GND_",			I3_TERRAIN_GROUND_THIN		, 50 , 50	},
	{	"TH_RUB_",			I3_TERRAIN_GROUND_THIN		, 50 , 50	},

	{	"TH_BARBEDWIRE_",	I3_TERRAIN_BARBEDWIRE		, 10 , 10	},
	{	"TH_CLOTH_",		I3_TERRAIN_CLOTH_THIN		, 10 , 10	},
	{	"TH_PAP_",			I3_TERRAIN_PAPER_THIN		, 10 , 10	}

/*	{	"TH_SNOW_",			I3_TERRAIN_SNOW_THIN		,		},
	{	"TH_CLAY_",			I3_TERRAIN_WET_CLAY_THIN	,		},
	{	"TH_GRASS_",		I3_TERRAIN_GRASS_THIN		,		},
	{	"TH_LEAF_",			I3_TERRAIN_FALLLEAF_THIN	,		},
	{	"TH_GLASS_",		I3_TERRAIN_GLASS_THIN		,		},
	{	"TH_PLASTIC_",		I3_TERRAIN_PLASTIC_THIN		,		}, */
};

INT32 gs_numThicknessTable 	= sizeof(gs_thickness) / sizeof(gs_thickness[0]);

bool LookupThickness(const char* prefix, INT32* rstThickness)
{
	if(!prefix)
		return false;

	for(INT32 i=0; i<gs_numThicknessTable; i++)
	{
		ThicknessRow& row = gs_thickness[i];

		INT32 numLen = strlen(row.m_name);
		if(numLen == 0)
			break;

		if(!strnicmp(row.m_name, prefix, numLen))
		{
			if(rstThickness)
				*rstThickness = row.m_thickness;

			return true;
		}
	}

	return false;
}

bool OptiExpTaskSetupThickness::ShowSetupBox(HWND hWndParent, OptiExpTaskShareInfo* taskShInfo)
{
	BOOL rst = OPTIEXPTASK_SETUP_DLG_DOMODAL(OptiExpTaskSetupThickness, hWndParent);
	return rst ? true : false;
} 

const char* OptiExpTaskSetupThickness::GetDesc()
{
	return "OptiExpTaskSetupThickness";
}

void OptiExpTaskSetupThickness::OnReset()
{
	DWORD siz = sizeof(ThicknessRow);

	OnSetupDefaultParam();
}

void OptiExpTaskSetupThickness::OnSetupDefaultParam()
{
	m_verboseLog = false;
	m_numThicknessSet = 0;
}

bool OptiExpTaskSetupThickness::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskSetupThickness] executing..");

	i3Node* sceneRoot = GetTaskShareInfo()->GetExpSceneGraph()->getInstanceSg();

	if(!sceneRoot)
	{
		GetTaskShareInfo()->Log("[OptiExpTaskSetupThickness] Warn! - scene root is empty.. skip operation..");
		return true;
	}

	m_numThicknessSet = 0;
	RecursiveSetThickness(sceneRoot);

	GetTaskShareInfo()->Log("[OptiExpTaskSetupThickness] Total %d thickness of item is updated..", m_numThicknessSet);

	return true;
}

void OptiExpTaskSetupThickness::RecursiveSetThickness(i3Node* cur)
{
	if(!cur)
		return;

	if(i3::kind_of<i3Transform2*>(cur))
	{
		i3Transform2* t2 = (i3Transform2*)cur;

		INT32 thickness = 0;
		if(LookupThickness(t2->GetName(), &thickness))
		{
			i3PhysixShapeSet* psSet = t2->getShapeSet();
			if(psSet)
			{
				INT32 numTerCnt = psSet->getTerrainCount();

				if(psSet->getTerrainCount() != psSet->getNxShapeCount())
					psSet->PrepareTerrainArrayForConvex();

				// shape set마다 조정할수가없어 일괄적으로 세팅
				for(INT32 iTer=0 ; iTer<numTerCnt; iTer++)
				{
					psSet->getTerrain(iTer)->m_Thickness = thickness;
					m_numThicknessSet++;

					if(m_verboseLog)
					{
						GetTaskShareInfo()->Log("[OptiExpTaskSetupThickness] " \
							"[%s][Ter=%02d], set thickness[%03d]..", t2->GetName(), iTer, thickness);
					}
				}
			}
		}
	}

	INT32 numChild = cur->GetChildCount();
	for(INT32 i=0; i<numChild; i++)
	{
		i3Node* child = cur->GetChild(i);
		RecursiveSetThickness(child);
	}
}


OPTIEXPTASK_SETUP_DLG_DEF_IMPL(OptiExpTaskSetupThickness)

void OptiExpTaskSetupThickness::OnSetupDlgInit(HWND hWnd)
{
	INT32 top = 10, left = 10;
	INT32 curX=left, curY = top;
	HINSTANCE hInst = OptiExpPlug_HInstance;

	m_setupDlg = hWnd;

	HWND desc = CreateWindow("static", "Show Thickness Value Help", WS_CHILD | WS_VISIBLE,
		curX, curY, 170, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX += 180;
	CreateWindow("button", "Help", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, curX, curY,
		70, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX = left;
	curY += 30;

	for(INT32 i=0; i<gs_numThicknessTable; i++)
	{
		ThicknessRow& row = gs_thickness[i];

		curX = left;
		CreateWindow("static", row.m_name, WS_CHILD | WS_VISIBLE, curX, curY, 
			150, 20, hWnd, (HMENU)-1, hInst, NULL);
		
		curX += 160;
		HWND ebThickness = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, curX, curY,
			50, 20, hWnd, (HMENU)0x0000fa01, hInst, NULL);
		SetWindowLongPtr(ebThickness, GWL_USERDATA, (LONG_PTR)i);
		char szThickVal[MAX_PATH] = {0};
		itoa(row.m_thickness, szThickVal, 10);

		SetWindowText(ebThickness, szThickVal);

		curX += 60;
		HWND btMod = CreateWindow("button", "Modify", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, curX, curY,
			70, 20, hWnd, (HMENU)0x0000fa02, hInst, NULL);
		SetWindowLongPtr(btMod, GWL_USERDATA, (LONG_PTR)ebThickness);

		curX += 80;
		HWND btReset = CreateWindow("button", "Reset", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, curX, curY,
			70, 20, hWnd, (HMENU)0x0000fa03 , hInst, NULL);
		SetWindowLongPtr(btReset, GWL_USERDATA, (LONG_PTR)ebThickness);
		
		curY += 30;
	}

	curX = left;
	curY += 50;
	
	CreateWindow("button", "Verbose Log", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, curX, curY,
		120, 20, hWnd, (HMENU)0x0000fc00, hInst, NULL);

	//INT32 sizW = big
	//INT32 sizH = curY + 20;

	//SetWindowPos(hWnd, NULL, 0, 0, SizW, SizH, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);
}

BOOL OptiExpTaskSetupThickness::OnSetupDlgCmd(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD evt = HIWORD(wParam);


	if( (id & 0xfffffa00) == 0x0000fa00)
	{
		switch(id)
		{
		case 0x0000fa02:
		case 0x0000fa03:
			{ // modify
				HWND edBox = (HWND)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
				INT32 index = (INT32)GetWindowLongPtr((HWND)edBox, GWL_USERDATA);

				if(index > -1 || index < gs_numThicknessTable)
				{
					if(id == 0x0000fa02) // modify
					{
						char szVal[MAX_PATH] = {0};
						GetWindowText(edBox, szVal, MAX_PATH);
						gs_thickness[index].m_thickness = atoi(szVal);
					}
					else
					{
						gs_thickness[index].m_thickness = gs_thickness[index].m_thickness_ori;

						char szVal[MAX_PATH]={0};
						itoa(gs_thickness[index].m_thickness, szVal, 10);
						SetWindowText(edBox, szVal);
					}
				}				
			}

			return TRUE;

		default:
			break;
		}
	}

	if(id == 0x0000fc00)
	{
		if(SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			m_verboseLog = false;
		else
			m_verboseLog = true;

		return TRUE;
	}

	if(id == IDOK || id == IDCANCEL)
	{
		if(id == IDOK)
		{
		}

		EndDialog(m_setupDlg, id == IDOK ? 1 : 0);
		return TRUE;
	}

	return FALSE;
}


//
// OptiExpTaskRenAttr
//

const char* OptiExpTaskRenAttr::GetDesc()
{
	return "Remove dupplicated i3RenderAttr, Reconstruct SceneGraph, Remove empty i3AttrSet";
}

void OptiExpTaskRenAttr::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptRenAttr::static_meta()->create_instance();	
	m_worker->AddRef();

	((i3OptRenAttr*)m_worker)->m_supportProgress = false;
	OnSetupDefaultParam();
}

void OptiExpTaskRenAttr::OnSetupDefaultParam()
{
	i3OptRenAttr* optRenAttr = (i3OptRenAttr*)GetSceneOptimizer();
}

bool OptiExpTaskRenAttr::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();

	tsi->Log("[OptiExpTaskRenAttr] starting..");
	i3OptRenAttr* optRenAttr = (i3OptRenAttr*)GetSceneOptimizer();

	optRenAttr->Optimize(tsi->GetExpSceneGraph());
	optRenAttr->setSg(NULL);

	if(optRenAttr->GetOptimized())
	{
		i3OptRenAttr::OptiRst optRst = optRenAttr->GetOptiRst();

		if(optRenAttr->GetOptimized())
		{
			tsi->Log("[OptiExpTaskRenAttr] Total %d pass, AttrSet(%3.1f%%) [%03d/%03d], RenderAttr(%3.1f%%) [%03d/%03d], numCancelOper=%d",
				optRst.m_numPass,
				((float)optRst.m_numAttrSet[1] / optRst.m_numAttrSet[0]) * 100.0f, optRst.m_numAttrSet[0], optRst.m_numAttrSet[1],
				((float)optRst.m_numRenAttr[1] / optRst.m_numRenAttr[0]) * 100.0f, optRst.m_numRenAttr[0], optRst.m_numRenAttr[1],
				optRst.m_numCancelOper);
		}
		else
		{
			tsi->Log("[OptiExpTaskRenAttr] ERR! - Error in operation , errNum=%d !!!!", optRst.m_operRstFlag);
			return false;
		}
	}
	else
	{
		MessageBox(tsi->m_ownerWnd, "Failed to optimize i3RenderAttr\n"
			"export result may be Corrupted..!!!\n"
			"please reexport scene node again except this task..", 
			"ERR!", MB_ICONERROR);

		return false;
	}

	return true;
}


//
// OptiExpTaskRemoveAnim
//

const char* OptiExpTaskRemoveAnim::GetDesc()
{
	return "i3OptRemoveAnim";
}

void OptiExpTaskRemoveAnim::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	m_worker = i3OptRemoveAnim::static_meta()->create_instance();
	m_worker->AddRef();

	OnSetupDefaultParam();
}

void OptiExpTaskRemoveAnim::OnSetupDefaultParam()
{
	i3OptRemoveAnim* optRemoveAnim = (i3OptRemoveAnim*)GetSceneOptimizer();
}

bool OptiExpTaskRemoveAnim::OnWork()
{
	GetTaskShareInfo()->Log("[OptiExpTaskRemoveAnim] starting..");

	GetSceneOptimizer()->Optimize(GetTaskShareInfo()->GetExpSceneGraph());
	GetSceneOptimizer()->setSg(NULL);

	return true;
}


//
// OptiExpDupplicatedNameChecker
//

const char* OptiExpDupplicatedNameChecker::GetDesc()
{
	return "Check Dupplicated Mesh Name";
}

void OptiExpDupplicatedNameChecker::OnReset()
{
	I3_SAFE_RELEASE(m_worker);

	OnSetupDefaultParam();
}

void OptiExpDupplicatedNameChecker::OnSetupDefaultParam()
{
}

bool OptiExpDupplicatedNameChecker::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();
	const char* _CURFN_ = "OptiExpDupplicatedNameChecker";

	tsi->Log("[%s] is starting..", _CURFN_);
	
	OptiExpPlugSer::DupplicatedNamedNodeChecker* rst = NULL;
	if(!tsi->GetExpCallBack()->OEPC_CheckDupplicatedName((void**)&rst))
	{
		tsi->Log("[%s] ERR! - Dupplicated or empty named node found.. listing ..", _CURFN_);

		for(INT32 i=0;i<rst->GetNumDupplicated(); i++)
		{
			tsi->Log("[%s] ERR! - [Dupplicated named][%03d] [%s] ,", _CURFN_, 
				i, rst->GetDupplicatedNodeName(i));
		}

		tsi->Log("[%s] ERR! - [Empty named] count = %d..", _CURFN_, rst->GetNumEmptyNamed());

		return false;
	}

	return true;
}


//
// OptiExpCheckCollisionMesh
//

OptiExpCheckCollision::OptiExpCheckCollision()
{
	OnReset();
}

OptiExpCheckCollision::~OptiExpCheckCollision()
{
}

bool OptiExpCheckCollision::ShowSetupBox(HWND hWndParent, 
	OptiExpTaskShareInfo* taskShInfo)
{
	BOOL rst = OPTIEXPTASK_SETUP_DLG_DOMODAL(OptiExpCheckCollision, hWndParent);
	return rst ? true : false;
} 

const char* OptiExpCheckCollision::GetDesc()
{
	return "Check Collision Mesh";
}

void OptiExpCheckCollision::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	OnSetupDefaultParam();
}

void OptiExpCheckCollision::OnSetupDefaultParam()
{
	m_logFlag = 0x1 | 0x4;
}

bool OptiExpCheckCollision::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();
	const char* _CURFN_ = "OptiExpCheckCollisionMesh";

	tsi->Log("[%s] is starting..", _CURFN_);
	
	if(!tsi->GetExpCallBack()->OEPC_CheckCollsionMesh(m_logFlag))
	{
		tsi->Log("[%s] ERR! - Failed to check collision mesh, see log to verfiy..", _CURFN_);
		return false;
	}

	return true;
}

OPTIEXPTASK_SETUP_DLG_DEF_IMPL(OptiExpCheckCollision)

void OptiExpCheckCollision::OnSetupDlgInit(HWND hWnd)
{
	m_setupDlg = hWnd;
	memset(m_logTypeWnd, 0, sizeof(m_logTypeWnd));

	INT32 top = 10, left = 10;
	INT32 curX=left, curY = top;
	HINSTANCE hInst = OptiExpPlug_HInstance;

	HWND desc = CreateWindow("static", "* Logging Type", WS_CHILD | WS_VISIBLE,
		curX, curY, 100, 20, hWnd, (HMENU)-1, hInst, NULL);

	curX += 20;
	curY += 25;

	m_logTypeWnd[0] = CreateWindow("button", "skipped mesh (not a triangle mesh, except 'Scene Root' )", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, curX, curY,
		400, 20, hWnd, (HMENU)0x0000fa01, hInst, NULL);

	curY += 25;
	m_logTypeWnd[1] = CreateWindow("button", "succeed mesh (baking convex mesh is succeed)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, curX, curY,
		400, 20, hWnd, (HMENU)0x0000fa02, hInst, NULL);

	curY += 25;
	m_logTypeWnd[2] = CreateWindow("button", "failed mesh (baking convex mesh is failed(non mainfold)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, curX, curY,
		400, 20, hWnd, (HMENU)0x0000fa03, hInst, NULL);
	
	SendMessage(m_logTypeWnd[0], BM_SETCHECK, m_logFlag & 0x1 ? TRUE : FALSE, 0);
	SendMessage(m_logTypeWnd[1], BM_SETCHECK, m_logFlag & 0x2 ? TRUE : FALSE, 0);
	SendMessage(m_logTypeWnd[2], BM_SETCHECK, m_logFlag & 0x4 ? TRUE : FALSE, 0);
}

BOOL OptiExpCheckCollision::OnSetupDlgCmd(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD evt = HIWORD(wParam);

	if(id == IDOK || id == IDCANCEL)
	{
		if(id == IDOK)
		{
			m_logFlag = 0;
			m_logFlag |= SendMessage(m_logTypeWnd[0], BM_GETCHECK, 0, 0) == BST_CHECKED ? 0x1 : 0;
			m_logFlag |= SendMessage(m_logTypeWnd[1], BM_GETCHECK, 0, 0) == BST_CHECKED ? 0x2 : 0;
			m_logFlag |= SendMessage(m_logTypeWnd[2], BM_GETCHECK, 0, 0) == BST_CHECKED ? 0x4 : 0;
		}

		EndDialog(m_setupDlg, id == IDOK ? 1 : 0);
		return TRUE;
	}

	return FALSE;
}


//
// OptiExpCheckPhysXShapeSet
//

const char* OptiExpCheckPhysXShapeSet::GetDesc()
{
	return "Check PhysXShape";
}

void OptiExpCheckPhysXShapeSet::OnReset()
{
	I3_SAFE_RELEASE(m_worker);
	OnSetupDefaultParam();
}

void OptiExpCheckPhysXShapeSet::OnSetupDefaultParam()
{
}

bool OptiExpCheckPhysXShapeSet::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();
	const char* _CURFN_ = "OptiExpCheckPhysXShapeSet";

	tsi->Log("[%s] is starting..", _CURFN_);

	i3Node* root = tsi->GetExpSceneGraph()->getInstanceSg();

	i3Node* testParent = i3Node::new_object();

	i3SceneUtil::BuildLogger logger;
	logger.SetSection("Checking PhysXShapeSet");

	i3SceneUtil::BuildRendableCollider(root, testParent, TRUE, TRUE, 
		true, NULL, NULL, true, &logger, 1);

	I3_SAFE_RELEASE(testParent);

	if(logger.GetNumGenFailColliders())
	{
		i3SceneUtil::BuildLogger::MsgStatus status;	
		if(logger.GenerateStatusMsg(&status, i3SceneUtil::BuildLogger::ELF_INC_PHYSHAPE_MISS))
		{
			tsi->Log("[%s] Err! - some nodes is missing physxShapeSet.. see below", _CURFN_);
			for(i3SceneUtil::BuildLogger::MsgStatus::size_type i=1; i<status.size(); i++)
				tsi->Log("[%s] Err! - %s ", _CURFN_, status[i].c_str());

			return false;
		}
	}

	return true;
}


//
// OptiExpAnimBoneListManager
//

const char* OptiExpAnimBoneListManager::GetDesc()
{
	return "OptiExpAnimBoneListManager";
}

void OptiExpAnimBoneListManager::OnReset() {}
void OptiExpAnimBoneListManager::OnSetupDefaultParam() {}
bool OptiExpAnimBoneListManager::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();
	const char* _CURFN_ = "OptiExpAnimBoneListManager";
	tsi->Log("[%s] is starting..", _CURFN_);
	return true;
}

INT32 OptiExpAnimBoneListManager::ReadListFromMaxFile()
{
	return 0;
}


//
// OptiExpTaskAnimPackBuilder
//

const char* OptiExpTaskAnimPackBuilder::GetDesc()
{
	return "OptiExpAnimBoneListManager";
}

void OptiExpTaskAnimPackBuilder::OnReset() {}
void OptiExpTaskAnimPackBuilder::OnSetupDefaultParam() {}

bool OptiExpTaskAnimPackBuilder::OnWork()
{
	OptiExpTaskShareInfo* tsi = GetTaskShareInfo();
	const char* _CURFN_ = "OptiExpTaskAnimPackBuilder";
	tsi->Log("[%s] is starting..", _CURFN_);
	return true;
}
