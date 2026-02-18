
#ifndef _OPTIEXPTASK_H_
#define _OPTIEXPTASK_H_

class OptiExpTask;
class i3SceneGraphInfo;
class OptiExpAnimBoneListManager;


typedef i3::vector<OptiExpTask*>		OptiExpTaskVec;

#define Opti_SNDMSG ::SendMessage
#define Opti_Button_GetCheck(hwndCtl) ((int)(DWORD)Opti_SNDMSG((hwndCtl), BM_GETCHECK, 0L, 0L))
#define Opti_Button_SetCheck(hwndCtl, check) ((void)Opti_SNDMSG((hwndCtl), BM_SETCHECK, (WPARAM)(int)(check), 0L))



class OptiExpTaskShareInfo
{
public:
	OptiExpTaskShareInfo();
	~OptiExpTaskShareInfo();

	void Reset();
	bool Loadi3SceneFile(const char* path=NULL);
	bool IsLastTaskOk() { return m_prevTask ? m_lastTaskRst : true; }

	i3SceneGraphInfo* GetExpSceneGraph() { return m_sgInfo; }
	void GenerateSceneGraphInfoFrom(i3Node* node);

	OptiExpPlugCallBack* GetExpCallBack() { return m_expCallBack; }
	
	void Log(const char* fmt, ...);
	void SetDeferredLogging(bool deferred=false);
	void FlushLog();

	void SetFilePathName(const char* path=NULL, const char* name=NULL);

	OptiExpTask*					m_prevTask;
	bool							m_lastTaskRst;
	i3SceneGraphInfo*				m_sgInfo;
	OptiExpPlugCallBack*			m_expCallBack;
	HWND							m_ownerWnd;

	char							m_expFilePath[MAX_PATH];
	char							m_expFileName[MAX_PATH];

	bool							m_deferredLog;
	i3::vector<i3::string>			m_deferredLogVec;
};


//
// OptiExpTaskProgressWnd
//
class OptiExpProgressWnd
{
public:
	struct CallBack
	{
		virtual bool OptiExpProgressWnd_OnCancel() = 0;
	};

	enum ECTL
	{
		ECTL_ST_00,
		ECTL_ST_01,
		ECTL_ST_MAX,
		ECTL_PB_00,
		ECTL_MAX
	};

	OptiExpProgressWnd(CallBack* callback, HWND parent=NULL, bool show=false);
	~OptiExpProgressWnd();

	void Show(bool show);

	bool PumpMsg();
	void SetText(ECTL item, const char* fmt, ...);
	
	void SetProgressInfo(INT32 minVal, INT32 maxVal);
	void SetProgress(INT32 val);

	bool OnCancel();

private:
	void OnInitDialog(HWND hWnd);
	void OnDestroyWnd();
	
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND					m_dlg;
	HWND					m_parent;

	HWND					m_ctl[ECTL_MAX];
	CallBack*				m_callback;
};


//
// OptiExpTaskProcessor
//
class OptiExpTaskProcessor : public OptiExpProgressWnd::CallBack
{
public:
	bool DoWithStatus(OptiExpTaskShareInfo* taskShInfo, 
	OptiExpTaskVec& tasks, HWND parentWnd=NULL);

	virtual bool OptiExpProgressWnd_OnCancel();

	bool IsQuitProcessing() { return m_quitProcessing; }

private:
	bool							m_quitProcessing;	
};


//
// OptiExpTask
//

class OptiExpTask
{
public:
	virtual ~OptiExpTask();

	virtual bool Do(OptiExpTaskShareInfo* taskMan=NULL);

	virtual void SetEnable(bool enable) { m_enable = enable; }
	virtual bool GetEnable() { return m_enable; }

	virtual void SetDefaultParam(INT32 from=0) = 0;
	virtual void Reset() = 0;

	virtual bool ShowSetupBox(HWND hWndParent=NULL, OptiExpTaskShareInfo* taskShInfo=NULL);
	
