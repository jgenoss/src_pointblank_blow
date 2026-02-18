#ifndef __I3NETWORKPCH_H
#define __I3NETWORKPCH_H

#include "i3Base.h"

#ifdef I3_WIPI
	#include "i3NetworkWIPI.h"
	#ifdef I3_WIPI_KTF
		#include "i3NetworkDef_NHN.h"
	#endif
#endif

#endif	// __I3NETWORKPCH_H
