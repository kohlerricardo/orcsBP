#include "simulator.hpp"
#include<fstream>
config_t::config_t()
{
    //ctor
}

config_t::~config_t()
{
    //dtor
}
void config_t::allocate(const char *path){
    fprintf(stderr,"%s\n",path);
    ////
    FILE *arq = NULL;
    if((arq = fopen(path,"r")) == NULL){
        perror("Erro:");
        
    }
    uint32_t bufferSize = 100;
    char *linha = new char[bufferSize];
    char *opt = new char[bufferSize];
    if(fgets(linha,bufferSize,arq)!=NULL)
        fprintf(stderr,"%s\n",linha);
    this->config = new config_t[atoi(linha)];
    while((fgets(linha,bufferSize,arq))!= NULL){
        fprintf(stderr,"%s\n",linha);
    }
    fclose(arq);
    delete linha;
    delete opt;

};

