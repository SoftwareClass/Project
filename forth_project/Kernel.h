#pragma once
class Kernel {

public:
	Kernel(int n, double *data);
	int getSize() const;
	double getVal(int x, int y) const;
	~Kernel();
private:
	double *data;
	int n;
};