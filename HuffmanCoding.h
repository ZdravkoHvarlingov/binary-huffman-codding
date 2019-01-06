#pragma once

#include <string>
#include <map>

#define TABLE_SIZE 256

class HuffmanCoding
{
public:
	HuffmanCoding();
	void Encode(const char*, const char*, const char*);
	void Decode(const char*, const char*, const char*);

	struct TreeNode
	{
		unsigned long long weight;
		char symbol;
		TreeNode *left, *right;

		TreeNode(long long weight, char symbol, TreeNode *left = nullptr, TreeNode *right = nullptr)
			: weight(weight), symbol(symbol), left(left), right(right)
		{}
	};


private:

	void BuildFrequencyTable(const char*);
	void BuildTree(const char*);
	void BuildCodesTable(TreeNode*, std::string, std::map<char, std::string>&);
	void DeleteTree(TreeNode *);
	void SerializeTree(TreeNode *, std::ofstream&);
	TreeNode* DeserializeTree(std::ifstream&);

	TreeNode *root;
	unsigned long long asciiTable[TABLE_SIZE];
};
