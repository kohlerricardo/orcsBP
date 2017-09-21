#include "simulator.hpp"

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {
	size_t size = ENTRY/WAYS;
	fprintf(stderr,"Sets %lu",size);
	this->btb = new btb_t[size];
	for (size_t i = 0; i < size; i++)
	{
		this->btb[i].btb_entry = new btb_line_t[WAYS];
	}
	
	// Initialize statistics counters
	this->branches = 0;
	this->btbHits = 0;
	this->btbMiss = 0;
	this->branchNotTaken=0;
	this->branchTaken=0;
	this->BtMiss=0;
	this->BntMiss=0;
	// direct acess
	this->index = 0;
	this->assoc = 0;
	this->has_branch = FAIL;
	//allocate plbt variables
	this->predict = NOT_TAKEN;
	this->oldAdd = 0;
};

// =====================================================================

void processor_t::clock(){
/// Get the next instruction from the trace
opcode_package_t new_instruction;
if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
	/// If EOF
	orcs_engine.simulator_alive = false;
}
if(this->has_branch==OK){
	if(new_instruction.opcode_address!=this->nextInstruction){
			this->branchTaken++;
			if(this->predict!=TAKEN){
				orcs_engine.plbp->train(this->oldAdd,TAKEN);		
				orcs_engine.global_cycle+=BTB_MISS_PENALITY;
				this->BtMiss++;
			}
	}else{
		this->branchNotTaken++;
		if(this->predict!=NOT_TAKEN){
			orcs_engine.plbp->train(this->oldAdd,NOT_TAKEN);
			orcs_engine.global_cycle+=BTB_MISS_PENALITY;
			this->BntMiss++;
		}
		
	}
	this->has_branch = FAIL;
}
	if (new_instruction.opcode_operation==INSTRUCTION_OPERATION_BRANCH)
	{	
		this->has_branch = OK;
		this->nextInstruction = new_instruction.opcode_address+new_instruction.opcode_size;
		this->oldAdd = new_instruction.opcode_address;
		this->branches++;
		this->predict = orcs_engine.plbp->predict(new_instruction.opcode_address);
		//BTB
		uint32_t hit = this->searchLine(new_instruction.opcode_address);
		if(hit==HIT){
			this->btbHits++;
			this->updateLruAll(HIT);
		}else{
			this->btbMiss++;
			this->updateLruAll(BTB_MISS_PENALITY);
			//install line
			this->installLine(new_instruction);					
			orcs_engine.global_cycle+=BTB_MISS_PENALITY;
		}
	}


};
// =====================================================================
void processor_t::statistics() {
	fprintf(stderr,"######################################################\n");
	fprintf(stderr,"processor_t\n");
	fprintf(stderr,"*******\nBTB\n********\n");
	fprintf(stderr,"*******\nValues\n********\n");
	fprintf(stderr,"BTB Hits ;%u\n",this->btbHits);
	fprintf(stderr,"BTB Miss ;%u\n",this->btbMiss);
	fprintf(stderr,"\n\nPercentage\n\n");
	double percent=0;
	percent = ((this->btbHits*100)/this->branches);
	fprintf(stderr,"BTB Hits %1.2f\n",percent);
	percent = ((this->btbMiss*100)/this->branches);
	fprintf(stderr,"BTB Miss %1.2f\n",percent);


	fprintf(stderr,"**********\nPieceWise\n**********\n");
	fprintf(stderr,"**********\nValues\n**********\n");
	fprintf(stderr,"Taken Branches ;%u \n",this->branchTaken);
	fprintf(stderr,"Not Taken Branches ;%u\n",this->branchNotTaken);
	fprintf(stderr,"Correct Prediction Taken ;%u\n",(this->branchTaken-this->BtMiss));
	fprintf(stderr,"Misprediction Taken ;%u\n",this->BtMiss);
	fprintf(stderr,"Correct Prediction NotTaken ;%u\n",(this->branchNotTaken-this->BntMiss));
	fprintf(stderr,"Mispredicion NotTaken ;%u\n",this->BntMiss);

	fprintf(stderr,"\n**********\nPercentage\n**********\n");
	percent = (this->branchTaken*100)/this->branches;
	fprintf(stderr,"Taken Branches %1.2f \n",percent);
	percent = (this->branchNotTaken*100)/this->branches;
	fprintf(stderr,"Not Taken Branches %1.2f\n",percent);
	percent = ((this->branchTaken-this->BtMiss)*100)/this->branchTaken;
	fprintf(stderr,"Correct Prediction Taken %1.2f\n",percent);
	percent = (this->BtMiss*100)/this->branchTaken;
	fprintf(stderr,"Misprediction Taken %1.2f\n",percent);
	percent = ((this->branchNotTaken-this->BntMiss)*100)/this->branchNotTaken;
	fprintf(stderr,"Correct Prediction NotTaken %1.2f\n",percent);
	percent = (this->BntMiss*100)/this->branchNotTaken;
	fprintf(stderr,"Misprediction NotTaken %1.2f\n",percent);
	

	fprintf(stderr,"Total Branches ;%u\n",this->branches);
	fprintf(stderr,"Total Cicle ;%lu\n",orcs_engine.get_global_cycle());


};
void processor_t::updateLruAll(uint32_t add){
	size_t size = ENTRY/WAYS;
	for(size_t i = 0;i<size;i++){
		for (size_t j = 0; j < WAYS; j++)
		{
			this->btb[i].btb_entry[j].lru +=add;
		}
	}
};

