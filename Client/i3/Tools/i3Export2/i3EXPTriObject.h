#if !defined( __I3EXP_TRI_OBJECT_H)
#define __I3EXP_TRI_OBJECT_H

/**
*	Extracts the tri object from an object
*
*	handle the fact that the tri object should be destructed or not
*/
class i3EXPTriObject 
{
	bool		_needDelete;
	TriObject*	_tri;
public:
	//	Constructor
	i3EXPTriObject(INode* node,TimeValue time);

	//	Destructor
	~i3EXPTriObject();

	Mesh *		GetMesh(void)
	{
		return &(_tri->mesh);
	}

	TriObject *	GetTriObject(void)
	{
		return _tri; 
	}

	//	Return true if the tri was correctly constructed
	operator bool() const
	{
		return (_tri != NULL);
	}

	//	Return the tri
	TriObject*	operator&()
	{
		return _tri;
	}
};

#endif
