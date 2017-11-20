#ifndef LINHA_H
#define LINHA_H


class linha_t
{
    public:
        uint64_t tag;
        uint32_t dirty;
        uint32_t LRU;
        uint32_t prefetched;
        uint32_t valid;
        uint64_t readyCycle;
};

#endif // LINHA_H
