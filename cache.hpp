#ifndef CACHE_H
#define CACHE_H


class cache_t
{
    public:
        cache_t();
        virtual ~cache_t();
        /**
        Functions of Cache
        */
        //functions
        void allocate(config_t config);
        void statistics();
        //AddressCalc
        uint32_t idxSetCalculation(uint64_t address);
        uint32_t tagSetCalculation(uint64_t address);
        //Search Function
        uint32_t searchAddress(uint64_t address);
        void writeBack(cache_t *cache);
        void installLine(uint64_t address);
        //atributtes
        uint32_t id;
        uint32_t level;
        uint32_t sizeLine;
        uint32_t associativity;
        uint32_t nSets;
        cacheSet_t *sets;
        uint32_t shiftData;
        uint32_t cacheHit;
        uint32_t cacheMiss;
        uint32_t cacheAccess;
        uint32_t latency;
        
};

#endif // CACHE_H
