#include "./simulator.hpp"

sdc_prefetcher_t::sdc_prefetcher_t()
{
    //ctor

};
sdc_prefetcher_t::~sdc_prefetcher_t()
{
    //dtor
};

void sdc_prefetcher_t::allocate(){
    this->ghb = new ghb_t[GHB_SIZE];
    this->ppt = new ppt_t[PPT_TABLE_SIZE];
    this->queue = new prefetch_queue_t;
    this->queue->allocate();
    this->installedLines=0;
};
void sdc_prefetcher_t::printLine(uint32_t entry){
    fprintf(stderr,"Entry %u \n",entry);
    fprintf(stderr,"Tag %lu \n",this->ppt[entry].tag);
    fprintf(stderr,"Idx1 %u\n",this->ppt[entry].lastIdx1);
    fprintf(stderr,"Idx0 %u\n",this->ppt[entry].lastIdx0);
    fprintf(stderr,"State %u\n",this->ppt[entry].state);
    fprintf(stderr,"CNT %u\n",this->ppt[entry].cnt);
};
void sdc_prefetcher_t::prefetchFunction(uint32_t entry, uint64_t address){
    if(this->ppt[entry].state==STRIDE){
        uint64_t pAddress = this->generatePrefetchAddress(address,entry);
        // fprintf(stdout,"add gerado to pref. %lu\n",pAddress);
        this->issuePrefetchQueue(pAddress);
        uint64_t fila = this->queue->dequeue();
        // fprintf(stdout,"tirado pref queue %lu\n",fila);
        this->prefetch(fila);
        // fprintf(stdout,"address %lu, p_address %lu\n", address,pAddress);
        // sleep(1);
    }
    else if (this->ppt[entry].state==DELTA){
        // fprintf(stdout,"delta\n");
    }
};
void sdc_prefetcher_t::issuePrefetchQueue(uint64_t pAddress){
    int32_t status = MISS;
    for (size_t i = 0; i < SIZE_QUEUES; i++)
    {

        if((pAddress == this->queue->p_queue[i].address)){
          status = HIT;
          break;
        }
    }
     if(status !=HIT){
        if(this->queue->enqueue(pAddress)!=HIT){
            this->prefetch(this->queue->dequeue());
            this->queue->enqueue(pAddress);
        }             
                
    }
};
void sdc_prefetcher_t::prefetch(uint64_t address){
    // fprintf(stdout,"add recv. to prefetch%lu,\n",address);
    int32_t status = orcs_engine.cache[LLC].searchAddress(address);
    if(status == MISS){
        orcs_engine.cache[LLC].installLinePrefetched(address);
        orcs_engine.prefetcher->totalPrefetched++;

    }
};
// =================================

inline uint64_t sdc_prefetcher_t::generatePrefetchAddress(uint64_t address,uint32_t entry){
//  fprintf(stdout,"%lu\n",address);
#if BITS_32
    address = address >>TAG_SHIFT_64;
    address = ((address <<TAG_SHIFT_64) | ((this->ppt[entry].tag<<TAG_SHIFT_32)|this->ppt[entry].lastIdx0));
#else
    address = (this->ppt[entry].tag<<TAG_SHIFT_64)|this->ppt[entry].lastIdx0;
#endif
     uint32_t stride = abs(this->ppt[entry].lastIdx1-this->ppt[entry].lastIdx0);
    // fprintf(stdout,"%lu\n",address);
     address += (stride* DISTANCE);
     
     return address;
};

