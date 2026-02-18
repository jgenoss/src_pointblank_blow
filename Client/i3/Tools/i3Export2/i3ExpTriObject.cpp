#include "stdafx.h"
#include "i3EXPTriObject.h"

i3EXPTriObject::i3EXPTriObject( INode* node, TimeValue time):
	_needDelete(false),
	_tri(NULL)
{
	Object *obj = node->EvalWorldState( time).obj;
	//Object *obj = node->GetObjectRef();

	if(obj && obj->CanConvertToType( Class_ID(TRIOBJ_CLASS_ID, 0))) 
	{ 
		_tri = (TriObject *) obj->ConvertToType( time, Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != _tri) 
		{
			_needDelete = true;	
		}
	}
	else
	{
		I3TRACE( "[%s] : Empty Geometry\n", node->GetName());
	}
}	

i3EXPTriObject::~i3EXPTriObject()
{
	if (_needDelete && _tri)
	{
		_tri->AutoDelete();
	}
}