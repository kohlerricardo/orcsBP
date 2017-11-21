#include "simulator.hpp"


cache_t::cache_t()
{
    //ctor
}

cache_t::~cache_t()
{
    //dtor
}
void cache_t::allocate(uint32_t level){
	
	
	switch(level){
		case L1:{
			this->shiftData = utils_t::powerOf2(BLOCK_SIZE);
			this->level = level;
			this->cacheHit=0;
			this->cacheMiss=0;
			this->cacheAccess=0;
			this->nSets = L1_SETS;
			this->nLines = L1_ASSOCIATIVITY;
			this->WBs = 0;
			this->sets = new cacheSet_t[L1_SETS];
			for (size_t i = 0; i < L1_SETS; i++)
			{
				this->sets[i].linhas = new linha_t[L1_ASSOCIATIVITY];
				std::memset(this->sets[i].linhas,0,BYTES_ON_LINE);
			}
			break;
			}
		case LLC:{
			this->shiftData = utils_t::powerOf2(BLOCK_SIZE);
			this->level = level;
			this->cacheHit=0;
			this->cacheMiss=0;
			this->cacheAccess=0;
			this->nSets = LLC_SETS;
			this->nLines = LLC_ASSOCIATIVITY;
			this->WBs = 0;
			this->sets = new cacheSet_t[LLC_SETS];
			for (size_t i = 0; i < LLC_SETS; i++)
			{
				this->sets[i].linhas = new linha_t[LLC_ASSOCIATIVITY];
				std::memset(this->sets[i].linhas,0,BYTES_ON_LINE);
			}
			break;
			}
		}
};
uint32_t cache_t::idxSetCalculation(uint64_t address){
	uint32_t getBits = (this->nSets)-1;
	uint32_t tag = (address >> this->shiftData);
	uint32_t index = tag&getBits;
	return index;

};
inline uint32_t cache_t::searchLru(cacheSet_t *set){
	uint32_t index=0;
	for (size_t i = 1; i < this->nLines; i++)
	{
		index = (set->linhas[index].LRU <= set->linhas[i].LRU)? index : i ;
	}
	return index;
};
int32_t cache_t::searchAddress(uint64_t address){
	uint32_t idx = this->idxSetCalculation(address);
	uint64_t tag = address >> this->shiftData;
	for (size_t i = 0; i < this->nLines; i++)
	{
		if(this->sets[idx].linhas[i].tag==tag){
			return HIT;
		}
	}
	return MISS;
};
uint32_t cache_t::installLine(uint64_t address){
	uint32_t idx = this->idxSetCalculation(address);
	uint64_t tag = address >> this->shiftData;
	for (size_t i = 0; i < this->nLines; i++)
	{
		if(this->sets[idx].linhas[i].valid == 0){
			this->sets[idx].linhas[i].tag = tag;
			this->sets[idx].linhas[i].valid = 1;
			this->sets[idx].linhas[i].dirty = 0;
			this->sets[idx].linhas[i].prefetched = 0;
			this->sets[idx].linhas[i].readyCycle = 0;
			this->sets[idx].linhas[i].LRU = orcs_engine.get_global_cycle();
			return OK;
		}
	}
		uint32_t linha = this->searchLru(&this->sets[idx]);
		if(this->sets[idx].linhas[linha].dirty == 1){
			this->writeBack(idx,linha);
		}
		this->sets[idx].linhas[linha].tag = tag;
		this->sets[idx].linhas[linha].valid = 1;
		this->sets[idx].linhas[linha].dirty = 0;
		this->sets[idx].linhas[linha].prefetched = 0;
		this->sets[idx].linhas[linha].readyCycle = 0;
		this->sets[idx].linhas[linha].LRU = orcs_engine.get_global_cycle();
		return OK;
};
void cache_t::writeBack(uint32_t idx, uint32_t linha){
	this->WBs+=1;
	this->sets[idx].linhas[linha].valid = 0;
};
uint32_t cache_t::writeAllocate(uint64_t address){
	int32_t status = this->searchAddress(address);
	if(status == HIT){
		uint64_t tag = address >> this->shiftData;
		uint32_t idx = this->idxSetCalculation(address);
		for (size_t i = 0; i < this->nLines; i++)
		{
			if(this->sets[idx].linhas[i].tag == tag){
				this->sets[idx].linhas[i].dirty = 1;
			}
		}
		return HIT;
	}else{
		return MISS;
	}
};
void cache_t::statistics(){
	// fprintf(stderr,"Cache Level; %u\n",this->level+1);
	// fprintf(stderr,"Cache Access; %u\n",this->cacheAccess);
	// fprintf(stderr,"Cache Hits; %u\n",this->cacheHit);
	// fprintf(stderr,"Cache Miss; %u\n",this->cacheMiss);
	std::cout<<"Cache Level;"<<this->level+1<<std::endl;
	std::cout<<"Cache Access;"<<this->cacheAccess<<std::endl;
	std::cout<<"Cache Hits;"<<this->cacheHit<<std::endl;
	std::cout<<"Cache Miss;"<<this->cacheMiss<<std::endl;
	std::cout<<"Cache WB;"<<this->WBs<<std::endl;

}