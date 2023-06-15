#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

string inputFile = "data.txt";
string outputFile = "output.txt";
string timeStampsFile = "timestamps.txt";

ofstream fout(outputFile);

int batchNum = 7;

int dims[7] = { 100, 500, 1000, 5000, 10000, 50000, 100000 };

//ключ - поле name
struct Automobile {
    string name;
    string brand;
    int year;
    string number;
    string color;

    Automobile() {

    }

    Automobile(string name, string brand, int year, string number, string color)
    {
        this->name = name;
        this->brand = brand;
        this->year = year;
        this->number = number;
        this->color = color;
    }

    friend bool operator== (const Automobile& a, const Automobile& b) {
        if (a.year == b.year && a.name == b.name && a.brand == b.brand
            && a.number == b.number && a.color == b.color)
            return true;
        return false;
    }

    friend bool operator< (const Automobile& a, const Automobile& b) {
        if (a.number < b.number || ((a.number == b.number) && (a.year < b.year)) ||
            ((a.number == b.number) && (a.year == b.year) && (a.brand < b.brand)) ||
            ((a.number == b.number) && (a.year == b.year) && (a.brand == b.brand) && (a.color < b.color)) ||
            ((a.number == b.number) && (a.year == b.year) && (a.brand == b.brand) && (a.color == b.color) && (a.name < b.name)))
            return true;
        return false;
    }

    friend bool operator<= (const Automobile& a, const Automobile& b) {
        if (a < b || a == b)
            return true;
        return false;
    }

    friend bool operator> (const Automobile& a, const Automobile& b) {
        if (!(a < b) && !(a == b))
            return true;
        return false;
    }

    friend bool operator>= (const Automobile& a, const Automobile& b) {
        if (!(a < b))
            return true;
        return false;
    }


    friend ostream& operator<<(ostream& os, const Automobile& a) {
        os << a.name << ' ' << a.brand << ' ' << a.year << ' ' << a.number << ' ' << a.color << '\n';
        return os;
    }

    string getName() const {
        return this->name;
    }
};

//записывает время по 2 врменным точкам
void writeTime(string title, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    fout << title << endl;

    fout << chrono::duration_cast<chrono::microseconds>(end - start).count() << " [микросекунд]\n\n";
}

//функция прямого поиска, возвращает массив найденных индексов
vector<int> straightSearch(vector<Automobile>& v, string key) {
    vector<int> res;

    for (int i = 0; i < v.size(); ++i)
        if (v[i].getName() == key)
            res.push_back(i);

    return res;
}

