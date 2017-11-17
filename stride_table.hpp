#ifndef STRIDE_TABLE_H
#define STRIDE_TABLE_H

class stride_table_t{

    public:
      uint64_t tag;
      uint64_t last_address;
      uint32_t stride;
      uint32_t status;
      uint64_t lru;
      stride_table_t *strideTable;
    stride_table_t(){
        this->strideTable = NULL;
    };
    ~stride_table_t(){
        if(this->strideTable) delete[] &this->strideTable;
    };
    //search entry
    uint32_t searchPattern(uint64_t pc);
    void allocate();
    inline uint32_t searchLRU();
    void installStride(uint64_t pc, uint64_t address);
    void updateStride(uint32_t idx, uint64_t address, uint32_t status);
};
#endif