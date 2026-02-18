#if !defined( __I3_GPU_COMMAND_LIST_H)
#define __I3_GPU_COMMAND_LIST_H

#include "i3Base.h"
#include "i3GfxResource.h"

class I3_EXPORT_GFX i3GPUCommandList : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3GPUCommandList, i3PersistantElement);
protected:
	// Persistent Members
	I3G_GPU_TYPE	m_GPUType = I3G_GPU_NONE;
	UINT32			m_Size = 0;

	i3::vector<i3GfxResource*>	m_ResList;

	// Volatile Members
	INT8 *			m_pCommand = nullptr;

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
	void			SetBuffer( INT8 * pBuffer);

	i3GfxResource *	GetResource( INT32 idx)			{ return m_ResList[idx]; }
	INT32			GetResourceCount(void)			{ return (INT32)m_ResList.size(); }
	INT32			AddResource( i3GfxResource * pRes);
	void			RemoveAllResource(void);

	void			CopyBuffer( INT8 * pBuffer, UINT32 Sz);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	void	Dump(void);
	#endif
};

#endif
