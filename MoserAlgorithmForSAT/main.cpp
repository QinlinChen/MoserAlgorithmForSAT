#include "CNF.h"
#include "Stopwatch.h"
using namespace std;

#define KSAT 9
#define NCLAUSES 100000
#define NVAR 112500

int x[NVAR];

void naiveCNFTest()
{
	Clause c1 = { { &x[1], false }, { &x[2], true }, { &x[3], false } };
	Clause c2 = { { &x[2], false }, { &x[3], false } };
	CNF cnf = { c2, c1 };
	cnf.solve();
}

void CNFTest(int times)
{
	memset(x, 0, sizeof(x));
	double totalTime = 0.0;
	double totalNCallFix = 0;

	for (int i = 1; i <= times; ++i) {
		cout << "Round: " << i << endl;
		Stopwatch timer;
		CNF cnf = CNF::randomCNF(x, NVAR, KSAT, NCLAUSES);
		cout << "generate: " << timer << endl;
		timer.reset();
		cout << (cnf.solve() ? "success" : "failed") << ": " << timer << endl;
		totalTime += timer.elaspedTime();
		cout << "nCallFix: " << cnf.nCallFix << endl;
		totalNCallFix += cnf.nCallFix;
	}

	cout << "average time: " << totalTime / times << endl;
	cout << "average nCallFix: " << (double)totalNCallFix / times << endl;
}

int main()
{
	CNFTest(100);
	return 0;
}