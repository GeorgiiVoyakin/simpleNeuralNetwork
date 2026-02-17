#include "network.h"
#include "MNIST_reader.h"
#include <cmath>
#include <iostream>
#include <string>
#include <chrono>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

double network::sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

double network::derived_sigmoid(double x) {
    return sigmoid(x) * (1 - sigmoid(x));
}

double network::special_derived_sigmoid(double x) {
    return x * (1 - x);
}

network::network(vector<vector<double>>& input, string desktopPath) {
    this->desktopPath = desktopPath;
    //Заполнение входного слоя
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < input.size(); j++) {
            enters.push_back(input[i][j]);
        }
    }
    //Установка размеров для всех векторов
    hidden1.resize(16);
    hidden2.resize(16);
    output.resize(10);
    biasesOfHidden1.resize(hidden1.size());
    biasesOfHidden2.resize(hidden2.size());
    biasesOfOutput.resize(output.size());
    weightsToHidden1.resize(enters.size());/*При установке размеров векторов весов сначала идёт номер нейрона,от которого идёт синапс,
        потом - номер нейрона,к которому приходит синапс */
    for (int i = 0; i < weightsToHidden1.size(); i++) {
        weightsToHidden1[i].resize(hidden1.size());
    }
    weightsToHidden2.resize(hidden1.size());
    for (int i = 0; i < weightsToHidden2.size(); i++) {
        weightsToHidden2[i].resize(hidden2.size());
    }
    weightsToOutput.resize(hidden2.size());
    for (int i = 0; i < weightsToOutput.size(); i++) {
        weightsToOutput[i].resize(output.size());
    }
    //Запись правильных ответов, которые должна давать сеть в вектор labels
    MNIST_reader::read_mnist_labels_in_vector(desktopPath, training, labels);

    cout << "Загрузить веса ? (1/0)" << endl;
    bool load;
    cin >> load;
    if (load) {
        loadWeights();
        loadBiases();

        feedForward();
        makeDecision();
    }
    else {
        setWeights();
        setBiases();
    }

    bool load_images;
    cout << "Do you want to load images";
    cin >> load_images;
    if (load_images) {
        vector<Image> digits;
        Image buffer_image;
        for (int i = 0; i < 60000; i++) {
            if (!buffer_image.loadFromFile(desktopPath + "\\network_data\\" + to_string(i) + ".png"))
                cout << "Error. Не удалось загрузить картинку с цифрой.";
            digits.push_back(buffer_image);
        }

        cout << "Хотите начать обучение сети ? (1/0)" << endl;
        bool sure;
        cin >> sure;
        if (sure) {
            auto start = chrono::high_resolution_clock::now();
            for (int epoch = 0; epoch < 3; epoch++) {
                for (int i = 0; i < 60000; i++) {
                    createInput(digits[i]);
                    feedForward();
                    vector<double> target;
                    target.resize(output.size());
                    target[labels[i]] = 1.0;
                    backpropagation(output, target);

                    if (i % 100 == 0)
                        cout << epoch << " " << i << endl;
                }
            }
            auto stop = chrono::high_resolution_clock::now();
            cout << "Time in seconds: " << chrono::duration<double, ratio<1>>(stop - start).count() << endl;

        }

        bool accuracy;
        cout << "Do you want to check accuracy of the network: ";
        cin >> accuracy;
        if (accuracy) {
            int amount = 0;
            for (int i = 0; i < 60000; i++) {
                createInput(digits[i]);
                feedForward();
                if (labels[i] == max_output()) {
                    amount++;
                }

                if (i % 100 == 0)
                    cout << i << endl;
            }
            cout << "Total accuracy: " << amount / 60000.0 << endl;
        }

        if (!(load && !sure)) {
            cout << "Сохранить веса ? (1/0)" << endl;
            bool save;
            cin >> save;
            if (save) {
                saveWeights();
                saveBiases();
            }
        }
    }
}

void network::setWeights() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < weightsToHidden1.size(); i++) {
        for (int j = 0; j < weightsToHidden1[i].size(); j++) {
            weightsToHidden1[i][j] = (rand() % 10) * 0.01;
        }
    }

    for (int i = 0; i < weightsToHidden2.size(); i++) {
        for (int j = 0; j < weightsToHidden2[i].size(); j++) {
            weightsToHidden2[i][j] = (rand() % 10) * 0.01;
        }
    }

    for (int i = 0; i < weightsToOutput.size(); i++) {
        for (int j = 0; j < weightsToOutput[i].size(); j++) {
            weightsToOutput[i][j] = (rand() % 10) * 0.01;
        }
    }
}

void network::setBiases() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < biasesOfHidden1.size(); i++)
        biasesOfHidden1[i] = (rand() % 10) * 0.01;

    for (int i = 0; i < biasesOfHidden2.size(); i++)
        biasesOfHidden2[i] = (rand() % 10) * 0.01;

    for (int i = 0; i < biasesOfOutput.size(); i++)
        biasesOfOutput[i] = (rand() % 10) * 0.01;
}

