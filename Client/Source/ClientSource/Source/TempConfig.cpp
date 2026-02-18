#include "pch.h"
#include "TempConfig.h"

CTempConfig * g_pTempConfig = nullptr;

//I3_CLASS_INSTANCE( CTempConfig, i3ElementBase);
I3_CLASS_INSTANCE( CTempConfig);

CTempConfig::CTempConfig(void)
{
	INT32 i = 0;
	for( i=0; i<MAX_AI_COUNT+1; i++)
	{
		m_Speed[i] = 0.0f;
		m_Damage[i] = 0.0f;
		m_Weapon[i] = 0;
	}

	for( i=0; i<EVADE_TYPE_COUNT; i++)
	{
		m_Evade[i] = 0;
	}

	m_RespawnTime = 0.0f;
	m_JumpHeight = 0.0f;
	m_JumpTime = 0.0f;

	Load();
}

CTempConfig::~CTempConfig(void)
{
}

void CTempConfig::Load()
{
	i3IniParser Parser;

	if( !Parser.OpenFromFile( "config/DieHardMode_Ai.ini"))
	{
		return;
	}
	 
	if( Parser.ReadSection("Default"))
	{	
		INT32 i = 0;
		char Temp[MAX_PATH];
		
		for( i=MAX_AI_COUNT; i>=0; i--)
		{
			sprintf_s( Temp, "Difficulty_%d_Speed", i);
			Parser.GetValue( Temp,	&m_Speed[i]);

			sprintf_s( Temp, "Difficulty_%d_Damage", i);
			Parser.GetValue( Temp,	&m_Damage[i]);

			sprintf_s( Temp, "Difficulty_%d_Weapon", i);
			Parser.GetValue( Temp,	&m_Weapon[i]);
		}

		for( i=0; i<4; i++)
		{
			sprintf_s( Temp, "Player_Count_%d", i+1);
			Parser.GetValue( Temp,	&m_PlayerCountPerDamage[i]);
		}

		for( i=0; i<EVADE_TYPE_COUNT; i++)
		{
			sprintf_s( Temp, "Evade_%d", i+1);
			Parser.GetValue( Temp,	&m_Evade[i]);
		}

		Parser.GetValue( "RespawnTime",	&m_RespawnTime);

		Parser.GetValue( "Jump_Height",	&m_JumpHeight);

		Parser.GetValue( "Jump_Time",	&m_JumpTime);
	}

	Parser.Close();

	return;
}
