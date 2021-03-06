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

#include "MasterModeExecutor.h"

#include <RayPlatform/core/types.h>

#include <stdlib.h> /* for NULL */
#ifdef CONFIG_ASSERT
#include <assert.h>
#endif

void MasterModeExecutor::callHandler(MasterMode mode){

#ifdef CONFIG_CACHE_OPERATION_CODES
	MasterModeHandlerReference object=m_cachedOperationHandler;
/*
 * We don't want to look up too much.
 */
	if(mode!=m_cachedOperationCode)
		object=m_objects[mode];
#else
	MasterModeHandlerReference object=m_objects[mode];
#endif /* CONFIG_CACHE_OPERATION_CODES */

	// don't do it if it is NULL because it does nothing
	if(object==NULL)
		return;

	/** otherwise, fetch the method and call it*/

#ifdef CONFIG_MINI_RANKS
	object->call();
#else
	object();
#endif
}

MasterModeExecutor::MasterModeExecutor(){
	for(int i=0;i<MAXIMUM_NUMBER_OF_MASTER_HANDLERS;i++){
		m_objects[i]=NULL;
	}

#ifdef CONFIG_CACHE_OPERATION_CODES
	m_cachedOperationCode=INVALID_HANDLE;
	m_cachedOperationHandler=NULL;
#endif
}

void MasterModeExecutor::setObjectHandler(MasterMode mode,MasterModeHandlerReference object){

#ifdef CONFIG_ASSERT
	assert(mode>=0);
	assert(mode < MAXIMUM_NUMBER_OF_MASTER_HANDLERS);
#endif

	m_objects[mode]=object;
}


