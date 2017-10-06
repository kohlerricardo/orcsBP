#include "simulator.hpp"
#include <algorithm>
#include <cmath>
plbp_t::plbp_t(){
    this->W = NULL;
    this->GA = NULL;
    this->GHR = NULL;
};
plbp_t::~plbp_t(){
    if((this->W) && (this->SR) && (this->R)) {
        for (size_t i = 0; i < N; i++)
        {            
            for (size_t j = 0; j < M; j++)
            {   
                delete [] this->W[i][j];
            }
            delete[] this->W[i];
            delete[] this->SR[i];
            delete[] this->R[i];
        }
    }
    delete[] W;
    delete[] R;
    delete[] SR;
    if(this->GA) delete [] GA;
    if(this->GHR) delete [] GHR;
};

void plbp_t::allocate(){
    this->W = new int8_t**[N];
    this->SR = new int8_t*[N];
    this->R = new int8_t*[N];
    
    for (size_t i = 0; i < N; i++)
    {
        this->W[i] = new int8_t*[M];
        for (size_t j = 0; j < M; j++)
        {
            this->W[i][j] = new int8_t[H];
            this->SR[i] = new int8_t[H];
            this->R[i] = new int8_t[H];
            
        }
    }
    
    this->GA = new uint32_t[M];
    this->GHR = new uint8_t[H];
    this->saida = 0;
}
uint32_t plbp_t::predict(uint64_t address){
    uint32_t indexA = address%N;
    uint32_t indexB = address%M;
    this->saida = this->R[indexA][(H-1)]+this->W[indexA][indexB][0];
    uint32_t predict = (this->saida>=0)?TAKEN:NOT_TAKEN;

    for(size_t i = 0;i<N;i++){
        for(size_t j=0;j<H;j++){
            int8_t a = H-j;
            (predict == TAKEN)?this->SR[i][a+1]=this->R[i][a]+this->W[i][indexB][j]:this->SR[i][a+1]=this->R[i][a]-this->W[i][indexB][j];
        }
        std::memcpy(this->R[i],this->SR[i],H);
    }
    for(size_t i = 0;i<N;i++){
        this->R[i][0] = 0;
    }
    for (size_t i = 0; i < H; i++)
    {
     (this->GHR[i]==TAKEN)?this->saida += this->W[indexA][this->GA[i]][i]
     :this->saida -= this->W[indexA][this->GA[i]][i];   
    }
    // practical plbp


    return (this->saida >=0)? TAKEN: NOT_TAKEN;
    
}
void plbp_t::train(uint64_t address,uint32_t taken){
    uint32_t indexA = address%N;
    uint32_t indexB = address%M;
    if((abs(this->saida)<THETA)||(this->predict(address) != taken)){
        if(taken==TAKEN){
            this->W[indexA][0][0] = this->W[indexA][0][0] + 1;
        }else{
            this->W[indexA][0][0] = this->W[indexA][0][0] - 1;
        }
        for (size_t i = 0; i < H; i++)
        {
            if(this->GHR[i]==TAKEN){
                this->W[indexA][this->GA[i]][i]+=1;
            }else{
                this->W[indexA][this->GA[i]][i]-=1;
            }
        }
    }
    std::rotate(this->GA, this->GA+M, this->GA+(M+1));
    this->GA[0]=indexB;
    std::rotate(this->GHR, this->GHR+H, this->GHR+(H+1));
    this->GHR[0]=taken;
};
