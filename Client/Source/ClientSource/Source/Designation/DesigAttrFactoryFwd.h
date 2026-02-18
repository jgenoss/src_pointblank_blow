#pragma once

class DesigAttr;

namespace i3 
{	
	template<class, class> class factory;  
}

typedef i3::factory< i3::rc_wstring, 
DesigAttr* (const i3::rc_wstring&, const i3::rc_wstring&, REAL32) >	DesigAttrFactory;
