#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>
#include "BitStream.h"
using namespace std;

#define FILEPATH(directory, filename) (string(directory) + "/" + filename).c_str()

#define PSEUDO_EOF '#' // used to mark the end of bitstream (char)
#define HEADER_END 0xFE // used to mark the end of header (uint8_t)

// huffman tree node
struct Node {
	char ch;
	unsigned freq;
	Node* left;
	Node* right;
	Node(char character = '\0', unsigned frequency = 0) {
		ch = character;
		freq = frequency;
		left = right = nullptr;
	}
};

unordered_map<char, int> buildFrequencyTable(istream& input) {
	unordered_map<char, int> map;
	char ch;
	while (input.get(ch)) {
		//if input stream is cin, stop when \n is encountered
		if (&input == &cin && ch == '\n') break;
		map[ch]++;
	}
	map.insert({ PSEUDO_EOF, 1 });
	input.clear();
	input.seekg(0, ios::beg);
	return map;
}

//for debugging
template <typename T>
void printMap(unordered_map<char, T>map) {
	for (const auto& pair : map) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
}

// comparing function for priority queue
struct compare {
	bool operator()(Node* const& n1, Node* const& n2) {
		return (n1->freq > n2->freq);
	}
};

Node* buildEncodingTree(unordered_map<char, int>& freqTable) {
	priority_queue<Node*, vector<Node*>, compare> queue;

	// gnerate forest of nodes with data from table of frequencies
	for (const auto& pair : freqTable) {
		queue.push(new Node(pair.first, pair.second));
	}
	while(queue.size() != 1){
		// creates new internal node ( \0 is special char for internal nodes in this example)
		Node* n = new Node('\0', 0);
		// take 2 nodes with lowest weights (frequencies)
		Node* n1 = queue.top();
		queue.pop();
		Node* n2 = queue.top();
		queue.pop();
		// make them children of new internal node and update frequency of internal node
		n->freq = n1->freq + n2->freq;
		n->left = n1;
		n->right = n2;
		// push new subtree in queue
		queue.push(n);
	}
	//return root
	return queue.top();
}

// helper function
bool isLeaf(Node* node){
	return !(node->left) && !(node->right);
}
// helper recursive function
void fillMap(Node* root, string code, unordered_map<char, string>& encodingMap) {

	if (!root) return;

	//only leafs nodes are considered
	if (isLeaf(root)) {
		encodingMap[root->ch] = code;
	}
	// each recursive call has its own copy of code which is assigned to leaf nodes
	fillMap(root->left, code + "0", encodingMap);
	fillMap(root->right, code + "1", encodingMap);
}

unordered_map<char, string> buildEncodingMap(Node* root) {
	unordered_map<char, string> encodingMap;
	fillMap(root, "", encodingMap);
	return encodingMap;
}

void encodeData(istream& input, const unordered_map<char, string>& encodingMap, obitstream& output) {
	char ch;
	// iterate over input text, read character encoding and write corresponding bits to bitstream
	while (input.get(ch)) {
		const string& temp = encodingMap.at(ch);
		for (char bit : temp) {
			if (bit == '1') {
				output.writeBit(1);
			}
			else if (bit == '0') {
				output.writeBit(0);
			}
		}
	}
	// PSEUDO_EOF is used to mark the end of bitstream
	const string& eof = encodingMap.at(PSEUDO_EOF); 
	for (char bit : eof) {
		if (bit == '1') {
			output.writeBit(1);
		}
		else if (bit == '0') {
			output.writeBit(0);
		}
	}

	output.flushToFile(); // assuming output stream was set in obitstream
}

void decodeData(ibitstream& input, Node* encodingTree, ostream& output) {
	//assuming file for reading ibitstream is already set up
	int bit;
	char ch;
	Node* current = encodingTree;
	while (true) {
		bit = input.readBit();
		if (bit == -1) {
			cout << "invalid bit\n";
			return;
		}
		if (bit) {
			current = current->right;
		}
		else {
			current = current->left;
		}
		if (isLeaf(current)) {
			ch = current->ch;
			if (ch == PSEUDO_EOF) {
				break;
			}
			output.put(ch);
			current = encodingTree;
		}
	}
}

//write frequency table as header
void writeHeader(ofstream& output, unordered_map<char, int>& freqTable) {
	for (const auto& pair : freqTable) {
		uint8_t ch = pair.first;
		// here int is converted to short int for memory efficieny, this should just be int for large files
		unsigned short int frequency = static_cast<unsigned short int>(pair.second);

		output.write(reinterpret_cast<const char*>(&ch), sizeof(ch));
		output.write(reinterpret_cast<const char*>(&frequency), sizeof(frequency));
	}
	// mark the end of header
	uint8_t end = HEADER_END;
	output.write(reinterpret_cast<const char*>(&end), sizeof(end));
}

unordered_map<char, int> readHeader(ifstream& input) {
	unordered_map<char, int> freqTable;
	uint8_t byte;
	unsigned short int freq;
	while (true) {
		input.read(reinterpret_cast<char*>(&byte), sizeof(byte));
		if (byte == HEADER_END) {
			break;
		}
		input.read(reinterpret_cast<char*>(&freq), sizeof(freq));
		freqTable[byte] = freq;
	}
	//printMap(freqTable);
	return freqTable;
}

void freeTree(Node* root) {
	if (root == nullptr) return;
	freeTree(root->left);
	freeTree(root->right);
	delete root;
}

// since my bitstream library is not the same as Standfords, i changed obitstream& output parameter to ostream
void compress(istream& input, ofstream& output) {

	obitstream bitstream(1024);
	bitstream.fileToWrite(output);

	unordered_map<char, int> frequencies = buildFrequencyTable(input);
	writeHeader(output, frequencies);
	Node* root = buildEncodingTree(frequencies);
	unordered_map<char, string> codes = buildEncodingMap(root);
	encodeData(input, codes, bitstream);
	freeTree(root);

}
void decompress(ifstream& input, ostream& output) {

	ibitstream bitstream(1024);
	bitstream.fileToRead(input);
	
	unordered_map<char, int> freqTable = readHeader(input);	
	Node* root = buildEncodingTree(freqTable);
	decodeData(bitstream, root, output);
	freeTree(root);
}

//void compressionReport(const string& original, const string& compressed) {
//	
//}

#endif

