#ifndef __ARTPBILLINGERROR_H__
#define __ARTPBILLINGERROR_H__

#include "ARTPError.h"

// about rcs
#define ARTPRET_RCS_UNKNOWNREASON		-11		// as words, unknown error (out of expected)
#define ARTPRET_RCS_INTERNALERROR		-12		// internal error. cause will be various
#define ARTPRET_RCS_RETRY				-13		// plz, retry...
#define ARTPRET_RCS_FAILQUERY			-14		// db error
#define ARTPRET_RCS_FAILDBCONNECTION	-17		// db error

// DB ERROR
#define ARTPRET_DB_SESSIONERROR			-200	
#define ARTPRET_DB_OPENERROR			-201	
// SP ERROR
#define ARTPRET_SP_INTERNALERROR		-400
// SP RETURN VALUE
#define SP_OK							0
#define SP_NOUSER						11
#define SP_NOHASHCODE					12
#define SP_NOMONEY						71
#define SP_HACKING						81


#endif //__ARTPBILLINGERROR_H__