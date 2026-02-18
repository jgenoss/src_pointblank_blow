#pragma once

class XmlDocumentWrapper;

class StageTable
{
	UINT8	m_ui8NationalCode;
	char	m_astrColumnName[3][MAX_STRING_COUNT];

public:
	bool	m_abBombB[STAGE_UID_MAX];
	NxVec3	m_vBombPosition_A[STAGE_UID_MAX];
	NxVec3	m_vBombPosition_B[STAGE_UID_MAX];
	REAL32	m_fJumpRunSpeed[STAGE_UID_MAX];
	REAL32	m_fFrontMoveSpeed[STAGE_UID_MAX];
	REAL32	m_rGravity[STAGE_UID_MAX];

	static StageTable m_Instance;

	StageTable(void);
	~StageTable(void);

	static StageTable&		GetInstance()			{ return m_Instance; }

	void	SetNationalCode( UINT8 u8NationalCode )
	{
		m_ui8NationalCode = u8NationalCode;
		SetNationalColumnName(u8NationalCode);
	}

	void	SetNationalColumnName(UINT8 ui8NationalCode);
	void	ParseStageInfo( IDispatch *pNode );
};

