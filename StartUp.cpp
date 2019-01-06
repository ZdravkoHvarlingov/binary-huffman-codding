// HuffmanCodingCompression.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "HuffmanCoding.h"

int main()
{
	HuffmanCoding hf;
	
	hf.Encode("bible.txt", "compressed", "tree");
	hf.Decode("compressed", "decompressed.txt" , "tree");


    return 0;
}

