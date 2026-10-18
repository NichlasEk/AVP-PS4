#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "huffman.hpp"
int main(int argc, char **argv) {
    if (argc != 3) return 2;
    FILE *f = fopen(argv[1], "rb");
    if (!f) return 3;
    fseek(f, 0, SEEK_END); long n=ftell(f); rewind(f);
    if (n < (long)sizeof(HuffmanPackage)) return 4;
    auto *p=(HuffmanPackage*)calloc(1, n);
    if (!p || fread(p, 1, n, f)!=(size_t)n) return 5;
    fclose(f);
    if (memcmp(p->Identifier,"REBCRIF1",8) || p->UncompressedDataSize<0 || p->UncompressedDataSize>10000000) return 6;
    if (p->CompressedDataSize<0 || p->CompressedDataSize>n-(long)sizeof(HuffmanPackage)) return 6;
    char *s=HuffmanDecompress(p);
    f=fopen(argv[2],"wb");
    if (!s || !f) return 7;
    bool ok=fwrite(s,1,p->UncompressedDataSize,f)==(size_t)p->UncompressedDataSize;
    fclose(f); free(s); free(p);
    return ok ? 0 : 8;
}
