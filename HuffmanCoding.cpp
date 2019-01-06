#include "HuffmanCoding.h"
#include <queue>
#include <fstream>
#include <vector>
#include <bitset>

#define BUFF_SIZE 1024

struct Comparator
{
	bool operator()(HuffmanCoding::TreeNode *first, HuffmanCoding::TreeNode *second) const
	{
		return first->weight > second->weight;
	}
};

HuffmanCoding::HuffmanCoding()
	: root(nullptr)
{
}

void HuffmanCoding::Encode(const char *inputFileName, const char *outputFileName, const char *treeFileName)
{
	BuildTree(inputFileName);
	std::map<char, std::string> codesTable;
	BuildCodesTable(root, "", codesTable);
	
	unsigned long long numberOfBits = 0;
	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		if (asciiTable[i] != 0)
		{
			numberOfBits += codesTable[(char)i].length() * asciiTable[i];
		}
	}

	std::ifstream inp(inputFileName, std::ios::binary | std::ios::in | std::ios::ate);
	unsigned long long length = inp.tellg();
	inp.seekg(0);

	std::ofstream out(outputFileName, std::ios::out | std::ios::binary);
	out.write((char*)&numberOfBits, sizeof(unsigned long long));

	char buff[BUFF_SIZE];
	unsigned long long bytesRead = 0;
	std::string nextByte = "";
	while (bytesRead < length)
	{
		inp.read(buff, BUFF_SIZE);
		bytesRead += inp.gcount();
		for (size_t i = 0; i < inp.gcount(); i++)
		{
			nextByte += codesTable[buff[i]];
			while (nextByte.length() >= sizeof(unsigned int) * 8)
			{
				//32 bit chunks
				unsigned int bytesToWrite = std::stoul(nextByte.substr(0, sizeof(unsigned int) * 8), nullptr, 2);
				out.write((char*)&bytesToWrite, sizeof(bytesToWrite));
				nextByte = nextByte.substr(sizeof(unsigned int) * 8);
			}
		}
	}
	inp.close();

	if (nextByte.length() > 0)
	{
		while (nextByte.length() < sizeof(unsigned int) * 8)
		{
			nextByte += "0";
		}
		unsigned int bytesToWrite = std::stoul(nextByte, nullptr, 2);
		out.write((char *)&bytesToWrite, sizeof(bytesToWrite));
	}
	out.close();

	std::ofstream treeOut(treeFileName);
	SerializeTree(root, treeOut);
	treeOut.close();
	DeleteTree(root);
}

void HuffmanCoding::Decode(const char *inputFileName, const char *outputFileName, const char *treeFileName)
{
	std::ifstream treeInp(treeFileName);	
	root = DeserializeTree(treeInp);
	treeInp.close();

	std::ifstream inp(inputFileName, std::ios::in | std::ios::binary);
	std::ofstream out(outputFileName, std::ios::out | std::ios::binary);

	unsigned long long numberOfBits = 0;
	inp.read((char*)&numberOfBits, sizeof(unsigned long long));
	unsigned long long bitsRead = 0;

	//32 bits chunk
	unsigned int bits;
	char buff[BUFF_SIZE];
	int ind = 0;
	HuffmanCoding::TreeNode *crr = root;
	while (bitsRead < numberOfBits)
	{
		inp.read((char*)&bits, sizeof(bits));
		bitsRead += sizeof(bits) * 8;

		std::string binary = std::bitset<sizeof(bits) * 8>(bits).to_string();
		if (bitsRead > numberOfBits)
		{
			binary = binary.substr(0, sizeof(bits) * 8 - (bitsRead - numberOfBits));
		}

		for (size_t i = 0; i < binary.length(); i++)
		{
			if (binary[i] == '1')
			{
				crr = crr->right;
			}
			else
			{
				crr = crr->left;
			}

			if (crr->left == nullptr && crr->right == nullptr)
			{
				buff[ind++] = crr->symbol;
				crr = root;
			}

			if (ind == BUFF_SIZE)
			{
				out.write(buff, BUFF_SIZE);
				ind = 0;
			}
		}
	}
	out.write(buff, ind);
	out.close();
	inp.close();
	DeleteTree(root);
	
}

void HuffmanCoding::BuildFrequencyTable(const char *inputFileName)
{
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		asciiTable[i] = 0;
	}

	std::ifstream inp(inputFileName, std::ios::binary | std::ios::in | std::ios::ate);
	unsigned long long length = inp.tellg();
	inp.seekg(0);

	char buff[BUFF_SIZE];
	unsigned long long bytesRead = 0;
	while (bytesRead < length)
	{
		inp.read(buff, BUFF_SIZE);
		for (size_t i = 0; i < inp.gcount(); i++)
		{
			++asciiTable[buff[i]];
		}
		bytesRead += inp.gcount();
	}
	inp.close();
}

void HuffmanCoding::BuildTree(const char *inputFileName)
{
	BuildFrequencyTable(inputFileName);
	std::priority_queue<HuffmanCoding::TreeNode*, std::vector<HuffmanCoding::TreeNode*>, Comparator> que;

	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		if (asciiTable[i] != 0)
		{
			que.push(new HuffmanCoding::TreeNode(asciiTable[i], i));
		}
	}

	while (que.size() > 1)
	{
		HuffmanCoding::TreeNode *left = que.top(); que.pop();
		HuffmanCoding::TreeNode *right = que.top(); que.pop();

		que.push(new HuffmanCoding::TreeNode(left->weight + right->weight, '$', left, right));
	}

	root = que.top();
}

void HuffmanCoding::BuildCodesTable(TreeNode* root, std::string code, std::map<char, std::string> &table)
{
	if (root->left == nullptr && root->right == nullptr)
	{
		table[root->symbol] = code;
		return;
	}

	BuildCodesTable(root->left, code + "0", table);
	BuildCodesTable(root->right, code + "1", table);
}

void HuffmanCoding::DeleteTree(TreeNode *node)
{
	if (node == nullptr)
	{
		return;
	}

	DeleteTree(node->left);
	DeleteTree(node->right);
	delete node;
}

void HuffmanCoding::SerializeTree(TreeNode *node, std::ofstream &out)
{
	if (node == nullptr)
	{
		out << "()";
		return;
	}

	out << "(" << node->weight << " " << (int)node->symbol << " ";
	SerializeTree(node->left, out);
	out << " ";
	SerializeTree(node->right, out);
	out << ")";
}

HuffmanCoding::TreeNode* HuffmanCoding::DeserializeTree(std::ifstream &in)
{
	in.get();
	if (in.peek() == ')')
	{
		in.get();
		return nullptr;
	}

	unsigned long long weight;
	in >> weight;
	int symbolAscii;
	in >> symbolAscii;
	//get the space;
	in.get();

	HuffmanCoding::TreeNode *left = DeserializeTree(in);
	in.get();
	HuffmanCoding::TreeNode *right = DeserializeTree(in);
	in.get();
	return new HuffmanCoding::TreeNode(weight, (char)symbolAscii, left, right);
}
