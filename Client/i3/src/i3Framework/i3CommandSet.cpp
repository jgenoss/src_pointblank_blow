#include "i3FrameworkPCH.h"
#include "i3CommandSet.h"
#include "i3Framework.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_cat.h"

//I3_CLASS_INSTANCE( i3CommandSet, i3ResourceObject);
I3_CLASS_INSTANCE( i3CommandSet);

#define	KEY1( key)						((I3I_KEY)((key >> 8) & 0x000000FF))
#define KEY2( key)						((I3I_KEY)(key & 0x000000FF))

static UINT32	COMBINE_KEY( I3I_KEY key1, I3I_KEY key2)
{
	if( key1 > key2)
	{
		I3I_KEY temp = key2;
		key2 = key1;
		key1 = temp;
	}

	return (((key1 << 8) & 0x0000FF00) | (key2 & 0x000000FF));
}

i3CommandSet::i3CommandSet(void)
{
	INT32 i;
	for( i = 0; i < I3I_KEY_MAX; i++)
	{
		m_KeyPressState[i] = 0;
		m_KeyStrokeState[i] = 0;
	}
	m_MousePressState = 0;
	m_MouseStrokeState = 0;
}

i3CommandSet::~i3CommandSet(void)
{

}



UINT32 i3CommandSet::GenCode( const char * pszName)
{
	return CRC32( 0xFFFFFFFF, (UINT8 *) pszName, i3::generic_string_size( pszName) * sizeof(char));
}

I3_HCMD	i3CommandSet::Add( const char * pszName)
{
	const size_t idx = m_CmdSetList.size();
	
	m_CmdSetList.resize(idx + 1);

	I3_COMMAND_INFO * pInfo = &m_CmdSetList.back();

	i3::safe_string_copy( pInfo->m_szName, pszName, sizeof( pInfo->m_szName));
	pInfo->m_ID = GenCode( pszName);
	pInfo->m_InputDevice = 0;
	pInfo->m_Input = 0;
	pInfo->m_Group = 0;

	return (I3_HCMD) idx;
}

void i3CommandSet::Remove( I3_HCMD hCmd)
{
	if ( hCmd < 0 || (INT32)hCmd >= (INT32)m_CmdSetList.size() )
		return;
	
	m_CmdSetList.erase( m_CmdSetList.begin() + (INT32)hCmd );
}

const I3_COMMAND_INFO *	i3CommandSet::Get( I3_HCMD hCmd)
{
	I3_BOUNDCHK( (INT32) hCmd, (INT32)m_CmdSetList.size() );

	return &m_CmdSetList[ (INT32) hCmd];
}

void i3CommandSet::SetCount( INT32 cnt)
{
	if( cnt > (INT32)m_CmdSetList.size() )
		m_CmdSetList.resize(cnt);
}

void i3CommandSet::Set( I3_HCMD hCmd, I3_COMMAND_INFO * pInfo)
{
	I3_BOUNDCHK( (INT32) hCmd, INT32(m_CmdSetList.size()) );

	i3mem::Copy( & m_CmdSetList[ (INT32) hCmd], pInfo, sizeof( I3_COMMAND_INFO));
}

UINT32 i3CommandSet::_GetInputCode( UINT32 input, bool bStroke, bool bShift, bool bCtrl, bool bAlt)
{
	UINT32 code = input;

	if( bStroke)
		code |= I3_CMD_INPUTFLAG_STROKE;

	if( bShift)
		code |= I3_CMD_INPUTFLAG_SHIFT;

	if( bCtrl)
		code |= I3_CMD_INPUTFLAG_CTRL;

	if( bAlt)
		code |= I3_CMD_INPUTFLAG_ALT;

	return code;
}

void i3CommandSet::SetInput( I3_HCMD hCmd, I3I_DEVICE_TYPE device, UINT32 input)
{
	I3_BOUNDCHK( (INT32) hCmd,  INT32(m_CmdSetList.size()));

	m_CmdSetList[ (INT32) hCmd].m_InputDevice = device;
	m_CmdSetList[ (INT32) hCmd].m_Input = input;
}

