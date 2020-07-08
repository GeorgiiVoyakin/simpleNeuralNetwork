#include "Matrix.h"
using namespace std;

vector<vector<double>> Matrix::add(vector<vector<double>>& a, vector<vector<double>>& b) {
    vector<vector<double>> result;
    result.resize(a.size());
    for (int i = 0; i < a.size(); i++)
        result[i].resize(a[0].size());
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < a[i].size(); j++)
            result[i][j] = a[i][j] + b[i][j];
    return result;
}

vector<vector<double>> Matrix::transpose(vector<vector<double>>& a) {
    vector<vector<double>> result;
    result.resize(a[0].size());
    for (int i = 0; i < a[0].size(); i++)
        result[i].resize(a.size());
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < a[i].size(); j++)
            result[j][i] = a[i][j];
    return result;
}

vector<vector<double>> Matrix::multiply(vector<vector<double>>& a, vector<vector<double>>& b) {
    vector<vector<double>> result;
    result.resize(a.size());
    for (int i = 0; i < a.size(); i++)
        result[i].resize(b[0].size());
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < b[0].size(); j++) {
            result[i][j] = 0;
            for (int k = 0; k < b.size(); k++)
                result[i][j] += a[i][k] * b[k][j];
        }
    return result;
}

vector<vector<double>> Matrix::multiply(vector<double>& a, vector<vector<double>>& b) {
    vector<vector<double>> result;
    result.resize(a.size());
    for (int i = 0; i < a.size(); i++)
        result[i].resize(b[0].size());
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < b[0].size(); j++) {
            result[i][j] = 0;
            for (int k = 0; k < b.size(); k++)
                result[i][j] += a[i] * b[k][j];
        }
    return result;
}