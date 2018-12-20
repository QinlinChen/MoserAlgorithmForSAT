#pragma once

#include <vector>
#include <iostream>
#include <initializer_list>
#include <string>
#include <random>
#include <unordered_map>
#include <list>

class Literal
{
public:
	bool isTrue() const { return (bool)(*pVar ^ (int)neg); }
	void randomSample() { *pVar = distribution(engine); }
	bool varEqualsTo(const Literal &other) const { return pVar == other.pVar; }

	int *pVar;
	bool neg;
	//std::string name;

private:
	static std::default_random_engine engine;
	static std::uniform_int_distribution<int> distribution;
};

std::ostream &operator<<(std::ostream &os, const Literal &liter);

class Clause
{
	friend std::ostream &operator<<(std::ostream &os, const Clause &clause);

public:
	Clause() = default;
	Clause(const Clause &other) : liters(other.liters) {}
	Clause(Clause &&other) : liters(std::move(other.liters)) {}
	Clause(const std::initializer_list<Literal> &initLiters) : liters(initLiters) {}
	Clause(std::initializer_list<Literal> &&initLiters) : liters(std::move(initLiters)) {}

	void addLiteral(const Literal &liter) { liters.push_back(liter); }
	void addLiteral(Literal &&liter) { liters.push_back(std::move(liter)); }
	void reserve(const size_t newcapacity) { liters.reserve(newcapacity); }
	const std::vector<Literal> &getLiterals() const { return liters; }

	bool isSatisfied() const
	{
		for (auto &liter : liters)
			if (liter.isTrue())
				return true;
		return false;
	}

	void randomSample()
	{
		for (auto &liter : liters)
			liter.randomSample();
	}

	bool intersects(const Clause &other) const
	{
		for (auto &liter : liters)
			for (auto &otherLiter : other.liters)
				if (liter.varEqualsTo(otherLiter))
					return true;
		return false;
	}

	bool hasLiteral(const Literal &liter) const
	{
		for (auto &myLiter : liters)
			if (myLiter.varEqualsTo(liter))
				return true;
		return false;
	}

private:
	std::vector<Literal> liters;
};

class CNF
{
	friend std::ostream &operator<<(std::ostream &os, const CNF &cnf);

public:
	CNF() = default;
	CNF(const CNF &other) : clauses(other.clauses) {}
	CNF(CNF &&other) : clauses(std::move(other.clauses)) {}
	CNF(const std::initializer_list<Clause> &initClauses) : clauses(initClauses) {}
	CNF(std::initializer_list<Clause> &&initClauses) : clauses(std::move(initClauses)) {}

	void addClause(const Clause &clause) { clauses.push_back(clause); }
	void addClause(Clause &&clause) { clauses.push_back(std::move(clause)); }
	void reserve(const size_t newcapacity) { clauses.reserve(newcapacity); }

	static CNF randomCNF(int *var, size_t nVar, size_t kCNF, size_t nClauses);

	bool isSatisfied() const
	{
		for (auto &clause : clauses)
			if (!clause.isSatisfied())
				return false;
		return true;
	}

	void randomSample()
	{
		for (auto &clause : clauses)
			clause.randomSample();
	}

	bool solve();

	static int nCallFix;

private:
	void initProfile();
	void constructDependency();
	void fix(Clause &clause);

	Clause *existsUnsatisfiedClauseIntersecting(const Clause &clause)
	{
		for (auto &l : clause.getLiterals())
			for (auto c : dependency[l.pVar])
				if (!c->isSatisfied())
					return c;
		return nullptr;
	}

	std::vector<Clause> clauses;
	std::unordered_map<int *, std::vector<Clause *>> dependency;
	static std::default_random_engine engine;
	static std::uniform_int_distribution<int> distribution;
};