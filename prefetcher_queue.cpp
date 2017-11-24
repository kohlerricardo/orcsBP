#include "./simulator.hpp"

prefetch_queue_t::prefetch_queue_t()
{
    //ctor

};
prefetch_queue_t::~prefetch_queue_t() 
{
    //dtor
};


 //implementacao externs
int8_t prefetch_queue_t::enqueue(uint64_t pAddress){
    if(!isFull()){
        // fprintf(stdout,"adicionado queue %lu\n",pAddress);
        if((this->rear == SIZE_QUEUES -1) && (this->front!=0)){
            this->rear = -1;
        }
        this->rear++;
        this->p_queue[this->rear].address = pAddress;
        this->p_queue[this->rear].valid = 1;
        this->p_queue[this->rear].issued = 0;
        if(this->front ==-1)this->front=0;
        return HIT;
    }else{
        return MISS;
    }
};

uint64_t prefetch_queue_t::dequeue(){
    uint64_t address =0;
    if(!isEmpty()){
        // fprintf(stdout,"inside\n");
        address= this->p_queue[this->front].address;
        this->p_queue[this->front].issued=1;
        this->p_queue[this->front].issued=0;
        this->front++;
        if(this->front==SIZE_QUEUES)
            this->front = 0;
        if((this->front-1)==(this->rear))
        this->front=this->rear=-1;
    }
    return address;
};

void prefetch_queue_t::allocate(){
    this->front = -1;
    this->rear = -1;
    this->p_queue = new pQueue[SIZE_QUEUES];
};
int32_t prefetch_queue_t::isFull(){
    return ((this->rear == (SIZE_QUEUES-1) && this->front == 0) || (this->front == this->rear+1))? OK : FAIL;
}
int32_t prefetch_queue_t::isEmpty(){
    return ((this->front == -1) && (this->rear ==-1))?OK:FAIL;
}