inline uint32_t sdc_prefetcher_t::searchLRU(){
    uint idx = 0;
    for (size_t i = 1; i < PPT_TABLE_SIZE; i++)
    {
        idx = (this->ppt[idx].lru <= this->ppt[i].lru)?idx:i;
    }
    return idx;
};
void sdc_prefetcher_t::statistics(){
    fprintf(stdout,"Installed Lines %lu\n",this->installedLines);
};
// =================================
uint64_t sdc_prefetcher_t::generateTag(uint64_t address){
#if BITS_32
    address = address & LEAST_S64;
    return (address>>TAG_SHIFT_32);
#else
    return ((address>>TAG_SHIFT_64));
#endif  
};
uint32_t sdc_prefetcher_t::generateIdx(uint64_t address){
#if BITS_32
    address = (address & LEAST_S64) & LEAST_S32;
    return address>>orcs_engine.cache[LLC].shiftData;
#else
    return ((address & LEAST_S64))>>orcs_engine.cache[LLC].shiftData;
#endif
};
uint32_t sdc_prefetcher_t::installNew(uint64_t address){
    uint32_t idx = -1;
    for (size_t i = 0; i <PPT_TABLE_SIZE; i++)
    {
        if(this->ppt[i].state == INVALID){
            this->ppt[i].cnt = 0;
            this->ppt[i].lastIdx0 = 0;
            this->ppt[i].lastIdx1 = this->generateIdx(address);
            this->ppt[i].lru = orcs_engine.get_global_cycle();
            this->ppt[i].tag = this->generateTag(address);
            this->ppt[i].state = INIT;
            this->ppt[i].ptr =0;
            this->installedLines+=1;
            idx = i;
            return idx;
        }
    }
    idx = this->searchLRU();
    this->ppt[idx].cnt = 0;
    this->ppt[idx].lastIdx0 = 0;
    this->ppt[idx].lastIdx1 = this->generateIdx(address);
    this->ppt[idx].lru = orcs_engine.get_global_cycle();
    this->ppt[idx].tag = this->generateTag(address);
    this->ppt[idx].state = INIT;
    this->ppt[idx].ptr =0;
    //
    this->installedLines+=1;
    return idx;
};
// =================================
int32_t sdc_prefetcher_t::searchEntry(uint64_t address){
   
    int32_t idx = MISS;
    uint64_t tag = this->generateTag(address);
   //fprintf(stdout,"tag %lu, address %lu\n",tag, address);
    for(size_t i = 0; i < PPT_TABLE_SIZE; i++)
    {
        if((this->ppt[i].tag == tag) && (this->ppt[i].state != INVALID)){
            idx = i;
            return idx;
        }
    }
    return idx;
};
uint32_t sdc_prefetcher_t::updateEntry(uint64_t address){
    //  uint64_t tag = this->generateTag(address);
     uint64_t lastIdx1 = this->generateIdx(address);
     int32_t idx = this->searchEntry(address);
    uint32_t stride0 = abs(this->ppt[idx].lastIdx1 - this->ppt[idx].lastIdx0);
    uint32_t stride1 = abs(this->ppt[idx].lastIdx1 - lastIdx1);
     if(idx == MISS){
         exit(1);
     }else{
        if(this->ppt[idx].state==INIT){
            this->ppt[idx].lastIdx0 = this->ppt[idx].lastIdx1;
            this->ppt[idx].lastIdx1 = lastIdx1;
            this->ppt[idx].state = STRIDE;
            this->ppt[idx].ptr = 0;
        //    fprintf(stdout,"update from init to STRIDE\n");
        }else if(this->ppt[idx].state==STRIDE){
            if(stride0==stride1){
                if(this->ppt[idx].cnt==3){
                    this->ppt[idx].cnt=0;
                    }else{
                        this->ppt[idx].cnt+=1;
                        }
            this->ppt[idx].lastIdx0 = this->ppt[idx].lastIdx1;
            this->ppt[idx].lastIdx1 = lastIdx1;
            this->ppt[idx].cnt = 0;
            this->ppt[idx].ptr = 0;
            // fprintf(stdout,"Permanece Stride\n");
            }else{
                //altera para delta
            //shifta para ghb
            //this->shiftGHB(idx);
            this->ppt[idx].lastIdx0 = this->ppt[idx].lastIdx1;
            this->ppt[idx].lastIdx1 = lastIdx1;
            this->ppt[idx].state = DELTA;
            // fprintf(stdout,"update from Stride to DELTA\n");
            }

         }else{
            if(stride0==stride1){
                this->ppt[idx].state = STRIDE;
                // fprintf(stdout,"update from delta to STRIDE\n");
                }
            this->ppt[idx].lastIdx0 = this->ppt[idx].lastIdx1;
            this->ppt[idx].lastIdx1 = lastIdx1;
            this->ppt[idx].cnt = 1;
         }

     }
     return idx;

 };