void i3CommandSet::SetInputAsMouse( I3_HCMD hCmd, UINT32 input, bool bStroke, bool bShift, bool bCtrl, bool bAlt)
{
	SetInput( hCmd, I3I_DEVICE_MOUSE, _GetInputCode( input, bStroke, bShift, bCtrl, bAlt));
}

void i3CommandSet::SetInputAsKeyboard( I3_HCMD hCmd, I3I_KEY key1, I3I_KEY key2, bool bStroke, bool bShift, bool bCtrl, bool bAlt)
{
	UINT32 key;

	key = COMBINE_KEY( key1, key2);

	SetInput( hCmd, I3I_DEVICE_KEYBOARD, _GetInputCode( (UINT32) key, bStroke, bShift, bCtrl, bAlt));
}

I3_HCMD i3CommandSet::Find( const char * pszName)
{
	UINT32	code;

	code = GenCode( pszName);

	return Find( code);	
}

I3_HCMD i3CommandSet::Find( UINT32 id)
{

	for(size_t i = 0; i < m_CmdSetList.size() ; i++)
	{
		if( m_CmdSetList[i].m_ID == id)
			return (I3_HCMD) i;
	}

	return I3_INVALID_CMD;
}

I3_HCMD	 i3CommandSet::FindByMouse( UINT32 button, bool bStroke, bool bShift, bool bCtrl, bool bAlt)
{

	UINT32 input;

	if( m_CmdSetList.empty() ) 
		return I3_INVALID_CMD;

	input = _GetInputCode( button, bStroke, bShift, bCtrl, bAlt);

	for(size_t i = 0; i < m_CmdSetList.size() ; i++)
	{
		if( m_CmdSetList[i].m_InputDevice != I3I_DEVICE_MOUSE)
			continue;

		if( m_CmdSetList[i].m_Input == input)
			return (I3_HCMD) i;
	}

	return I3_INVALID_CMD;
}

