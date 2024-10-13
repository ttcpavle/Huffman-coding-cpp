#include <iostream>
#include <fstream>
#include "HuffmanCoding.h"
#include "config.h"
using namespace std;

int main() {
    //ifstream original(FILEPATH(INPUT_FILES, "test_large.txt"));
    ifstream original(FILEPATH(INPUT_FILES, "test.txt"));
    ofstream compressed(FILEPATH(OUTPUT_FILES, "compressed.bin"), ios::binary);
    compress(original, compressed);

    compressed.close();
    original.close();

    ifstream compressedRead(FILEPATH(OUTPUT_FILES, "compressed.bin"), ios::binary);
    ofstream decompressed(FILEPATH(OUTPUT_FILES, "decompressed.txt"));
    decompress(compressedRead, decompressed);

    compressedRead.close();
    decompressed.close();

    return 0;
}
