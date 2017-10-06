#ifndef CONFIG_H
#define CONFIG_H


class config_t
{
    public:
        config_t();
        virtual ~config_t();
        void allocate(const char *path);
        config_t *config;
        uint32_t level;
        uint32_t associativity;
        uint32_t totalLinhas;
        uint32_t sizeLinha;

        
        

};

#endif // CONFIG_H
