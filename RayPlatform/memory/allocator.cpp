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

#include "allocator.h"

#include <RayPlatform/core/OperatingSystem.h>

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

/**
 * wrapper around malloc
 */
void*__Malloc(int c,const char*description,bool show){
	#ifdef CONFIG_ASSERT
	if(c==0){
		cout<<"Requested "<<c<<" bytes of type "<<description<<endl;
	}
	assert(c!=0);
	assert(c>0);
	#endif

	void*a=NULL;
	a=malloc(c);
	if(a==NULL){
		cout<<"Critical exception: The system is out of memory, returned NULL."<<endl;
		cout<<"Requested "<<c<<" bytes of type "<<description<<endl;
		exit(EXIT_NO_MORE_MEMORY);
	}

	assert(a!=NULL);

	if(show){
		printf("%s %i\t%s\t%i bytes, ret\t%p\t%s\n",__FILE__,__LINE__,__func__,c,a,description);
		fflush(stdout);
	}
	return a;
}

/**
 * wrapper around free
 */
void __Free(void*a,const char*description,bool show){
	if(show){
		printf("%s %i\t%s\t%p\t%s\n",__FILE__,__LINE__,__func__,a,description);
		fflush(stdout);
	}

	free(a);
}


