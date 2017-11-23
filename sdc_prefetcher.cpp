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
};
void sdc_prefetcher_t::printLine(uint32_t entry){
    fprintf(stderr,"Entry %u \n",entry);
    fprintf(stderr,"Tag %lu \n",this->ppt[entry].tag);
    fprintf(stderr,"Idx1 %u\n",this->ppt[entry].lastIdx1);
    fprintf(stderr,"Idx0 %u\n",this->ppt[entry].lastIdx0);
    fprintf(stderr,"State %u",this->ppt[entry].state);
    fprintf(stderr,"CNT %u\n",this->ppt[entry].cnt);
};
// =================================
inline uint32_t sdc_prefetcher_t::searchLRU(){
    uint idx = 0;
    for (size_t i = 1; i < PPT_TABLE_SIZE; i++)
    {
        idx = (this->ppt[idx].lru <= this->ppt[i].lru)?idx:i;
    }
    return idx;
};
inline uint64_t sdc_prefetcher_t::generateAddress(uint32_t idx){
    return (this->ppt[idx].tag << orcs_engine.cache[LLC].shiftData) | this->ppt[idx].lastIdx1;
}
// =================================
uint64_t sdc_prefetcher_t::generateTag(uint64_t address){
    return (address>>orcs_engine.cache[LLC].shiftData);
};
uint32_t sdc_prefetcher_t::generateIdx(uint64_t address){
    return (address & 0x3F);
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
    return idx;
};
// =================================
int32_t sdc_prefetcher_t::searchEntry(uint64_t address){
   
    int32_t idx = MISS;
    uint64_t tag = address >> orcs_engine.cache[LLC].shiftData;
    for(size_t i = 0; i < PPT_TABLE_SIZE; i++)
    {
        if((this->ppt[i].tag == tag) && (this->ppt[i].state != INVALID)){
            idx = i;
            return idx;
        }
    }
    return idx;
};
