
#ifndef _I3OPTRENATTR_H_
#define _I3OPTRENATTR_H_


// 
// remove dupplicated i3RenderAttr derived class
//

#include "i3SceneOptimizer.h"


class I3_EXPORT_OPT i3OptRenAttr : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE(i3OptRenAttr, i3SceneOptimizer);

public:
	// implements i3SceneOptimizer
	virtual void			Trace( i3Node * pRoot) override;
	virtual void			SetProperty( char * pszFieldName, char * pszValue) override;
	virtual const char *	getDesc(void) override;
	virtual bool			isSupportProgress(void) override;

	// i3OptRenAttr
	i3OptRenAttr();
	virtual ~i3OptRenAttr();

	struct I3_EXPORT_OPT OptiRst
	{
		OptiRst() {}
		void Clear();

		OptiRst operator+(const OptiRst& rhs) const;
		OptiRst& operator=(const OptiRst& rhs);

		INT32		m_numPass = 0;
		INT32		m_numAttrSet[2] = { 0, 0 };		// prev/optimized
		INT32		m_numRenAttr[2] = { 0, 0 };		// prev/optimized
		
		INT32		m_numCancelOper = 0;

		enum EOPER_RST_FLAG
		{
			ORF_NOTHING_IS_OPTIMIZED		= 0x00000001,
			ORF_OPTIMIZED_AND_SAVED			= 0x00000010,

			ORF_ERR_DUPPLICATED_RENATTR		= 0x81000000,
			ORF_ERR_INTERNAL_OPERATION		= 0x82000000,
			ORF_ERR_FAILED_SAVE_OPTIMIZED	= 0x80100000,
		};

		UINT32		m_operRstFlag = 0;
		void		(*m_errOutput)(const char* errText) = nullptr;

		bool IsError()
		{
			return (m_operRstFlag & 0x80000000) ? true : false;
		}

		bool IsOptimized()
		{
			return IsError() ? false : true;
		}

		bool IsNothingOptimized() 
		{ 
			if(IsError())
				return true;

			return m_operRstFlag & ORF_NOTHING_IS_OPTIMIZED ? true : false;
		}

		bool IsAttrSetRenAttrOptimized()
		{
			if( (m_numAttrSet[0] == m_numAttrSet[1]) && 
					(m_numRenAttr[0] == m_numRenAttr[1]) )
				return false;

			return true;
		}	
	};

	bool DoOptimize(i3SceneGraphInfo* inScene, OptiRst* rst= nullptr);
	bool DoOptimize(i3Node* inStartNode, OptiRst* rst= nullptr);
	bool DoOptimize(const char* i3sfilePath, OptiRst* rst = nullptr,
		bool saveI3SOnOptiOk=false);

	i3OptRenAttr::OptiRst& GetOptiRst() { return m_rst; }
	bool GetOptimized() { return ( m_rst.m_operRstFlag & 0x80000000) ? false : true; }

protected:
	bool _DoOptimize(i3Node* inStartNode, OptiRst* rst= nullptr);

	OptiRst					m_rst;
	bool					m_bOptimized;

public:
	bool					m_supportProgress;
	INT32					m_numMaximumPass;
};


#endif