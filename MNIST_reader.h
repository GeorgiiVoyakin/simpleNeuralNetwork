#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace sf;

class MNIST_reader {
    enum { training, test };
public:
    static int reverseInt(int i) {
        unsigned char c1, c2, c3, c4;

        c1 = i & 255;
        c2 = (i >> 8) & 255;
        c3 = (i >> 16) & 255;
        c4 = (i >> 24) & 255;

        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    }

    static vector<double> read_mnist(string desktopPath, int mode, int number) {
        ifstream file;
        switch (mode) {
        case training: file.open(desktopPath + "\\mnist\\train-images.idx3-ubyte", ios::in | ios::binary); break;
        case test: file.open(desktopPath + "\\mnist\\t10k-images.idx3-ubyte", ios::in | ios::binary); break;
        }

        if (file.is_open()) {
            vector<double> result;
            result.reserve(28 * 28 + 1);
            int number_of_images = 0;
            int n_rows = 0;
            int n_cols = 0;
            int magic_number;
            file.read((char*)&magic_number, sizeof(magic_number));
            magic_number = reverseInt(magic_number);
            file.read((char*)&number_of_images, sizeof(number_of_images));
            number_of_images = reverseInt(number_of_images);
            file.read((char*)&n_rows, sizeof(n_rows));
            n_rows = reverseInt(n_rows);
            file.read((char*)&n_cols, sizeof(n_cols));
            n_cols = reverseInt(n_cols);
            Image image;
            image.loadFromFile(desktopPath + "\\image.png");//Загрузка пустого изображения

            unsigned char temp;
            for (int i = 0; i < number_of_images; ++i) {
                for (int r = 0; r < n_rows; ++r) {
                    for (int c = 0; c < n_cols; ++c) {
                        if (!file.read((char*)&temp, sizeof(temp)))
                            exit(1);

                        //i - номер цифры
                        if (i == number) {
                            result.push_back(static_cast<int>(temp));
                        }
                        Color color(static_cast<int>(temp), static_cast<int>(temp), static_cast<int>(temp));
                        image.setPixel(c, r, color);
                    }
                }
                //Сохранение картинки
                image.saveToFile(desktopPath + "\\network_data_test\\" + to_string(i) + ".png");
            }
            file.close();
            return result;
        }
    }

    static int read_mnist_labels(string desktopPath, int mode, int number) {
        ifstream file;
        switch (mode) {
        case training: file.open(desktopPath + "\\mnist\\train-labels.idx1-ubyte"); break;
        case test: file.open(desktopPath + "\\mnist\\t10k-labels.idx1-ubyte"); break;
        }

        if (file.is_open()) {
            int number_of_items = 0;
            int magic_number;
            file.read((char*)&magic_number, sizeof(magic_number));
            magic_number = reverseInt(magic_number);
            file.read((char*)&number_of_items, sizeof(number_of_items));
            number_of_items = reverseInt(number_of_items);

            unsigned char temp = 0;
            for (int i = 0; i < number_of_items; ++i) {
                if (!file.read((char*)&temp, sizeof(temp)))
                    exit(1);

                if (i == number) {
                    file.close();
                    return static_cast<int>(temp);
                }
            }
            file.close();
        }
    }

    static void read_mnist_labels_in_vector(string desktopPath, int mode, vector<int>& destination) {
        ifstream file;
        switch (mode) {
        case training: file.open(desktopPath + "\\mnist\\train-labels.idx1-ubyte"); break;
        case test: file.open(desktopPath + "\\mnist\\t10k-labels.idx1-ubyte"); break;
        }

        if (file.is_open()) {
            int number_of_items = 0;
            int magic_number;
            file.read((char*)&magic_number, sizeof(magic_number));
            magic_number = reverseInt(magic_number);
            file.read((char*)&number_of_items, sizeof(number_of_items));
            number_of_items = reverseInt(number_of_items);

            unsigned char temp = 0;
            for (int i = 0; i < number_of_items; ++i) {
                if (!file.read((char*)&temp, sizeof(temp)))
                    exit(1);

                destination.push_back(static_cast<int>(temp));
            }
            file.close();
        }
    }
};
