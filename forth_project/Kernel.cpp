#include "Kernel.h"

Kernel::Kernel(int n, double * data)
{
	this->data = data;
	this->n = n;
}

int Kernel::getSize() const
{
	return n;
}

double Kernel::getVal(int x, int y) const
{
	return data[x*n+y];
}

Kernel::~Kernel()
{
	delete[] data;
}
