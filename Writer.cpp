#include "Writer.hpp"
#include <fstream>

using namespace OctroWriter;

Writer::Writer()
{
    writerCmds.push_back(WriterCmd("confront","fb"));
    writerCmds.push_back(WriterCmd("confrontname","fbs"));
    writerCmds.push_back(WriterCmd("conback","bb"));
    writerCmds.push_back(WriterCmd("conbackname","bbs"));

    writerCmds.push_back(WriterCmd("copyfront","f"));
    writerCmds.push_back(WriterCmd("copyfrontname","fs"));
    writerCmds.push_back(WriterCmd("copyback","b"));
    writerCmds.push_back(WriterCmd("copybackname","bs"));

    writerCmds.push_back(WriterCmd("svi","i"));
    writerCmds.push_back(WriterCmd("svd","d"));
    writerCmds.push_back(WriterCmd("svs","s"));
    writerCmds.push_back(WriterCmd("svm","m"));

    writerCmds.push_back(WriterCmd("repushfront","f"));
    writerCmds.push_back(WriterCmd("repushback","b"));

    writerCmds.push_back(WriterCmd("end","i"));
    writerCmds.push_back(WriterCmd("pop","b"));


    writerCmds.push_back(WriterCmd("collect","b"));
    writerCmds.push_back(WriterCmd("anticollect",""));

    writerCmds.push_back(WriterCmd("arrayinsert","x"));
    writerCmds.push_back(WriterCmd("arrayremove","x"));
    writerCmds.push_back(WriterCmd("arrayrename","xs"));
    writerCmds.push_back(WriterCmd("arrayget","s"));
    
    writerCmds.push_back(WriterCmd("execfront","fb"));
    writerCmds.push_back(WriterCmd("execback","bi"));
    writerCmds.push_back(WriterCmd("exec","sb"));

    writerCmds.push_back(WriterCmd("if","m"));
    writerCmds.push_back(WriterCmd("ifelse","mm"));
    writerCmds.push_back(WriterCmd("while","m"));
    
    
    std::vector<std::string> ops = {
       "add","sub","multi","div","mod",
       "setadd","setsub","setmulti","setdiv","setmod",
       "set","equals","unequals","smaller","smallersame",
       "bigger","biggersame","and","or","not",
       "deref","address",
       "index"
    };
    for (size_t i = 0; i < ops.size(); i++)
    {
        writerCmds.push_back(WriterCmd(ops[i],""));
    }
}

Writer::~Writer()
{
}
std::string Writer::convert(int i)
{
    union MyUnion
    {
        int i;
        char c[4];
    }mu;
	mu.i = i;
	return std::string() + mu.c[0] + mu.c[1] + mu.c[2] + mu.c[3];
}
std::string Writer::convert(double d)
{
    union MyUnion2
    {
        double i;
        char c[8];
    }mu2;

	mu2.i = d;
	return std::string() + mu2.c[0] + mu2.c[1] + mu2.c[2] + mu2.c[3] + mu2.c[4] + mu2.c[5] + mu2.c[6] + mu2.c[7];
}
std::string Writer::convert(std::string v){
	return convert((int)v.length())+v;
}

