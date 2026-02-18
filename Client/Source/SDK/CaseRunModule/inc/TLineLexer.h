#pragma once

namespace CaseRun
{
	template <class T>
	class TLineLexer
	{
		const T*	m_pszMaster;
		int	m_nCur;
		T		m_WhiteCodes[256];

		static const int nDefWhiteCodes = 5;
		T _DefWhiteCodes[nDefWhiteCodes + 1];// = { (T)0x20, (T)'\n', (T)'\r', (T)'\t', (T)'\b', 0};	// 0x20: Space

		bool	IsWhiteCode(T Code);
		bool	IsSeparator(T Code, const T* SepArr);

	public:
		TLineLexer(void)
		{
			_DefWhiteCodes[0] = ' ';
			_DefWhiteCodes[1] = '\n';
			_DefWhiteCodes[2] = '\r';
			_DefWhiteCodes[3] = '\t';
			_DefWhiteCodes[4] = '\n';
			_DefWhiteCodes[5] = 0;

			SetMasterString(NULL);
		}

		virtual ~TLineLexer(void) {};

		void	SetMasterString(const T* str)
		{
			m_pszMaster = str;
			m_nCur = 0;
			memcpy(m_WhiteCodes, _DefWhiteCodes, sizeof(T)*(nDefWhiteCodes + 1));
			//I3TRACE("TLineLexer::%d\n", sizeof(T));
		}

		const T*		GetMasterString(void) const { return m_pszMaster; }

		int	GetCurrentPosition(void) { return m_nCur; }

		int	GetNextToken(T* Buf, const T * Separators, T* pLastSep = nullptr);
	};

	template <class T>
	bool TLineLexer<T>::IsSeparator(T Code, const T* SepArr)
	{
		while (*SepArr != 0) {
			if (*SepArr == Code)
				return true;
			SepArr++;
		}
		return false;
	}

	template <class T>
	bool TLineLexer<T>::IsWhiteCode(T Code)
	{
		T* pTemp = m_WhiteCodes;

		while (*pTemp) {
			if (*pTemp == Code)
				return true;
			pTemp++;
		}
		return false;
	}

	template <class T>
	INT32 TLineLexer<T>::GetNextToken(T* Buf, const T* Seperators, T* pchSep)
	{
		int Count = 0;

		// Skip white space
		while ((m_pszMaster[m_nCur] != 0) && (IsWhiteCode(m_pszMaster[m_nCur]) == true))
		{
			m_nCur++;
		}

		// End of string
		if (m_pszMaster[m_nCur] == 0)
		{
			if(pchSep != nullptr)
				*pchSep = 0;
			return -1;
		}


		while (m_pszMaster[m_nCur] != 0)
		{
			if (IsSeparator(m_pszMaster[m_nCur], Seperators) == true)
				break;

			*Buf = m_pszMaster[m_nCur];

			Buf++;
			m_nCur++;
			Count++;
		}

		*Buf = 0;

		if (IsSeparator(m_pszMaster[m_nCur], Seperators) == true)
		{
			if (pchSep != nullptr)
				*pchSep = m_pszMaster[m_nCur];
			m_nCur++;
			Count++;
		}
		else
		{
			if (pchSep != nullptr)
				*pchSep = 0;
		}

		return Count;
	}

}