I3_HCMD	 i3CommandSet::FindByKeyboard( I3I_KEY key1, I3I_KEY key2, bool bStroke, bool bShift, bool bCtrl, bool bAlt)
{
	
	UINT32 input, key;

	if( m_CmdSetList.empty() )
		return I3_INVALID_CMD;

	key = COMBINE_KEY( key1, key2);

	input = _GetInputCode( (UINT32) key, bStroke, bShift, bCtrl, bAlt);

	for(size_t i = 0; i < m_CmdSetList.size(); i++)
	{
		if( m_CmdSetList[i].m_InputDevice != I3I_DEVICE_KEYBOARD)
			continue;

		if( m_CmdSetList[i].m_Input == input)
			return (I3_HCMD) i;
	}

	return I3_INVALID_CMD;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct I3_PERSIST_COMMANDSET
{
	INT8		m_ID[4];
	INT32		m_Count;
	UINT32		pad[8];
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3CommandSet::OnSave( i3ResourceFile * pFile)
{
	UINT32 rc, result;
	I3_PERSIST_COMMANDSET data;

	result = i3ResourceObject::OnSave( pFile);
	I3_CHKIO( result);

	i3mem::FillZero( &data, sizeof(data));

	i3mem::Copy( data.m_ID, "CDS1", 4);
	data.m_Count = INT32(m_CmdSetList.size());

	rc = pFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( m_CmdSetList.size() > 0)
	{
		rc = pFile->Write( &m_CmdSetList[0], sizeof( I3_COMMAND_INFO) * m_CmdSetList.size() );
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32 i3CommandSet::OnLoad( i3ResourceFile * pFile)
{
	UINT32 rc, result;
	I3_PERSIST_COMMANDSET data;

	result = i3ResourceObject::OnLoad( pFile);
	I3_CHKIO( result);

	rc = pFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_Count > 0)
	{
		m_CmdSetList.resize( data.m_Count );
				
		rc = pFile->Read( &m_CmdSetList[0], sizeof( I3_COMMAND_INFO) * m_CmdSetList.size() );
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

bool i3CommandSet::OnSaveCommandXML( i3XMLElement * pXML, INT32 idx)
{
	i3XMLElement * pChild;
	I3_COMMAND_INFO * pInfo;
	char	szName[256];

	i3::snprintf( szName, sizeof( szName), "CMD%d", idx);

	pChild = new i3XMLElement( szName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);

	pInfo = &m_CmdSetList[idx];

	if( pChild != NULL)
	{
		pChild->addAttr( "Name", pInfo->m_szName);
		pChild->addAttr( "Device", pInfo->m_InputDevice);
		pChild->addAttr( "Input", pInfo->m_Input);
	}

	return true;
}

bool i3CommandSet::OnLoadCommandXML( i3XMLElement * pXML, INT32 idx)
{
	I3_COMMAND_INFO * pInfo;
	i3XMLElement * pChild;
	char szName[ 256];

	i3::snprintf( szName, sizeof( szName), "CMD%d", idx);

	pChild = (i3XMLElement *) pXML->FirstChild( szName);
	
	if( pChild != NULL)
	{
		pInfo = &m_CmdSetList[idx];

		i3::safe_string_copy( pInfo->m_szName, pChild->getAttr( "Name"), sizeof( pInfo->m_szName));
		pChild->getAttr( "Device", &pInfo->m_InputDevice);
		pChild->getAttr( "Input", &pInfo->m_Input);

		pInfo->m_ID = GenCode( pInfo->m_szName);
	}
	else
	{
		I3TRACE( "XML : Could not found %s::%s node\n", pXML->GetText(), szName);
	}

	return true;
}

bool i3CommandSet::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	
	pXML->addAttr( "Version", 1);
	pXML->addAttr( "Count",	(INT32)m_CmdSetList.size()); 

	for(size_t i = 0; i < m_CmdSetList.size(); i++)
	{
		OnSaveCommandXML( pXML, i);
	}

	return true;
}

bool i3CommandSet::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 count, i;

	pXML->getAttr( "Count", &count);

	if( count > 0)
	{
		m_CmdSetList.resize(count);
	
		for( i = 0; i < count; i++)
		{
			OnLoadCommandXML( pXML, i);
		}
	}

	return true;
}

void i3CommandSet::GetDeviceString( UINT32 device, char * pszStr, INT32 len)
{
	static char	s_InputDevice[I3I_DEVICE_MAX][128] =
	{
		"Unknown",			// I3I_DEVICE_UNKNOWN,
		"Keyboard",		// I3I_DEVICE_KEYBOARD,
		"Mouse",			// I3I_DEVICE_MOUSE,
		"Controller",		// I3I_DEVICE_CONTROLLER,
		"Wheel",			// I3I_DEVICE_WHEEL,
		"GunCon"			// I3I_DEVICE_GUNCON,
	};

	i3::string_ncopy_nullpad( pszStr, s_InputDevice[ device], len);
	pszStr[len+1] = 0;
}

static void AddToken( char * pszStr, const char * pszToken)
{
	if( pszStr[0] != 0)
	{
		i3::generic_string_cat( pszStr, " + ");
	}

	i3::generic_string_cat( pszStr, pszToken);

}

static void _GetStringFlag( INT32 input, char * pszStr, INT32 len)
{
	if( input & (I3_CMD_INPUTFLAG_SHIFT | I3_CMD_INPUTFLAG_CTRL | I3_CMD_INPUTFLAG_ALT))
	{
		i3::string_ncopy_nullpad( pszStr, "[", len);

		if( input & I3_CMD_INPUTFLAG_SHIFT)
			i3::generic_string_cat( pszStr, "Shift ");

		if( input & I3_CMD_INPUTFLAG_CTRL)
			i3::generic_string_cat( pszStr, "Ctrl ");

		if( input & I3_CMD_INPUTFLAG_ALT)
			i3::generic_string_cat( pszStr, "Alt");

		i3::generic_string_cat( pszStr, "]");
	}
	else
	{
		pszStr[0] = 0;
	}
}

static void _GetStringMouseInput( INT32 input, char * pszStr, INT32 len)
{
	UINT32 code = input & ~I3_CMD_INPUTFLAG_MASK;

	_GetStringFlag( input, pszStr, len);

	if( code & I3I_MOUSE_LBUTTON)
		AddToken( pszStr, "L Button");

	if( code & I3I_MOUSE_MBUTTON)
		AddToken( pszStr, "M Button");

	if( code & I3I_MOUSE_RBUTTON)
		AddToken( pszStr, "R Button");

	if( input & I3_CMD_INPUTFLAG_STROKE)
		i3::generic_string_cat( pszStr, " (Stroke)");
}

static void _GetStringKeyboardInput( INT32 input, char * pszStr, INT32 len)
{
	//UINT32 code = input & ~I3_CMD_INPUTFLAG_MASK;

	_GetStringFlag( input, pszStr, len);

	I3I_KEY key1, key2;

	key1 = KEY1( input);
	key2 = KEY2( input);

	if( key1 != I3I_KEY_UNKNOWN)
		AddToken( pszStr, i3InputKeyboard::GetKeyName( key1));
	
	if( key2 != I3I_KEY_UNKNOWN)
		AddToken( pszStr, i3InputKeyboard::GetKeyName( key2));

	if( input & I3_CMD_INPUTFLAG_STROKE)
		i3::generic_string_cat( pszStr, " (Stroke)");
}

void i3CommandSet::GetInputString( UINT32 device, UINT32 input, char * pszStr, INT32 len)
{
	if( input == (UINT32) I3_INVALID_CMD)
	{
		i3::safe_string_copy( pszStr, "N/A", len);
	}
	else
	{
		switch( device)
		{
			case I3I_DEVICE_MOUSE :		_GetStringMouseInput( input, pszStr, len);	break;
			case I3I_DEVICE_KEYBOARD :	_GetStringKeyboardInput( input, pszStr, len);	break;
			default :					i3::safe_string_copy( pszStr, "Unknown", len);	break;
		}
	}
}

bool i3CommandSet::CheckCommonKeys( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager)
{
	if( pInfo->m_Input & I3_CMD_INPUTFLAG_SHIFT)
	{
		if( ! m_KeyPressState[ I3I_KEY_LSHIFT] && ! m_KeyPressState[ I3I_KEY_RSHIFT])
			return false;
	}

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_CTRL)
	{
		if( ! m_KeyPressState[ I3I_KEY_LCTRL] && ! m_KeyPressState[ I3I_KEY_RCTRL])
			return false;
	}

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_ALT)
	{
		if( ! m_KeyPressState[ I3I_KEY_LALT] && ! m_KeyPressState[ I3I_KEY_RALT])
			return false;
	}

	return true;
}

bool i3CommandSet::CheckKeyboardInput( I3_HCMD hCmd, i3InputDeviceManager * pManager)
{
	const I3_COMMAND_INFO * pInfo = Get( hCmd);
	I3I_KEY key1, key2;

	if( CheckCommonKeys( pInfo, pManager) == false)
		return false;

	key1 = KEY1( pInfo->m_Input);
	key2 = KEY2( pInfo->m_Input);

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_STROKE)
	{
		if((key1 != I3I_KEY_UNKNOWN) && ( m_KeyStrokeState[ key1] == FALSE))
			return false;

		if((key2 != I3I_KEY_UNKNOWN) && ( m_KeyStrokeState[ key2] == FALSE))
			return false;

  		return true;
	}

	if((key1 != I3I_KEY_UNKNOWN) && ( m_KeyPressState[ key1] == FALSE))
			return false;

	if((key2 != I3I_KEY_UNKNOWN) && ( m_KeyPressState[ key2] == FALSE))
		return false;

	return true;
}

