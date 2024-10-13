#include <iostream>
#include <fstream>
#include <chrono>
#include "HuffmanCoding.h"
#include "config.h"

using namespace std;
using namespace chrono;

int main() {

    // COMPRESS: --------------------------------------------------------------------
    auto compressStart = high_resolution_clock::now();
    ifstream original(FILEPATH(INPUT_FILES, "test.txt"), ios::binary | ios::ate);
    ofstream compressed(FILEPATH(OUTPUT_FILES, "compressed.bin"), ios::binary);

    // get size of original file
    streamsize originalSize = original.tellg();
    original.seekg(0, ios::beg);

    cout << "Compression started..." << endl;
    compress(original, compressed);
    cout << "Compression ended successfully" << endl;

    compressed.close();
    original.close();

    auto compressEnd = high_resolution_clock::now();
    auto compressTime = duration_cast<microseconds>(compressEnd - compressStart).count();
    // COMPRESSION END --------------------------------------------------------------------

    // calculate compressed file size
    ifstream compressedFile(FILEPATH(OUTPUT_FILES, "compressed.bin"), ios::binary | ios::ate);
    streamsize compressedSize = compressedFile.tellg();
    compressedFile.close();

    // DECOMPRESS: --------------------------------------------------------------------
    auto decompressStart = high_resolution_clock::now();

    ifstream compressedRead(FILEPATH(OUTPUT_FILES, "compressed.bin"), ios::binary);
    ofstream decompressed(FILEPATH(OUTPUT_FILES, "decompressed.txt"));

    cout << "Decompression started..." << endl;
    decompress(compressedRead, decompressed);
    cout << "Decompression ended successfully" << endl;

    compressedRead.close();
    decompressed.close();

    auto decompressEnd = high_resolution_clock::now();
    auto decompressDuration = duration_cast<microseconds>(decompressEnd - decompressStart).count();
    // DECOMPRESSION END --------------------------------------------------------------------

    cout << "\nSTATS:-----------------" << endl;
    cout << "Compression time: " << compressTime << " microseconds" << endl;
    cout << "Decompression time: " << decompressDuration << " microseconds" << endl;

    float compressionRate = ((float)compressedSize / originalSize) * 100.0;
    cout << "Original size: " << originalSize << " bytes" << endl;
    cout << "Compressed size: " << compressedSize << " bytes" << endl;
    cout << "Compression rate: " << compressionRate << "%" << endl;

    return 0;
}
