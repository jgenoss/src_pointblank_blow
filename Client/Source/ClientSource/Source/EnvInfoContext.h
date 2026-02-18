#pragma once

class EnvInfoContext : public i3::shared_ginst<EnvInfoContext>
{

//----------------------------------------------------------------------------------//
//							Environment info save system							//
//----------------------------------------------------------------------------------//

public:
	void			SetKeyMapArray( GAME_KEY_STRUCT * pArrays)
	{
		i3mem::Copy( m_KeyMapArray, pArrays, sizeof(m_KeyMapArray));
	}

	const GAME_KEY_STRUCT	* GetKeyMapArray(UINT32 idx) const
	{
		I3ASSERT_RETURN( idx < GAME_KEY_MAP_COUNT, nullptr);			
		return &m_KeyMapArray[idx]; 
	}

	const i3::rc_wstring&	GetMacro(UINT32 idx) const
	{
		I3ASSERT( idx < MACRO_MAX_COUNT);	
		return m_wstrMacro[idx];
	}

	void	SetMacro(UINT32 idx, const i3::rc_wstring& wstr)
	{
		I3ASSERT_RETURN( idx < MACRO_MAX_COUNT);
		m_wstrMacro[idx] = wstr;
	}

	void			SetNetEnvSet( const NET_ENV_SET * pData)					{ return i3mem::Copy( &m_NetEnvSet, pData, sizeof(NET_ENV_SET)); }
	const NET_ENV_SET *	GetNetEnvSet(void) const								{ return &m_NetEnvSet; }

	void			SetNetEnvSetBasicChange(UINT32 NetEnvSetBasicChange)		{ m_NetEnvSetBasicChange = NetEnvSetBasicChange; }
	UINT32			GetNetEnvSetBasicChange(void) const							{ return m_NetEnvSetBasicChange; }
	void			AddNetEnvSetBasicChange(UINT32 Flag)						{ m_NetEnvSetBasicChange |= Flag;	}
	void			RemoveNetEnvSetBasicChange(UINT32 Flag)						{ m_NetEnvSetBasicChange &= Flag;	}

	void			SetEnvSetDefault(UINT8 EnvSetDefault)						{ m_EnvSetDefault = EnvSetDefault; }
	UINT8			GetEnvSetDefault(void) const								{ return m_EnvSetDefault; }

private:

	GAME_KEY_STRUCT		m_KeyMapArray[GAME_KEY_MAP_COUNT];
	i3::rc_wstring		m_wstrMacro[MACRO_MAX_COUNT];
	NET_ENV_SET			m_NetEnvSet;											//로그인후 저장 
	UINT32				m_NetEnvSetBasicChange = 0;								//서버에 저장할때 사용		0x FF FF FF(Macro) FF(0:변화없음 1:Basic 2:Key 4:Macro)
	UINT8				m_EnvSetDefault = 0;
};