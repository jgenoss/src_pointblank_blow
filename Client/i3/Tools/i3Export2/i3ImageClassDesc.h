#if !defined( __I3_IMAGE_CLASS_DESC_H)
#define __I3_IMAGE_CLASS_DESC_H

#define	I3_IMAGE_CLASS_ID		Class_ID(0x28125d36, 0x58bf38f4)

class i3ImageClassDesc : public ClassDesc2
{
public:
	i3ImageClassDesc(void);
	virtual ~i3ImageClassDesc(void);

	// Show this in create branch?
	virtual int				IsPublic(void) override					{ return TRUE; }

	// return a pointer to an instance of the class.
	virtual void *			Create(BOOL loading=FALSE) override;

	virtual	int 			BeginCreate(Interface *i) override { return 0; }
	virtual int 			EndCreate(Interface *i) override		{ return 0;};
	virtual const TCHAR* 	ClassName(void) override;
	virtual SClass_ID		SuperClassID(void) override;
	virtual Class_ID		ClassID(void) override;
	virtual const TCHAR* 	Category(void) override;   // primitive/spline/loft/ etc

    // Returns true of the class implements a manipulator object.
    virtual BOOL IsManipulator() override { return FALSE; }

    // Returns true if the class is a manipulator and it
    // manipulates the given reference targer which can be a base
    // object, modifier or controller.  When starting "Manipulate"
    // mode, this is called on selected nodes for the base object,
    // all modifiers, the TM controller and the position, rotation
    // and scale controllers, if the TM controller is a
    // PRSController.
    virtual BOOL CanManipulate(ReferenceTarget* hTarget) override { return FALSE; }

    // Returns true if the manipulator applies to the given node.
    // This is general case if CanManipulateClassID isn't sufficient.
    // This can be used to indicate that the manipulator works on a part
    // of the node not covered by the cases mentioned above, such as the
    // visibility controller.
    virtual BOOL CanManipulateNode(INode* pNode) override { return FALSE; }

    // When a manipulator returns TRUE to CanManipulateClassID(),
    // the system calls this version of CreateManipulator to
    // create an instance.  The hTarget passed is the object,
    // modifier or controller that the manipulator said it could
    // manipulate.
    virtual Manipulator* CreateManipulator(ReferenceTarget* hTarget,
                                            INode* pNode) override {return NULL;}

    // When a manipulator returns TRUE to CanManipulateNode(INode*),
    // the system calls this version of CreateManipulator to
    // create an instance.  
    virtual Manipulator* CreateManipulator(INode* pNode) override {return NULL;}

	// Class IO
	virtual BOOL			NeedsToSave(void) override { return FALSE; }
	virtual IOResult 		Save(ISave *isave) override { return IO_OK; }
	virtual IOResult 		Load(ILoad *iload) override { return IO_OK; }

	// bits of dword set indicate corrresponding rollup page is closed.
	// the value 0x7fffffff is returned by the default implementation so the
	// command panel can detect this method is not being overridden, and just leave 
	// the rollups as is.
	virtual DWORD			InitialRollupPageState(void) override { return 0x7fffffff; }

	// ParamBlock2-related metadata interface, supplied & implemented in ClassDesc2 (see maxsdk\include\iparamb2.h)
	virtual const TCHAR*	InternalName(void) override { return NULL; }
	virtual HINSTANCE		HInstance(void) override { return NULL; }

	// access parameter block descriptors for this class
	virtual int				NumParamBlockDescs(void) override { return 0; }
	virtual ParamBlockDesc2* GetParamBlockDesc(int i) override { return NULL; }
	virtual ParamBlockDesc2* GetParamBlockDescByID(BlockID id) override { return NULL; }
	virtual void			AddParamBlockDesc(ParamBlockDesc2* pbd) override { }
	// automatic UI management
	virtual void			BeginEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) override { }
	virtual void			EndEditParams(IObjParam *ip, ReferenceMaker* obj, ULONG flags, Animatable *prev) override { }
	virtual void			InvalidateUI(ParamBlockDesc2* pbd)  override { }

	// automatic ParamBlock construction
	virtual void			MakeAutoParamBlocks(ReferenceMaker* owner) override { }
	// access automatically-maintained ParamMaps, by simple index or by associated ParamBlockDesc
	virtual int				NumParamMaps() override { return 0; }
	virtual IParamMap2*		GetParamMap(int i) override { return NULL; }
	virtual IParamMap2*		GetParamMap(ParamBlockDesc2* pbd) override { return NULL; }
	// maintain user dialog procs on automatically-maintained ParamMaps
	virtual void			SetUserDlgProc(ParamBlockDesc2* pbd, ParamMap2UserDlgProc* proc=NULL) override { }
	virtual ParamMap2UserDlgProc* GetUserDlgProc(ParamBlockDesc2* pbd) override { return NULL; }

	// Class can draw an image to represent itself graphically...
	virtual bool DrawRepresentation(COLORREF bkColor, HDC hDC, Rect &rect) override { return FALSE; }
	
	// This method can be used for further categorizing plugin's. If a plugin has 
	// sub-plugins(like light > shadows, particles > operators), this method can be 
	// used to differentiate them. sub-plugins can be derived off reference target
	// but return a particular class ID published by the parent plugins SDK headers. 
	// Then parent plugin can get a list of all reference targets whose SubClassID 
	// matches the published SubClassID
	virtual Class_ID		SubClassID() override { return Class_ID(); }

	// Generic expansion function
	virtual INT_PTR Execute(int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) override { return 0; }

};

ClassDesc * GetImageClassDesc(void);

#endif
