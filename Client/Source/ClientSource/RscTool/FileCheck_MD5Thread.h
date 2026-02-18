#pragma once


// 개별 생성..  (FileCheck_ProgressMD5View에 직접 넣음)
// 목록이 먼저이고, 인터럽트로 인한 완전한 중지가 가능해야함..
// 귀찮으니..버튼 누를때 쓰레드 생성하도록 유도한다...
//
#include "i3Base/itl/queue.h"
#include "i3Base/itl/function/function.h"
#include "i3Base/itl/signal/signal_common.h"

struct FileCheck_MD5Thread_Callback
{
	virtual ~FileCheck_MD5Thread_Callback() {}	
	
	virtual void	OnRedrawBlockStart() {}

	virtual void	OnStart(size_t num_list) {}

	virtual void	OnLeftFileCheckStart(size_t index) {}
	virtual void	OnLeftFileCheckEnd(size_t index, bool bSuccess, const UINT128& md5) {}
	virtual void	OnRightFileCheckStart(size_t index) {}
	virtual void	OnRightFileCheckEnd(size_t index, bool bSuccess, bool is_enc,
										const UINT128& md5, bool is_same) {}
	
	virtual void	OnPause(size_t next_index, int next_l_r) {}
	virtual void	OnResume(size_t next_index, int next_l_r) {}
	virtual void	OnInterrupt(size_t next_index, int next_l_r) {}

	virtual void	OnEnd() {}

	virtual void	OnRedrawBlockEnd() {}
};

struct PackPathUnit;
typedef i3::shared_ptr<PackPathUnit>		PackPathUnitPtr;

struct DiffResult_PackScriptResult;
typedef i3::shared_ptr<class i3MFCThread>		i3MFCThreadPtr;

enum PathMode;
enum PackMode;

//__declspec(align(64))
class FileCheck_MD5Queue : public i3::shared_disconnect<>
{
public:
	typedef i3::function<void (FileCheck_MD5Thread_Callback*)> progress_fun_type;

	FileCheck_MD5Queue();
	~FileCheck_MD5Queue();

	void		set_curr_path_unit(const PackPathUnit* ptr);		// copy처리해야함..
	void		SetPackScriptResult(const DiffResult_PackScriptResult* res, PackMode m);


	void		Update_InSubThread();

	void		UpdateProgress_InMainThread(FileCheck_MD5Thread_Callback* cb);

	void		Start();
	void		Pause();
	void		Resume();
	void		Interrupt();

private:
	void		push_progress_function(const progress_fun_type& fun);
	bool		pop_progress_function(progress_fun_type& fun);
	bool		pop_progress_function();


	PackPathUnitPtr							m_path_unit;

	i3MFCThreadPtr							m_pthread;
	const DiffResult_PackScriptResult*		m_ps_result;
	PathMode								m_path_mode[2];
	
	i3::queue< progress_fun_type >			m_progress_queue;
	i3::cs									m_cs_progress;
	HANDLE									m_h_resume;

	volatile bool							m_interrupt;		// 일단, 64비트 캐시라인은 무시한다.
	volatile bool							m_pause;			// 자주 호출되지 않을것이라고 본다..
	volatile bool							m_idle;
};
