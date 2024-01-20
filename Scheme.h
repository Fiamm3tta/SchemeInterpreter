#ifndef SCHEME_H
#define SCHEME_H

#include <iostream>
#include <string>
#include <stack>
using namespace std;

using std::istream;
using std::ostream;

struct Node
{
	int left;
	int right;
	bool garbage;
};

struct Hash
{
	string symbol;
	int link;
};

class Scheme
{
private:
	Hash* HashTable;
	int MaxHash;

	Node* NodeArray;
	int MaxNode;
	int Current;
	int Next;

	string inputCode;

	stack<int> CalStack1;
	stack<int> CalStack2;

public:
	Scheme(int MaxHashTableSize, int MaxNodeArraySize); // constructor
	~Scheme(); // destructor
	void Initialize();

	int GetHashValue(string target); // double hasing, return hashtable index
	string Tokenizer(); // make code token
	string ReplaceCode(string str); // Add quote, lamda
	void PostProcessing(); // Execute ReplaceCode and replace inputCode
	
	int Read(); // Read code to save at hashtable and make parsetree at nodearray. And return root index
	void Print(int index, bool startlist); // Print result
	
	int Eval(int root); // Evaluation code. And return result after evaluation index
	void GarbageCollect(); // GarbageCollecting
	
	friend istream& operator>>(istream& is, Scheme& scheme);
	friend ostream& operator<<(ostream& os, Scheme& scheme);
};

#endif