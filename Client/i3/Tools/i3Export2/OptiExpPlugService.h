
#ifndef _OPTIEXPPLUGSERVICE_H_
#define _OPTIEXPPLUGSERVICE_H_


class i3Export;

//////////////////////////////////////////////////////////////////////////
namespace OptiExpPlugSer {
//////////////////////////////////////////////////////////////////////////

enum ENODE_TRAVRST
{
	ENODE_TRAVRST_STOP = 0,
	ENODE_TRAVRST_CONT = 1,
	ENODE_TRAVRST_CONT_AND_STOP_CURLOWER = 2
};


class ExportableINodeChecker
{
public:
	virtual bool isExportableINode(INode* node) = 0;
};

class DupplicatedNamedNodeChecker
{
public:
	static DupplicatedNamedNodeChecker* GetDefault();

	virtual ENODE_TRAVRST Do(INode* node) = 0;

	virtual void Reset() = 0;

	virtual INT32 GetNumDupplicated() = 0;
	virtual const char* GetDupplicatedNodeName(INT32 idx) = 0;

	virtual INT32 GetNumEmptyNamed() = 0;
	virtual INode* GetEmptyNamedNode(INT32 idx) = 0;
	virtual void SetExporter(i3Export* exporter) = 0;
};


/*
	ret) number of count to setted
	 0: ---- , 1:dynamic, 2:kinematic, 3:static, 4:none
*/

INT32 SetPhysXOption(INode* node, INT32 physXDynType, ExportableINodeChecker* checker);
bool CheckDupplictedOrEmptyNamedINode(INode* node, DupplicatedNamedNodeChecker& checker);

// text=aaa;bbb;ccc;
bool FindNameInText(const char* name, const char* text);


//////////////////////////////////////////////////////////////////////////
} // end of namespace
//////////////////////////////////////////////////////////////////////////

#endif
