#if !defined( __I3EXP_CLASS_DESC_H)
#define __I3EXP_CLASS_DESC_H

#define I3_EXPORT2_CLASS_ID		Class_ID(0x26526137, 0xf3a11cb)

class i3EXPClassDesc : public ClassDesc2
{
public:
	i3EXPClassDesc(void);
	virtual ~i3EXPClassDesc(void);

	// Show this in create branch?
	virtual int				IsPublic(void)							{ return TRUE; }

	// return a pointer to an instance of the class.
	virtual void *			Create(BOOL loading=FALSE);

	virtual	int 			BeginCreate(Interface *i)				{ return 0;}
	virtual int 			EndCreate(Interface *i)					{ return 0;};
	virtual const TCHAR* 	ClassName(void);
	virtual SClass_ID		SuperClassID(void);
	virtual Class_ID		ClassID(void);
	virtual const TCHAR* 	Category(void);   // primitive/spline/loft/ etc

    // Returns true of the class implements a manipulator object.
    virtual BOOL IsManipulator()									{ return FALSE; }

    // Returns true if the class is a manipulator and it
    // manipulates the given reference targer which can be a base
    // object, modifier or controller.  When starting "Manipulate"
    // mode, this is called on selected nodes for the base object,
    // all modifiers, the TM controller and the position, rotation
    // and scale controllers, if the TM controller is a
    // PRSController.
    virtual BOOL CanManipulate(ReferenceTarget* hTarget)			{ return FALSE; }

    // Returns true if the manipulator applies to the given node.
    // This is general case if CanManipulateClassID isn't sufficient.
    // This can be used to indicate that the manipulator works on a part
    // of the node not covered by the cases mentioned above, such as the
    // visibility controller.
    virtual BOOL CanManipulateNode(INode* pNode)					{ return FALSE; }

    // When a manipulator returns TRUE to CanManipulateClassID(),
    // the system calls this version of CreateManipulator to
    // create an instance.  The hTarget passed is the object,
    // modifier or controller that the manipulator said it could
    // manipulate.
    virtual Manipulator* CreateManipulator(ReferenceTarget* hTarget,
                                            INode* pNode)			{return NULL;}

    // When a manipulator returns TRUE to CanManipulateNode(INode*),
    // the system calls this version of CreateManipulator to
    // create an instance.  
    virtual Manipulator* CreateManipulator(INode* pNode)			{return NULL;}

	// Class IO
	virtual BOOL			NeedsToSave(void)						{ return FALSE; }
	virtual IOResult 		Save(ISave *isave)						{ return IO_OK; }
	virtual IOResult 		Load(ILoad *iload)						{ return IO_OK; }

	// bits of dword set indicate corrresponding rollup page is closed.
	// the value 0x7fffffff is returned by the default implementation so the
	// command panel can detect this method is not being overridden, and just leave 
	// the rollups as is.
	virtual DWORD			InitialRollupPageState(void)			{ return 0x7fffffff; }

	// ParamBlock2-related metadata interface, supplied & implemented in ClassDesc2 (see maxsdk\include\iparamb2.h)
	virtual const TCHAR*	InternalName(void)						{ return NULL; }
	virtual HINSTANCE		HInstance(void)							{ return NULL; }

	// access parameter block descriptors for this class
	virtual int				NumParamBlockDescs(void)				{ return 0; }
	virtual ParamBlockDesc2* GetParamBlockDesc(int i)				{ return NULL; }
	virtual ParamBlockDesc2* GetParamBlockDescByID(BlockID id)		{ return NULL; }
	virtual void			AddParamBlockDesc(ParamBlockDesc2* pbd)	{ }
	// automatic UI management
	virtual void			BeginEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) { }
	virtual void			EndEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) { }
	virtual void			InvalidateUI(ParamBlockDesc2* pbd) { }

	// automatic ParamBlock construction
	virtual void			MakeAutoParamBlocks(ReferenceMaker* owner) { }
	// access automatically-maintained ParamMaps, by simple index or by associated ParamBlockDesc
	virtual int				NumParamMaps() { return 0; }
	virtual IParamMap2*		GetParamMap(int i) { return NULL; }
	virtual IParamMap2*		GetParamMap(ParamBlockDesc2* pbd) { return NULL; }
	// maintain user dialog procs on automatically-maintained ParamMaps
	virtual void			SetUserDlgProc(ParamBlockDesc2* pbd, ParamMap2UserDlgProc* proc=NULL) { }
	virtual ParamMap2UserDlgProc* GetUserDlgProc(ParamBlockDesc2* pbd) { return NULL; }

	// Class can draw an image to represent itself graphically...
	virtual bool DrawRepresentation(COLORREF bkColor, HDC hDC, Rect &rect) { return FALSE; }
	
	// This method can be used for further categorizing plugin's. If a plugin has 
	// sub-plugins(like light > shadows, particles > operators), this method can be 
	// used to differentiate them. sub-plugins can be derived off reference target
	// but return a particular class ID published by the parent plugins SDK headers. 
	// Then parent plugin can get a list of all reference targets whose SubClassID 
	// matches the published SubClassID
	virtual Class_ID		SubClassID() { return Class_ID(); }

	// Generic expansion function
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return 0; } 

};

ClassDesc * GetExportedClassDesc(void);

#endif