	virtual const char* GetDesc() = 0;

	i3SceneOptimizer* GetSceneOptimizer() 
		{	return i3::kind_cast<i3SceneOptimizer*>(m_worker); }

	OptiExpTaskShareInfo* GetTaskShareInfo() { return m_taskShInfo; }

	virtual bool HasSetupBox() { return true; }

	virtual OptiExpTaskVec::size_type GetNumSubTask() { return 0; }
	virtual OptiExpTask* GetSubTask(OptiExpTaskVec::size_type index) { return NULL; }
	virtual void AddSubTask(OptiExpTask* task) {}
	virtual OptiExpTaskVec* GetSubTasks() { return NULL; }

//	static void ResetTaskes(OptiExpTaskVec* taskes);
	virtual i3ElementBase* GetWorker() { return m_worker; }

protected:
	OptiExpTask();

	virtual bool OnWork();

	OptiExpTaskShareInfo*		m_taskShInfo;

	i3ElementBase*				m_worker;
	bool						m_enable;
};


#define OPTIEXPTASK_CLS_CONSTRUCTOR_DEFINE(cls) \
	public: \
	cls() { OnReset(); } \
	virtual ~##cls##() {}

#define OPTIEXPTASK_CLS_BODY_DEFINE(cls) \
	public: \
	typedef	OptiExpTask	super;	\
								\
	virtual void SetDefaultParam(INT32 from=0) { OnSetupDefaultParam(); } \
	virtual void Reset() { OnReset(); } \
	virtual const char* GetDesc();	\
									\
	protected: \
		void OnReset(); \
		void OnSetupDefaultParam(); \
									\
		virtual bool OnWork();


#define OPTIEXPTASK_CLS_DEFINE(cls) \
	OPTIEXPTASK_CLS_CONSTRUCTOR_DEFINE(cls) \
	OPTIEXPTASK_CLS_BODY_DEFINE(cls)

#define OPTIEXPTASK_CLS_DEFINE_BODYONLY(cls) \
	OPTIEXPTASK_CLS_BODY_DEFINE(cls)


#define OPTIEXPTASK_SETUP_DLG_DEF_FUNC \
private: \
	static BOOL WINAPI SetupDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); \
	void OnSetupDlgInit(HWND hWnd); \
	BOOL OnSetupDlgCmd(WPARAM wParam, LPARAM lParam); \
	HWND						m_setupDlg;

