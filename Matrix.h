#pragma once
#include <vector>
using namespace std;

class Matrix {
public:
	static vector<vector<double>> add(vector<vector<double>>& a, vector<vector<double>>& b);

	static vector<vector<double>> transpose(vector<vector<double>>& a);

	static vector<vector<double>> multiply(vector<vector<double>>& a, vector<vector<double>>& b);

	static vector<vector<double>> multiply(vector<double>& a, vector<vector<double>>& b);
};
