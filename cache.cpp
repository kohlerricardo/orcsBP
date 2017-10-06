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
        
			
			fprintf(stderr,"%d\n",L1_SIZE);
			fprintf(stderr,"linhas %d\n",(L1_SIZE/BLOCK_SIZE));
			fprintf(stderr,"sets %d\n",(L1_SETS));
			
			break;
			}
		case LLC:{
			this->shiftData = utils_t::powerOf2(BLOCK_SIZE);
			this->level = level;
			this->cacheHit=0;
			this->cacheMiss=0;
			this->cacheAccess=0;
        
			
			fprintf(stderr,"%d\n",LLC_SIZE);
			fprintf(stderr,"linhas %d\n",(LLC_SIZE/BLOCK_SIZE));
			fprintf(stderr,"sets %d\n",(LLC_SETS));
			break;
			}
		}
    //verificador de numeros
   // this->setId(config.getLevel()-1);
    //linhas da cache e vias

    // this->nSets = this->getNLines()/this->getAssociativity()));
    // //shifts
    // //
    // this->setShiftRightTag(utils_t::powerOf2(this->getSizeLine()));
    // this->setShiftRightIdx(Utils::powerOf2(this->getNSets())+this->getShiftRightTag());

    // this->sets = new cacheSet[this->getNSets()];
    // for (size_t i = 0; i < this->getNSets(); i++)
    // {
    //     this->sets[i].linhas = new Linha[this->getAssociativity()];
    // }

}
uint32_t cache_t::idxSetCalculation(uint64_t address){
    return address;
};
uint32_t cache_t::tagSetCalculation(uint64_t address){
    return address;
};
uint32_t cache_t::searchAddress(uint64_t address){
    uint32_t set = this->tagSetCalculation(address);
    uint32_t idx = this->idxSetCalculation(address);

    return set+idx ;
};
