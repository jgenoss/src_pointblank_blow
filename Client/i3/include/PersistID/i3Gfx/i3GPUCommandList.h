#if !defined( __I3_GPU_COMMAND_LIST_H)
#define __I3_GPU_COMMAND_LIST_H

#include "i3Base.h"
#include "i3GfxResource.h"

class I3_EXPORT_GFX i3GPUCommandList : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3GPUCommandList);
protected:
	// Persistent Members
	I3G_GPU_TYPE	m_GPUType;
	UINT32			m_Size;

	i3List			m_ResList;

	// Volatile Members
	INT8 *			m_pCommand;
	//INT32			m_CommandIdx; 

protected:

#if defined( I3G_PSP)
	void			BindForPSP(void);
#endif

public:
	i3GPUCommandList(void);
	virtual ~i3GPUCommandList(void);

	I3G_GPU_TYPE	GetGPUType(void)		{ return m_GPUType; }
	void			SetGPUType( I3G_GPU_TYPE type)	{ m_GPUType = type; }

	INT8 *			GetBuffer(void)			{ return m_pCommand; }
	//void			SetBuffer( INT8 * pBuffer, INT32 idx);
	void			SetBuffer( INT8 * pBuffer);

	i3GfxResource *	GetResource( INT32 idx)			{ return (i3GfxResource *) m_ResList.Items[idx]; }
	INT32			GetResourceCount(void)			{ return m_ResList.GetCount(); }
	INT32			AddResource( i3GfxResource * pRes);
	void			RemoveAllResource(void);

	void			CopyBuffer( INT8 * pBuffer, UINT32 Sz);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	void	Dump(void);
	#endif
};

#endif
