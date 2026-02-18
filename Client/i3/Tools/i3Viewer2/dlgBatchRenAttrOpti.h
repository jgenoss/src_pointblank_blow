
#ifndef _DLGBATCHRENATTROPTI_H_
#define _DLGBATCHRENATTROPTI_H_

#include "resource.h"

#include <string>
#include <map>
#include <hash_map>


class i3OptRenAttr;

class dlgBatchRenAttrOpti : public CDialog
{
public:
	dlgBatchRenAttrOpti();
	virtual ~dlgBatchRenAttrOpti();

	enum { IDD = IDD_BATCH_RENATTR_OPTI };

	struct Statitics
	{
		INT32			m_numAttrSet = 0;
		INT32			m_numRenAttr = 0;
		INT32			m_numCancel = 0;
	};

	struct OptiRst
	{
		OptiRst();

		enum ESTATE
		{
			ESTATE_OPTIMIZE_FAILED = -1,
			ESTATE_READY_FOR_OPTIMIZE = 0,
			ESTATE_OPTIMIZED = 1,			
			ESTATE_MAX
		};

		// 0:ready for opti, 1:optimized, -1: err in optimization
		INT32								m_state = 0;		
		typedef std::list<std::string>		StateDesc;
		StateDesc							m_stateDesc;
		UINT32								m_errCode = 0;

		std::string							m_basePath;
		std::string							m_filename;

		Statitics							m_statitics[2];	//[0] before, [1] after
		float								m_attrSetOptiRatio = 0.0f;
		float								m_renAttrOptiRatio = 0.0f;
	};

	// <dir, rst>
	typedef std::multimap<std::string, OptiRst* >		OptiRstRow;

	void Clean();
	void PumpMessage();

	void Log(const char* fmt, ...);

	virtual BOOL OnInitDialog() override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;

	INT32 Scani3S(const char* path);

	struct FoundFileInserter
	{
		FoundFileInserter(OptiRstRow& out);

		bool isSkipDir(const char* path);
		bool TestFilter(const char* filename);
		void AddFilePath(const char* basePath, const char* fileName);		

		OptiRstRow&				m_out;
	};

	INT32 Collecti3sFile(FoundFileInserter& inserter, const char* path, 
		bool recSubDirScan);

	void Optimize(OptiRst* rst);

	void SetProgressRange(INT32 rangeMax);
	void SetProgressPos(INT32 pos);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStartOptimize();
	afx_msg void OnWriteResult();
	afx_msg void OnClose();

protected:

	CEdit							m_workPath;
	CEdit							m_subPath;
	CButton							m_btnOptimize;
	CListBox						m_log;
	CButton							m_saveI3SOnOptiOk;

	OptiRstRow						m_row;
	i3OptRenAttr*					m_optimizer;
	bool							m_continue;

	char							m_oldWorkPath[MAX_PATH];


};


#endif
