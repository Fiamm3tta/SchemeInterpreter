#include <iostream>
#include <string>
#include "Scheme.h"
using namespace std;

const int MAXHASHSIZE = 101;
const int MAXNODESIZE = 101;

Scheme scheme(MAXHASHSIZE, MAXNODESIZE);

int main()
{
	scheme.Initialize();

	while (1)
	{
		cout << "> ";
		cin >> scheme;

		scheme.PostProcessing();

		int root = 0;
		root = scheme.Read();
		if (root != 0)
		{
			
			//cout << "> ";
			//scheme.Print(root, true);
			//cout << "\n";

			int index = scheme.Eval(root);

			//cout << "\n";
			//cout << scheme;
			
			cout << "] ";
			scheme.Print(index, true);
			cout << "\n";
		}
		else
		{
			scheme.GarbageCollect();
			cout << "\n";
			cout << scheme;
		}
	}
	
	return 0;
}