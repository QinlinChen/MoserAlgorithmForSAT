#include "CNF.h"
#include <algorithm>
#include <stack>
#include <sstream>
#include <assert.h>
#include "Stopwatch.h"

using namespace std;

default_random_engine Literal::engine;
uniform_int_distribution<int> Literal::distribution(0, 1);

ostream &operator<<(ostream &os, const Literal &liter)
{
	if (liter.neg)
		os << "not ";
	os << *liter.pVar;
	//os << liter.name;
	return os;
}

ostream &operator<<(ostream &os, const Clause &clause)
{
	for (auto &liter : clause.liters)
		cout << "(" << liter << ")";
	return os;
}

ostream &operator<<(ostream &os, const CNF &cnf)
{
	for (auto &clause : cnf.clauses)
		cout << "[" << clause << "]";
	return os;
}

default_random_engine CNF::engine;
uniform_int_distribution<int> CNF::distribution(0, 1);
int CNF::nCallFix = 0;

/*
Randomly generate clauses:
set up some copies of each of the given variables
($kCNF * $nClauses total variables), permute them,
and use the ordering to assign the variables to
the $nClauses clauses. (If a variable appears in a
clause multiple times, try to locally correct this
by swapping one copy to another clause.) Then assign
a random "sign" to each variable: with probability 1/2.
*/
CNF CNF::randomCNF(int *var, size_t nVar, size_t kCNF, size_t nClauses)
{
	size_t nLiters = kCNF * nClauses;
	vector<Literal> liters;
	liters.reserve(nLiters);

	for (size_t i = 0, varCtr = 0; i < nLiters; ++i, varCtr = (varCtr + 1) % nVar) {
		Literal liter;
		liter.pVar = var + varCtr;
		liter.neg = (distribution(engine) == 1);
		//ostringstream oss;
		//oss << "x" << varCtr;
		//liter.name = oss.str();
		liters.push_back(std::move(liter));
	}
	shuffle(liters.begin(), liters.end(), engine);

	CNF cnf;
	cnf.reserve(nClauses);
	size_t literCtr = 0;
	for (size_t i = 0; i < nClauses; ++i) {
		Clause clause;
		clause.reserve(kCNF);
		for (size_t j = 0; j < kCNF; ++j) {
			size_t k = literCtr;
			while (clause.hasLiteral(liters[k]))
				k++;
			assert(k < nLiters);
			std::swap(liters[k], liters[literCtr]);
			clause.addLiteral(liters[literCtr]);
			literCtr = (literCtr + 1) % nLiters;
		}
		cnf.addClause(std::move(clause));
	}

	return cnf;
}

bool CNF::solve()
{
	initProfile();
	constructDependency();
	
	randomSample();
	for (auto &clause : clauses)
		if (!clause.isSatisfied())
			fix(clause);

	return isSatisfied();
}

void CNF::initProfile()
{
	nCallFix = 0;
}

void CNF::constructDependency()
{
	if (!dependency.empty())
		dependency.clear();
	for (auto &clause : clauses)
		for (auto &liter : clause.getLiterals())
			dependency[liter.pVar].push_back(&clause);
}

void CNF::fix(Clause &clause)
{
	nCallFix++;
	clause.randomSample();
	while (true) {
		auto iter = existsUnsatisfiedClauseIntersecting(clause);
		if (iter == nullptr)
			return;
		fix(*iter);
	}
}
