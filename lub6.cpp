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
    printf("Введите вероятность появления ребра: ");
    scanf_s("%lf", &pr);
    double p = pr / 100.0;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double r = (double)rand() / (double)RAND_MAX;
            if (r < p) {
                m[i][j] = 1;
                m[j][i] = 1;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        int hasEdge = 0;
        for (int j = 0; j < n; j++) {
            if (m[i][j]) { hasEdge = 1; break; }
        }
        if (!hasEdge) {
            int target = (i + 1) % n;
            m[i][target] = 1;
            m[target][i] = 1;
        }
    }
}

/* ---------- Отождествление вершин ---------- */

void mergeVertices(int n, int m[MAX][MAX], int u, int v, int res[MAX][MAX], int* newN) {
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
        if (i == v) continue;
        for (int j = 0; j < n; j++) {
            if (j == v) continue;
            res[map[i]][map[j]] = m[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        if (i == u || i == v) continue;
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

    if (*newN > 0) {
        int map[MAX];
        int idx = 0;
        for (int i = 0; i < n; i++) {
            if (i != v)
                map[i] = idx++;
        }
        map[v] = map[u];
        res[map[u]][map[u]] = 1;
    }
}

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

    int newV = n;

    for (int i = 0; i < n; i++) {
        res[v][i] = 0;
        res[i][v] = 0;
    }

    for (int u = 0; u < n; u++) {
        if (u == v || !m[v][u]) continue;
        res[v][u] = res[u][v] = 1;
        res[newV][u] = res[u][newV] = 1;
    }

    res[v][newV] = res[newV][v] = 1;
}

/* ---------- Удаление изолированных вершин ---------- */

void removeIsolatedVertices(int n, int m[MAX][MAX], int res[MAX][MAX], int* newN) {
    int map[MAX];
    int idx = 0;

    for (int i = 0; i < n; i++) {
        int hasEdge = 0;
        for (int j = 0; j < n; j++) {
            if (m[i][j]) { hasEdge = 1; break; }
        }
        map[i] = hasEdge ? idx++ : -1;
    }

    *newN = idx;
    if (*newN == 0) {
        zeroMatrix(0, res);
        return;
    }

    zeroMatrix(*newN, res);

    for (int i = 0; i < n; i++) {
        if (map[i] == -1) continue;
        for (int j = 0; j < n; j++) {
            if (map[j] == -1) continue;
            res[map[i]][map[j]] = m[i][j];
        }
    }
}

/* ---------- Операции над графами ---------- */

void unionGraphs(int n1, int a[MAX][MAX],
    int n2, int b[MAX][MAX],
    int res[MAX][MAX], int* newN) {
    *newN = n1 + n2;
    if (*newN > MAX) { *newN = 0; return; }

    zeroMatrix(*newN, res);

    for (int i = 0; i < n1; i++)
        for (int j = 0; j < n1; j++)
            res[i][j] = a[i][j];

    for (int i = 0; i < n2; i++)
        for (int j = 0; j < n2; j++)
            res[n1 + i][n1 + j] = b[i][j];
}

void intersectionGraphs(int n1, int a[MAX][MAX],
    int n2, int b[MAX][MAX],
    int res[MAX][MAX], int* newN) {
    *newN = (n1 < n2) ? n1 : n2;
    if (*newN <= 0) { *newN = 0; return; }
    zeroMatrix(*newN, res);

    for (int i = 0; i < *newN; i++)
        for (int j = 0; j < *newN; j++)
            res[i][j] = a[i][j] && b[i][j];
}

void ringSumGraphs(int n1, int a[MAX][MAX],
    int n2, int b[MAX][MAX],
    int res[MAX][MAX], int* newN) {
    int N = (n1 > n2) ? n1 : n2;
    if (N > MAX) { *newN = 0; return; }

    int tmp[MAX][MAX];
    zeroMatrix(N, tmp);

    for (int i = 0; i < n1; i++)
        for (int j = 0; j < n1; j++)
            tmp[i][j] = a[i][j];

    for (int i = 0; i < n2; i++)
        for (int j = 0; j < n2; j++)
            tmp[i][j] = tmp[i][j] ^ b[i][j];

    removeIsolatedVertices(N, tmp, res, newN);
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
                    if (i1 == i2 && b[j1][j2]) res[v1][v2] = 1;
                    if (j1 == j2 && a[i1][i2]) res[v1][v2] = 1;
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

    if (scanf("%d", &op) != 1) return;
    if (op == 0) return;
    if (op < 1 || op > 3) { printf("Неверный пункт меню!\n"); return; }

    if (op == 1 || op == 2) {
        printf("Введите номера двух вершин: ");
        if (scanf("%d%d", &u, &v) != 2) return;
        if (u < 1 || u > n || v < 1 || v > n || u == v) {
            printf("Некорректные номера вершин!\n"); return;
        }
        u--; v--;
        if (op == 1)
            mergeVertices(n, m, u, v, R, &newN);
        else
            contractEdge(n, m, u, v, R, &newN);
    }
    else {
        printf("Введите номер вершины: ");
        if (scanf("%d", &v) != 1) return;
        if (v < 1 || v > n) { printf("Некорректный номер вершины!\n"); return; }
        splitVertex(n, m, v - 1, R, &newN);
    }

    if (newN <= 0) return;

    printf("\nРезультат операции:\n");
    if (useList) printAdjList(newN, R);
    else         printMatrix(newN, R);
}

/* ---------- Главная программа ---------- */

int main(void) {
    srand((unsigned int)time(NULL));
    setlocale(LC_ALL, "Russian");

    int n1 = 0, n2 = 0;
    int M1[MAX][MAX] = { 0 };
    int M2[MAX][MAX] = { 0 };
    int R[MAX][MAX] = { 0 };

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

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
        case 1:
            printf("Введите количество вершин для G1: ");
            if (scanf("%d", &n1) != 1 || n1 < 2 || n1 > MAX) {
                printf("Некорректное число вершин для G1.\n"); break;
            }
            printf("Введите количество вершин для G2: ");
            if (scanf("%d", &n2) != 1 || n2 < 2 || n2 > MAX) {
                printf("Некорректное число вершин для G2.\n"); break;
            }

            generateGraph(n1, M1);
            generateGraph(n2, M2);
            created = 1;

            printf("\nМатрица смежности G1:\n"); printMatrix(n1, M1);
            printf("\nМатрица смежности G2:\n"); printMatrix(n2, M2);
            break;

        case 2:
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            printf("\nМатрица M1 :\n"); printMatrix(n1, M1);
            printf("\nМатрица M2 :\n"); printMatrix(n2, M2);
            break;

        case 3:
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            printf("\nСписок смежности G1 :\n"); printAdjList(n1, M1);
            printf("\nСписок смежности G2 :\n"); printAdjList(n2, M2);
            break;

        case 4:
        case 5: {
            if (!created) { printf("Сначала создайте графы!\n"); break; }

            int graphChoice;
            printf("Выберите граф: 1 - G1, 2 - G2: ");
            if (scanf("%d", &graphChoice) != 1) goto finish;

            if (graphChoice == 1)
                vertexMenu(n1, M1, choice == 5);
            else if (graphChoice == 2)
                vertexMenu(n2, M2, choice == 5);
            else
                printf("Неверный номер графа!\n");
            break;
        }

        case 6: {
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            int newN_union = 0;
            unionGraphs(n1, M1, n2, M2, R, &newN_union);
            if (newN_union == 0) { printf("\nСлишком большой результат.\n"); break; }
            printf("\nОбъединение G1 и G2:\n");
            printMatrix(newN_union, R);
            break;
        }

        case 7: {
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            int newN_inter = 0;
            intersectionGraphs(n1, M1, n2, M2, R, &newN_inter);
            if (newN_inter == 0) { printf("\nПересечение: невозможно построить.\n"); break; }
            printf("\nПересечение G1 и G2:\n");
            printMatrix(newN_inter, R);
            break;
        }

        case 8: {
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            int newN_ring = 0;
            ringSumGraphs(n1, M1, n2, M2, R, &newN_ring);
            if (newN_ring == 0)
                printf("\nКольцевая сумма: после удаления изолированных вершин граф пуст.\n");
            else {
                printf("\nКольцевая сумма G1 и G2:\n");
                printMatrix(newN_ring, R);
            }
            break;
        }

        case 9: {
            if (!created) { printf("Сначала создайте графы!\n"); break; }
            int nCart = n1 * n2;
            if (nCart > MAX) {
                printf("\nДекартово произведение невозможно.\n");
                break;
            }
            int newN_cart = 0;
            cartesianProduct(n1, M1, n2, M2, R, &newN_cart);
            printf("\nДекартово произведение G1 x G2:\n");
            printMatrix(newN_cart, R);
            break;
        }

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
