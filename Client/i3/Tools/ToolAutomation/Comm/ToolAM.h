
#ifndef _TOOLAM_H_
#define _TOOLAM_H_

#include <deque>
#include <vector>

#include "ToolAM_Def.h"
#include "ToolAM_Util.h"

namespace ToolAM
{
	const UINT ECMD_MAX = 1024;

	struct _CmdArgNull;

	struct CmdArg
	{
		enum ETYPE
		{
			ENONE, 
			EINT, 
			EFLOAT, 
			ESTR,
			ETYPE_MAX
		};

		enum EPRMOPER_TYPE
		{
			PRMOPER_ALLOC,
			PRMOPER_DEALLOC,
			PRMOPER_COPY,
			PRMOPER_GENPRMSTR,
			PRMOPER_MAX
		};

		const char* CmdArg::GetStr();
		INT32 CmdArg::GetInt();

		typedef void (*PRMOPER_PFN)(void*, void*, unsigned);

		CmdArg();
		CmdArg(INT32 v);
		CmdArg(const char* str);
		CmdArg(const CmdArg& rhs);
		~CmdArg();

		CmdArg& operator=(const CmdArg& rhs);

		template<class TPRM> void Init(void* v) {}

		template<> void Init<INT32>(void* v) 
		{
			m_prmOper[PRMOPER_ALLOC] = _INT32_ALLOC; m_prmOper[PRMOPER_DEALLOC] = _INT32_DEALLOC;
			m_prmOper[PRMOPER_COPY] = _INT32_COPY;
			m_prmOper[PRMOPER_GENPRMSTR] = _INT32_GENPRMSTR;

			m_siz = 1;
			m_prmOper[PRMOPER_ALLOC](&m_v, NULL, 1);
			m_prmOper[PRMOPER_COPY](m_v, v, m_siz);
		}

		template<> void Init<char*>(void* v) 
		{
			m_prmOper[PRMOPER_ALLOC] = _CHAR_ALLOC; m_prmOper[PRMOPER_DEALLOC] = _CHAR_DEALLOC;
			m_prmOper[PRMOPER_COPY] = _CHAR_COPY;
			m_prmOper[PRMOPER_GENPRMSTR] = _CHAR_GENPRMSTR;

			char* c = (char*)v;			
			unsigned strSiz = 0;
			while(c && *c != NULL) { c++; strSiz++; }

			if(strSiz)
				m_siz = sizeof(char) *(strSiz + 1);
			else
				m_siz = 0;

			m_prmOper[PRMOPER_ALLOC](&m_v, NULL, m_siz);
			m_prmOper[PRMOPER_COPY](m_v, v, m_siz);
		}

		std::string GenParamStr() 
		{
			std::string str;
			m_prmOper[PRMOPER_GENPRMSTR](&m_v, (void*)&str, m_siz);

			return str;
		}

		ETYPE					m_type = ETYPE::ENONE;
		void*					m_v = nullptr;
		unsigned				m_siz = 0;

		PRMOPER_PFN				m_prmOper[PRMOPER_MAX] = { nullptr, nullptr, nullptr, nullptr };

		////////////////////////////////
		#define _PRMOPER_FUNC_DEF(x) static void x##ALLOC(void*, void*, unsigned); \
			static void x##DEALLOC(void*, void*, unsigned); \
			static void x##COPY(void*, void*, unsigned); \
			static void x##GENPRMSTR(void*, void*, unsigned);

		_PRMOPER_FUNC_DEF(_INT32_)
		_PRMOPER_FUNC_DEF(_CHAR_)

	};

	typedef	std::vector<CmdArg> CmdArgStore;

	struct _CmdArgNull : public CmdArg {};
	extern _CmdArgNull CmdArgNull;

	void GenerateCmdArgStr(CmdArgStore& in, std::string& out);

	enum ECMD_PROC_RST
	{
		ECMD_PROC_RST_OK,
		ECMD_PROC_RST_FAIL
	};

	class DispatchCallBack
	{
	public:
		DispatchCallBack() { m_prm = NULL; m_cmdData = NULL; }
		virtual void Log(INT32 flag, const char* fmt, ...) {}
		void*					m_prm;
		void*					m_cmdData; // CmdQueue::Data*
	};

	class CmdQueue
	{
		friend class CmdQueueDispatchCallBack;
	public:	
		struct Data
		{	
			enum ETYPE 
			{ 
				ETYPE_CMD, 
				ETYPE_TRANSACTION_BEGIN, 
				ETYPE_TRANSACTION_END
			};

			bool InTransaction() { return m_transactionID != 0 ? true : false; }

			ETYPE						m_type = ETYPE::ETYPE_CMD;
			UINT						m_transactionID = 0;
			DWORD						m_cmd = 0;
			
			std::string					m_transactionDesc;
			CmdArgStore					m_args;
		};

		typedef std::deque<Data>		DataQType;
		typedef INT32 (*CmdProcPFN)(DispatchCallBack*, CmdArgStore&);

		class ReportCallBack
		{
		public:
			virtual void ToolAM_CmdQueue_Report(DWORD errFlag, Data& cmdData, const char* message) = 0;
			virtual void ToolAM_CmdQueue_Report2(const char* message) {}
		};

