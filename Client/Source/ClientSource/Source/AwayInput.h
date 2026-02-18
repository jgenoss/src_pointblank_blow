#if !defined(__AWAY_INPUT_H)
#define __AWAY_INPUT_H

template<> struct is_tinst_manual<class CAwayInput> : std::tr1::true_type {};

class CAwayInput
{
public:
	static CAwayInput*	i() { return tinst<CAwayInput>();	}

	CAwayInput();
	~CAwayInput();

	void			ProcessCreateStage();
	void			Process_Update_OnCheckAwayInput(REAL32 rDeltaSeconds);

	void			SetAwayInputTime(REAL32 rAwayInputTime,REAL32 rCautionTime);

	bool			isAwayInput(void)								{ return m_bOffAwayInput ? false : m_rAwayInputTime <= 0.0f;	}
	bool			isCaution(void)									{ return m_bOffCaution ? false : m_rCautionTime <= 0.0f;		}
	void			setOffAwayInput(void)							{ m_bOffAwayInput = true;	}
	void			setOffCaution(void)								{ m_bOffCaution = true;		}
	bool			getOffAwayInput(void)							{ return m_bOffAwayInput;	}

private:
	REAL32					m_rAwayInputTime;
	REAL32					m_rCautionTime;
	bool					m_bOffAwayInput;
	bool					m_bOffCaution;

	REAL32					m_rCountDown;
	
	UIMessageBox*			m_pCautionMessageBox;
};






#endif