void Writer::convert(std::string ipath,std::string opath){
    Tokens tokens(0);
    tokens.loadFromFile(ipath);
    size_t pos = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    std::fstream file(opath,std::ios::out);
    //set version
    file<<loadVersion(&tokens,pos);
    //load strings
    file<<loadStrings(&tokens,pos);
    //load libs
    file<<loadLibraries(&tokens,pos);

    //generate protocmds
    auto a = generateProtoCmd(&tokens,pos);
    
    //find the biggest things
    file << convert(max_f = findMax('f',a));
    file << convert(max_b = findMax('b',a));
    file << convert(max_i = findMax('i',a));
    file << convert(max_s = findMax('s',a));
    file << convert(max_m = findMax('m',a));
    file << convert(max_x = findMax('x',a));
    
    //man findet nun welche byteposition die befehle haben
    findByteSizes(a);
    file << write(a);
    file.close();
}
std::string Writer::loadVersion(Tokens* t,size_t& i){
    auto rv = convert(0) //Octro Version 0
        +convert(t->children[i]->value); //Skript Version
    i++;
    return rv;
}
std::string Writer::loadStrings(Tokens* t,size_t&i){
    int count = 0;
    std::string rv = "";
    while(i<t->children.size()){
        if(t->children[i]->type==Tokens::STRING){
            rv += convert(t->children[i]->value);
            i++;
            count++;
        }
        else {
            return convert(count)+rv;
        }
    }
    return convert(count)+rv;
}
std::string Writer::loadLibraries(Tokens* t,size_t& i){
    //lib name version size
    int count = 0;
    std::string rv = "";
    while(i<t->children.size()){
        if(t->children[i]->type==Tokens::LETTER?t->children[i]->value=="lib":false){
            i++;
            rv += convert(t->children[i]->value);//name
            i++;
            rv += convert(t->children[i]->value);//version
            i++;
            rv += convert(std::stoi(t->children[i]->value));//size
            i++;
            count++;
        }
        else {
            return convert(count)+rv;
        }
    }
    return convert(count)+rv;
}
std::vector<ProtoCmd> Writer::generateProtoCmd(Tokens* t,size_t& i){
    std::map<std::string,size_t> marker;
    std::vector<ProtoCmd> rv;
    while(i<t->children.size()){
        int cmd = -1;
        for(int e = 0;e<writerCmds.size();e++)
            if(t->children[i]->value==writerCmds[e].cmd)
                cmd = e;
        if(t->children[i]->value=="mark"){
            i++;
            if(i<t->children.size())
                marker[t->children[i]->value] = rv.size();
            i++;            
        }else if(cmd==-1){
                std::cout << "UNKNOWN CMD!"<<std::endl;
                exit(-1);
            }
        else{
            i++;
            ProtoCmd pc;
            pc.cmd = cmd;

            std::string parameters = writerCmds[cmd].parameter;
            for(int e = 0;e<parameters.size();e++){
                if(i<t->children.size()){
                    pc.params.push_back(t->children[i]->value);
                }
                i++;
            }
            rv.push_back(pc);
        }
    }
    //marker replacement
    for(auto& x:rv){
        std::string parameters = writerCmds[x.cmd].parameter;
        for(int e = 0;e<parameters.size();e++){
            if(parameters[e]=='m'&&x.params.size()>e)// Es ist ein Marker
                x.params[e] = std::to_string(marker[x.params[e]]);
        }
    }
                     
    return rv;
}
int Writer::findMax(char c,std::vector<ProtoCmd>& cmds){
    int highest = 0;
    for(auto& a:cmds){
        if(writerCmds[a.cmd].parameter.find(c)!=std::string::npos)
        {
            auto p = writerCmds[a.cmd].parameter;
            for(int e = 0; e<p.size()&&e<a.params.size();e++){
                if(p[e]==c){
                    int n = std::stoi(a.params[e]);
                    if(n>highest)
                        highest = n;
                }
            }
        }
    }
    int count = 0;
    while(highest){
        highest/=256;
        count++;
    }
    return count;
}
void Writer::findByteSizes(std::vector<ProtoCmd>& a){
    size_t pos = 0;
    for(ProtoCmd&b :a){
        b.codePos = pos;
        pos++;//fuer den Befehl
        for(auto c:writerCmds[b.cmd].parameter){
            switch(c){
                case 'd':
                    pos+=8;
                    break;
                case 'f':
                    pos+=max_f;
                    break;
                case 's':
                    pos+=max_s;
                    break;
                case 'b':
                    pos+=max_b;
                    break;
                case 'i':
                    pos+=max_i;
                    break;
                case 'm':
                    pos+=max_m;
                    break;
                case 'x':
                    pos+=max_x;
                    break;
                break;
                default:
                break;
            }
        }
    }
    //d = 8
}
std::string Writer::write(std::vector<ProtoCmd>&a){
    std::string rv = "";
    for(auto&b:a){
        rv+= (char)(unsigned int)b.cmd;
        auto param = writerCmds[b.cmd].parameter;
        for(int i=0;i<param.size();i++){
            char c = param[i];
            if(c=='f'||c=='s'||c=='b'||c=='i'||c=='x'||c=='m'){
                int zahl = std::stoi(b.params[i]);
                rv+=convert(zahl).substr(0,
                    c=='f'?max_f:
                    c=='s'?max_s:
                    c=='b'?max_b:
                    c=='i'?max_i:
                    c=='x'?max_x:
                    c=='m'?max_m:0
                );
            }
            if(c=='d'){
                rv+=convert(std::stod(b.params[i]));
            }
        }
    }
    return rv;
}