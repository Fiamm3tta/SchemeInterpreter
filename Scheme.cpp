#include "Scheme.h"
#include <iostream>
#include <string>
using namespace std;

using std::istream;
using std::ostream;

Scheme::Scheme(int MaxHashTableSize, int MaxNodeArraySize) // constructor
{
	MaxHash = MaxHashTableSize;
	MaxNode = MaxNodeArraySize;
	HashTable = new Hash[MaxHash];
	NodeArray = new Node[MaxNode];
	inputCode = "";
	Current = 0;
	Next = 1;
}

Scheme::~Scheme() // destructor
{
	delete[] HashTable;
	delete[] NodeArray;
}

void Scheme::Initialize()
{
	for (int i = 0; i < MaxNode; i++)
	{
		NodeArray[i].left = 0;
		NodeArray[i].right = i + 1;
		NodeArray[i].garbage = true;
	}
	NodeArray[0].right = 0;
	NodeArray[MaxNode - 1].right = 0;

	for (int i = 0; i < MaxHash; i++)
	{
		HashTable[i].symbol = "";
		HashTable[i].link = 0;
	}
}

istream& operator>>(istream& is, Scheme& scheme)
{
	scheme.inputCode.clear();
	getline(cin, scheme.inputCode);
	return is;
}

ostream& operator<<(ostream& os, Scheme& scheme)
{
	cout << "NodeArray" << "\n";
	for (int i = 0; i < scheme.MaxNode; i++)
	{
		cout << i << " " << scheme.NodeArray[i].left << " " << scheme.NodeArray[i].right << " ";
		if (scheme.NodeArray[i].garbage)
		{
			cout << "TRUE" << "\n";
		}
		else
		{
			cout << "FALSE" << "\n";
		}
	}

	cout << "\n";

	cout << "HashTable" << "\n";
	for (int i = 0; i < scheme.MaxHash; i++)
	{
		if (scheme.HashTable[i].symbol != "")
		{
			cout << -1 * i << " " << scheme.HashTable[i].symbol << " " << scheme.HashTable[i].link << "\n";
		}
	}

	cout << "\n";

	return os;
}

unsigned int StringToInt(string s)
{
	int length = (int)s.length();
	unsigned int answer = 0;
	if (length % 2 == 1)
	{
		answer = s.at(length - 1);
		length--;
	}

	for (int i = 0; i < length; i += 2)
	{
		answer += s.at(i);
		answer += ((int)s.at(i + 1)) << 8;
	}
	return answer;
}

int Scheme::GetHashValue(string target) // double hasing, return hashtable index
{
	int x = StringToInt(target);
	int h1 = x % (MaxHash - 1);
	if (h1 == 0)
	{
		h1++;
	}
	int h2 = 1 + (x % 17);
	int answer = 1;

	if (HashTable[h1].symbol == "")
	{
		HashTable[h1].symbol = target;
		answer = h1;
	}
	else if (HashTable[h1].symbol == target)
	{
		answer = h1;
	}
	else
	{
		while (1)
		{
			h1 += h2;
			if (h1 > MaxHash - 1)
			{
				h1 -= (MaxHash - 1);
			}

			if (HashTable[h1].symbol == target)
			{
				answer = h1;
				break;
			}

			if (HashTable[h1].symbol == "")
			{
				HashTable[h1].symbol = target;
				answer = h1;
				break;
			}
		}
	}

	return -1 * answer;
}

string SmallAlphabet(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] >= 65 && s[i] <= 90)
		{
			s[i] = s[i] + 32;
		}
	}
	return s;
}

