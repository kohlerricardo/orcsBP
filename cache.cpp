#include "simulator.hpp"


cache_t::cache_t()
{
    //ctor
}

cache_t::~cache_t()
{
    //dtor
}
void cache_t::allocate(config_t config){
    //verificador de numeros
    this->sizeLine = config.sizeLinha;
    this->associativity = config.associativity;
    this->level = config.level;
   // this->setId(config.getLevel()-1);
    //linhas da cache e vias

    // this->nSets = this->getNLines()/this->getAssociativity()));
    // //shifts
    // //
    // this->setShiftRightTag(utils_t::powerOf2(this->getSizeLine()));
    // this->setShiftRightIdx(Utils::powerOf2(this->getNSets())+this->getShiftRightTag());

    // this->sets = new cacheSet[this->getNSets()];
    // for (size_t i = 0; i < this->getNSets(); i++)
    // {
    //     this->sets[i].linhas = new Linha[this->getAssociativity()];
    // }

}
uint32_t cache_t::idxSetCalculation(uint64_t address){
    return address;
};
uint32_t cache_t::tagSetCalculation(uint64_t address){
    return address;
};
uint32_t cache_t::searchAddress(uint64_t address){
    uint32_t set = this->tagSetCalculation(address);
    uint32_t idx = this->idxSetCalculation(address);

    return set+idx ;
};