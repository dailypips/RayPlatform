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
    along with this program (LICENSE).  
	see <http://www.gnu.org/licenses/>

*/

#include<string.h>
#include<SequencesLoader.h>
#include<Message.h>
#include<DistributionData.h>
#include<BubbleData.h>
#include<assert.h>
#include<vector>
#include<common_functions.h>
#include<Loader.h>
#include<Read.h>
#include<iostream>
using namespace std;

bool SequencesLoader::loadSequences(int rank,int size,vector<Read*>*m_distribution_reads,int*m_distribution_sequence_id,
	bool*m_LOADER_isLeftFile,StaticVector*m_outbox,int*m_distribution_file_id,MyAllocator*m_distributionAllocator,
	bool*m_LOADER_isRightFile,int*m_LOADER_averageFragmentLength,DistributionData*m_disData,	
	int*m_LOADER_numberOfSequencesInLeftFile,RingAllocator*m_outboxAllocator,
	int*m_distribution_currentSequenceId,int*m_LOADER_deviation,bool*m_loadSequenceStep,BubbleData*m_bubbleData,
	time_t*m_lastTime,
	Parameters*m_parameters,int*m_master_mode
){
	if(!m_ready){
		return true;
	}
	vector<string> allFiles=(*m_parameters).getAllFiles();
	if((*m_distribution_reads).size()>0 and (*m_distribution_sequence_id)>(int)(*m_distribution_reads).size()-1){
		// we reached the end of the file.

		cout<<"Rank "<<rank<<" distributes sequences, "<<(*m_distribution_reads).size()<<"/"<<(*m_distribution_reads).size()<<" (DONE)"<<endl;
		(*m_distribution_file_id)++;
		if((*m_LOADER_isLeftFile)){
			(*m_LOADER_numberOfSequencesInLeftFile)=(*m_distribution_sequence_id);
		}
		(*m_distribution_sequence_id)=0;
		(*m_distribution_reads).clear();
	}
	if((*m_distribution_file_id)>(int)allFiles.size()-1){
		(*m_master_mode)=MASTER_MODE_DO_NOTHING;
		(*m_loadSequenceStep)=true;
		flushPairedStock(1,m_outbox,m_outboxAllocator,m_disData,rank,size);
		cout<<"Rank 0 asks others to share their number of sequence reads"<<endl;
		for(int i=0;i<size;i++){
			Message aMessage(NULL, 0, MPI_UNSIGNED_LONG_LONG, i,TAG_MASTER_IS_DONE_SENDING_ITS_SEQUENCES_TO_OTHERS,rank);
			(*m_outbox).push_back(aMessage);
		}
		(*m_distributionAllocator).clear();
		(*m_distribution_reads).clear();
		return true;
	}
	if((*m_distribution_reads).size()==0){
		Loader loader;
		(*m_distribution_reads).clear();
		(*m_distributionAllocator).clear();
		(*m_distributionAllocator).constructor(DISTRIBUTION_ALLOCATOR_CHUNK_SIZE);
		#ifdef SHOW_PROGRESS
		cout<<endl<<"Rank "<<rank<<" loads "<<allFiles[(*m_distribution_file_id)]<<"."<<endl;
		#else
		cout<<endl<<"Loading "<<allFiles[(*m_distribution_file_id)]<<""<<endl;
		#endif
		loader.load(allFiles[(*m_distribution_file_id)],&(*m_distribution_reads),&(*m_distributionAllocator),&(*m_distributionAllocator));
		m_parameters->setNumberOfSequences(m_distribution_reads->size());
		if((*m_distribution_reads).size()==0){
			return false;
		}

		// write Reads in AMOS format.
		if((*m_parameters).useAmos()){
			FILE*fp=(*m_bubbleData).m_amos;
			for(int i=0;i<(int)(*m_distribution_reads).size();i++){
				int iid=(*m_distribution_currentSequenceId)+i;
				char*seq=(*m_distribution_reads).at(i)->getSeq();
				char*qlt=(char*)__Malloc(strlen(seq)+1);
				strcpy(qlt,seq);
				// spec: https://sourceforge.net/apps/mediawiki/amos/index.php?title=Message_Types#Sequence_t_:_Universal_t
				for(int j=0;j<(int)strlen(qlt);j++)
					qlt[j]='D';
				fprintf(fp,"{RED\niid:%i\neid:%i\nseq:\n%s\n.\nqlt:\n%s\n.\n}\n",iid+1,iid+1,seq,qlt);
				__Free(qlt);
			}
		}

		(*m_LOADER_isLeftFile)=(*m_LOADER_isRightFile)=false;
		if((*m_parameters).isLeftFile((*m_distribution_file_id))){
			(*m_LOADER_isLeftFile)=true;
		}else if((*m_parameters).isRightFile((*m_distribution_file_id))){
			(*m_LOADER_isRightFile)=true;
			(*m_LOADER_averageFragmentLength)=(*m_parameters).getFragmentLength((*m_distribution_file_id));
			(*m_LOADER_deviation)=(*m_parameters).getStandardDeviation((*m_distribution_file_id));
		}else{
			(*m_LOADER_isLeftFile)=(*m_LOADER_isRightFile)=false;
		}
		
		if((*m_parameters).isInterleavedFile((*m_distribution_file_id))){
			m_isInterleavedFile=true;
			(*m_LOADER_averageFragmentLength)=(*m_parameters).getFragmentLength((*m_distribution_file_id));
			(*m_LOADER_deviation)=(*m_parameters).getStandardDeviation((*m_distribution_file_id));
		}else{
			m_isInterleavedFile=false;
		}

		#ifdef SHOW_PROGRESS
		cout<<"Rank "<<rank<<" has "<<(*m_distribution_reads).size()<<" sequences to distribute."<<endl;
		#else
		cout<<"Distributing sequences"<<endl;
		#endif
	}

	#ifndef SHOW_PROGRESS
	time_t tmp=time(NULL);
	if(tmp>(*m_lastTime)){
		(*m_lastTime)=tmp;
		showProgress(*m_lastTime);
	}
	#endif

	for(int i=0;i<1;i++){
		if((*m_distribution_sequence_id)>(int)(*m_distribution_reads).size()-1){
			#ifdef SHOW_PROGRESS
			#endif
			break;
		}
		int destination=(*m_distribution_currentSequenceId)%size;
		#ifdef DEBUG
		assert(destination>=0);
		assert(destination<size);
		#endif
		char*sequence=((*m_distribution_reads))[(*m_distribution_sequence_id)]->getSeq();
		int cells=roundNumber(strlen(sequence)+1,8);
		VERTEX_TYPE*message=(VERTEX_TYPE*)m_outboxAllocator->allocate(cells);
		char*destinationBuffer=(char*)message;
		strcpy(destinationBuffer,sequence);
		#ifdef SHOW_PROGRESS
		if((*m_distribution_sequence_id)%100000==0){
			cout<<"Rank "<<rank<<" distributes sequences, "<<(*m_distribution_sequence_id)+1<<"/"<<(*m_distribution_reads).size()<<endl;
		}
		#endif

 		// make it wait some times
 		// this avoids spinning too fast in the memory ring of the outbox <
 		// allocator

		time_t theTime=time(NULL);
		if(theTime!=m_last){
			//cout<<m_last<<" RING PRODUCE "<<m_produced<<endl;
			m_produced=0;
			m_last=theTime;
		}
		m_produced++;

		if((*m_distribution_sequence_id)%500==0){
			Message aMessage(message,cells,MPI_UNSIGNED_LONG_LONG,destination,TAG_SEND_SEQUENCE_REGULATOR,rank);
			(*m_outbox).push_back(aMessage);
			m_ready=false;
		}else{
			Message aMessage(message,cells,MPI_UNSIGNED_LONG_LONG,destination,TAG_SEND_SEQUENCE,rank);
			(*m_outbox).push_back(aMessage);
		}

		// add paired information here..
		// algorithm follows.
		// check if current file is in a right file.
		// if yes, the leftDistributionCurrentSequenceId=(*m_distribution_currentSequenceId)-NumberOfSequencesInRightFile.
		// the destination of a read i is i%getSize()
		// the readId on destination is i/getSize()
		// so, basically, send these bits to destination:
		//
		// rightSequenceGlobalId:= (*m_distribution_currentSequenceId)
		// rightSequenceRank:= rightSequenceGlobalId%getSize
		// rightSequenceIdOnRank:= rightSequenceGlobalId/getSize
		// leftSequenceGlobalId:= rightSequenceGlobalId-numberOfSequencesInRightFile
		// leftSequenceRank:= leftSequenceGlobalId%getSize
		// leftSequenceIdOnRank:= leftSequenceGlobalId/getSize
		// averageFragmentLength:= ask the pairedFiles in (*m_parameters).
		if((*m_LOADER_isRightFile)){
			int rightSequenceGlobalId=(*m_distribution_currentSequenceId);
			int rightSequenceRank=rightSequenceGlobalId%size;
			int rightSequenceIdOnRank=rightSequenceGlobalId/size;
			int leftSequenceGlobalId=rightSequenceGlobalId-(*m_LOADER_numberOfSequencesInLeftFile);
			int leftSequenceRank=leftSequenceGlobalId%size;
			int leftSequenceIdOnRank=leftSequenceGlobalId/size;
			int averageFragmentLength=(*m_LOADER_averageFragmentLength);
			int deviation=(*m_LOADER_deviation);

			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,rightSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,leftSequenceRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,leftSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,averageFragmentLength);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,deviation);

			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,leftSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,rightSequenceRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,rightSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,averageFragmentLength);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,deviation);

			// 4096 bytes allow the sending of 512 64-bits integers.
			// however, in this function m_messagesStockPaired contains multiple of 10.
			// thus, the threshold must be 512-2
			flushPairedStock(MAX_UINT64_T_PER_MESSAGE-2,m_outbox,m_outboxAllocator,m_disData,rank,size);
		}else if(m_isInterleavedFile
			&&((*m_distribution_sequence_id)%2)==1){// only the right sequence.
			int rightSequenceGlobalId=(*m_distribution_currentSequenceId);
			int rightSequenceRank=rightSequenceGlobalId%size;
			int rightSequenceIdOnRank=rightSequenceGlobalId/size;
			int leftSequenceGlobalId=rightSequenceGlobalId-1;
			int leftSequenceRank=leftSequenceGlobalId%size;
			int leftSequenceIdOnRank=leftSequenceGlobalId/size;
			int averageFragmentLength=(*m_LOADER_averageFragmentLength);
			int deviation=(*m_LOADER_deviation);

			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,rightSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,leftSequenceRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,leftSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,averageFragmentLength);
			m_disData->m_messagesStockPaired.addAt(rightSequenceRank,deviation);

			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,leftSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,rightSequenceRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,rightSequenceIdOnRank);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,averageFragmentLength);
			m_disData->m_messagesStockPaired.addAt(leftSequenceRank,deviation);

			// 4096 bytes allow the sending of 512 64-bits integers.
			// however, in this function m_messagesStockPaired contains multiple of 10.
			// thus, the threshold must be 512-2
			flushPairedStock(MAX_UINT64_T_PER_MESSAGE-2,m_outbox,m_outboxAllocator,m_disData,rank,size);
		}

		(*m_distribution_currentSequenceId)++;
		(*m_distribution_sequence_id)++;
	}
	return true;
}

void SequencesLoader::flushPairedStock(int threshold,StaticVector*m_outbox,
	RingAllocator*m_outboxAllocator,DistributionData*m_disData,
			int rank,int size){
	for(int rankId=0;rankId<size;rankId++){
		int rightSequenceRank=rankId;
		int count=m_disData->m_messagesStockPaired.size(rankId);
		if(count<threshold)
			continue;

		VERTEX_TYPE*message=(VERTEX_TYPE*)(*m_outboxAllocator).allocate(count*sizeof(VERTEX_TYPE));
		for(int j=0;j<count;j++){
			message[j]=m_disData->m_messagesStockPaired.getAt(rankId,j);
		}

		Message aMessage(message,count,MPI_UNSIGNED_LONG_LONG,rightSequenceRank,TAG_INDEX_PAIRED_SEQUENCE,rank);
		(*m_outbox).push_back(aMessage);
		m_disData->m_messagesStockPaired.reset(rankId);
	}
}

SequencesLoader::SequencesLoader(){
	setReadiness();
	m_produced=0;
	m_last=time(NULL);
}

void SequencesLoader::setReadiness(){
	m_ready=true;
}
