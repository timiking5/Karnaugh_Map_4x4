#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

char **input_truth_table(int n);
void print_table(char **table, int n, int m);
char **create_karnaugh_map(char **truth_table, int n);
int translate_function(int i, int j);
void free_array(char **matrix, int n);
vector<int> parse_line_horizontal(char **kmap, int k, int m, int n);
vector<int> parse_line_vertical(char **kmap, int k, int m, int n);
vector<int> parse_rectangle_vertical(char **kmap, int k, int m, int n);
vector<int> parse_rectangle_horizontal(char **kmap, int k, int m, int n); // k - j, m - i
pair<int, int> parse_square(char **kmap, int k, int m, int n);            // k - i, m - j
bool is_square(char **kmap, int i, int j, int n);
string build_expression(char **kmap, int n);
bool check_all_zeros(char **kmap, int n);
bool check_all_ones(char **kmap, int n);
char **create_unchecked_map(char **kmap, int n);
string to_expression_horizontal_line(char **checked, vector<int> indexes, int line);
string to_expression_vertical_line(char **checked, vector<int> indexes, int line);
string to_expression_vertical_rectangle(char **checked, vector<int> lines, int n);
string to_expression_horizontal_rectangle(char **checked, vector<int> lines, int n);
string to_expression_square(char **checked, pair<int, int> position, int n);
string to_expression_single_one(char **checked, int i, int j);
void test_cases();

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

int main()
{
    test_cases();
    return 0;
}

char **input_truth_table(int n)
{ // n - кол-во переменных
    char **respond = new char *[pow(2, n)];
    for (int i = 0; i < pow(2, n); i++)
    {
        respond[i] = new char[n + 1]; // n + 1 т.к. n переменных и 1 столбец значения функции
    }
    for (int i = 0; i < pow(2, n); i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            cin >> respond[i][j];
        }
    }
    return respond;
}

void print_table(char **table, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cout << table[i][j];
            if (j != m - 1)
                cout << " ";
        }
        if (i != n - 1)
            cout << endl;
    }
}

char **create_karnaugh_map(char **truth_table, int n)
{
    char **respond = new char *[n];
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

vector<int> parse_line_horizontal(char **kmap, int k, int m, int n)
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

vector<int> parse_line_vertical(char **kmap, int k, int m, int n)
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

vector<int> parse_rectangle_vertical(char **kmap, int k, int m, int n)
{
    // Когда мы парсим прямоугольник, то нас в случае 4х4 интересует всего 3 фигуры:
    // квадрат 2х2, пр-ки 2x4, 4x2, все фигуры можно индетифицировать с помощью функций parse_line
    vector<int> respond;
    vector<int> mid_line = parse_line_vertical(kmap, k, m, n);
    vector<int> left_line;
    vector<int> right_line;
    if (k + 1 < n)
    {
        right_line = parse_line_vertical(kmap, k + 1, m, n);
    }
    if (k - 1 >= 0)
    {
        left_line = parse_line_vertical(kmap, k - 1, m, n);
    }
    if (mid_line.size() == 4 && right_line.size() == 4)
    {
        respond = vector<int>{k, k + 1};
        return respond;
    }
    else if (mid_line.size() == 4 && left_line.size() == 4)
    {
        respond = vector<int>{k - 1, k};
        return respond;
    }
    else
    {
        return respond;
    }
}

vector<int> parse_rectangle_horizontal(char **kmap, int k, int m, int n)
{
    vector<int> respond;
    vector<int> mid_line = parse_line_horizontal(kmap, k, m, n);
    vector<int> top_line;
    vector<int> bottom_line;
    if (m + 1 < n)
    {
        bottom_line = parse_line_horizontal(kmap, k, m + 1, n);
    }
    if (m - 1 >= 0)
    {
        top_line = parse_line_horizontal(kmap, k, m - 1, n);
    }
    if (mid_line.size() == 4 && top_line.size() == 4)
    {
        respond = vector<int>{m - 1, m};
        return respond;
    }
    else if (mid_line.size() == 4 && bottom_line.size() == 4)
    {
        respond = vector<int>{m, m + 1};
        return respond;
    }

    return respond;
}

pair<int, int> parse_square(char **kmap, int k, int m, int n)
{
    if (is_square(kmap, k, m, n))
    {
        return pair<int, int>{k, m};
    }
    if (k - 1 < 0 && m - 1 < 0)
    {
        if (is_square(kmap, n - 1, n - 1, n))
        {
            return pair<int, int>{n - 1, n - 1};
        }
    }
    if (k - 1 < 0)
    {
        if (is_square(kmap, n - 1, m, n))
        {
            return pair<int, int>{n - 1, m};
        }
    }
    if (m - 1 < 0)
    {
        if (is_square(kmap, k, n - 1, n))
        {
            return pair<int, int>{k, n - 1};
        }
    }

    return pair<int, int>{-1, -1};
}

bool is_square(char **kmap, int i, int j, int n)
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

string build_expression(char **kmap, int n)
{
    if (check_all_ones(kmap, n))
        return "1";
    if (check_all_zeros(kmap, n))
        return "0";
    char **checked = create_unchecked_map(kmap, n);
    string expression = "";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // cout << "checked: " << checked[i][j] << " ; i = " << i << " ; j = " << j << "; expression: " << expression << endl;
            if (checked[i][j] == '0')
            {

                if (parse_rectangle_horizontal(kmap, j, i, n).size() > 0)
                {
                    expression = expression + " + " +
                                 to_expression_horizontal_rectangle(checked, parse_rectangle_horizontal(kmap, j, i, n), n);
                    ;
                }
                else if (parse_rectangle_vertical(kmap, j, i, n).size() > 0)
                {
                    expression = expression + " + " +
                                 to_expression_vertical_rectangle(checked, parse_rectangle_vertical(kmap, j, i, n), n);
                }
                else if (parse_square(kmap, i, j, n).first != -1)
                {
                    expression = expression + " + " +
                                 to_expression_square(checked, parse_square(kmap, i, j, n), n);
                }
                else if (parse_line_horizontal(kmap, j, i, n).size() == 1 && parse_line_vertical(kmap, j, i, n).size() == 1)
                {
                    expression = expression + " + " +
                                 to_expression_single_one(checked, i, j);
                }
                else if (parse_line_horizontal(kmap, j, i, n).size() > 0 || parse_line_vertical(kmap, j, i, n).size() > 0)
                {
                    if (parse_line_horizontal(kmap, j, i, n).size() > parse_line_vertical(kmap, j, i, n).size())
                    {
                        expression = expression + " + " +
                                     to_expression_horizontal_line(checked, parse_line_horizontal(kmap, j, i, n), i);
                    }
                    else
                    {
                        expression = expression + " + " +
                                     to_expression_vertical_line(checked, parse_line_vertical(kmap, j, i, n), j);
                    }
                }
            }
        }
    }
    free_array(checked, n);
    return expression.substr(3, expression.length() + 1);
}

