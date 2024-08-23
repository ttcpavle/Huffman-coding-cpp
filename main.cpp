#include <iostream>
#include <fstream>
#include "HuffmanCoding.h"
using namespace std;

int main() {
    //ifstream original("test_large.txt");
    ifstream original("test.txt");
    ofstream compressed("compressed.bin", ios::binary);
    compress(original, compressed);

    compressed.close();
    original.close();

    ifstream compressedRead("compressed.bin", ios::binary);
    ofstream decompressed("decompressed.txt");
    decompress(compressedRead, decompressed);

    compressedRead.close();
    decompressed.close();

    return 0;
}
