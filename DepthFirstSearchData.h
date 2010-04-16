/*
 	Ray
    Copyright (C) 2010  Sébastien Boisvert

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

#ifndef _DepthFirstSearchData
#define _DepthFirstSearchData

#include<stack>
#include<vector>
#include<map>
#include<set>
#include<common_functions.h>
using namespace std;

class DepthFirstSearchData{
public:
	// depth first search
	bool m_doChoice_tips_Initiated;
	bool m_doChoice_tips_dfs_done;
	int m_depthFirstSearch_maxDepth;
	stack<int> m_depthFirstSearchDepths;
	int m_doChoice_tips_i;
	vector<int> m_doChoice_tips_newEdges;
	bool m_doChoice_tips_dfs_initiated;
	set<VERTEX_TYPE> m_depthFirstSearchVisitedVertices;
	stack<VERTEX_TYPE> m_depthFirstSearchVerticesToVisit;
	vector<VERTEX_TYPE> m_depthFirstSearchVisitedVertices_vector;
	vector<int> m_depthFirstSearchVisitedVertices_depths;
	map<VERTEX_TYPE,int> m_coverages;

};

#endif