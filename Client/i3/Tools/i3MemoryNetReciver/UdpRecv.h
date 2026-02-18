#ifndef __UDPRECV_H__
#define __UDPRECV_H__

//#include <map>

class CUdpRecv : public i3Thread
{
	I3_CLASS_DEFINE( CUdpRecv, i3Thread ); 
private: 
	
	HANDLE			m_hFile;	
protected:	
	BOOL		*	m_pMainRecv; 
	CListCtrl	*	m_pMainList; 

	BOOL			m_bIsRun;

	i3Mutex			m_WriteList; 
	i3::unordered_map<UINT32,MEM_NET_ALLOC_INFO*>	m_AllocList; 
//	i3::vector<MEM_NET_ALLOC_INFO*>	m_AddressList; 
	
	MEM_NET_ALLOC_INFO *	m_pStartInfo; 	

	void Insert(MEM_NET_ALLOC_INFO * pMemInfo); 
	void Remove(MEM_NET_ALLOC_INFO * pMemInfo);
	
public :
	FILE_MAPPING_HEADER * m_pHeader;

	CUdpRecv(); 
	virtual ~CUdpRecv(); 

	BOOL OnCreate( UINT16 Port, BOOL * pMainRecv, CListCtrl	* pMainList);
	BOOL OnDestroy(void); 

	virtual void	BeforeRunning( void * pUserData);
	virtual UINT32	OnRunning( void * pUserData);
	virtual void	AfterRunning( void * pUserData); 	

	void	dump(void); 
	void	DumpFile(void); 
	void	ClearList(void);

	void	SortByUpperSize( std::vector<MEM_NET_ALLOC_INFO*> & sortedList);
}; 
#endif
