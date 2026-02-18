#include "i3InputType.h"
#include "i3InputDeviceManager.h"

//I3_CLASS_INSTANCE( i3InputDeviceManager, i3ElementBase);
I3_CLASS_INSTANCE( i3InputDeviceManager);

#ifndef I3_WINCE
void i3InputDeviceManager::SetKeyboard( i3InputKeyboard * pKeyboard)
{
	I3_REF_CHANGE( m_pKeyboard, pKeyboard);
}

void i3InputDeviceManager::SetMouse( i3InputMouse * pMouse)
{
	I3_REF_CHANGE( m_pMouse, pMouse);
}
#endif

void i3InputDeviceManager::SetController( INT32 idx, i3InputController * pController)
{
	if( m_pController[idx] == nullptr)
		m_ControllerCount++;

	I3_REF_CHANGE( m_pController[idx], pController);
}

// 지정된 회수만큼 업데이트를 하지 않음.
void i3InputDeviceManager::SkipUpdate( INT32 nCount )
{
	m_SkipUpdateCount = nCount;
}

void i3InputDeviceManager::Update(void)
{
	if( m_SkipUpdateCount > 0 )
	{
		m_SkipUpdateCount--;
		return;
	}

	REAL32 dt;

	dt = m_pTimer->GetDeltaSec();

#ifndef I3_WINCE
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->Update( dt);
	}

	if( m_pMouse != nullptr)
	{
		m_pMouse->Update( dt);
	}
#endif

	for( INT32 i = 0; i < I3I_CONTROLLER_MAX; i++)
	{
		if( m_pController[i] != nullptr)
		{
			m_pController[i]->Update( dt);
		}
	}

	if( m_bMapped )
	{
		UpdateMapData();
	}
}

void i3InputDeviceManager::Reset(void)
{
	if( m_bMapped )
	{
		ResetMapData();
	}


#ifndef I3_WINCE
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->Reset();
	}

	if( m_pMouse != nullptr)
	{
		m_pMouse->Reset();
	}
#endif

	for( INT32 i = 0; i < I3I_CONTROLLER_MAX; i++)
	{
		if( m_pController[i] != nullptr)
		{
			m_pController[i]->Reset();
		}
	}
}

void i3InputDeviceManager::InitCommon()
{
	m_SkipUpdateCount = 0;

	m_bMapped = false;

	m_pAnalogMap				= nullptr;
//	m_hAnalogMapMemID			= I3INVALIDHMEM;

	m_pButtonMap				= nullptr;
//	m_hButtonMapMemID			= I3INVALIDHMEM;

	m_nJoyMapIndex = 0;

	m_nAnalogMapCount = 0;
	m_nButtonMapCount = 0;
}

void i3InputDeviceManager::DestroyCommon()
{
	I3MEM_SAFE_FREE_POINTER( m_pAnalogMap );
	I3MEM_SAFE_FREE_POINTER( m_pButtonMap );
}

// Mapped Input Management
bool i3InputDeviceManager::CreateMap( INT32 nAnalogCount, INT32 nButtonCount, INT32 nJoyIndex )
{
	I3ASSERT( ! m_bMapped );

	m_nAnalogMapCount = nAnalogCount;
	m_nButtonMapCount = nButtonCount;
	m_nJoyMapIndex = nJoyIndex;

	if( m_nAnalogMapCount > 0 )
	{
		//m_pAnalogMap = (I3I_MAP_ANALOG*)I3MEM_ALLOC( m_hAnalogMapMemID, sizeof(I3I_MAP_ANALOG) * m_nAnalogMapCount );
		m_pAnalogMap = (I3I_MAP_ANALOG*)i3MemAlloc( sizeof(I3I_MAP_ANALOG) * m_nAnalogMapCount );
		I3ASSERT( m_pAnalogMap != nullptr);
	}

	if( m_nButtonMapCount > 0 )
	{
		//m_pButtonMap = (I3I_MAP_BUTTON*)I3MEM_ALLOC( m_hButtonMapMemID, sizeof(I3I_MAP_BUTTON) * m_nButtonMapCount );
		m_pButtonMap = (I3I_MAP_BUTTON*)i3MemAlloc( sizeof(I3I_MAP_BUTTON) * m_nButtonMapCount );
		I3ASSERT( m_pButtonMap != nullptr);
	}

	INT32 i;

	for( i = 0; i < m_nAnalogMapCount; i++ )
	{
		m_pAnalogMap[i].Init();
	}

	for( i = 0; i< m_nButtonMapCount; i++ )
	{
		m_pButtonMap[i].Init();
	}

	m_bMapped = true;

	return true;
}

void i3InputDeviceManager::ResetMapData()
{
	INT32 i;

	for( i = 0; i < m_nAnalogMapCount; i++ )
	{
		m_pAnalogMap[i].Reset();
	}

	for( i = 0; i< m_nButtonMapCount; i++ )
	{
		m_pButtonMap[i].Reset();
	}
}

// 아날로그 스틱에 대한 맵핑
// nIndex : 맵핑할려는 사용자키 인덱스
// nDir : 조이스틱의 키 방향
// nKey : 키보드 키
// rKeyValue : 키보드 입력시 설정된 아날로그 값
void i3InputDeviceManager::MapAnalog( INT32 nIndex, I3I_ANALOG_DIR nDir, I3I_KEY nMinusKey, I3I_KEY nPlusKey )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nAnalogMapCount );
	m_pAnalogMap[ nIndex ].nDir = nDir;
	m_pAnalogMap[ nIndex ].nMinusKey	= nMinusKey;
	m_pAnalogMap[ nIndex ].nPlusKey		= nPlusKey;
}

