#ifndef PPT_H
#define PPT_H
class ppt_t{

    public:
        uint64_t tag;
        uint32_t lastIdx1;
        uint32_t lastIdx0;
        uint32_t state; 
        uint32_t cnt;
        uint32_t ptr;
        uint64_t lru;

};

#endif