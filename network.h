#pragma once
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class network {
    vector<double> enters;//Входной слой
    vector<double> hidden1;//Первый скрытый слой
    vector<double> hidden2;//Второй скрытый слой
    vector<double> output;//Выходной слой
    vector<vector<double>> weightsToHidden1;//Веса синапсов от входного слоя к первому скрытому слою
    vector<vector<double>> weightsToHidden2;//Веса синапсов от первого скрытого слоя ко второму скрытому слою
    vector<vector<double>> weightsToOutput;//Веса синапсов от второго скрытого слоя к выходому слою
    vector<double>biasesOfHidden1;//Нейроны смещения первого скрытыго слоя
    vector<double>biasesOfHidden2;//Нейроны смещения второго скрытыго слоя
    vector<double>biasesOfOutput;//Нейроны смещения выходного слоя
    vector<int>labels;//Верные ответы, которые должна давать сеть
    bool correct = false;
    fstream weights;
    fstream biases;
    double learningRate = 0.1;
    string desktopPath;

    enum { training, test };

    //Функция активации
    static double sigmoid(double x);
    //Производная сигмоидной функции
    static double derived_sigmoid(double x);
    static double special_derived_sigmoid(double x);

    void setWeights();

    void countOutput(vector<double>& start, vector<double>& finish, vector<vector<double>>& sinapsW, vector<double>& biases);

    void study();

    void makeDecision();

    void feedForward();

    void saveWeights();

    void loadWeights();

    void setBiases();

    void saveBiases();

    void loadBiases();

    void backpropagation(vector<double> input_array, vector<double> target_array);

    void createInput(Image digit);

    void backpropagation_matrix(vector<double> input_array, vector<double> target_array);

    int max_output();
public:
    network(vector<vector<double>>& input, string desktopPath);
};