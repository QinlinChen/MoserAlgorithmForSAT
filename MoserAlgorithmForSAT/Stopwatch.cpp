#include "Stopwatch.h"

std::ostream &operator<<(std::ostream &os, const Stopwatch &timer)
{
	os << timer.elaspedTime() << " seconds";
	return os;
}