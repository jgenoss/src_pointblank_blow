
#include "ToolAMPCH.h"
#include "ToolAM.h"


namespace ToolAM
{
	namespace CharToolAM
	{

	} // end of namespace CharToolAM


	//
	// CmdQueueInfo
	//

	enum { MAX_NUM_CMDQUEUEINFO = 5 };

	struct CmdQueueInfo
	{
	public:
		CmdQueueInfo() 
		{
			INT32 index = -1;
			for(INT32 i=0; i<MAX_NUM_CMDQUEUEINFO; i++)
			{
				if(ms_queueInfoInit[i] == this)
				{
					index = i;
					break;
				}
			}
			
			if(index == -1)
			{
				m_queue = NULL;
			}
		}

		~CmdQueueInfo()
		{ 
#ifdef _DEBUG
//			OutputDebugString("CmdQueueInfo Destroying! \n");
#endif
			if(m_queue) 
				delete m_queue;

			m_queue = NULL;
		}

		static CmdQueue* GetCmdQueue(const char* name)
		{
			if(!name)
				return NULL;

			INT32 i=0;

			for(; i<ms_numQueue; i++)
			{
				if(!ms_queueInfo[i].m_queue)
					break;

				if(!_stricmp(name, ms_queueInfo[i].m_queue->GetName()))
					return ms_queueInfo[i].m_queue;
			}

			if( (ms_numQueue+1) == MAX_NUM_CMDQUEUEINFO)
				return NULL;

			CmdQueue* newCmdQueue = new CmdQueue;
			ms_queueInfo[i].m_queue = newCmdQueue;

			ms_queueInfoInit[i] = (void*)(&ms_queueInfo[i]);

			newCmdQueue->SetName(name);
			ms_numQueue++;

			return ms_queueInfo[i].m_queue;
		}

	private:
		CmdQueue*					m_queue = nullptr;

		static INT32				ms_numQueue;
		static CmdQueueInfo			ms_queueInfo[MAX_NUM_CMDQUEUEINFO];
		static void*				ms_queueInfoInit[MAX_NUM_CMDQUEUEINFO];
	};

	INT32 CmdQueueInfo::ms_numQueue = 0;

	CmdQueueInfo CmdQueueInfo::ms_queueInfo[MAX_NUM_CMDQUEUEINFO];
	void* CmdQueueInfo::ms_queueInfoInit[MAX_NUM_CMDQUEUEINFO] = {0};


	//
	// CmdArg
	//

	CmdArg::CmdArg()
	{
		m_type = ENONE;
		m_v = NULL;
		m_siz = 0; 
		
		memset(m_prmOper, 0, sizeof(m_prmOper));
	}

	CmdArg::CmdArg(INT32 v) 
	{ 
		m_type = EINT;
		Init<INT32>((void*)&v);
	}

	CmdArg::CmdArg(const char* str)
	{ 
		m_type = ESTR;
		Init<char*>((void*)str);
	}

	CmdArg::CmdArg(const CmdArg& rhs)
	{
		m_type = rhs.m_type;
		memcpy(m_prmOper, rhs.m_prmOper, sizeof(m_prmOper));
		m_v = NULL; 
		m_siz = 0; 

		*this = rhs;
	}

	CmdArg::~CmdArg()
	{
		if(m_v && m_type != ENONE)
			m_prmOper[PRMOPER_DEALLOC](&m_v, NULL, NULL);
	}

	CmdArg& CmdArg::operator=(const CmdArg& rhs)
	{
		if(this == &rhs)
			return *this;

		assert(rhs.m_type == m_type);

		if(m_v && m_type != ENONE)
			m_prmOper[PRMOPER_DEALLOC](&m_v, NULL, NULL);

		m_siz = rhs.m_siz;

		if(m_type == ENONE)
			return *this;

		rhs.m_prmOper[PRMOPER_ALLOC](&m_v, NULL, rhs.m_siz);
		rhs.m_prmOper[PRMOPER_COPY](m_v, rhs.m_v, rhs.m_siz);

		return *this;
	}


	void GenerateCmdArgStr(CmdArgStore& in, std::string& out)
	{
		for(INT32 i=0; i<(INT32)in.size(); i++)
		{
			CmdArg& cur = in[i];
			out += cur.GenParamStr();
		}
	}

	//
	// CmdQueue
	//

	void CmdQueue::SetName(const char* name)
	{
		strcpy_s(m_name, MAX_PATH, name); 
		strlwr(m_name);
	}


	//
	// CmdArg
	//

	_CmdArgNull CmdArgNull;

	const char* CmdArg::GetStr()
	{
		if(m_type != ESTR) return NULL;
		return (const char*)m_v;
	}

