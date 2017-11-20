#include "./simulator.hpp"

prefetcher_t::prefetcher_t(){
    //ctor
};

prefetcher_t::~prefetcher_t()
{
    //dtor
};
void prefetcher_t::allocate(){
    this->stridePrefetcher = new stride_table_t;
    this->stridePrefetcher->allocate();
    this->totalPrefetched = 0;
    this->usefulPrefetches = 0;
};
void prefetcher_t::verify(uint64_t pc,uint64_t address){
    int32_t idx = this->stridePrefetcher->searchPattern(pc);
    if(idx == MISS){
        this->stridePrefetcher->installStride(pc,address);
    }else{
        uint32_t new_stride = this->stridePrefetcher->strideTable[idx].last_address - address;
        this->stridePrefetcher->printLine(idx);
        sleep(1);
        if (new_stride == this->stridePrefetcher->strideTable[idx].stride)
        {
             this->totalPrefetched +=1;//counter prefetched lines
            if (this->stridePrefetcher->strideTable[idx].status == TRAINING)
            {   
                //atualiza linha do strideTable
                this->stridePrefetcher->updateStride(idx,address,ACTIVE);
               //cria end novo
                uint64_t new_address = address+(this->stridePrefetcher->strideTable[idx].stride*DISTANCE);
               //verifica se esta instalado 
                uint32_t status = orcs_engine.cache[LLC].searchAddress(new_address);
                if(status!=HIT){
                    //instala linha e atualiza parametros
                    orcs_engine.cache[LLC].installLine(new_address);
                    uint16_t index = orcs_engine.cache[LLC].idxSetCalculation(new_address);
                    uint16_t line = orcs_engine.cache[LLC].searchMru(&orcs_engine.cache[LLC].sets[index]);
                    orcs_engine.cache[LLC].sets[index].linhas[line].prefetched = 1;
                    orcs_engine.cache[LLC].sets[index].linhas[line].readyCycle = orcs_engine.get_global_cycle()+RAM_LATENCY;
                    orcs_engine.cache[LLC].sets[index].linhas[line].LRU = orcs_engine.get_global_cycle()+RAM_LATENCY;
                }
            }else if (this->stridePrefetcher->strideTable[idx].status == ACTIVE)
            {
                //atualiza linha do strideTable
                this->stridePrefetcher->updateStride(idx,address,ACTIVE);   
                //cria end novo
                uint64_t new_address = address+(this->stridePrefetcher->strideTable[idx].stride*DISTANCE);
                //verifica se esta instalado 
                uint32_t status = orcs_engine.cache[LLC].searchAddress(new_address);
                if(status!=HIT){
                     //instala linha e atualiza parametros
                    orcs_engine.cache[LLC].installLine(new_address);
                    uint16_t index = orcs_engine.cache[LLC].idxSetCalculation(new_address);
                    uint16_t line = orcs_engine.cache[LLC].searchMru(&orcs_engine.cache[LLC].sets[index]);
                    orcs_engine.cache[LLC].sets[index].linhas[line].prefetched = 1;
                    orcs_engine.cache[LLC].sets[index].linhas[line].readyCycle = orcs_engine.get_global_cycle()+RAM_LATENCY;
                    orcs_engine.cache[LLC].sets[index].linhas[line].LRU = orcs_engine.get_global_cycle()+RAM_LATENCY;
                }             
            }
        }
        else{
           if(this->stridePrefetcher->strideTable[idx].stride==0){
            //atualiza pra training
            this->stridePrefetcher->updateStride(idx,address,TRAINING);
           }else{   
                this->stridePrefetcher->updateStride(idx,address,INVALID);
           }
        }
    }
};
void prefetcher_t::statistics(){
fprintf(stderr,"Total Prefetches; %u\n",this->totalPrefetched); 
fprintf(stderr,"Useful Prefetches; %u\n",this->usefulPrefetches); 
};