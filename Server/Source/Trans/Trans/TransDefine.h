#ifndef __TRANSDEFINE_H__
#define __TRANSDEFINE_H__

#define	NEW_SAFE_DELETE(P)				if(P!=NULL){delete(P); P=NULL; }
#define	NEW_SAFE_DELETE_ARRAY(P)		if(P!=NULL){delete[](P); P=NULL; }

#endif