uint32_t processor_t::searchLine(uint32_t pc){
	uint32_t getBits = (ENTRY/WAYS)-1;
	uint32_t tag = (pc >> 2);
	uint32_t index = tag&getBits;
	// fprintf(stderr,"bits %u, tag %u index %u\n",getBits,tag,index);
	for (size_t i = 0; i < WAYS; i++)
	{
		//fprintf(stderr,"%u\n",this->btb[index].btb_entry[i].tag);
		if(this->btb[index].btb_entry[i].tag == pc){
			//fprintf(stderr,"BTB_Hit");
			this->btb[index].btb_entry[i].lru=0;
			//save locate from line
			this->index = index;
			this->assoc = i;
			return HIT;
		}
	}
	//fprintf(stderr,"BTB_Miss");
	return MISS;
}
uint32_t processor_t::installLine(opcode_package_t instruction){
	uint32_t getBits = (ENTRY/WAYS)-1;
	uint32_t tag = (instruction.opcode_address >> 2);
	uint32_t index = tag&getBits;
	// fprintf(stderr,"bits %u, tag %u index %u\n",getBits,tag,index);
	// verify TypeBranch
	uint32_t typeBranch;
	switch(instruction.branch_type){
		case BRANCH_SYSCALL:
			typeBranch = BRANCH_SYSCALL;
			break;
		case BRANCH_CALL:
			typeBranch = BRANCH_SYSCALL;
			break;
		case BRANCH_RETURN:
			typeBranch = BRANCH_SYSCALL;
			break;
		case BRANCH_UNCOND:
			typeBranch = BRANCH_SYSCALL;
			break;
		default: 
			typeBranch = BRANCH_COND;
			break;
	}

	for (size_t i = 0; i < WAYS; i++)
	{
		// instala no primeiro invalido 
		if(this->btb[index].btb_entry[i].validade == 0){
			this->btb[index].btb_entry[i].tag=instruction.opcode_address;
			this->btb[index].btb_entry[i].lru=0;
			this->btb[index].btb_entry[i].targetAddress=instruction.opcode_address+instruction.opcode_size;
			this->btb[index].btb_entry[i].validade=1;
			this->btb[index].btb_entry[i].typeBranch=typeBranch;
			this->btb[index].btb_entry[i].bht=NOT_TAKEN;
			return OK;
		}			
	}
	uint32_t way = this->searchLru(&this->btb[index]);
	this->btb[index].btb_entry[way].tag=instruction.opcode_address;
	this->btb[index].btb_entry[way].lru=0;
	this->btb[index].btb_entry[way].targetAddress=instruction.opcode_address+instruction.opcode_size;
	this->btb[index].btb_entry[way].validade=1;
	this->btb[index].btb_entry[way].typeBranch=typeBranch;
	this->btb[index].btb_entry[way].bht=NOT_TAKEN;
	//indexes
	this->index = index;
	this->assoc = way;
	return OK;
};
inline uint32_t processor_t::searchLru(btb_t *btb){
	uint32_t index=0;
	for (size_t i = 1; i < WAYS; i++)
	{
		index = (btb->btb_entry[index].lru > btb->btb_entry[i].lru)? index : i ;
	}
	return index;
}
