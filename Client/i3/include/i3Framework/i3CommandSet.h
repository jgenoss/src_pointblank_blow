#if !defined( __I3_COMMAND_SET_H)
#define __I3_COMMAND_SET_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct I3_COMMAND_INFO
{
	char		m_szName[64];					// 파일에서 읽어들이기 때문에 변경이 불가능하다. (2012.11.21.수빈)
												// 가능하게 하려면 이 구조체를 PERSIST로 바꾸고, 읽어들일때 새형식 구조체로 카피해야한다.
	UINT32		m_ID;

	UINT32		m_InputDevice;
	UINT32		m_Input;

	UINT32		m_Group;
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

typedef INT32		I3_HCMD;

#define		I3_INVALID_CMD		((I3_HCMD) -1)

#define		I3_CMD_INPUTFLAG_MASK				0xFFFF0000
#define		I3_CMD_INPUTCODE_MASK				0x0000FFFF

#define		I3_CMD_INPUTFLAG_STROKE				0x40000000
#define		I3_CMD_INPUTFLAG_SHIFT				0x20000000
#define		I3_CMD_INPUTFLAG_CTRL				0x10000000
#define		I3_CMD_INPUTFLAG_ALT				0x08000000
#define		I3_CMD_INPUTFLAG_MOUSE_WHEEL		0x04000000


class I3_EXPORT_FRAMEWORK i3CommandSet : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE( i3CommandSet, i3ResourceObject);
protected:
	i3::vector<I3_COMMAND_INFO>	m_CmdSetList;			// 크기가 자라나는 배열 취지에 맞춰서 i3::vector로 변경하면 됨.

	char					m_KeyPressState[ I3I_KEY_MAX];
	char					m_KeyStrokeState[ I3I_KEY_MAX];
	UINT32					m_MousePressState;
	UINT32					m_MouseStrokeState;

protected:

	void					SetInput( I3_HCMD hCmd, I3I_DEVICE_TYPE device, UINT32 input);

	UINT32					_GetInputCode( UINT32 input, bool bStroke, bool bShift, bool bCtrl, bool bAlt);

	bool					OnSaveCommandXML( i3XMLElement * pXML, INT32 idx);
	bool					OnLoadCommandXML( i3XMLElement * pXML, INT32 idx);

	bool					CheckCommonKeys( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager);
	bool					CheckKeyboardInput( I3_HCMD hCmd, i3InputDeviceManager * pManager);
	bool					CheckMouseInput( I3_HCMD hCmd, i3InputDeviceManager * pManager);

	void					ClearInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager);
	void					ClearKeyboardInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager);
	void					ClearMouseInput( const I3_COMMAND_INFO * pInfo, i3InputDeviceManager * pManager);

public:
	i3CommandSet(void);
	virtual ~i3CommandSet(void);

	INT32					GetCount(void) const	{ return m_CmdSetList.size(); }
	void					SetCount( INT32 cnt);

	I3_HCMD					Add( const char * pszName);
	void					Remove( I3_HCMD hCmd);
	const I3_COMMAND_INFO *	Get( I3_HCMD hCmd);
	void					Set( I3_HCMD hCmd, I3_COMMAND_INFO * pInfo);

	void					SetInputAsMouse( I3_HCMD hCmd, UINT32 buttons, bool bStroke, bool bShift = false, bool bCtrl = false, bool bAlt = false);
	void					SetInputAsKeyboard( I3_HCMD hCmd, I3I_KEY key1, I3I_KEY key2, bool bStroke, bool bShift = false, bool bCtrl = false, bool bAlt = false);

	I3_HCMD					Find( const char * pszName);
	I3_HCMD					Find( UINT32 id);
	I3_HCMD					FindByMouse( UINT32 button, bool bStroke, bool bShift, bool bCtrl, bool bAlt);
	I3_HCMD					FindByKeyboard( I3I_KEY key1, I3I_KEY key2, bool bStroke, bool bShift, bool bCtrl, bool bAlt);

	bool					IsInputed( I3_HCMD cmd, i3InputDeviceManager * pManager);
	void					BeginCheck( i3InputDeviceManager * pManager);
	void					EndCheck(void);

	virtual UINT32			OnSave( i3ResourceFile * pFile);
	virtual UINT32			OnLoad( i3ResourceFile * pFile);
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	static UINT32			GenCode( const char * pszName);
	static void				GetDeviceString( UINT32 device, char * pszStr, INT32 len);
	static void				GetInputString( UINT32 device, UINT32 input, char * pszStr, INT32 len);

#if defined( I3_DEBUG)
	void					OnQuery_AllCommands( i3::vector<i3::rc_string>& List);
#endif
};

#endif
