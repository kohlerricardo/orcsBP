#include "simulator.hpp"
#include <algorithm>
#include <cmath>
void plbp_t::allocate(){
    this->W = new int8_t**[N];
    for(size_t i = 0; i<N;i++){
        this->W[i] = new int8_t*[M];
        for(size_t j = 0; j<M;j++){
            this->W[i][j] = new int8_t[H];
        }
    }

    this->GA = new uint32_t[M];
    this->GHR = new uint8_t[H];
    this->saida = 0;
}
uint32_t plbp_t::predict(uint64_t address){
    uint32_t index = address%N;
    fprintf(stderr,"aaaaaaaa %u\n",index);
    this->saida = this->W[index][0][0];
    for (size_t i = 0; i < H; i++)
    {
     (this->GHR[i]==TAKEN)?this->saida += this->W[index][this->GA[i]][i]\
     :this->saida -= this->W[index][this->GA[i]][i];   
    }
    return (this->saida >=0)? TAKEN: NOT_TAKEN;
    
}
void plbp_t::train(uint64_t address,uint32_t taken){
    uint32_t index = address%N;
    if((abs(this->saida)<THETA)||(this->predict(address) != taken)){
        if(taken ==TAKEN){
            this->W[index][0][0]+=1;
        }else{
            this->W[index][0][0]-=1;
        }
        for (size_t i = 0; i < H; i++)
        {
            if(this->GHR[i]==TAKEN){
                this->W[index][this->GA[i]][i]+=1;
            }else{
                this->W[index][this->GA[i]][i]-=1;
            }
        }
    }
    std::rotate(this->GA, this->GA+M, this->GA+(M+1));
    this->GA[0]=index;
    // GA[2..h] := GA[1..h − 1]
    // GA[1] := address
    std::rotate(this->GHR, this->GHR+H, this->GHR+(H+1));
    this->GHR[0]=taken;
    // GHR[2..h] := GHR[1..h − 1]
    // GHR[1] := taken
};