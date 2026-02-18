#if !defined( __I3_PHYSICS_PARTICLE_IDX_H )
#define __I3_PHYSICS_PARTICLE_IDX_H


class I3_EXPORT_PHYSICS i3PHSParticleIdxTable : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3PHSParticleIdxTable );
protected:
	INT32		m_VertexCount;
	INT32		* m_VIndexArray;

public:
	i3PHSParticleIdxTable();
	~i3PHSParticleIdxTable();

	void	Create( INT32 VertexCount );
	void	Create( INT32 VertexCount, INT32 * pTable);

	void	setIndex( INT32 idx, INT32 val)			{ m_VIndexArray[idx] = val; }
	INT32	getIndex( INT32 idx)					{ return m_VIndexArray[idx]; }

	INT32	getVertexCount(void)					{ return m_VertexCount; }

	virtual UINT32 OnSave( i3ResourceFile * pResFile );
	virtual UINT32 OnLoad( i3ResourceFile * pResFile );

};

#endif // __I3_PHYSICS_PARTICLE_IDX_H
