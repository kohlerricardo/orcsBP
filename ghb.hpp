#ifndef GHB_H
#define GHB_H

class ghb_t{
    protected:
    int8_t front;
    int8_t rear;

    public:
    ghb_t();
    virtual ~ghb_t();
    typedef struct
    {
        uint8_t valid;
        uint64_t address;
        uint8_t issued;
    }ghbQueue;
    ghbQueue *ghb_queue;
    int8_t enqueue(uint64_t address);
    uint64_t dequeue();
    void allocate();
    int32_t isFull();
    int32_t isEmpty();
};
#endif