#include "simulator.hpp"
#include <algorithm>
void plbp_t::allocate(){
    this->W = new int8_t[SIZE_W*SIZE_GA*(HISTORY)];
    this->GA = new uint32_t[SIZE_GA];
    this->GHR = new uint8_t[HISTORY];
    this->saida = 0;
}
uint32_t plbp_t::predict(uint64_t address){
    uint32_t index = address & (SIZE_W-1);
    this->saida = this->W[index];
    for (size_t i = 0; i < HISTORY; i++)
    {
     (this->GHR[i]==TAKEN)?this->saida += this->W[index+(this->GA[i]*i)+i]:this->saida -= this->W[index+(this->GA[i]*i)+i];   
    }
    return (this->saida >=0)? TAKEN: NOT_TAKEN;
    
}
void plbp_t::train(uint64_t address,uint32_t taken){
    uint32_t index = address & (SIZE_W-1);
    if((this->saida<THETA)||(this->predict(address) != taken)){
        if(taken ==TAKEN){
        this->W[index]+=1;
        }else{
            this->W[index]-=1;
        }
        for (size_t i = 0; i < HISTORY; i++)
        {
            if(this->GHR[i]==TAKEN){
                this->W[index+(this->GA[i]*i)+i]+=1;
            }else{
                this->W[index+(this->GA[i]*i)+i]-=1;
            }
        }
    }
    std::rotate(this->GA, this->GA+SIZE_GA, this->GA+(SIZE_GA+1));
    this->GA[0]=index;
    // GA[2..h] := GA[1..h − 1]
    // GA[1] := address
    std::rotate(this->GHR, this->GHR+HISTORY, this->GHR+(HISTORY+1));
    this->GHR[0]=TAKEN;
    // GHR[2..h] := GHR[1..h − 1]
    // GHR[1] := taken
}