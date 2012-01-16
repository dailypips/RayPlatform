/*
 	Ray
    Copyright (C) 2010, 2011, 2012 Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (COPYING).  
	see <http://www.gnu.org/licenses/>
*/

#ifndef _MessageTagHandler_h
#define _MessageTagHandler_h

#include <communication/Message.h>
#include <core/types.h>
#include <communication/mpi_tags.h>

#define MAXIMUM_NUMBER_OF_TAG_HANDLERS 256

/**
 * base class for handling message tags
 * \author Sébastien Boisvert
 * with help from Élénie Godzaridis for the design
 */
class MessageTagHandler{

/** table of method handlers */
	MessageTagHandlerMethod m_methods[MAXIMUM_NUMBER_OF_TAG_HANDLERS];

/** table of object handlers */
	MessageTagHandler*m_objects[MAXIMUM_NUMBER_OF_TAG_HANDLERS];

/** sets the method to call for a given message tag */
	void setMethodHandler(Tag messageTag,MessageTagHandlerMethod method);

public:
	// generate the prototypes with the list */
	#define ITEM(x) virtual void call_ ## x(Message*message);

	/** message tag prototypes */
	#include <mpi_tags.txt>

	#undef ITEM

	/** call the correct handler for a tag on a message */
	void callHandler(Tag messageTag,Message*message);

/** set the object to call for a given tag */
	void setObjectHandler(Tag messageTag,MessageTagHandler*object);

/** set default object and method handlers */
	MessageTagHandler();
};

#endif