string Scheme::Tokenizer() // make code token
{
	int tmp = 0;
	for (int i = 0; i < inputCode.length(); i++)
	{
		if (inputCode[i] != ' ')
		{
			tmp = i;
			break;
		}
	}

	if (tmp > 0)
	{
		inputCode = inputCode.substr(tmp);
	}

	string ans = "";
	for (int i = 0; i < inputCode.length(); i++)
	{
		if (inputCode[i] == ' ')
		{
			inputCode = inputCode.substr(i);
			break;
		}
		else if (inputCode[i] == '(' || inputCode[i] == ')')
		{
			if (ans.empty())
			{
				ans = inputCode[i];
				if (i < inputCode.length() - 1)
				{
					inputCode = inputCode.substr(i + 1);
				}
				else
				{
					inputCode.clear();
				}
				break;
			}
			else
			{
				inputCode = inputCode.substr(i);
				break;
			}
		}
		else
		{
			ans += inputCode[i];
			if (i == inputCode.length() - 1)
			{
				inputCode.clear();
				break;
			}
		}
	}

	return SmallAlphabet(ans);
}

string Scheme::ReplaceCode(string str) // quote, lamda
{
	string sub = "";
	int NumOfLeftParen = 0;
	while (1)
	{
		sub = Tokenizer();
		if (sub == "define")
		{
			str.append(sub + " ");
			sub = Tokenizer();
			if (sub == "(")
			{
				sub = Tokenizer();
				string tmp = "";
				str.append(sub + " (lambda (" + ReplaceCode(tmp) + ")");
			}
			else
			{
				str.append(sub + " ");
			}
		}
		else if (sub == "`")
		{
			str.append("(quote");
			NumOfLeftParen = 0;
			while (1)
			{
				sub = Tokenizer();
				str.append(sub);
				if (sub == "(")
				{
					NumOfLeftParen++;
				}
				else if (sub == ")")
				{
					NumOfLeftParen--;
				}
				else
				{
					str.append(" ");
				}

				if (NumOfLeftParen == 0)
				{
					break;
				}
			}
			str.append(")");
		}
		else
		{
			str.append(sub);
			if (inputCode.empty())
			{
				break;
			}
			else if (sub != ")")
			{
				str.append(" ");
			}
		}
	}
	return str;
}

void Scheme::PostProcessing()
{
	string newcommand = "";
	newcommand = ReplaceCode(newcommand);
	if (inputCode.empty())
	{
		inputCode += newcommand;
	}
}

int Scheme::Read() // Read code to save at hashtable and make parsetree at nodearray. And return root index
{
	int root = 0;
	bool first = true;
	int tmp = 0;
	int tokenHashVal = GetHashValue(Tokenizer());
	if (HashTable[-1 * tokenHashVal].symbol == "(")
	{
		while (1)
		{
			tokenHashVal = GetHashValue(Tokenizer());
			if (HashTable[-1 * tokenHashVal].symbol == ")")
			{
				break;
			}

			if (first)
			{
				tmp = Next;
				Current = tmp;
				Next = NodeArray[Current].right;
				root = Current;
				first = false;
			}
			else
			{
				NodeArray[tmp].right = Next;
				tmp = NodeArray[tmp].right;
				Current = tmp;
				Next = NodeArray[tmp].right;
			}
			
			if (HashTable[-1 * tokenHashVal].symbol == "(")
			{
				inputCode.insert(0, "(");
				NodeArray[tmp].left = Read();
			}
			else
			{
				NodeArray[tmp].left = tokenHashVal;
			}
			NodeArray[tmp].right = 0;
		}
		if (Current == 0)
		{
			return 0;
		}
		return root;
	}
	else
	{
		return tokenHashVal;
	}
}

void Scheme::Print(int index, bool startlist) // Print result
{
	if (index < 0)
	{
		cout << HashTable[-1 * index].symbol;
	}
	else if (index > 0)
	{
		if (startlist)
		{
			cout << "(";
		}
		Print(NodeArray[index].left, true);

		if (NodeArray[index].right != 0)
		{
			cout << " ";
			Print(NodeArray[index].right, false);
		}
		else
		{
			cout << ")";
		}
	}
	else
	{
		cout << "()";
	}
}