bool i3CommandSet::CheckMouseInput( I3_HCMD hCmd, i3InputDeviceManager * pManager)
{
	const I3_COMMAND_INFO * pInfo = Get( hCmd);
	UINT32 buttons, code;

	if( CheckCommonKeys( pInfo, pManager) == false)
		return false;

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_STROKE)
		buttons = m_MouseStrokeState;
	else
		buttons = m_MousePressState;

	code = pInfo->m_Input & ~I3_CMD_INPUTFLAG_MASK;

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_MOUSE_WHEEL)
	{
		if( code == 0)
			return (buttons & I3I_MOUSE_WHEELDOWN) != 0;

		return (buttons & I3I_MOUSE_WHEELUP) != 0;
	}

	return ((buttons & code) == code);
}

void i3CommandSet::BeginCheck( i3InputDeviceManager * pManager)
{
	i3InputKeyboard * pKeyboard = pManager->GetKeyboard();
	i3InputMouse * pMouse = pManager->GetMouse();
	INT32 i;

	for( i = 0; i < I3I_KEY_MAX; i++)
	{
		m_KeyPressState[i] = (char) pKeyboard->GetPressState( (I3I_KEY) i);
		m_KeyStrokeState[i] = (char) pKeyboard->GetStrokeState( (I3I_KEY) i);
	}

	m_MousePressState = pMouse->GetButtonState();
	m_MouseStrokeState = pMouse->GetStrokeButtonState();
}

