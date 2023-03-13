#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

/*
Для 4 переменных будет выглядеть следующим образом:
Таблица истинности:
x1 x2 x3 x4| f
0  0  0  0 | 0
...
Да, во многих решениях предлагается считывать только позиции единиц,
но мне кажется, что такой вид представления таблицы истинности
позволяет внедрять новые функции - построение КНФ, ДНФ по таблице истинности, например.
Да и работать - дебажить - с такой таблицей проще и нагляднее.
Карта Карно:
x3, x4| 0 0 | 0 1 | 1 1 | 1 0
x1, x2|  f  |  f  |  f  |  f
 0  0 |  f  |  f  |  f  |  f
 0  1 |  f  |  f  |  f  |  f
 1  1 |  f  |  f  |  f  |  f
 1  0 |  f  |  f  |  f  |  f
*/

class Kmap_Solution {
    char** truth_table;
    char** kmap;
    int n;
public:
    Kmap_Solution(int variables) {
        n = variables;
        if (n > 0) {
            truth_table = input_truth_table();
            kmap = create_karnaugh_map();
            print_table(kmap, n, n);
            cout << endl << build_expression();
        }
        else {
            truth_table = nullptr;
            kmap = nullptr;
        }
    }
    Kmap_Solution(char** Karnaugh_map, int variables) {
        truth_table = nullptr;
        kmap = Karnaugh_map;
        n = variables;
        print_table(kmap, n, n);
        cout << endl << build_expression();
    }
    ~Kmap_Solution() {
        if (truth_table != nullptr)
            free_array(truth_table, 1 << n);
        free_array(kmap, n);
    }
private:
    char** input_truth_table()
    { // n - кол-во переменных
        char** respond = new char* [1 << n];
        for (int i = 0; i < 1 << n; i++)
        {
            respond[i] = new char[n + 1]; // n + 1 т.к. n переменных и 1 столбец значения функции
        }
        for (int i = 0; i < 1 << n; i++)
        {
            for (int j = 0; j < n + 1; j++)
            {
                cin >> respond[i][j];
            }
        }
        return respond;
    }