bool isNumber(string str)
{
	if (str.length() == 1)
	{
		if (!isdigit(str[0]))
		{
			return false;
		}
	}
	else
	{
		bool firstdot = true;
		bool minus = false;

		if (str[0] == '-')
		{
			minus = true;
		}
		else if (!isdigit(str[0]))
		{
			return false;
		}

		if (!isdigit(str[1]))
		{
			if (str[1] == '.' && !minus)
			{
				firstdot = false;
			}
			else
			{
				return false;
			}
		}

		for (int i = 2; i < str.length(); i++)
		{
			if (!isdigit(str[i]))
			{
				if (str[i] == '.' && firstdot)
				{
					firstdot = false;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

int Scheme::Eval(int root) // Evaluation code. And return result after evaluation index
{
	int tokenindex = 0;
	string Symbol = "";

	if (root < 0)
	{
		if (HashTable[-1 * root].link == 0)
		{
			return root;
		}
		else
		{
			return HashTable[-1 * root].link;
		}
	}
	else if (root > 0)
	{
		tokenindex = NodeArray[root].left;
		if (tokenindex > 0)
		{
			return Eval(tokenindex);
		}
		Symbol = HashTable[-1 * tokenindex].symbol;
	}

	if (Symbol == "+")
	{
		int a = 0;
		int b = 0;
		a = Eval(NodeArray[NodeArray[root].right].left);
		b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (isNumber(HashTable[-1 * a].symbol) && isNumber(HashTable[-1 * b].symbol))
		{
			a = stoi(HashTable[-1 * a].symbol);
			b = stoi(HashTable[-1 * b].symbol);
			return GetHashValue(to_string(a + b));
		}
		else
		{
			return GetHashValue("ERROR!");
		}
	}
	else if (Symbol == "-")
	{
		int a = 0;
		int b = 0;
		a = Eval(NodeArray[NodeArray[root].right].left);
		b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (isNumber(HashTable[-1 * a].symbol) && isNumber(HashTable[-1 * b].symbol))
		{
			a = stoi(HashTable[-1 * a].symbol);
			b = stoi(HashTable[-1 * b].symbol);
			return GetHashValue(to_string(a - b));
		}
		else
		{
			return GetHashValue("ERROR!");
		}
	}
	else if (Symbol == "*")
	{
		int a = 0;
		int b = 0;
		a = Eval(NodeArray[NodeArray[root].right].left);
		b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (isNumber(HashTable[-1 * a].symbol) && isNumber(HashTable[-1 * b].symbol))
		{
			a = stoi(HashTable[-1 * a].symbol);
			b = stoi(HashTable[-1 * b].symbol);
			return GetHashValue(to_string(a * b));
		}
		else
		{
			return GetHashValue("ERROR!");
		}
	}
	else if (Symbol == "*")
	{
		int a = 0;
		int b = 0;
		a = Eval(NodeArray[NodeArray[root].right].left);
		b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (isNumber(HashTable[-1 * a].symbol) && isNumber(HashTable[-1 * b].symbol))
		{
			a = stoi(HashTable[-1 * a].symbol);
			b = stoi(HashTable[-1 * b].symbol);
			return GetHashValue(to_string(a * b));
		}
		else
		{
			return GetHashValue("ERROR!");
		}
	}
	else if (Symbol == "/")
	{
		int a = 0;
		int b = 0;
		a = Eval(NodeArray[NodeArray[root].right].left);
		b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (isNumber(HashTable[-1 * a].symbol) && isNumber(HashTable[-1 * b].symbol))
		{
			float c = stof(HashTable[-1 * a].symbol);
			float d = stof(HashTable[-1 * b].symbol);
			return GetHashValue(to_string(c / d));
		}
		else
		{
			return GetHashValue("ERROR!");
		}
	}
	else if (Symbol == "number?")
	{
		int tmp = NodeArray[NodeArray[root].right].left;
		if (isNumber(HashTable[-1 * tmp].symbol))
		{
			return GetHashValue("#t");
		}
		else
		{
			return GetHashValue("#f");
		}
	}
	else if (Symbol == "symbol?")
	{
		int tmp = NodeArray[NodeArray[root].right].left;
		if (!isNumber(HashTable[-1 * tmp].symbol))
		{
			return GetHashValue("#t");
		}
		else
		{
			return GetHashValue("#f");
		}
	}
	else if (Symbol == "null?")
	{
		if (NodeArray[root].right == 0 || Eval(NodeArray[root].right) == 0)
		{
			return GetHashValue("#t");
		}
		else 
		{
			return GetHashValue("#f");
		}
	}
	else if (Symbol == "cons")
	{
		int tmp = Next;
		Next = NodeArray[tmp].right;
		NodeArray[tmp].left = Eval(NodeArray[NodeArray[root].right].left);
		NodeArray[tmp].right = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		return tmp;
	}
	else if (Symbol == "cond")
	{
		while (NodeArray[NodeArray[root].right].right != 0)
		{
			root = NodeArray[root].right;
			if (Eval(NodeArray[NodeArray[root].left].left) == GetHashValue("#t"))
			{
				return Eval(NodeArray[NodeArray[root].left].right);
			}
		}
		if (NodeArray[NodeArray[NodeArray[root].right].left].left != GetHashValue("else"))
		{
			return GetHashValue("ERROR!");
		}
		return Eval(NodeArray[NodeArray[NodeArray[NodeArray[root].right].left].right].left);
	}
	else if (Symbol == "car")
	{
		return NodeArray[Eval(NodeArray[NodeArray[root].right].left)].left;
	}
	else if (Symbol == "cdr")
	{
		return NodeArray[Eval(NodeArray[NodeArray[root].right].left)].right;
	}
	else if (Symbol == "define")
	{
		int tmp = NodeArray[NodeArray[root].right].left;
		if (NodeArray[NodeArray[NodeArray[root].right].right].left > 0 && HashTable[-1 * NodeArray[NodeArray[NodeArray[NodeArray[root].right].right].left].left].symbol == "lambda")
		{
			HashTable[-1 * tmp].link = NodeArray[NodeArray[NodeArray[root].right].right].left;
		}
		else
		{
			HashTable[-1 * tmp].link = Eval(NodeArray[NodeArray[root].right].right);
		}
		return HashTable[-1 * tmp].link;
	}
	else if (Symbol == "quote")
	{
		return NodeArray[NodeArray[root].right].left;
	}
	else if (HashTable[-1 * NodeArray[HashTable[-1 * tokenindex].link].left].symbol == "lambda")
	{
		int param = NodeArray[NodeArray[HashTable[-1 * tokenindex].link].right].left;
		int val = NodeArray[root].right;
		while (1)
		{
			if (param == 0 && val == 0)
			{
				break;
			}
			CalStack1.push(HashTable[-1 * NodeArray[param].left].link); // parameter link
			CalStack2.push(param); // parameter
			HashTable[-1 * NodeArray[param].left].link = Eval(NodeArray[val].left);
			param = NodeArray[param].right;
			val = NodeArray[val].right;
		}

		int tmp = Eval(NodeArray[NodeArray[NodeArray[HashTable[-1 * NodeArray[root].left].link].right].right].left);

		while (!CalStack1.empty() && !CalStack2.empty())
		{
			param = CalStack2.top();
			HashTable[-1 * NodeArray[param].left].link = CalStack1.top();
			CalStack1.pop();
			CalStack2.pop();
		}

		return tmp;
	}
	else if (Symbol == "=")
	{
		int a = NodeArray[NodeArray[root].right].left;
		int b = NodeArray[NodeArray[NodeArray[root].right].right].left;
		if (a > 0 || b > 0)
		{
			return GetHashValue("ERROR!");
		}
		else if (!isNumber(HashTable[-1 * a].symbol) || !isNumber(HashTable[-1 * b].symbol))
		{
			return GetHashValue("ERROR!");
		}
		else
		{
			a = stoi(HashTable[-1 * a].symbol);
			b = stoi(HashTable[-1 * b].symbol);
		}

		if (a == b)
		{
			return GetHashValue("#t");
		}
		else
		{
			return GetHashValue("#f");
		}
	}
	else if (Symbol == "eq?")
	{
		int a = Eval(NodeArray[NodeArray[root].right].left);
		int b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (a == b)
		{
			return GetHashValue("#t");
		}
		else
		{
			return GetHashValue("#f");
		}
	}
	else if (Symbol == "equal?")
	{
		int a = Eval(NodeArray[NodeArray[root].right].left);
		int b = Eval(NodeArray[NodeArray[NodeArray[root].right].right].left);
		if (a < 0)
		{
			if (a == b)
			{
				return GetHashValue("#t");
			}
			else
			{
				return GetHashValue("#f");
			}
		}
		else if (a == 0)
		{
			return GetHashValue("ERROR!");
		}
		else
		{
			if (a == b)
			{
				return GetHashValue("#t");
			}
			else 
			{
				int c = 0;
				int d = 0;
				while (1)
				{
					if (a == 0 && b == 0)
					{
						if (CalStack1.empty() && CalStack2.empty())
						{
							break;
						}
						else if (!CalStack1.empty() && !CalStack2.empty())
						{
							a = CalStack1.top();
							b = CalStack2.top();
							CalStack1.pop();
							CalStack2.pop();
						}
						else
						{
							return GetHashValue("ERROR!");
						}
					}
					c = NodeArray[a].left;
					d = NodeArray[b].left;
					if (c < 0)
					{
						if (c == d)
						{
							a = NodeArray[a].right;
							b = NodeArray[b].right;
						}
						else
						{
							return GetHashValue("#f");
						}
					}
					else if (c == 0)
					{
						return GetHashValue("ERROR!");
					}
					else
					{
						if (NodeArray[a].right != 0)
						{
							CalStack1.push(NodeArray[a].right);
							CalStack2.push(NodeArray[b].right);
						}
						a = c;
						b = d;
					}
				}
				return GetHashValue("#t");
			}
		}
	}
	
	if (isNumber(Symbol))
	{
		return tokenindex;
	}
	else
	{
		return HashTable[-1 * tokenindex].link;
	}
}

void Scheme::GarbageCollect() // GarbageCollecting
{
	int tmp = 0;
	int nodetmp = 0;
	for (int i = 0; i < MaxHash; i++)
	{
		tmp = HashTable[i].link;
		if (tmp > 0)
		{
			while (1)
			{
				if (tmp == 0)
				{
					if (CalStack1.empty())
					{
						break;
					}
					else
					{
						tmp = CalStack1.top();
						CalStack1.pop();
					}
				}

				NodeArray[tmp].garbage = false;
				nodetmp = NodeArray[tmp].left;
				if (nodetmp > 0)
				{
					CalStack1.push(NodeArray[tmp].right);
					tmp = nodetmp;
				}
				else
				{
					tmp = NodeArray[tmp].right;
				}
			}
		}
	}

	int previous = 0;
	bool first = true;
	bool second = false;
	for (int i = 1; i < MaxNode; i++)
	{
		if (NodeArray[i].garbage)
		{
			NodeArray[i].left = 0;
			NodeArray[i].right = 0;
			NodeArray[previous].right = i;
			previous = i;
			if (first)
			{
				Current = i;
				first = false;
				second = true;
			}

			if (second)
			{
				Next = i;
				second = false;
			}
		}
	}

	NodeArray[0].left = 0;
	NodeArray[0].right = 0;

	cout << "GarbageCollector is called. Please re-enter to code." << "\n";
}