#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <string>
#include <fstream>
using namespace std;

/*=====================================================================================
* This is custom BitStream library. Classes are not inherited from iostream since it operates on byte level.
* -ibitstream is used to read data and obitstream to write data
=====================================================================================*/

class BitStream {
protected:
    uint8_t* stream;          // pointer to bitstream (array of bytes)
    int stream_capacity;      // size of bitstream IN BITS
    int current_bit;          // current bit position for reading/writing
    int num_bytes;            // number of bytes written

    BitStream(int stream_capacity = 1024)
        : stream_capacity(stream_capacity), current_bit(0), num_bytes(0) {
        stream = new uint8_t[stream_capacity / 8 + (stream_capacity % 8 ? 1 : 0)]();
    }
    ~BitStream() {
        delete[] stream;
    }

public:

    // set any bit in allocated stream (bits are set from left to right in bytes)
    bool setBit(unsigned int bit_pos, int value) {
        if (bit_pos >= stream_capacity) {
            cerr << "Out of bounds\n";
            return false;
        }
        if (value != 0 && value != 1) {
            cerr << "Invalid bit value\n";
            return false;
        }
        int byte_index = bit_pos / 8;
        int bit_index = bit_pos % 8;

        if (value) {
            stream[byte_index] |= (1 << (7 - bit_index)); // set to 1
        }
        else {
            stream[byte_index] &= ~(1 << (7 - bit_index)); // set to 0
        }
        return true; // successfully set
    }

    // get any bit in allocated stream
    int getBit(unsigned int bit_pos) const {
        if (bit_pos >= stream_capacity) {
            cerr << "Out of bounds\n";
            return -1;
        }
        int byte_index = bit_pos / 8;
        int bit_index = bit_pos % 8;

        return (stream[byte_index] >> (7 - bit_index)) & 1;
    }

    void reset() {
        current_bit = 0;
        num_bytes = 0;
        memset(stream, 0x00, stream_capacity / 8 + (stream_capacity % 8 ? 1 : 0));
    }

    int tell() const {
        return current_bit;
    }
};

/*=====================================================================================
* obitstream class
* You can choose buffer size for writing ( >=8)
* void writeBit(int value) writes 0 or 1 in bitstream
* bytes are filled from left to right with bits:
*
* |10110101|10110010|10(1)00000|00000000|...
*    byte               ^current_bit ->>
*
* bool flushToFile(ofstream& outfile) flushes bitstream to provided file stream. Last byte may have padding like |101(00000)|
* bytes are flushed automatically when buffer is full. They should also be flushed explicitly when finishing writing.
* note: it doesn't work properly for stream_capacity set to 7 or less
=====================================================================================*/
class obitstream : public BitStream {
    ostream* outfile;
public:
    obitstream(int stream_capacity = 1024) : BitStream(stream_capacity), outfile(nullptr) {}

    void fileToWrite(ostream& writefile) {
        outfile = &writefile;
    }
    void tellPos() {
        cout << "Position in outfile: " << outfile->tellp() << endl;
    }

    // write a single bit in stream
    void writeBit(int value) {
        if (current_bit >= stream_capacity) {
            // auto flush when buffer is full
            flushToFile();
        }
        if (setBit(current_bit, value)) {
            current_bit++;
            num_bytes = max(current_bit / 8 + (current_bit % 8 ? 1 : 0), num_bytes); // adjust num_bytes
        }
        else {
            cerr << "Couldn't write bit\n";
        }
    }

    // write whole stream to file (writes all bytes up to last byte modified, last byte may have padding)
    // its basically putting a string to a file
    bool flushToFile() {
        if (!outfile) {
            cerr << "Set filestream for writing!\n";
            return false;
        }
        if (outfile->write(reinterpret_cast<const char*>(stream), num_bytes)) {
            reset();
            //cout << "Stream flushed to file\n";
            return true;
        }
        cout << "Couldn't flush to file\n";
        return false;
    }
};

/*=====================================================================================
* ibitstream class
* You can choose buffer size for reading ( >=8)
* void fileToRead(istream& readfile) set file for bit reading
* void refillStreamBuffer() refills stream with new data from file
* int readBit() sequentially reads bits from buffer. Returns 0 or 1 if byte is valid and -1 if eof is reached.
* you will have to manually determine how many bits (bytes) you can read with some indicator for end of stream,
* for example, if last bits are 11111111, you can stop reading bits. Either that or check for -1 if you will read till
* the end of file
* note: it doesn't work properly for stream_capacity set to 7 or less
=====================================================================================*/
class ibitstream : public BitStream {
    istream* infile;
public:
    ibitstream(int stream_capacity = 1024) : BitStream(stream_capacity), infile(nullptr) {}

    void fileToRead(istream& readfile) {
        infile = &readfile;
    }

    void tellPos() {
        cout << "Position in infile: " << infile->tellg() << endl;
    }

    void refillStreamBuffer() {
        if (!infile) {
            num_bytes = 0;
            cerr << "Set filestream for reading!\n";
            return;
        }
        int bytes_to_read = stream_capacity / 8 + (stream_capacity % 8 ? 1 : 0);
        memset(stream, 0x00, bytes_to_read);
        infile->read(reinterpret_cast<char*>(stream), bytes_to_read);
        current_bit = 0;
        num_bytes = infile->gcount();

        if (num_bytes == 0) {
            if (infile->eof()) {
                cerr << "End of file reached\n";
            }
            else if (infile->fail()) {
                cerr << "File read failed\n";
            }
            else if (infile->bad()) {
                cerr << "Critical I/O error occurred\n";
            }
            return;
        }
    }
    // fill buffer if needed and read bits sequentially
    int readBit() {
        if (current_bit >= num_bytes * 8) {
            refillStreamBuffer();
        }
        // if buffer is not refilled with new data, eof is reached and -1 is returned
        if (num_bytes == 0) {
            cout << "num_bytes in stream is 0\n";
            return -1;
        }         
        int bit = getBit(current_bit);
        current_bit++;
        return bit;
    }
};
#endif