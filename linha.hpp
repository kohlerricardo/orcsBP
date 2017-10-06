#ifndef LINHA_H
#define LINHA_H


class linha_t
{
    public:
        linha_t();
        virtual ~linha_t();
        uint32_t tag;
        uint32_t dirty;
        uint32_t LRU;
};

#endif // LINHA_H