string to_expression_horizontal_line(char **checked, vector<int> indexes, int line)
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

string to_expression_vertical_line(char **checked, vector<int> indexes, int line)
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

string to_expression_vertical_rectangle(char **checked, vector<int> lines, int n)
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

string to_expression_horizontal_rectangle(char **checked, vector<int> lines, int n)
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

string to_expression_square(char **checked, pair<int, int> position, int n)
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

string to_expression_single_one(char **checked, int i, int j)
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

bool check_all_zeros(char **kmap, int n)
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

bool check_all_ones(char **kmap, int n)
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

char **create_unchecked_map(char **kmap, int n)
{
    char **respond = new char *[n];
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

void free_array(char **matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        delete[] matrix[i]; // delete array within matrix
    }
    // delete actual matrix
    delete[] matrix;
}

void test_cases()
{
    const int n = 4;
    char kmap1_static[4][4] = {{'0', '0', '1', '1'},
                               {'1', '0', '0', '1'},
                               {'0', '1', '1', '1'},
                               {'1', '1', '1', '1'}};

    char **kmap1 = new char *[n];
    for (int i = 0; i < n; i++)
    {
        kmap1[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap1[i][j] = kmap1_static[i][j];
    print_table(kmap1, n, n);
    cout << endl
         << build_expression(kmap1, n) << "\n\n";
    free_array(kmap1, n);

    char **kmap2 = new char *[n];
    for (int i = 0; i < n; i++)
    {
        kmap2[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap2[i][j] = '0';
    kmap2[0][0] = '1';
    print_table(kmap2, n, n);
    cout << endl
         << build_expression(kmap2, n) << "\n\n";
    free_array(kmap2, n);

    char **kmap3 = new char *[n];
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
    print_table(kmap3, n, n);
    cout << endl
         << build_expression(kmap3, n) << "\n\n";
    free_array(kmap3, n);

    char **kmap4 = new char *[n];
    for (int i = 0; i < n; i++)
    {
        kmap4[i] = new char[n];
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            kmap4[i][j] = '0';
    print_table(kmap4, n, n);

    cout << endl
         << build_expression(kmap4, n);
    free_array(kmap4, n);
}
