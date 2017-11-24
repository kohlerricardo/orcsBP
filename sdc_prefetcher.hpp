#ifndef SDC_PREFETCHER_H
#define SDC_PREFETCHER_H

class sdc_prefetcher_t{

    public:
    //construct
    sdc_prefetcher_t();
    virtual ~sdc_prefetcher_t();
    //methods
    //basics
    void allocate();//ok
    void statistics();//ok
    void prefetchFunction(uint32_t entry, uint64_t address);
    void issuePrefetchQueue(uint64_t pAdress);

    //methods for manipulate address
    void shiftGHB(uint32_t idx);
    //debug Methods
    void printLine(uint32_t entry);//ok;
    void prefetch(uint64_t address);

    //inline
    inline uint32_t searchLRU();//OK
    inline uint64_t generateAddress(uint32_t idx);//OK
    inline uint64_t generatePrefetchAddress(uint64_t address,uint32_t entry);
    //uints 64, 32, 16, 8
    uint64_t generateTag(uint64_t address);//OK
    uint32_t generateIdx(uint64_t address);//OK
    uint32_t installNew(uint64_t address);//ok
    uint32_t updateEntry(uint64_t address);
    //ints 64, 32, 16, 8
    int32_t searchEntry(uint64_t address); //OK
   

    //object pointers
    ppt_t *ppt;
    ghb_t *ghb;
    prefetch_queue_t *queue;
    //statistics atrib
    uint64_t installedLines;
};

#endif