#define OPTIEXPTASK_SETUP_DLG_DOMODAL(cls, hWndParent) \
	DialogBoxParam(OptiExpPlug_HInstance, MAKEINTRESOURCE(IDD_DLG_OPTIEXP_SETUP_PARAM), \
	hWndParent, ##cls##::SetupDlgProc##, (LPARAM)this)

#define OPTIEXPTASK_SETUP_DLG_DEF_IMPL(cls) BOOL WINAPI cls##::SetupDlgProc( \
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)	\
	{	\
	switch(msg) \
	{ \
	case WM_INITDIALOG:	\
		{ \
			cls* inst = (cls*)lParam; \
			SetWindowLongPtr(hWnd, DWL_USER, (LONG_PTR)lParam); \
			inst->OnSetupDlgInit(hWnd); \
		} \
		break; \
					\
	case WM_COMMAND:	\
		{ \
			cls* inst \
			= (##cls##*)GetWindowLongPtr(hWnd, DWL_USER); \
			\
			if(!inst->OnSetupDlgCmd(wParam, lParam)) \
				return FALSE; \
		} \
		break; \
	\
	case WM_DESTROY: \
		{ \
			SetWindowLongPtr(hWnd, DWL_USER, NULL); \
		}	\
		break; \
	\
	default: \
		return FALSE; \
	} \
	\
	return TRUE; \
}


//
//	OptiExpTaskDx9RenContext
//

class OptiExpTaskDx9RenCtxContainer : public OptiExpTask
{
public:
	OptiExpTaskDx9RenCtxContainer();
	virtual ~OptiExpTaskDx9RenCtxContainer();

	OPTIEXPTASK_CLS_DEFINE_BODYONLY(OptiExpTaskDx9RenCtxContainer)

public:
	virtual bool HasSetupBox() { return false; }

	virtual OptiExpTaskVec::size_type GetNumSubTask();
	virtual OptiExpTask* GetSubTask(OptiExpTaskVec::size_type index);

	virtual void AddSubTask(OptiExpTask* task);
	virtual OptiExpTaskVec* GetSubTasks() { return &m_subTask; }

private:
	bool DoOperationInDxRenContext();
	static unsigned __stdcall RenCtxCntrThread(void* param);

private:
	OptiExpTaskVec				m_subTask;
	bool						m_subTaskOperCanceled;
};


//
//	OptiExpTaskI3Exporter
//

class OptiExpTaskI3Exporter : public OptiExpTask
{
public:
	OptiExpTaskI3Exporter();
	virtual ~OptiExpTaskI3Exporter();
	OPTIEXPTASK_CLS_DEFINE_BODYONLY(OptiExpTaskI3Exporter)

public:
	virtual bool ShowSetupBox(HWND hWndParent=NULL, OptiExpTaskShareInfo* taskShInfo=NULL);
	
	virtual void SetEnable(bool enable) { m_enable = true; } // this task must be enabled evertime
	i3tool::exporter::pack::OPTION_INFO* GetExporterOption() { return m_option; }

	void ApplyAnimBoneListOpt(OptiExpAnimBoneListManager* boneList);

public:
	i3tool::exporter::pack::OPTION_INFO*				m_option;
};


//
//	OptiExpTaskMipmapGen
//
class OptiExpTaskMipmapGen : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskMipmapGen)
};


//
//	OptiExpCvtTextureToDDS
//
class OptiExpTaskCvtTextureToDDS : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskCvtTextureToDDS)

public:
	void* OptiExpTaskCvtTextureToDDS::MakeToDXT(void* pSrc);
};


//
//	OptiExpTaskNodeOpti
//
class OptiExpTaskNodeOpti : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskNodeOpti)
};


//
//	OptiExpTaskCreateBoundBox
//
class OptiExpTaskCreateBoundBox : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskCreateBoundBox)

public:
	virtual bool HasSetupBox() { return false; }
};


//
// OptiExpTaskRemoveVtxComponent
//
class OptiExpTaskRemoveVtxComponent : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskRemoveVtxComponent)
};

//
// OptiExpTaskRemoveVtxComponent_occluder
// 
class OptiExpTaskRemoveVtxComponent_occluder : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskRemoveVtxComponent_occluder)
};

//
// OptiExpTaskRemoveTextureBindAttr_occluder
//
class OptiExpTaskRemoveAttr_occluder : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskRemoveAttr_occluder)
};


//
// OptiExpTaskPhsXSetBeforeExport : OptiExpTask
//

class OptiExpTaskPhsXSetBeforeExport : public OptiExpTask
{
public:
	OptiExpTaskPhsXSetBeforeExport();
	virtual ~OptiExpTaskPhsXSetBeforeExport();

	OPTIEXPTASK_CLS_DEFINE_BODYONLY(OptiExpTaskPhsXSetBeforeExport)

public:
	enum EPHYSX_DYNAMIC_TYPE
	{
		EPHY_DYT_DYNAMIC	= 1, //"dynamic"
		EPHY_DYT_KINEMATIC  = 2, //"kinematic"
		EPHY_DYT_STATIC		= 3, //"static"
		EPHY_DYT_NONE		= 4, //"(none)"		
		EPHY_DYT_MAX
	};

	EPHYSX_DYNAMIC_TYPE				m_dynType;
	char							m_excludeNode[1024];

	char							m_includeNode[1024];
	bool							m_includeOnly;

	void SetExcludeNode(const char* excludeNode);
	
