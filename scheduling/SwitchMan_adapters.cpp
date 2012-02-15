/*
 	Ray
    Copyright (C) 2010, 2011, 2012  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (gpl-3.0.txt).  
	see <http://www.gnu.org/licenses/>

*/

#include <scheduling/SwitchMan.h>
#include <scheduling/SwitchMan_adapters.h>

void Adapter_RAY_MPI_TAG_SWITCHMAN_COMPLETION_SIGNAL::setObject(SwitchMan*object){
	m_object=object;
}

void Adapter_RAY_MPI_TAG_SWITCHMAN_COMPLETION_SIGNAL::call(Message*m){
	m_object->call_RAY_MPI_TAG_SWITCHMAN_COMPLETION_SIGNAL(m);
}


void Adapter_RAY_SLAVE_MODE_STOP::setObject(SwitchMan*object){
	m_object=object;
}

void Adapter_RAY_SLAVE_MODE_STOP::call(){
	m_object->call_RAY_SLAVE_MODE_STOP();
}