    void print_table(char** table, int k, int m)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < m; j++)
            {
                cout << table[i][j] << " ";
            }
            cout << endl;
        }
    }

    char** create_karnaugh_map()
    {
        char** respond = new char* [n];
        for (int i = 0; i < n; i++)
        {
            respond[i] = new char[n];
        }
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                respond[i][j] = truth_table[translate_function(i, j)][n]; // в n-ом столбце (нумерация с 0) будет лежать значение функции для столбца
                // cout << "i = " << i << "; j = " << j << "; func = " << translate_function(i, j) << "; table_val = " << truth_table[translate_function(i, j)][n] << endl;
            }
        }
        return respond;
    }

    int translate_function(int i, int j)
    {
        // функция выполнена для случая 4х переменных
        int result = 0;
        if (j == 2)
            result += 3;
        else if (j == 3)
            result += 2;
        else
            result += j;

        if (i == 1)
            result += 4;
        else if (i == 2)
            result += 12;
        else if (i == 3)
            result += 8;

        return result;
    }

    vector<int> parse_line_horizontal(int k, int m)
    {
        // k, m - позиция точки, n - кол-во переменных. В нашем случае их 4
        // функция не должна запускаться для точек, где f = 0;
        vector<int> result;
        if (kmap[m][k] == '0')
            return result;
        int addition = 1;
        int j = k;
        while (result.size() < n)
        {
            if (kmap[m][j] == '1')
                result.push_back(j);
            else if (kmap[m][j % n] == '0')
            {
                if (result.size() == 3)
                {
                    break;
                }
                else if (addition == -1)
                {
                    break;
                }
                else
                {
                    j = k;
                    addition = -1;
                }
            }
            j += addition;
            if (j < 0)
                j = n - 1;
            if (j >= n)
                j = 0;
        }
        sort(result.begin(), result.end());
        return result;
    }

    vector<int> parse_line_vertical(int k, int m)
    {
        vector<int> result;
        if (kmap[m][k] == '0')
            return result;
        int addition = 1;
        int i = m;
        while (result.size() < n)
        {
            if (kmap[i][k] == '1')
            {
                result.push_back(i);
            }
            else if (kmap[i % n][k] == '0')
            {
                if (result.size() == 3)
                {
                    break;
                }
                else if (addition == -1)
                {
                    break;
                }
                else
                {
                    i = m;
                    addition = -1;
                }
            }
            i += addition;
            if (i < 0)
                i = n - 1;
            if (i >= n)
                i = 0;
        }
        return result;
    }

    vector<int> parse_rectangle_vertical(int k, int m)
    {
        // Когда мы парсим прямоугольник, то нас в случае 4х4 интересует всего 3 фигуры:
        // квадрат 2х2, пр-ки 2x4, 4x2, все фигуры можно индетифицировать с помощью функций parse_line
        vector<int> respond;
        vector<int> mid_line = parse_line_vertical(k, m);
        vector<int> left_line;
        vector<int> right_line;
        if (k + 1 < n)
        {
            right_line = parse_line_vertical(k + 1, m);
        }
        if (k - 1 >= 0)
        {
            left_line = parse_line_vertical(k - 1, m);
        }
        if (mid_line.size() == 4 && right_line.size() == 4)
        {
            respond = vector<int>{ k, k + 1 };
            return respond;
        }
        else if (mid_line.size() == 4 && left_line.size() == 4)
        {
            respond = vector<int>{ k - 1, k };
            return respond;
        }
        else
        {
            return respond;
        }
    }

    vector<int> parse_rectangle_horizontal(int k, int m)
    {
        vector<int> respond;
        vector<int> mid_line = parse_line_horizontal(k, m);
        vector<int> top_line;
        vector<int> bottom_line;
        if (m + 1 < n)
        {
            bottom_line = parse_line_horizontal(k, m + 1);
        }
        if (m - 1 >= 0)
        {
            top_line = parse_line_horizontal(k, m - 1);
        }
        if (mid_line.size() == 4 && top_line.size() == 4)
        {
            respond = vector<int>{ m - 1, m };
            return respond;
        }
        else if (mid_line.size() == 4 && bottom_line.size() == 4)
        {
            respond = vector<int>{ m, m + 1 };
            return respond;
        }

        return respond;
    }

    pair<int, int> parse_square(int k, int m)
    {
        if (is_square(k, m))
        {
            return pair<int, int>{k, m};
        }
        if (k - 1 < 0)
        {
            if (is_square(n - 1, m))
            {
                return pair<int, int>{n - 1, m};
            }
        }
        if (m - 1 < 0)
        {
            if (is_square(k, n - 1))
            {
                return pair<int, int>{k, n - 1};
            }
        }
        if (k - 1 < 0 && m - 1 < 0 && k != 0 && m != 0)
        {
            if (is_square(n - 1, n - 1))
            {
                return pair<int, int>{n - 1, n - 1};
            }
        }
        return pair<int, int>{-1, -1};
    }

    bool is_square(int i, int j)
    {
        int k = i + 1;
        int m = j + 1;
        if (k >= n)
            k = 0;
        if (m >= n)
            m = 0;

        if (kmap[i][j] == '1' && kmap[k][j] == '1' && kmap[i][m] == '1' && kmap[k][m] == '1')
            return true;
        else
            return false;
    }

    string build_expression()
    {
        if (check_all_ones())
            return "1";
        if (check_all_zeros())
            return "0";
        char** checked = create_unchecked_map();
        string expression = "";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                // cout << "checked: " << checked[i][j] << " ; i = " << i << " ; j = " << j << "; expression: " << expression << endl;
                if (checked[i][j] == '0')
                {

                    if (parse_rectangle_horizontal(j, i).size() > 0)
                    {
                        expression = expression + " + " +
                            to_expression_horizontal_rectangle(checked, parse_rectangle_horizontal(j, i));
                        ;
                    }
                    else if (parse_rectangle_vertical(j, i).size() > 0)
                    {
                        expression = expression + " + " +
                            to_expression_vertical_rectangle(checked, parse_rectangle_vertical(j, i));
                    }
                    else if (parse_square(i, j).first != -1)
                    {
                        expression = expression + " + " +
                            to_expression_square(checked, parse_square(i, j));
                    }
                    else if (parse_line_horizontal(j, i).size() == 1 && parse_line_vertical(j, i).size() == 1)
                    {
                        expression = expression + " + " +
                            to_expression_single_one(checked, i, j);
                    }
                    else if (parse_line_horizontal(j, i).size() > 0 || parse_line_vertical(j, i).size() > 0)
                    {
                        if (parse_line_horizontal(j, i).size() > parse_line_vertical(j, i).size())
                        {
                            expression = expression + " + " +
                                to_expression_horizontal_line(checked, parse_line_horizontal(j, i), i);
                        }
                        else
                        {
                            expression = expression + " + " +
                                to_expression_vertical_line(checked, parse_line_vertical(j, i), j);
                        }
                    }
                }
            }
        }
        free_array(checked, n);
        return expression.substr(3, expression.length() + 1);
    }

    string to_expression_horizontal_line(char** checked, vector<int> indexes, int line)
    {
        if (indexes.size() == 3)
        {
            // Надо удалить лишний элемент
            if (indexes[2] != line)
                indexes.pop_back();
            else
                indexes.erase(indexes.begin());
        }
        for (int j = 0; j < indexes.size(); j++)
        {
            checked[line][indexes[j]] = '1';
        }
        sort(indexes.begin(), indexes.end());
        // теперь надо составить выражение.
        string expr = "";
        switch (line)
        {
        case 0:
            expr = "x1' * x2'";
            break;
        case 1:
            expr = "x1' * x2";
            break;
        case 2:
            expr = "x1 * x2";
            break;
        case 3:
            expr = "x1 * x2'";
            break;
        default:
            break;
        }
        if (indexes.size() == 2)
        {
            if (indexes[0] == 0 && indexes[1] == 1)
            {
                expr += " * x3'";
            }
            else if (indexes[0] == 1 && indexes[1] == 2)
            {
                expr += " * x4";
            }
            else if (indexes[0] == 2 && indexes[1] == 3)
            {
                expr += " * x3";
            }
            else if (indexes[0] == 0 && indexes[1] == 3)
            {
                expr += " * x4'";
            }
        }
        return expr;
    }

    string to_expression_vertical_line(char** checked, vector<int> indexes, int line)
    {
        if (indexes.size() == 3)
        {
            // Надо удалить лишний элемент
            if (indexes[2] != line)
                indexes.pop_back();
            else
                indexes.erase(indexes.begin());
        }
        for (int i = 0; i < indexes.size(); i++)
        {
            checked[indexes[i]][line] = '1';
        }
        sort(indexes.begin(), indexes.end());
        string expr = "";
        switch (line)
        {
        case 0:
            expr = "x3' * x4'";
            break;
        case 1:
            expr = "x3' * x4";
            break;
        case 2:
            expr = "x3 * x4";
            break;
        case 3:
            expr = "x3 * x4'";
            break;
        default:
            break;
        }
        if (indexes.size() == 2)
        {
            if (indexes[0] == 0 && indexes[1] == 1)
            {
                expr += " * x1'";
            }
            else if (indexes[0] == 1 && indexes[1] == 2)
            {
                expr += " * x2";
            }
            else if (indexes[0] == 2 && indexes[1] == 3)
            {
                expr += " * x1";
            }
            else if (indexes[0] == 0 && indexes[1] == 3)
            {
                expr += " * x2'";
            }
        }
        return expr;
    }

    string to_expression_vertical_rectangle(char** checked, vector<int> lines)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < lines.size(); j++)
            {
                checked[i][lines[j]] = '1';
            }
        }
        string expr = "";
        if (lines[0] == 0 && lines[1] == 1)
        {
            expr = "x3'";
        }
        else if (lines[0] == 1 && lines[1] == 2)
        {
            expr = "x4";
        }
        else if (lines[0] == 2 && lines[1] == 3)
        {
            expr = "x3";
        }
        else if (lines[0] == 3 && lines[1] == 0)
        {
            expr = "x4'";
        }
        return expr;
    }

    string to_expression_horizontal_rectangle(char** checked, vector<int> lines)
    {
        for (int i = 0; i < lines.size(); i++)
        {
            for (int j = 0; j < n; j++)
            {
                checked[lines[i]][j] = '1';
            }
        }
        string expr = "";
        if (lines[0] == 0 && lines[1] == 1)
        {
            expr = "x1'";
        }
        else if (lines[0] == 1 && lines[1] == 2)
        {
            expr = "x2";
        }
        else if (lines[0] == 2 && lines[1] == 3)
        {
            expr = "x1";
        }
        else if (lines[0] == 3 && lines[1] == 0)
        {
            expr = "x2'";
        }
        return expr;
    }

    string to_expression_square(char** checked, pair<int, int> position)
    {
        int k = position.first + 1;
        int m = position.second + 1;
        if (k >= n)
            k = 0;
        if (m >= n)
            m = 0;
        checked[position.first][position.second] = '1';
        checked[position.first][m] = '1';
        checked[k][position.second] = '1';
        checked[k][m] = '1';

        string expr = "";
        switch (position.first)
        {
        case 0:
            expr += "x1'";
            break;
        case 1:
            expr += "x2";
            break;
        case 2:
            expr += "x1";
            break;
        case 3:
            expr += "x2'";
            break;
        default:
            break;
        }
        switch (position.second)
        {
        case 0:
            expr += " * x3'";
            break;
        case 1:
            expr += " * x4";
            break;
        case 2:
            expr += " * x3";
            break;
        case 3:
            expr += " * x4'";
            break;
        default:
            break;
        }
        return expr;
    }

    string to_expression_single_one(char** checked, int i, int j)
    {
        checked[i][j] = '1';
        string expr = "";
        switch (i)
        {
        case 0:
            expr += "x1' * x2'";
            break;
        case 1:
            expr += "x1' * x2";
            break;
        case 2:
            expr += "x1 * x2";
            break;
        case 3:
            expr += "x1 * x2'";
            break;
        default:
            break;
        }
        switch (j)
        {
        case 0:
            expr += " * x3' * x4'";
            break;
        case 1:
            expr += " * x3' * x4";
            break;
        case 2:
            expr += " * x3 * x4";
            break;
        case 3:
            expr += " * x3 * x4'";
            break;
        default:
            break;
        }
        return expr;
    }

    bool check_all_zeros()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (kmap[i][j] == '1')
                    return false;
            }
        }
        return true;
    }

    bool check_all_ones()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (kmap[i][j] == '0')
                    return false;
            }
        }
        return true;
    }

    char** create_unchecked_map()
    {
        char** respond = new char* [n];
        for (int i = 0; i < n; i++)
        {
            respond[i] = new char[n];
        }
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (kmap[i][j] == '1')
                    respond[i][j] = '0';
                else
                    respond[i][j] = '*'; // нам не важны нули
            }
        }
        return respond;
    }

    void free_array(char** matrix, int k)
    {
        for (int i = 0; i < k; i++)
        {
            delete[] matrix[i]; // delete array within matrix
        }
        // delete actual matrix
        delete[] matrix;
    }
};
void test_cases()
{
    const int n = 4;
    char kmap1_static[4][4] = { {'1', '0', '1', '1'},
                               {'1', '0', '0', '1'},
                               {'1', '1', '1', '1'},
                               {'1', '1', '1', '1'} };

    char** kmap1 = new char* [n];
    for (int i = 0; i < n; i++)
    {
        kmap1[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap1[i][j] = kmap1_static[i][j];
    Kmap_Solution k1 = Kmap_Solution(kmap1, n);
    cout << "\n\n";
    char** kmap2 = new char* [n];
    for (int i = 0; i < n; i++)
    {
        kmap2[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap2[i][j] = '0';
    kmap2[0][0] = '1';
    Kmap_Solution k2 = Kmap_Solution(kmap2, n);
    cout << "\n\n";

    char** kmap3 = new char* [n];
    for (int i = 0; i < n; i++)
    {
        kmap3[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < 2; j++)
            kmap3[i][j] = '1';
    for (int i = 0; i < n; i++)
        for (int j = 2; j < n; j++)
            kmap3[i][j] = '0';
    Kmap_Solution k3 = Kmap_Solution(kmap3, n);
    cout << "\n\n";

    char** kmap4 = new char* [n];
    for (int i = 0; i < n; i++)
    {
        kmap4[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap4[i][j] = '0';
    Kmap_Solution k4 = Kmap_Solution(kmap4, n);
}

int main()
{
    test_cases();
    return 0;
}