void network::countOutput(vector<double>& start, vector<double>& finish, vector<vector<double>>& sinapsW, vector<double>& biases) {
    //for weights: i - previous layer, j - next layer
    for (int i = 0; i < finish.size(); i++) {
        for (int j = 0; j < start.size(); j++) {
            finish[i] += start[j] * sinapsW[j][i];
        }
        finish[i] += biases[i];
    }
    for (int i = 0; i < finish.size(); i++)
        finish[i] = sigmoid(finish[i]);
}

void network::study() {
    bool right;
    feedForward();
    makeDecision();
    cout << "Сеть угадала число? (1/0)" << endl;
    cin >> right;
    if (!right) {
        setWeights();
        study();
    }
    correct = true;
}

void network::makeDecision() {
    int indx = -1;
    double max = -1;
    for (int i = 0; i < output.size(); i++) {
        if (output[i] > max) {
            max = output[i];
            indx = i;
        }
    }
    for (int i = 0; i < output.size(); i++) {
        cout << i << ": " << output[i] << endl;
    }
    cout << "Это цифра: " << indx << endl;
}

void network::feedForward() {
    countOutput(enters, hidden1, weightsToHidden1, biasesOfHidden1);
    countOutput(hidden1, hidden2, weightsToHidden2, biasesOfHidden2);
    countOutput(hidden2, output, weightsToOutput, biasesOfOutput);
}

void network::saveWeights() {
    weights.open(desktopPath + "\\weights.txt");
    for (int i = 0; i < weightsToHidden1.size(); i++) {
        for (int j = 0; j < weightsToHidden1[i].size(); j++) {
            weights << weightsToHidden1[i][j] << " ";
        }
    }
    for (int i = 0; i < weightsToHidden2.size(); i++) {
        for (int j = 0; j < weightsToHidden2[i].size(); j++) {
            weights << weightsToHidden2[i][j] << " ";
        }
    }
    for (int i = 0; i < weightsToOutput.size(); i++) {
        for (int j = 0; j < weightsToOutput[i].size(); j++) {
            weights << weightsToOutput[i][j] << " ";
        }
    }
    weights.close();
}

void network::loadWeights() {
    vector<double> W;
    double buffer;
    weights.open(desktopPath + "\\weights.txt");
    while (weights >> buffer)
        W.push_back(buffer);

    int indx = 0;
    for (int i = 0; i < weightsToHidden1.size(); i++) {
        for (int j = 0; j < weightsToHidden1[i].size(); j++) {
            weightsToHidden1[i][j] = W[indx];
            indx++;
        }
    }
    for (int i = 0; i < weightsToHidden2.size(); i++) {
        for (int j = 0; j < weightsToHidden2[i].size(); j++) {
            weightsToHidden2[i][j] = W[indx];
            indx++;
        }
    }
    for (int i = 0; i < weightsToOutput.size(); i++) {
        for (int j = 0; j < weightsToOutput[i].size(); j++) {
            weightsToOutput[i][j] = W[indx];
            indx++;
        }
    }
    weights.close();
}

void network::saveBiases() {
    biases.open(desktopPath + "\\biases.txt");
    for (int i = 0; i < biasesOfHidden1.size(); i++) {
        biases << biasesOfHidden1[i] << " ";
    }

    for (int i = 0; i < biasesOfHidden2.size(); i++) {
        biases << biasesOfHidden2[i] << " ";
    }

    for (int i = 0; i < biasesOfOutput.size(); i++) {
        biases << biasesOfOutput[i] << " ";
    }
    biases.close();
}

void network::loadBiases() {
    vector<double> B;
    double buffer;
    biases.open(desktopPath + "\\biases.txt");
    while (biases >> buffer)
        B.push_back(buffer);

    for (int i = 0; i < biasesOfHidden1.size(); i++) {
        biasesOfHidden1[i] = B[0];
        B.erase(B.begin());
    }

    for (int i = 0; i < biasesOfHidden2.size(); i++) {
        biasesOfHidden2[i] = B[0];
        B.erase(B.begin());
    }

    for (int i = 0; i < biasesOfOutput.size(); i++) {
        biasesOfOutput[i] = B[0];
        B.erase(B.begin());
    }
    biases.close();
}