//найти нижнюю границу удовлетворящих значений двоичным поиском
int getLowerBound(vector<Automobile>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key <= v[mid].getName())
            right = mid - 1;
        else if (key > v[mid].getName() && mid + 1 < v.size() && key == v[mid + 1].getName())
            return mid;
        else if (key > v[mid].getName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//найти верхнюю границу удовлетворящих значений двоичным поиском
int getUpperBound(vector<Automobile>& v, string key) {
    int mid = 0, left = 0, right = v.size();
    while (1)
    {
        mid = (left + right) / 2;

        if (mid < 0 || mid >= v.size())
            return -1;

        if (key < v[mid].getName() && mid - 1 >= 0 && key == v[mid - 1].getName())
            return mid;
        else if (key < v[mid].getName())
            right = mid - 1;
        else if (key >= v[mid].getName())
            left = mid + 1;

        if (left > right)
            return -1;
    }
}

//сам двоичный поиск
vector<Automobile> binarySearch(vector<Automobile>& v, string key) {
    int left = getLowerBound(v, key);
    int right = getUpperBound(v, key);

    if (left == -1 || right == -1)
        return vector<Automobile>();

    auto first = v.cbegin() + left + 1;
    auto last = v.cbegin() + right;

    vector<Automobile> res(first, last);

    return res;
}

//функция для генерации случайной строки с заданной длиной
string randomString(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

//функция, создающая текстовый файл для чтения после
void makeTxtFile() {
    srand((unsigned)time(NULL));
    ofstream fout(inputFile);

    for (int i = 0; i < batchNum; ++i) {
        //Вывод числа записей
        fout << dims[i] << "\n";
        for (int j = 0; j < dims[i]; ++j) {
            //Вывод полей по порядку
            fout << randomString(10) << "\n";   //Имя
            fout << randomString(8) << "\n";    //Марка
            fout << rand() % 10 + 1 << "\n";    //Год выпуска
            fout << randomString(8) << "\n";    //Гос номер
            fout << randomString(8) << "\n";    //Цвет
        }
    }
}

//функция для чтения объектов из текстового файла
vector<vector<Automobile>> readTxtFile() {
    ifstream fin(inputFile);

    vector<vector<Automobile>> result;

    int dim;
    string name, brand, number, color;
    int year;

    for (int i = 0; i < batchNum; ++i) {
        //Ввод числа записей
        fin >> dim;
        vector<Automobile> v;
        for (int j = 0; j < dim; ++j) {
            //Ввод полей по порядку
            fin >> name >> brand >> year >> number >> color;
            Automobile temp(name, brand, year, number, color);
            v.push_back(temp);
        }
        result.push_back(v);
    }

    return result;
}

// Merge two subarrays L and M into arr
void merge(vector<Automobile>& arr, int p, int q, int r) {

    // Create L ← A[p..q] and M ← A[q+1..r]
    int n1 = q - p + 1;
    int n2 = r - q;

    Automobile* L = new Automobile[n1];
    Automobile* M = new Automobile[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    // Maintain current index of sub-arrays and main array
    int i, j, k;
    i = 0;
    j = 0;
    k = p;

    // Until we reach either end of either L or M, pick larger among
    // elements L and M and place them in the correct position at A[p..r]
    while (i < n1 && j < n2) {
        if (L[i] <= M[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = M[j];
            j++;
        }
        k++;
    }

    // When we run out of elements in either L or M,
    // pick up the remaining elements and put in A[p..r]
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = M[j];
        j++;
        k++;
    }
}

// Divide the array into two subarrays, sort them and merge them
void mergeSort(vector<Automobile>& arr, int l, int r) {
    if (l < r) {
        // m is the point where the array is divided into two subarrays
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge the sorted subarrays
        merge(arr, l, m, r);
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    makeTxtFile();

    //массив объектов
    vector<vector<Automobile>> initialArray = readTxtFile();

    //массив с ключами, которые будут искаться 
    vector<string> selectedKeys;

    for (int i = 0; i < batchNum; ++i) {
        selectedKeys.push_back(initialArray[i][rand() % initialArray[i].size()].getName());
    }

    std::chrono::steady_clock::time_point start, end;
    Automobile foundValue;

    fout << "Прямой поиск:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<int> temp = straightSearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << endl;
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найден элемент: " << initialArray[i][temp[j]];
        writeTime("На прямой поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    fout << "Сортировка:" << endl << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        mergeSort(initialArray[i], 0, initialArray[i].size() - 1);
        binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        writeTime("На сортировку и двоичный поиск " + to_string(initialArray[i].size()) + " значениий ушло: ", start, end);
        start = end;
    }

    fout << "Бинарный поиск в отсортированном массиве:" << endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        vector<Automobile> temp = binarySearch(initialArray[i], selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << "\n";
        if (!temp.empty())
            for (int j = 0; j < temp.size(); ++j)
                fout << "Найден элемент: " << temp[j];
        writeTime("На бинарный поиск с " + to_string(initialArray[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    vector<multimap<string, Automobile>> m;

    for (int i = 0; i < batchNum; ++i) {
        multimap<string, Automobile> temp;
        for (int j = 0; j < initialArray[i].size(); ++j)
            temp.insert({ initialArray[i][j].getName(), initialArray[i][j] });
        m.push_back(temp);
    }

    initialArray.clear();

    fout << "Поиск в ассоциативном массиве:\n\n";

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < batchNum; ++i) {
        auto bounds = m[i].equal_range(selectedKeys[i]);
        end = std::chrono::steady_clock::now();
        fout << "Искался ключ: " << selectedKeys[i] << "\n";
        if (bounds.first != m[i].end())
            for (auto it = bounds.first; it != bounds.second; ++it)
                fout << "Найден элемент: " << it->second;
        writeTime("На поиск в ассоциативном массиве с " + to_string(m[i].size()) + " значениями ушло: ", start, end);
        start = end;
    }

    return 0;
}
