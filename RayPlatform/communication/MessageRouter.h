/*
 	RayPlatform: a message-passing development framework
    Copyright (C) 2010, 2011, 2012 Sébastien Boisvert

	http://github.com/sebhtml/RayPlatform

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You have received a copy of the GNU Lesser General Public License
    along with this program (lgpl-3.0.txt).  
	see <http://www.gnu.org/licenses/>

*/

#ifndef _MessageRouter_h
#define _MessageRouter_h

#include "Message.h"

#include <RayPlatform/memory/RingAllocator.h>
#include <RayPlatform/structures/StaticVector.h>
#include <RayPlatform/routing/ConnectionGraph.h>
#include <RayPlatform/core/types.h>

#include <string>
#include <map>
#include <vector>
#include <time.h> /* for time() */
using namespace std;

class ComputeCore;

/**
 * the MessageRouter makes communication more efficient.
 *
 * \author Sébastien Boisvert 2011-11-04
 * \reviewedBy Elénie Godzaridis 2011-11-05
 */
class MessageRouter {

	string m_prefix;

/** the extra ticks after the computation has completed. **/
	time_t m_deletionTime;


/**  the connection graph */
	ConnectionGraph m_graph;

/**
 * Is the router activated at all ?
 */
	bool m_enabled;

/**
 * The message inbox
 */
	StaticVector*m_inbox;

/**
 * The message outbox
 */
	StaticVector*m_outbox;

/**
 * The outbox buffer allocator
 */
	RingAllocator*m_outboxAllocator;

/**
 * The identifier of the current rank
 */
	Rank m_rank;

/**
 * The number of ranks
 */
	int m_size;

	void relayMessage(Message*message,Rank destination);

	MessageTag getMessageTagFromRoutingTag(MessageTag tag);
	Rank getSourceFromBuffer(MessageUnit*buffer,int count);
	Rank getDestinationFromBuffer(MessageUnit*buffer,int count);
	MessageTag getRoutingTag(MessageTag tag);
	void setSourceInBuffer(MessageUnit*buffer,int count,Rank source);
	void setDestinationInBuffer(MessageUnit*buffer,int count,Rank destination);

public:
	MessageRouter();

/**
 * Callback method to call before sending messages in the outbox onto the network
 */
	void routeOutcomingMessages();

/**
 * Callback method to call after receiving messages from the network into the inbox
 */
	bool routeIncomingMessages();

	bool isEnabled();

/**
 * Enable the router
 * This could be a constructor.
 */
	void enable(StaticVector*inbox,StaticVector*outbox,RingAllocator*outboxAllocator,Rank rank,
string prefix,int numberOfRanks,string type,int degree);

/**
 * Check if relayed messages have completed their transit.
 * This is required for the messages sent at the end
 * of the computation because we need ranks to shut down 
 * after they have relayed messages for which they act as
 * relay in the routing table.
 *
 * This is only called once the loop is dead, that is m_alive= false;
 */
	bool hasCompletedRelayEvents();

	ConnectionGraph*getGraph();

/**
 * Write routing information files.
 */
	void writeFiles();

	bool isRoutingTag(MessageTag tag);

};

#endif /* _MessageRouter_h */
