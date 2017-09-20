class plbp_t{

public:
    int8_t ***W;
    uint32_t *GA;
    uint8_t *GHR;
    float saida;
    void allocate();
    uint32_t predict(uint64_t address);
    void train(uint64_t address,uint32_t taken);
    plbp_t(){
        this->W = NULL;
        this->GA = NULL;
        this->GHR = NULL;
    };
    ~plbp_t(){
        if(this->W){
            for(size_t i = 0; i<128;i++){
                for(size_t j = 0; j<127;j++){
                    delete [] W[i][j];
                }
                delete [] W[i];                
            }
        delete[] W;
        }
        if(this->GA) delete [] GA;
        if(this->GHR) delete [] GHR;
    };
};