#include "./simulator.hpp"

prefetcher_t::prefetcher_t(){
    //ctor
};

prefetcher_t::~prefetcher_t()
{
    //dtor
};
void prefetcher_t::allocate(){
    this->stridePrefetcher = new stride_table_t[STRIDE_TABLE_SIZE];
    for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
    {
            this->stridePrefetcher[i].tag = 0;
            this->stridePrefetcher[i].last_address = 0;
            this->stridePrefetcher[i].stride = 0;
            this->stridePrefetcher[i].status = INVALID;
            this->stridePrefetcher[i].lru = 0;
    }
    this->totalPrefetched = 0;
    this->usefulPrefetches = 0;
    this->latePrefetches = 0;
    this->totalCycleLate = 0;
};
void prefetcher_t::verify(uint64_t pc,uint64_t address){
    int32_t idx = this->searchPattern(pc);
    
    if(idx == MISS){
        this->installStride(pc,address);
    }else{ 
        uint64_t anterior = this->stridePrefetcher[idx].last_address - this->stridePrefetcher[idx].stride;
        uint64_t posterior = this->stridePrefetcher[idx].last_address + this->stridePrefetcher[idx].stride;

        if((address >= anterior)||(address <=posterior)){
            if(this->stridePrefetcher[idx].status == INVALID){
                this->updateStride(pc,address,TRAINING);
            }
            if(this->stridePrefetcher[idx].status == ACTIVE){
                // uint32_t stride = labs(address-this->stridePrefetcher[idx].last_address);
                // if(stride == this->stridePrefetcher[idx].stride){
                // //verifica se ta instalada
                // this->updateStride(pc,address,ACTIVE);
                // uint64_t new_address= (DISTANCE*this->stridePrefetcher[idx].stride)+this->stridePrefetcher[idx].last_address; 
                // // int32_t status = orcs_engine.cache[LLC].searchLinePrefetched(new_address);
                // if(status == MISS){
                //     // orcs_engine.cache[LLC].installLinePrefetched(new_address);
                    // }
                }else{
                    this->updateStride(pc,address,INVALID);
                }
            // }
            if(this->stridePrefetcher[idx].status == TRAINING){
                    this->updateStride(pc,address,ACTIVE);
            }
        }
    }
};

int32_t prefetcher_t::searchPattern(uint64_t pc){
    uint64_t tag = pc>>SHIFT_STRIDE;
    // fprintf(stderr,"\n\n %lu \n\n",tag);
    for (int32_t i = 0; i < STRIDE_TABLE_SIZE; i++)
    {
        if((this->stridePrefetcher[i].tag == tag)&&(this->stridePrefetcher[i].status != INVALID)){
            this->stridePrefetcher[i].lru = orcs_engine.get_global_cycle();
            return i;
        }
    }
    return MISS;
};
inline uint32_t prefetcher_t::searchLRU(){
    uint32_t idx = 0;
      for (uint32_t i = 1; i < STRIDE_TABLE_SIZE; i++)
    {
        idx = (this->stridePrefetcher[idx].lru < this->stridePrefetcher[i].lru)? idx : i;
    }
    return idx;
};
uint32_t prefetcher_t::installStride(uint64_t pc,uint64_t address){
     uint64_t tag = pc>>SHIFT_STRIDE;
    //  fprintf(stderr,"%lu\n",tag);
     
        for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
        {
            if(this->stridePrefetcher[i].status == INVALID){
                this->stridePrefetcher[i].tag = tag;
                this->stridePrefetcher[i].last_address = address;
                this->stridePrefetcher[i].stride = 0;
                this->stridePrefetcher[i].status = TRAINING;
                this->stridePrefetcher[i].lru = orcs_engine.get_global_cycle();
            return OK;
            }
        }
        uint32_t idx = this->searchLRU();
        this->stridePrefetcher[idx].tag = tag;
        this->stridePrefetcher[idx].last_address = address;
        this->stridePrefetcher[idx].stride = 0;
        this->stridePrefetcher[idx].status = TRAINING;
        this->stridePrefetcher[idx].lru = orcs_engine.get_global_cycle();
        return OK;
};
uint32_t prefetcher_t::updateStride(uint64_t pc, uint64_t address,uint32_t status){
    // fprintf(stderr,"update stride\n");
     uint64_t tag = pc>>SHIFT_STRIDE;
    //  
     
        for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
        {
            // fprintf(stderr,"%lu\n",tag);
            if(this->stridePrefetcher[i].tag == tag){
                this->stridePrefetcher[i].stride = labs(this->stridePrefetcher[i].last_address - address);
                this->stridePrefetcher[i].last_address = address;
                this->stridePrefetcher[i].status = status;
                this->stridePrefetcher[i].lru = orcs_engine.get_global_cycle();
            return OK;
            }
        }
    return OK;
};
void prefetcher_t::printLine(int32_t idx){
    fprintf(stderr,"TAG:%lu;LA:%lu;Stride:%u;Status:%u;\n",this->stridePrefetcher[idx].tag,this->stridePrefetcher[idx].last_address,this->stridePrefetcher[idx].stride,this->stridePrefetcher[idx].status);
}
void prefetcher_t::statistics(){
    std::cout<<"Total Prefetches;"<< this->totalPrefetched<<std::endl; 
    std::cout<<"Useful Prefetches;"<<this->usefulPrefetches<<std::endl;
    std::cout<<"Late Prefetches; "<<this->latePrefetches<<std::endl;
    std::cout<<"CycleLate; "<<this->totalCycleLate<<std::endl;
};