void i3CommandSet::EndCheck(void)
{
}

bool i3CommandSet::IsInputed( I3_HCMD cmd, i3InputDeviceManager * pManager)
{
	const I3_COMMAND_INFO * pInfo = Get( cmd);
	bool bRv = false;

	switch( pInfo->m_InputDevice)
	{
		case I3I_DEVICE_KEYBOARD :	bRv = CheckKeyboardInput( cmd, pManager); break;
		case I3I_DEVICE_MOUSE :		bRv = CheckMouseInput( cmd, pManager); break;
	}

	if( bRv)
		ClearInput( pInfo, pManager);

	return bRv;
}

void i3CommandSet::ClearInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager)
{
	switch( pInfo->m_InputDevice)
	{
		case I3I_DEVICE_KEYBOARD :	return ClearKeyboardInput( pInfo, pManager);
		case I3I_DEVICE_MOUSE :		return ClearMouseInput( pInfo, pManager);
	}
}

void i3CommandSet::ClearKeyboardInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager)
{
	I3I_KEY key1, key2;

	key1 = KEY1( pInfo->m_Input);
	key2 = KEY2( pInfo->m_Input);

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_STROKE)
	{
		if( key1 != I3I_KEY_UNKNOWN)
			m_KeyStrokeState[ key1] = FALSE;

		if( key2 != I3I_KEY_UNKNOWN)
			m_KeyStrokeState[ key2] = FALSE;
	}
	//else
	{
		if( key1 != I3I_KEY_UNKNOWN)
			m_KeyPressState[ key1] = FALSE;

		if( key2 != I3I_KEY_UNKNOWN)
			m_KeyPressState[ key2] = FALSE;
	}
}

void i3CommandSet::ClearMouseInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager)
{
	UINT32 buttons, code;

	code = pInfo->m_Input & ~I3_CMD_INPUTFLAG_MASK;

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_STROKE)
		buttons = m_MouseStrokeState;
	else
		buttons = m_MousePressState;
	
	if( pInfo->m_Input & I3_CMD_INPUTFLAG_MOUSE_WHEEL)
	{
		if( code == 0)
			buttons &= ~I3I_MOUSE_WHEELDOWN;
		else
			buttons &= ~I3I_MOUSE_WHEELUP;
	}
	else
	{
		buttons &= ~code;
	}

	if( pInfo->m_Input & I3_CMD_INPUTFLAG_STROKE)
	{
		m_MouseStrokeState = buttons;
	}
	else
	{
		m_MousePressState = buttons;
	}
}

#if defined( I3_DEBUG)
void i3CommandSet::OnQuery_AllCommands( i3::vector<i3::rc_string>& List)
{
	
	for(INT32 i = 0; i < GetCount(); i++)
	{
		List.push_back( m_CmdSetList[i].m_szName);
	}
}

#endif