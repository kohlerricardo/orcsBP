#ifndef PREFETCHER_H
#define PREFETCHER_H


class prefetcher_t{

    public:
        prefetcher_t();
        virtual ~prefetcher_t();
        uint32_t totalPrefetched;
        uint32_t usefulPrefetches;
        uint32_t latePrefetches;
        uint64_t totalCycleLate;
        stride_table_t *stridePrefetcher;
        sdc_prefetcher_t *sdc_prefetcher;
        //search entry
        int32_t searchPattern(uint64_t pc);
        void allocate();
        inline uint32_t searchLRU();
        uint32_t installStride(uint64_t pc, uint64_t address);
        uint32_t updateStride(uint64_t pc, uint64_t address, uint32_t status);
        void printLine(int32_t idx);
        void verify(uint64_t pc,uint64_t address);
        void statistics();
};
#endif