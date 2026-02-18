#pragma once

class CNationEditorDlg;

class i3RegistrySet_Impl :	public i3RegistrySet
{
	I3_CLASS_DEFINE( i3RegistrySet_Impl, i3RegistrySet);

protected:
	CNationEditorDlg* m_pNationListPtr;

protected:
	virtual void _XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
		size_t Index, const char* szPreName,
		char * _DstBuf, size_t _SizeInBytes);

public:
	void SetListCtrlPtr(CNationEditorDlg* Ptr)	{	m_pNationListPtr = Ptr;	}
};