		class CmdQueueDispatchCallBack : public DispatchCallBack
		{
		public:
			CmdQueueDispatchCallBack() { m_owner =NULL; }
			virtual void Log(INT32 flag, const char* fmt, ...);

			CmdQueue*	m_owner;
		};

	public:
		CmdQueue();
		~CmdQueue();

		typedef const char* (*PFN_CvtCmd2Str)(DWORD);

		// SendCmd(ECMD_XXX, CmdArg0, 1, 2, NULL);
		// arguments is must be end with NULL
		void PostCmd(DWORD cmd, ...); // ECMD cmd
		INT32 DispatchCmd();

		UINT32 BeginTransaction(const char* transactionDesc=NULL);
		void EndTransaction(UINT32 transactionID=0);
		void AbortTransaction();
		UINT32 GetTransactionID();

		static INT32 Dispatch(const char* cmdQueueName);
		static CmdQueue* GetQ(const char* name);
		static void SetQueueParam(const char* cmdQueueName, void* prm);

		void SetName(const char* name) ;
		void SetParam(void* prm) { m_prm = prm; }

		const char* GetName() { return m_name; }
		void Add(DWORD cmd, CmdProcPFN pfn);

		ReportCallBack* SetReportCallBack(ReportCallBack* callback) 
		{
			ReportCallBack* old = m_reportCallBack;
			m_reportCallBack = callback; 
			return old;
		}

		void Report(DWORD errFlag, Data& cmdData, const char* fmt, ...);
		void Report(const char* fmt, ...);

		void SetCmd2StrFunc(PFN_CvtCmd2Str in) { m_pfnCvtCmd2Str = in; }
		UINT32 GetTransactionNum(){ return m_transactionNumber; }

	protected:
		DataQType::iterator RecursiveAbortTransaction(DataQType::iterator cur);

	private:		
		char							m_name[MAX_PATH];

		DataQType						m_cmdDataQ;
		void*							m_prm;		

		CmdProcPFN						m_cmdPFN[ToolAM::ECMD_MAX];

		UINT32							m_transactionNumber;
		UINT32							m_curTransactionID;
		std::string						m_curTransactionDesc;

		ReportCallBack*					m_reportCallBack;
		CmdQueueDispatchCallBack		m_dispCallBack;
		PFN_CvtCmd2Str					m_pfnCvtCmd2Str;		
	};

	#define TOOLAM_CMDQUEUE_NO_DISPATCH (-1)
	#define TOOLAM_CMDQUEUE_DISPATCH(x) ToolAM##::##CmdQueue##::##Dispatch(x)


	//
	// MFC Support
	//

	//BOOL Ci3CharaEditorApp::OnIdle(LONG lCount)
	//{
	//	#ifdef _USE_TOOLAM_
	//		if(TOOLAM_CMDQUEUE_DISPATCH("i3ChrTool") != TOOLAM_CMDQUEUE_NO_DISPATCH)
	//			return TRUE;
	//	#endif
	//	return CWinApp::OnIdle(lCount);
	//}

	#define TOOLAM_CMDQUEUE_MFC_INSTALL(cmdQueueName) \
		ToolAM##::##CmdQueue##::##SetQueueParam(##cmdQueueName##, (void*)AfxGetMainWnd());

	#define TOOLAM_CMDQUEUE_MFC_COMMIT(x) \
		while(TOOLAM_CMDQUEUE_DISPATCH(x) != TOOLAM_CMDQUEUE_NO_DISPATCH) \
		{ \
			AfxGetApp()->PumpMessage();\
			Sleep(1); \
		}
	#define TOOLAM_CMDQUEUE_MFC_POST_COMMIT AfxGetMainWnd()->SendMessage(WM_USER, 0, 0)

	#define TOOLAM_CMDQ_DEF(cmdqname, cmdStrCvter) \
		static struct _TOOLAM_CMDQ_DEF_Initializer { _TOOLAM_CMDQ_DEF_Initializer() { \
			ToolAM::CmdQueue::GetQ(cmdqname)->SetCmd2StrFunc(cmdStrCvter); } } _TOOLAM_CMDQ_DEF_Initializer_val;

	#define TOOLAM_CMDFN_REGISTER(cmdqname, cmd) \
		static INT32 _TOOLAM_Disp_##cmd##(ToolAM::DispatchCallBack* cb, ToolAM::CmdArgStore& args); \
		static struct _TOOLAM_Dispatch_##cmd##_register { _TOOLAM_Dispatch_##cmd##_register() { \
		   ToolAM::CmdQueue::GetQ(cmdqname)->Add(cmd, ##_TOOLAM_Disp_##cmd##); } } _TOOLAM_Dispatch_##cmd##_register_val;

	#define TOOLAM_CMDFN(cmd)  INT32 _TOOLAM_Disp_##cmd##(ToolAM::DispatchCallBack* cb, ToolAM::CmdArgStore& args)


//////////////////////////////////////////////////////////////////////////
} // end of ToolAM
//////////////////////////////////////////////////////////////////////////


#endif