void network::backpropagation(vector<double> input_array, vector<double> target_array) {

    //Вычисление ошибок выходного слоя
    // ERROR = TARGETS - OUTPUTS
    vector<double> output_errors;
    for (int i = 0; i < target_array.size(); i++)
        output_errors.push_back(target_array[i] - input_array[i]);
    for (auto error : output_errors)
        error = error * error;
    // Calculate gradient
    vector<double> gradients;
    for (int i = 0; i < output.size(); i++)
        gradients.push_back(learningRate * output_errors[i] * special_derived_sigmoid(output[i]));

    // Calculate hidden2->output deltas
    vector<vector<double>> weight_h2o_deltas;
    weight_h2o_deltas.resize(output.size());
    for (int i = 0; i < weight_h2o_deltas.size(); i++) {
        weight_h2o_deltas[i].resize(hidden2.size());
    }
    for (int i = 0; i < output.size(); i++) {
        for (int j = 0; j < hidden2.size(); j++) {
            weight_h2o_deltas[i][j] = gradients[i] * hidden2[j];
        }
    }

    // Calculate the hidden2 layer errors
    vector<double> hidden2_errors;
    hidden2_errors.resize(hidden2.size());
    for (int i = 0; i < hidden2.size(); i++) {
        for (int j = 0; j < output.size(); j++) {
            hidden2_errors[i] += weightsToOutput[i][j] * output_errors[j];
        }
    }

    // Calculate hidden2 gradient
    vector<double> hidden2_gradient;
    for (int i = 0; i < hidden2.size(); i++)
        hidden2_gradient.push_back(learningRate * hidden2_errors[i] * special_derived_sigmoid(hidden2[i]));

    // Calcuate hidden1->hidden2 deltas
    vector<vector<double>> weight_h1h2_deltas;
    weight_h1h2_deltas.resize(hidden2.size());
    for (int i = 0; i < weight_h1h2_deltas.size(); i++) {
        weight_h1h2_deltas[i].resize(hidden1.size());
    }
    for (int i = 0; i < hidden2.size(); i++) {
        for (int j = 0; j < hidden1.size(); j++) {
            weight_h1h2_deltas[i][j] = hidden2_gradient[i] * hidden1[j];
        }
    }

    // Calculate the hidden1 layer errors
    vector<double> hidden1_errors;
    hidden1_errors.resize(hidden1.size());
    for (int i = 0; i < hidden1.size(); i++) {
        for (int j = 0; j < hidden2.size(); j++) {
            hidden1_errors[i] += weightsToHidden1[i][j] * hidden2_errors[j];
        }
    }

    // Calculate hidden1 gradient
    vector<double> hidden1_gradient;
    for (int i = 0; i < hidden1.size(); i++)
        hidden1_gradient.push_back(learningRate * hidden1_errors[i] * special_derived_sigmoid(hidden1[i]));

    // Calcuate input->hidden1 deltas
    vector<vector<double>> weight_ih1_deltas;
    weight_ih1_deltas.resize(hidden1.size());
    for (int i = 0; i < weight_ih1_deltas.size(); i++) {
        weight_ih1_deltas[i].resize(enters.size());
    }
    for (int i = 0; i < hidden1.size(); i++) {
        for (int j = 0; j < enters.size(); j++) {
            weight_ih1_deltas[i][j] = hidden1_gradient[i] * enters[j];
        }
    }

    // Adjust the weights by deltas
    for (int i = 0; i < output.size(); i++) {
        for (int j = 0; j < hidden2.size(); j++) {
            weightsToOutput[j][i] += weight_h2o_deltas[i][j];
        }
    }
    // Adjust the bias by its deltas (which is just the gradients)
    for (int i = 0; i < biasesOfOutput.size(); i++)
        biasesOfOutput[i] += gradients[i];

    // Adjust the weights by deltas
    for (int i = 0; i < hidden2.size(); i++) {
        for (int j = 0; j < hidden1.size(); j++) {
            weightsToHidden2[j][i] += weight_h1h2_deltas[i][j];
        }
    }
    // Adjust the bias by its deltas (which is just the gradients)
    for (int i = 0; i < biasesOfHidden2.size(); i++)
        biasesOfHidden2[i] += hidden2_gradient[i];

    // Adjust the weights by deltas
    for (int i = 0; i < hidden1.size(); i++) {
        for (int j = 0; j < enters.size(); j++) {
            weightsToHidden1[j][i] += weight_ih1_deltas[i][j];
        }
    }
    // Adjust the bias by its deltas (which is just the gradients)
    for (int i = 0; i < biasesOfHidden1.size(); i++)
        biasesOfHidden1[i] += hidden1_gradient[i];
}

void network::createInput(Image digit) {
    //создание и заполнение вектора, который будет хранить входные значения нейросети
    vector<vector<Color>> colors(28, vector<Color>(28));
    for (unsigned int i = 0; i < 28; i++)
    {
        for (unsigned int j = 0; j < 28; j++) {
            colors[i][j] = digit.getPixel(i, j);
        }
    }
    //Подготовка входных значений
    vector<double> input;
    for (unsigned int i = 0; i < 28; i++)
    {
        for (unsigned int j = 0; j < 28; j++) {
            input.push_back((colors[i][j].r + colors[i][j].g + colors[i][j].b) / 765.f);
        }
    }
    enters = input;
}

int network::max_output() {
    int indx = -1;
    double max = -1;
    for (int i = 0; i < output.size(); i++) {
        if (output[i] > max) {
            max = output[i];
            indx = i;
        }
    }
    return indx;
}
