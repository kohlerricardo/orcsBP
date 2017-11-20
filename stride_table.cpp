#include "simulator.hpp"

void stride_table_t::allocate(){
    this->strideTable = new stride_table_t[STRIDE_TABLE_SIZE];
};
int32_t stride_table_t::searchPattern(uint64_t pc){
    uint64_t tag = pc>>2;
    int32_t idx = MISS;
    for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
    {
        idx = (tag == this->strideTable[i].tag)? i : idx;
    }
    if(idx!=MISS){
        this->strideTable[idx].lru = orcs_engine.get_global_cycle();
        return idx;
    }else{
        return MISS;
    }
};
inline uint32_t stride_table_t::searchLRU(){
    uint32_t idx = 0;
      for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
    {
        idx = (this->strideTable[idx].lru <= this->strideTable[i].lru)? idx : i;
    }
    return idx;
};
void stride_table_t::installStride(uint64_t pc,uint64_t address){
     uint64_t tag = pc>>2;
     fprintf(stderr,"%lu\n",tag);
     int32_t idx = MISS;
    for (size_t i = 0; i < STRIDE_TABLE_SIZE; i++)
    {
        idx = (this->strideTable[i].status == INVALID)? i : idx;
    }
    if(idx != MISS){
        this->strideTable[idx].tag = tag;
        this->strideTable[idx].last_address = address;
        this->strideTable[idx].stride = 0;
        this->strideTable[idx].status = TRAINING;
        this->strideTable[idx].lru = orcs_engine.get_global_cycle();
    }else{
    idx = this->searchLRU();
    this->strideTable[idx].tag = tag;
    this->strideTable[idx].last_address = address;
    this->strideTable[idx].stride = 0;
    this->strideTable[idx].status = TRAINING;
    this->strideTable[idx].lru = orcs_engine.get_global_cycle();
    }
};
void stride_table_t::updateStride(uint32_t idx, uint64_t address,uint32_t status){
    this->strideTable[idx].stride = address - this->strideTable[idx].last_address ;
    this->strideTable[idx].last_address = address;
    this->strideTable[idx].status = status;
    this->strideTable[idx].lru = orcs_engine.get_global_cycle();
};
void stride_table_t::printLine(int32_t idx){
    fprintf(stderr,"TAG:%lu;LA:%lu;Stride:%u;Status:%u;\n",this->strideTable[idx].tag,this->strideTable[idx].last_address,this->strideTable[idx].stride,this->strideTable[idx].status);
}