	void SetIncludeNode(const char* includeNode);
	void SetIncludeNodeOnly(bool includeOnly);

	const char* GetDynTypeStr(EPHYSX_DYNAMIC_TYPE type);
	virtual bool ShowSetupBox(HWND hWndParent=NULL, OptiExpTaskShareInfo* taskShInfo=NULL);

protected:
	i3::vector<i3::string>			m_dynTypeToStr; 

private:
	OPTIEXPTASK_SETUP_DLG_DEF_FUNC
	HWND							m_cbDyn;	// temporary

	HWND							m_ebExclude;
	HWND							m_ebInclude;
	HWND							m_ckIncludeOnly;

};


//
// OptiExpTaskSaveI3S
//

class OptiExpTaskSaveI3S : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskSaveI3S)

public:
	virtual bool HasSetupBox() { return false; }

private:
	char						m_filepath[MAX_PATH];
};


//
//  OptiExpTaskSetAnimationTime
//

class OptiExpTaskSetAniTimeRange : public OptiExpTask
{
public:
	OptiExpTaskSetAniTimeRange(INT32 startTime, INT32 endTime);
	OptiExpTaskSetAniTimeRange(OptiExpTaskSetAniTimeRange* restoreTo);
	virtual ~OptiExpTaskSetAniTimeRange();

	OPTIEXPTASK_CLS_DEFINE_BODYONLY(OptiExpTaskSetAniTimeRange)
	virtual bool HasSetupBox() { return false; }

public:
	INT32								m_action;     // 0:set, 1:restore

	INT32								m_startTime;
	INT32								m_endTime;

	INT32								m_prevStartTime;
	INT32								m_prevEndTime;

	OptiExpTaskSetAniTimeRange*		m_restoreTo;
};


//
// OptiExpTaskSetupThickness
//

class OptiExpTaskSetupThickness : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskSetupThickness)

public:
	virtual bool ShowSetupBox(HWND hWndParent=NULL, OptiExpTaskShareInfo* taskShInfo=NULL);

private:
	OPTIEXPTASK_SETUP_DLG_DEF_FUNC
	void RecursiveSetThickness(i3Node* cur);

private:
	bool							m_verboseLog; // 로그자세히
	INT32							m_numThicknessSet;

	friend bool LookupThickness(const char* prefix, INT32* rstThickness);
};


//
// OptiExpTaskRenAttr
//

class OptiExpTaskRenAttr : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskRenAttr)
};


//
// OptiExpRemoveAnim
//

class OptiExpTaskRemoveAnim : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskRemoveAnim)
};


//
// OptiExpDupplicatedNameChecker
//

class OptiExpDupplicatedNameChecker : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpDupplicatedNameChecker)
};


//
// OptiExpCheckCollisionMesh
//

class OptiExpCheckCollision : public OptiExpTask
{
public:
	OptiExpCheckCollision();
	virtual ~OptiExpCheckCollision();

	OPTIEXPTASK_CLS_DEFINE_BODYONLY(OptiExpTaskPhsXSetBeforeExport)

	virtual bool ShowSetupBox(HWND hWndParent=NULL, OptiExpTaskShareInfo* taskShInfo=NULL);

private:
	OPTIEXPTASK_SETUP_DLG_DEF_FUNC
	DWORD					m_logFlag;

	HWND					m_logTypeWnd[3];
};


//
// OptiExpCheckPhysXShapeSet
//

class OptiExpCheckPhysXShapeSet : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpCheckPhysXShapeSet)

public:
	
};


//
// OptiExpAnimBoneListManager
//

class OptiExpAnimBoneListManager : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpAnimBoneListManager)

public:	
	INT32 ReadListFromMaxFile();

protected:
};


//
// OptiExpTaskAnimPackBuilder
//

class OptiExpTaskAnimPackBuilder : public OptiExpTask
{
	OPTIEXPTASK_CLS_DEFINE(OptiExpTaskAnimPackBuilder)

public:
};


#endif
