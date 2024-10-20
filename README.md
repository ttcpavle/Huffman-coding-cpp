# Huffman-coding
This program can compress and decompress textual file using huffman binary tree. Compressed file can be aroud 30%-50% smaller in size.
## How it works

Textual data (from file) is analyzed and table with characters and their frequencies (how many times they appear) is created. That table is used to create huffman binary tree (minimized weight tree). Tree is used to find encoding for each character (no code is prefix of another).
<div align="center">
 <br>
    <img src="https://github.com/user-attachments/assets/2dce8bcc-fab4-4f82-a048-34e1d8848a0f" alt="huffman coding visually">
</div>
Frequencie table is written as header in binary file and than encoded text (bitstream) is written. Here is structure for compressed file: 

<div align="center">
 <br>
    <img src="https://github.com/user-attachments/assets/e212fbe6-fbb3-4051-bcc3-1083a43cec8a" alt="huffman coding visually">
 <br>
</div>

- Blue is used to mark elements of table (char and unsigned short int).
- Red is HEADER_END byte (0xFE in this case). When it is reached, reading of header stops.
- Green is where bitstream starts (encoded text). At the end of bitstream there is encoded character (that is already in table of codes) which in my case is PSEUDO_EOF. After it's bits are read and PSEUDO_EOF is found in binary tree, reading of bitstream stops. There also may be padding at the end filled with 0 bits.

Size of header may be large for smaller files but for larger files, it will not take too much space. Assuming that there is like 32 different characters in text, size of header will be 3x32=96 bytes and character codes will take between 3 and 10 bits. After analyzing file sizes, we can see that compressed file is usually 30-50% smaller (not in all cases).

Not everything is exactly the same as in Standfords assignment in pdf (see below).
## How to run (windows/unix/macOS)

1) Clone the repository in desired directory
 ```bash
 git clone https://github.com/ttcpavle/Huffman-coding-cpp.git
 ```
2) Navigate to build directory
3) Run:
```bash
cmake ..
```

After those steps:
- If you are using windows, open the `HuffmanCoding.sln` solution (if visual studio was selected compiler). Select the HuffmanCoding as startup project and run. Header files should appear in solution explorer under Header files. HINT: change mode from debug to release for better performance.
- If you are using linux, run generated files for building a project. If cmake generated makefile, run `make` and than run the executable `./HuffmanCoding`
<p>
Required software: <br>
Cmake installation: https://www.youtube.com/watch?v=8_X5Iq9niDE <br>
Git installation: https://www.youtube.com/watch?v=JgOs70Y7jew <br>
</p>

## Sources
This project was based on Stanfords assignment (huffman.pdf in this repo) which is publically available. I didn't use any external resources from Stanfords library. Here is link to pdf: https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1178/assn/huffman.pdf
