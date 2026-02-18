#ifndef __I3_GPU_COMMAND_PARSER_H
#define __I3_GPU_COMMAND_PARSER_H

//
// PSP GPU(GE)의 현재 상태를 보관하는 기능을 하는 Class.
// 실제 사용하기 위해서가 아니라, PSP Display List를 올바르게 Parsing하기 위해
// 필요한 값들을 보관하기 위한 Class이다.
//
// PSP Data Compiler에서 적절한 Display List를 생성하기 위해
// 이 Class를 사용한다.
//
class I3_EXPORT_GFX i3GPUContextPSP : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GPUContextPSP, i3ElementBase);
protected:
	// Context
	UINT32		m_Base = 0;
	UINT32		m_Offset = 0;
	UINT32		m_WorldN = 0;

	UINT32		m_pCallStack[2] = { 0, 0 };
	UINT32		m_CallCount = 0;

	UINT32		*m_pCommandBuffer = nullptr;
	UINT32		*m_pCur = nullptr;

	// Members
	bool		m_bReport = false;

public:
	UINT32		GetBASE(void)			{ return m_Base; }
	UINT32		GetOFFSET(void)			{ return m_Offset; }
	UINT32		GetWORLDN(void)			{ return m_WorldN; }

	UINT32 *	GetBuffer(void)			{ return m_pCommandBuffer; }

	// BufferLength가 -1이면 I3G_PSP_CMD_END을 만날 때까지 파싱을 한다.
	void		Parse( UINT32 * pBuffer, INT32 BufferLength = -1 );
};

#endif //__I3_GPU_COMMAND_PARSER_H
