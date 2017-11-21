#ifndef STRIDE_TABLE_H
#define STRIDE_TABLE_H

class stride_table_t{

    public:
    uint64_t tag;
    uint64_t last_address;
    uint32_t stride;
    uint32_t status;
    uint64_t lru;
};
#endif