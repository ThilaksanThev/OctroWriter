#include <iostream>
#include "Tokens.h"
#include <map>

namespace OctroWriter{
    //Die Befehle mit dem Parameter
    struct WriterCmd
    {
        WriterCmd(std::string cmd,std::string parameter){
            this->cmd = cmd;
            this->parameter = parameter;
        }
        std::string cmd;
        std::string parameter;
        /*
            [double]
            d ouble

            [int]
            f front
            s string
            b back 
            i int
            m marker
            x index
    */
    };
    /*
        Ein Befehl im Code,mit all seinen Informationen
    */
    struct ProtoCmd{
        int cmd;
        std::vector<std::string> params;
        /*
            params besteht nur aus 
            doubles oder ints 
            Markers sind als ProtoCmd Positionen gespeichert
                also in welchem ProtoCmd sie stehen
        */
        int codePos = 0;
    };
    class Writer
    {
        //Liste aller Commands
        std::vector<WriterCmd> writerCmds;

        
    
        //convertierung zum Speicherformat
        std::string convert(int i);
        std::string convert(double d);
        std::string convert(std::string s);

        /*
            Die grosste Zahl die als Parameter vorkommt aufschreiben
            damit man platzsparend ist
        */

        int max_f; //front
        int max_s; //string
        int max_b; //back 
        int max_i; //int
        int max_m; //marker
        int max_x; //index

        //writing metadata
        std::string loadVersion(Tokens*,size_t&);
        std::string loadStrings(Tokens*,size_t&);
        std::string loadLibraries(Tokens*,size_t&);

        //writing the code
        std::vector<ProtoCmd> generateProtoCmd(Tokens*,size_t&);
        int findMax(char c,std::vector<ProtoCmd>&);
        void findByteSizes(std::vector<ProtoCmd>&);
        std::string write(std::vector<ProtoCmd>&);

    public:
        Writer();
        ~Writer();

        void convert(std::string ipath,std::string opath);    
    };
}