	INT32 CmdArg::GetInt()
	{
		if(m_type != EINT) return 0;
		return *(INT32*)m_v;
	}

	void CmdArg::_INT32_ALLOC(void* v, void*, unsigned siz)
	{ 
		INT32** _v = (INT32**)v; (*_v) = NULL;

		if(siz > 0)
			*_v = new INT32[siz]; 
	}
	void CmdArg::_INT32_DEALLOC(void* v, void*, unsigned)
	{ 
		INT32** _v = (INT32**)v;
		if(!*_v) return;

		delete (INT32*)(*_v);
		(*_v) = NULL;
	}
	void CmdArg::_INT32_COPY(void* v, void* w, unsigned siz)
	{ 
		if(siz > 0) memcpy(v, w, sizeof(INT32) * siz); 
	}
	void CmdArg::_INT32_GENPRMSTR(void* v, void* stdstr, unsigned)
	{
		std::string& out = *((std::string*)stdstr);

		INT32** _v = (INT32**)v;
		if(!*_v) { out = "0"; return; }

		char buf[32] = {0};
		sprintf_s(buf, 32, "%d", *(*_v));
		out = buf;
	}

	void CmdArg::_CHAR_ALLOC(void* v, void*, unsigned siz)
	{ 
		char** _v = (char**)v; (*_v) = NULL;
		if(siz > 0) 
			*_v = new char[siz]; 
	}
	void CmdArg::_CHAR_DEALLOC(void* v, void*, unsigned)
	{ 
		char** _v = (char**)v;
		if(!*_v) return;

		delete (char*)(*_v); 
		(*_v) = NULL;
	}
	void CmdArg::_CHAR_COPY(void* v, void* w, unsigned siz)
	{ 
		if(siz > 0) memcpy(v, w, sizeof(char) * siz); 
	}
	void CmdArg::_CHAR_GENPRMSTR(void* v, void* stdstr, unsigned)
	{
		std::string& out = *((std::string*)stdstr);
		char** _v = (char**)v;

		if(!(*_v)) { out = "(null)"; return; }
		
		out = (*_v);
	}


	//
	// CmdQueue
	//

