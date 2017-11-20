#ifndef PREFETCHER_H
#define PREFETCHER_H


class prefetcher_t{

    public:
        prefetcher_t();
        virtual ~prefetcher_t();
        uint32_t totalPrefetched;
        uint32_t usefulPrefetches;
        stride_table_t *stridePrefetcher;
    void allocate();
    void verify(uint64_t pc,uint64_t address);
    void statistics();
};
#endif