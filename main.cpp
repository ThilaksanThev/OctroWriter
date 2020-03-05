#include "Writer.hpp"

int main(){
    OctroWriter::Writer writer;
    writer.convert("test.o","test.bin");
    return 0;
}