// 디지털 버튼에 대한 맵핑
// nIndex : 맵핑할려는 사용자키 인덱스
// nKey : 키보드 키
// nButton : 조이스틱의 버튼
void i3InputDeviceManager::MapButton( INT32 nIndex, I3I_BUTTON nButton, I3I_KEY nKey )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nButtonMapCount );
	m_pButtonMap[ nIndex ].nButton = nButton;
	m_pButtonMap[ nIndex ].nKey = nKey;
}

// nMappedButton : 디지털 버튼으로 아날로그 값을 조정시 사용될 사용자키 인덱스
// rMappedValue : 디지털 버튼의 값
void i3InputDeviceManager::MapAnalogByButton( INT32 nIndex, INT32 nMappedMinusButton, INT32 nMappedPlusButton )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nAnalogMapCount );
	m_pAnalogMap[ nIndex ].nMappedMinusButton	= nMappedMinusButton;
	m_pAnalogMap[ nIndex ].nMappedPlusButton	= nMappedPlusButton;
}

void i3InputDeviceManager::UpdateMapData()
{
	INT32 i;

	// Update by JoyPad
	if( m_pController[ m_nJoyMapIndex ] != nullptr)
	{
		i3InputController *pJoy = m_pController[ m_nJoyMapIndex ];

		// Get Analog Values
		for( i = 0; i < m_nAnalogMapCount; i++ )
		{
			if( m_pAnalogMap[i].nDir != I3I_ANALOG_DIR_UNKNOWN )
			{
				m_pAnalogMap[i].rValue = pJoy->GetAnalogValue( m_pAnalogMap[i].nDir );
				//I3TRACE( "Analog(%d) : %f\n", i, m_pAnalogMap[i].rValue );
			}
		}

		// Get Button Values
		for( i=0; i< m_nButtonMapCount; i++ )
		{
			if( m_pButtonMap[i].nButton != I3I_BTN_UNKNOWN )
			{
				if( pJoy->GetPressState() & m_pButtonMap[i].nButton )
				{
					m_pButtonMap[i].bPress = true;
					//I3TRACE( "JoyButton(%d) : %d\n", i, m_pButtonMap[i].bPress );
				}

				if( pJoy->GetStrokeState() & m_pButtonMap[i].nButton )
				{
					m_pButtonMap[i].bStroke = true;
					//I3TRACE( "JoyButton(%d) : %d\n", i, m_pButtonMap[i].bStroke );
				}
			}
		}
	}

#ifndef I3_WINCE
	// Update by Keyboard
	// 키보드의 입력이 있으면 JoyPad의 입력 값은 무시됨.
	if( m_pKeyboard != nullptr)
	{
		// Get Analog Values
		for( i = 0; i < m_nAnalogMapCount; i++ )
		{
			INT32 nDir = 0;

			// Minus
			// 올바른 키보드 키로 매핑되어 있는지 확인.
			if( m_pAnalogMap[i].nMinusKey != I3I_KEY_UNKNOWN )
			{
				// 키가 눌러 졌으면, 지정된 값을 대입
				if( m_pKeyboard->GetPressState( m_pAnalogMap[i].nMinusKey ) )
				{
					nDir--;
				}
			}

			// Plus
			// 올바른 키보드 키로 매핑되어 있는지 확인
			if( m_pAnalogMap[i].nPlusKey != I3I_KEY_UNKNOWN )
			{
				// 키가 눌러 졌으면, 지정된 값을 대입
				if( m_pKeyboard->GetPressState( m_pAnalogMap[i].nPlusKey ) )
				{
					nDir++;
				}
			}

			// 입력된 값이 있으면 설정
			if( nDir != 0 )
			{
				m_pAnalogMap[i].rValue = nDir * 1.0f;
			}
		}

		// Get Digital Button Values
		for( i = 0; i < m_nButtonMapCount; i++ )
		{
			if( m_pButtonMap[i].nKey != I3I_KEY_UNKNOWN )
			{
				// Press State
				if( m_pKeyboard->GetPressState( m_pButtonMap[i].nKey ) )
				{
					m_pButtonMap[i].bPress = true;
				}

				// Stroke State
				if( m_pKeyboard->GetStrokeState( m_pButtonMap[i].nKey ) )
				{
					m_pButtonMap[i].bStroke = true;
				}
			}
		}
	}
#endif

	// Digital 버튼으로 Analog 스틱 입력값 설정.
	for( i = 0; i < m_nAnalogMapCount; i++ )
	{
		INT32 nDir = 0;

		if( m_pAnalogMap[i].nMappedMinusButton >= 0 )
		{
			if( m_pButtonMap[ m_pAnalogMap[i].nMappedMinusButton ].bPress )
			{
				nDir--;
			}
		}
		if( m_pAnalogMap[i].nMappedPlusButton >= 0 )
		{
			if( m_pButtonMap[ m_pAnalogMap[i].nMappedPlusButton ].bPress )
			{
				nDir++;
			}
		}
		if( nDir != 0 )
		{
			m_pAnalogMap[i].rValue = nDir * 1.0f;
		}
	}
	
}

void i3InputDeviceManager::SetAccessRight( bool bGet)
{
#ifndef I3_WINCE
	if( m_pKeyboard != nullptr)
	{
		m_pKeyboard->SetAccessRight( bGet);
	}

	if( m_pMouse != nullptr)
	{
		m_pMouse->SetAccessRight( bGet);
	}
#endif

	for( INT32 i = 0; i < I3I_CONTROLLER_MAX; i++)
	{
		if( m_pController[i] != nullptr)
		{
			m_pController[i]->SetAccessRight( bGet);
		}
	}
}
