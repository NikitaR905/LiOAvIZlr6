
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#define MAX 30

/* ---------- Работа с матрицами ---------- */

void zeroMatrix(int n, int m[MAX][MAX]) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m[i][j] = 0;
}

void printMatrix(int n, int m[MAX][MAX]) {
    printf("    ");
    for (int j = 0; j < n; j++)
        printf("%3d", j + 1);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%3d ", i + 1);
        for (int j = 0; j < n; j++)
            printf("%3d", m[i][j]);
        printf("\n");
    }
}

void printAdjList(int n, int m[MAX][MAX]) {
    for (int i = 0; i < n; i++) {
        printf("%d:", i + 1);

        for (int j = 0; j < n; j++) {
            if (m[i][j])
                printf(" %d", j + 1);
        }

        printf("\n");
    }
}

/* ---------- Генерация графа ---------- */

void generateGraph(int n, int m[MAX][MAX]) {
    zeroMatrix(n, m);
    double pr;
    printf("введите вероятность появления ребра\n");
    scanf_s("%lf", &pr);
    double p;
    p = pr / 100;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double r = (double)rand() / (double)RAND_MAX; 
            if (r < p) {
                m[i][j] = 1;
                m[j][i] = 1;
            }
        }
    }
}

/* ---------- Отождествление вершин ---------- */

void mergeVertices(int n, int m[MAX][MAX],
    int u, int v, int res[MAX][MAX], int* newN) {

    if (u < 0 || u >= n || v < 0 || v >= n || u == v) {
        printf("Некорректные номера вершин!\n");
        *newN = 0;
        return;
    }

    *newN = n - 1;
    zeroMatrix(*newN, res);

    int map[MAX];
    int idx = 0;

    for (int i = 0; i < n; i++) {
        if (i != v)
            map[i] = idx++;
    }

    map[v] = map[u];

    for (int i = 0; i < n; i++) {
        if (i == v)
            continue;

        for (int j = 0; j < n; j++) {
            if (j == v)
                continue;

            res[map[i]][map[j]] = m[i][j];
        }
    }

    /* Объединяем соседей u и v */
    for (int i = 0; i < n; i++) {
        if (i == u || i == v)
            continue;

        if (m[u][i] || m[v][i]) {
            res[map[u]][map[i]] = 1;
            res[map[i]][map[u]] = 1;
        }
    }

    for (int i = 0; i < *newN; i++)
        res[i][i] = 0;
}

/* ---------- Стягивание ребра ---------- */

void contractEdge(int n, int m[MAX][MAX],
    int u, int v, int res[MAX][MAX], int* newN) {

    if (u < 0 || u >= n || v < 0 || v >= n || u == v) {
        printf("Некорректные номера вершин!\n");
        *newN = 0;
        return;
    }

    if (!m[u][v]) {
        printf("Ребро между этими вершинами не существует!\n");
        *newN = 0;
        return;
    }

    mergeVertices(n, m, u, v, res, newN);
}

/* ---------- Расщепление вершины ---------- */

void splitVertex(int n, int m[MAX][MAX],
    int v, int res[MAX][MAX], int* newN) {

    if (v < 0 || v >= n || n >= MAX) {
        printf("Некорректная вершина или достигнут MAX!\n");
        *newN = 0;
        return;
    }

    *newN = n + 1;
    zeroMatrix(*newN, res);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = m[i][j];

    int cnt = 0;

    /* Удаляем связи старой вершины */
    for (int i = 0; i < n; i++) {
        res[v][i] = 0;
        res[i][v] = 0;
    }

    /* Делим соседей между двумя вершинами */
    for (int u = 0; u < n; u++) {
        if (u == v || !m[v][u])
            continue;

        if (cnt % 2 == 0)
            res[v][u] = res[u][v] = 1;
        else
            res[n][u] = res[u][n] = 1;

        cnt++;
    }

    /* Соединяем две части расщеплённой вершины */
    res[v][n] = res[n][v] = 1;
}

/* ---------- Операции над графами ---------- */

void unionGraphs(int n, int a[MAX][MAX],
    int b[MAX][MAX], int res[MAX][MAX]) {

    zeroMatrix(n, res);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = a[i][j] || b[i][j];
}

void intersectionGraphs(int n, int a[MAX][MAX],
    int b[MAX][MAX], int res[MAX][MAX]) {

    zeroMatrix(n, res);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = a[i][j] && b[i][j];
}

void ringSumGraphs(int n, int a[MAX][MAX],
    int b[MAX][MAX], int res[MAX][MAX]) {

    zeroMatrix(n, res);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = a[i][j] ^ b[i][j];
}

/* ---------- Декартово произведение ---------- */

void cartesianProduct(int n1, int a[MAX][MAX],
    int n2, int b[MAX][MAX],
    int res[MAX][MAX], int* newN) {

    *newN = n1 * n2;
    zeroMatrix(*newN, res);

    for (int i1 = 0; i1 < n1; i1++) {
        for (int j1 = 0; j1 < n2; j1++) {
            int v1 = i1 * n2 + j1;

            for (int i2 = 0; i2 < n1; i2++) {
                for (int j2 = 0; j2 < n2; j2++) {
                    int v2 = i2 * n2 + j2;

                    if (i1 == i2 && b[j1][j2])
                        res[v1][v2] = 1;

                    if (j1 == j2 && a[i1][i2])
                        res[v1][v2] = 1;
                }
            }
        }
    }
}

