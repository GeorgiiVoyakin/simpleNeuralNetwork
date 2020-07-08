#include <iostream>
#include "network.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <shlobj.h>
#include <string>

using namespace std;
//пространство имён для sfml
using namespace sf;

/*функция для перевода пикселей изображения в числа
в последствии эти значения поступают на вход нейросети*/
float ColorToNumber(Color& color);

string tcharToString(TCHAR* array);

int main() {
    setlocale(LC_ALL, "rus");
    //Получение пути к рабочему столу
    TCHAR desktopPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL,
        CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
        NULL,
        SHGFP_TYPE_CURRENT,
        desktopPath)))
        cout << "Путь к рабочему столу получен." << endl;
    else
        cout << "Error. Не удалось получить путь к рабочему столу." << endl;

    //считывание картинки в объект digit
    Image digit;
    if (!digit.loadFromFile(tcharToString(desktopPath) + "\\DIGIT.png"))
        cout << "Error. Не удалось загрузить картинку с цифрой.";
    //создание и заполнение вектора, который будет хранить входные значения нейросети
    vector<vector<Color>> colors(28, vector<Color>(28));
    for (unsigned int i = 0; i < 28; i++)
    {
        for (unsigned int j = 0; j < 28; j++) {
            colors[i][j] = digit.getPixel(i, j);
        }
    }
    //Подготовка входных значений
    vector<vector<double>> input(28, vector<double>(28));
    for (unsigned int i = 0; i < 28; i++)
    {
        for (unsigned int j = 0; j < 28; j++) {
            input[i][j] = ColorToNumber(colors[i][j]);
        }
    }

    //Запуск
    network network(input, tcharToString(desktopPath));

    system("pause");
    return 0;
}

float ColorToNumber(Color& color) {
    return (1 - (color.r + color.g + color.b) / 765.f);
}

string tcharToString(TCHAR* source) {
    size_t   i;
    char* destination = new char[260];
    wcstombs_s(&i, destination, static_cast<size_t>(wcslen(source) + 1), source, static_cast<size_t>(wcslen(source) + 1));
    string result = destination;
    return result;
}
