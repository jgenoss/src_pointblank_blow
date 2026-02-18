
#ifndef _CSEARCHTOOLBAR_I_H_
#define _CSEARCHTOOLBAR_I_H_


class CSearchComboBoxCallBack
{
public:
	struct Candidate
	{
		std::string					m_name;
		void*						m_arg = nullptr;
	};

	typedef std::vector<Candidate> CandidateRst;
	
	struct CandidateSorter
	{
		bool operator()(const Candidate& a, const Candidate& b) const
		{
			if(stricmp(a.m_name.c_str(), b.m_name.c_str()) < 0)
				return true;
			return false;
		}
	};

	enum EOPT
	{
		EOPT_FROM_ROOT					= 0x00000001,
		EOPT_FROM_SELECTION				= 0x00000002,
		EOPT_SEARCH_PREFIX_ONLY			= 0x00000010,
	};

	virtual INT CSearchComboBoxCallBack_CollectCandidateByText(
		CandidateRst& rst, const char* text, DWORD opt=0) = 0;	

	virtual void CSearchComboBoxCallBack_OnSelectItem(void* arg) {}
	virtual void CSearchComboBoxCallBack_SelectClosestItem(const char* itemName, DWORD opt=0) = 0;
};


#endif
