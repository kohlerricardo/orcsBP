#ifndef PREFETCH_QUEUE_H
#define PREFETCH_QUEUE_H

class prefetch_queue_t{
    protected:
    int8_t front;
    int8_t rear;

    public:
    prefetch_queue_t();
    virtual ~prefetch_queue_t();
    typedef struct
    {
        uint8_t valid;
        uint64_t address;
        uint8_t issued;
    }pQueue;
    pQueue *p_queue;
    int8_t enqueue(uint64_t address);
    uint64_t dequeue();
    void allocate();
    int32_t isFull();
    int32_t isEmpty();

    

};
#endif 