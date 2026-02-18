//
//  ingesttester.h
//  twitchsdk
//
//  Created by Auston Stewart on 4/24/14.
//  Copyright (c) 2014 Twitch. All rights reserved.
//

#ifndef TTVSDK_INGESTTESTER_H
#define TTVSDK_INGESTTESTER_H

#include "ingesttestertypes.h"
#include <string>
#include <vector>
#include <map>

namespace ttv
{
	class TaskRunner;
	
	namespace broadcast
	{
		class IngestTester;
	}
}

/**
 *
 * This class is not yet implemented.
 *
 * Performs ingest bandwidth testing to determine the best server a user should connect to for broadcasting.  This will fill in the BitrateKbps
 * field of the given IngestServers.  Testing may take a while because there are several servers to test and each one may be tested for several
 * seconds.  This class cannot be used independently from the BroadcastController (BC) and is single-use only.  Begin a new test via BC.StartIngestTest().
 *
 * The IngestTester (IT) will fire events to indicate state changes such as the starting and stopping of a server test and completion of all tests.
 *
 * Progress of the overall test and of the current server can be gotten from TotalProgress and ServerProgress, respectively.  You may want to display a progress bar for each.
 *
 * The whole test can be cancelled via Cancel().  The current server test can be skipped via SkipCurrentServer() which will trigger the testing of the next server.
 */
class ttv::broadcast::IngestTester
{
public:
	
	IngestTester();
	~IngestTester();

};

#endif /* TTVSDK_INGESTTESTER_H */