	void CmdQueue::CmdQueueDispatchCallBack::Log(INT32 flag, const char* fmt, ...)
	{
		char buf[MAX_PATH] = {0};

		CmdQueue::Data* cmdData = (CmdQueue::Data*)m_cmdData;

		if(fmt)
		{
			va_list args;
			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);

			if(m_owner->m_reportCallBack)
			{
				char buf2[1024] = {0};

				const char* transactionDesc = cmdData->m_transactionDesc.c_str();
				if(!transactionDesc || !(*transactionDesc))
					transactionDesc = "NoTransactionDesc!";

				sprintf_s(buf2, 1024,"[%s][%s][%s][%s]", flag & 0x80000000 ? "ERR!" : "INFO",
					m_owner->m_pfnCvtCmd2Str ? m_owner->m_pfnCvtCmd2Str(cmdData->m_cmd) : "[NONE]", transactionDesc, buf);

				m_owner->m_reportCallBack->ToolAM_CmdQueue_Report2(buf2);
			}
		}
	}

	CmdQueue::CmdQueue()
	{
		m_prm = NULL;
		memset(m_cmdPFN, 0, sizeof(m_cmdPFN));
		m_transactionNumber = 1;
		m_curTransactionID = 0;

		m_reportCallBack = NULL;
		m_pfnCvtCmd2Str = NULL;

		m_dispCallBack.m_owner = this;
	}

	CmdQueue::~CmdQueue()
	{
	}

	void CmdQueue::PostCmd(DWORD cmd, ...)
	{
		Data cmdData;

		cmdData.m_type = Data::ETYPE_CMD;
		cmdData.m_transactionID = GetTransactionID();

		if(cmdData.InTransaction())
			cmdData.m_transactionDesc = m_curTransactionDesc;

		cmdData.m_cmd = cmd;

		va_list args;
		va_start(args, cmd);

		while(1)
		{
			CmdArg& cmdArg = (va_arg(args, CmdArg));

			if(cmdArg.m_type == CmdArg::ENONE)
				break;

			//		if(!cmdArg)
			//			break;

			cmdData.m_args.push_back(cmdArg);
		}

		va_end(args);

		m_cmdDataQ.push_back(cmdData);
	}

	void CmdQueue::Report(DWORD errFlag, Data& cmdData, const char* fmt, ...)
	{
		if(!m_reportCallBack)
			return;

		char buf[MAX_PATH] = {0};
		
		if(fmt)
		{
			va_list args;
			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);
		}

		m_reportCallBack->ToolAM_CmdQueue_Report(errFlag, cmdData, buf);
	}

	void CmdQueue::Report(const char* fmt, ...)
	{
		if(!m_reportCallBack)
			return;

		char buf[MAX_PATH] = {0};

		if(fmt)
		{
			va_list args;
			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);
		}

		m_reportCallBack->ToolAM_CmdQueue_Report2(buf);
	}

	INT32 CmdQueue::DispatchCmd()
	{
		if(m_cmdDataQ.empty())
			return TOOLAM_CMDQUEUE_NO_DISPATCH;

		Data cur = m_cmdDataQ.front(); 
		m_cmdDataQ.pop_front(); // for support aborting transaction

		if(cur.m_cmd == -1)
			return 0; // skip

		m_dispCallBack.m_prm = m_prm;
		m_dispCallBack.m_cmdData = &cur;
		m_dispCallBack.m_owner = this;
		
		ECMD_PROC_RST rst = (ECMD_PROC_RST)m_cmdPFN[cur.m_cmd](&m_dispCallBack, cur.m_args);

		if(rst == ECMD_PROC_RST_FAIL)
		{
			if(cur.InTransaction())
			{
				AbortTransaction();
				
				const char* transDesc = cur.m_transactionDesc.c_str();
				if(!transDesc || !(*transDesc))
					transDesc = "NoTransDesc";

				Report("[ERR!] transaction [%s] is aborted..", transDesc);
			}
		}

		return 0;
	}

	INT32 CmdQueue::Dispatch(const char* cmdQueueName)
	{
		CmdQueue* queue = GetQ(cmdQueueName);

		if(queue)
			return queue->DispatchCmd();

		return TOOLAM_CMDQUEUE_NO_DISPATCH;
	}

	CmdQueue* CmdQueue::GetQ(const char* name)
	{
		return CmdQueueInfo::GetCmdQueue(name);
	}

	void CmdQueue::SetQueueParam(const char* cmdQueueName, void* prm)
	{
		CmdQueue* queue = GetQ(cmdQueueName);

		if(!queue)
			return;

		queue->SetParam(prm);
	}

	void CmdQueue::Add(DWORD cmd, CmdQueue::CmdProcPFN pfn)
	{
		m_cmdPFN[cmd] = pfn;
	}

	UINT32 CmdQueue::BeginTransaction(const char* transactionDesc)
	{
		m_transactionNumber++;

		if(m_transactionNumber == 0) // skip zero
			m_transactionNumber++;

		m_curTransactionID = m_transactionNumber;

		Data cmdData;

		cmdData.m_type = Data::ETYPE_TRANSACTION_BEGIN;
		cmdData.m_transactionID = GetTransactionID();
		cmdData.m_cmd = -1;

		if(transactionDesc)
			cmdData.m_transactionDesc = transactionDesc;

		if(transactionDesc)
			m_curTransactionDesc = transactionDesc;
		else
			m_curTransactionDesc.clear();

		m_cmdDataQ.push_back(cmdData);

		return GetTransactionID();
	}

	void CmdQueue::AbortTransaction()
	{
		if(m_cmdDataQ.empty())
			return;

		Data& cur = m_cmdDataQ.front();

		if(cur.m_transactionID == 0)
			return;

		RecursiveAbortTransaction(m_cmdDataQ.begin());
	}

	CmdQueue::DataQType::iterator CmdQueue::RecursiveAbortTransaction(
		CmdQueue::DataQType::iterator cur)
	{
		if(m_cmdDataQ.empty())
			return m_cmdDataQ.end();

		UINT transactionID = (*cur).m_transactionID;

		DataQType::iterator i = m_cmdDataQ.begin();
		while(i != m_cmdDataQ.end())
		{
			if((*i).m_type == Data::ETYPE_TRANSACTION_BEGIN)
			{
				i = RecursiveAbortTransaction(++i);
				continue;
			}
			else if((*i).m_type == Data::ETYPE_TRANSACTION_END)
			{
				if(transactionID == (*i).m_transactionID)
				{
					OutputDebugString("[ERR!] Bad Transaction founded..!! \n");	
					assert(0);
				}

				return m_cmdDataQ.erase(cur, i);
			}

			i++;
		}

		return m_cmdDataQ.end();
	}

	void CmdQueue::EndTransaction(UINT32 transactionID)
	{
		Data cmdData;

		m_curTransactionID = 0;
		m_curTransactionDesc.clear();

		cmdData.m_type = Data::ETYPE_TRANSACTION_END;
		cmdData.m_transactionID = GetTransactionID();
		cmdData.m_cmd = -1;

		m_cmdDataQ.push_back(cmdData);
	}

	UINT32 CmdQueue::GetTransactionID()
	{
		return m_curTransactionID;
	}


//////////////////////////////////////////////////////////////////////////
} // end of ToolAM
//////////////////////////////////////////////////////////////////////////