/* ---------- Меню операций над вершинами ---------- */

void vertexMenu(int n, int m[MAX][MAX], int useList) {
    int op, u, v;
    int R[MAX][MAX];
    int newN = 0;

    printf("\n--- Операции над вершинами ---\n");
    printf("1 - Отождествление вершин\n");
    printf("2 - Стягивание ребра\n");
    printf("3 - Расщепление вершины\n");
    printf("0 - Назад\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &op) != 1)
        return;

    if (op == 0)
        return;

    if (op < 1 || op > 3) {
        printf("Неверный пункт меню!\n");
        return;
    }

    if (op == 1 || op == 2) {
        printf("Введите номера двух вершин: ");

        if (scanf("%d%d", &u, &v) != 2)
            return;

        if (u < 1 || u > n || v < 1 || v > n || u == v) {
            printf("Некорректные номера вершин!\n");
            return;
        }

        u--;
        v--;

        if (op == 1)
            mergeVertices(n, m, u, v, R, &newN);
        else
            contractEdge(n, m, u, v, R, &newN);
    }
    else {
        printf("Введите номер вершины: ");

        if (scanf("%d", &v) != 1)
            return;

        if (v < 1 || v > n) {
            printf("Некорректный номер вершины!\n");
            return;
        }

        splitVertex(n, m, v - 1, R, &newN);
    }

    if (newN <= 0)
        return;

    printf("\nРезультат операции:\n");

    if (useList)
        printAdjList(newN, R);
    else
        printMatrix(newN, R);
}

/* ---------- Главная программа ---------- */

int main(void) {
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "Russian");

    int n = 0;
    int M1[MAX][MAX] = { 0 };
    int M2[MAX][MAX] = { 0 };
    int R[MAX][MAX] = { 0 };

    int newN = 0;
    int choice;
    int created = 0;

    while (1) {
        printf("\n========== МЕНЮ ==========\n");
        printf("1 - Создать два случайных графа\n");
        printf("2 - Вывести матрицы смежности\n");
        printf("3 - Вывести списки смежности\n");
        printf("4 - Операции над вершинами (матрица)\n");
        printf("5 - Операции над вершинами (список)\n");
        printf("6 - Объединение графов G1 и G2\n");
        printf("7 - Пересечение графов G1 и G2\n");
        printf("8 - Кольцевая сумма графов\n");
        printf("9 - Декартово произведение графов\n");
        printf("0 - Выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1)
            break;

        switch (choice) {
        case 1:
            printf("Введите количество вершин (2-%d): ", MAX);

            if (scanf("%d", &n) != 1)
                goto finish;

            if (n < 2 || n > MAX) {
                printf("Количество вершин должно быть от 2 до %d!\n",
                    MAX);
                n = 0;
                created = 0;
                break;
            }

            generateGraph(n, M1);
            generateGraph(n, M2);
            created = 1;

            printf("\nМатрица смежности G1:\n");
            printMatrix(n, M1);

            printf("\nМатрица смежности G2:\n");
            printMatrix(n, M2);
            break;

        case 2:
            if (!created) {
                printf("Сначала создайте графы!\n");
                break;
            }

            printf("\nМатрица M1:\n");
            printMatrix(n, M1);

            printf("\nМатрица M2:\n");
            printMatrix(n, M2);
            break;

        case 3:
            if (!created) {
                printf("Сначала создайте графы!\n");
                break;
            }

            printf("\nСписок смежности G1:\n");
            printAdjList(n, M1);

            printf("\nСписок смежности G2:\n");
            printAdjList(n, M2);
            break;

        case 4:
        case 5: {
            if (!created) {
                printf("Сначала создайте графы!\n");
                break;
            }

            int graphChoice;

            printf("Выберите граф: 1 - G1, 2 - G2: ");

            if (scanf("%d", &graphChoice) != 1)
                goto finish;

            if (graphChoice == 1)
                vertexMenu(n, M1, choice == 5);
            else if (graphChoice == 2)
                vertexMenu(n, M2, choice == 5);
            else
                printf("Неверный номер графа!\n");

            break;
        }

        case 6:
        case 7:
        case 8:
            if (!created) {
                printf("Сначала создайте графы!\n");
                break;
            }

            if (choice == 6) {
                unionGraphs(n, M1, M2, R);
                printf("\nОбъединение G1 U G2:\n");
            }
            else if (choice == 7) {
                intersectionGraphs(n, M1, M2, R);
                printf("\nПересечение G1 и G2:\n");
            }
            else {
                ringSumGraphs(n, M1, M2, R);
                printf("\nКольцевая сумма G1 и G2:\n");
            }

            printMatrix(n, R);
            break;

        case 9:
            if (!created) {
                printf("Сначала создайте графы!\n");
                break;
            }

            if (n * n > MAX) {
                printf("Результат содержит %d вершин, "
                    "но матрица рассчитана максимум на %d.\n",
                    n * n, MAX);
                printf("Создайте графы с меньшим числом вершин.\n");
                break;
            }

            cartesianProduct(n, M1, n, M2, R, &newN);

            printf("\nДекартово произведение G1 x G2:\n");
            printMatrix(newN, R);
            break;

        case 0:
            goto finish;

        default:
            printf("Неверный пункт меню!\n");
        }
    }

finish:
    printf("\nПрограмма завершена.\n");
    return 0;
}
