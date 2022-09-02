#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define KEYSIZE 2
/*
Використовуємо алфавіт з ASCII таблиці, де 97 елемент це нульовий у нашому алфавіті та відповідає за букву a, 98 = b, 99 = c...
Індексація алфавіту починається з 0
*/
char encryptedText[] = "cvtt";
int alphSize = 26;

int keyMatrix[2][2] = {
    {2, 5},
    {1, 3},
};
int countOfCols = sizeof(encryptedText) / sizeof(encryptedText[0]) / KEYSIZE;
int textLength = sizeof(encryptedText) / sizeof(encryptedText[0]) - 1;

// Отримання матриці з зашифрованого тексту
void getTextMatrix(char text[], int matrix[KEYSIZE][(sizeof(encryptedText) / sizeof(encryptedText[0])) / KEYSIZE]) {
    int k = 0;
    for (int i = 0; i < countOfCols; i++)
    {
        for (int j = 0; j < KEYSIZE; j++)
        {
            matrix[j][i] = text[k] % 97;
            k++;
        }
    }
}

int det2(int a[2][2]) {
    int num1 = a[0][0] * a[1][1];
    int num2 = -a[0][1] * a[1][0];
    return num1 + num2;
}

// Обчислення оберненого за модулем числа(тільки для додатніх)
int modInverse(int a, int m)
{
    for (int x = 1; x < m; x++)
        if (((a % m) * (x % m)) % m == 1)
            return x;
}
// Множення матриці на число
void multiplyMatrixByNumber(int** matrix, int num) {
    for (int i = 0; i < KEYSIZE; i++) {
        for (int j = 0; j < KEYSIZE; j++) {
            matrix[i][j] = matrix[i][j] * num;
        }
    }
}
// Транспонування матриці
int** transpose(int** matrix) {
    int** transposed = calloc(2, sizeof(int*));
    for (int i = 0; i < 2; i++) {
        transposed[i] = calloc(2, sizeof(int));
    }
    for (int i = 0; i < KEYSIZE; i++) {
        for (int j = 0; j < KEYSIZE; j++) {
            transposed[i][j] = matrix[j][i];
        }
    }
    return transposed;
}
// Знаходження оберненої матриці
// Якщо визначник > 0, то використовуємо метод знаходження оберненого за модулем числа
// Якщо визначник < 0, тоді множимо 1/det на матрицю алгебраїчних доповнень, 
int** addition(int matrix[2][2])
{
    int** result = calloc(2, sizeof(int*));
    for (int i = 0; i < 2; i++) {
        result[i] = calloc(2, sizeof(int));
    }

    int det = det2(keyMatrix);

    if (det > 0) {
        int** alg_dop = calloc(2, sizeof(int*));
        for (int i = 0; i < 2; i++) {
            alg_dop[i] = calloc(2, sizeof(int));
        }

        alg_dop[0][0] = matrix[1][1];
        alg_dop[0][1] = -matrix[1][0] + alphSize;
        alg_dop[1][0] = -matrix[0][1] + alphSize;
        alg_dop[1][1] = matrix[0][0];

        int multiplier = modInverse(det, alphSize);

        multiplyMatrixByNumber(alg_dop, multiplier);

        result = alg_dop;
    }
    else {
        float** alg_dop = calloc(2, sizeof(float*));
        for (int i = 0; i < 2; i++) {
            alg_dop[i] = calloc(2, sizeof(float));
        }
        alg_dop[0][0] = (float)(matrix[1][1]) / (float)(det2(keyMatrix));
        alg_dop[0][1] = (float)(-matrix[1][0]) / (float)(det2(keyMatrix));
        alg_dop[1][0] = (float) (-matrix[0][1]) / (float)(det2(keyMatrix));
        alg_dop[1][1] = (float)(matrix[0][0]) / (float)(det2(keyMatrix));
        
        for (int i = 0; i < KEYSIZE; i++) {
            for (int j = 0; j < KEYSIZE; j++) {
                if (alg_dop[i][j] < 0 && roundf(alg_dop[i][j]) == alg_dop[i][j])
                    for (int k = 1;; k++)
                    {
                        alg_dop[i][j] = alg_dop[i][j] + alphSize;
                        if (alg_dop[i][j] > 0)
                            break;
                    }

                else if (roundf(alg_dop[i][j]) != alg_dop[i][j]) {
                    for (int k = 1;; k++)
                    {
                        if (alg_dop[i][j] > 0)
                            alg_dop[i][j] = (alg_dop[i][j] * det - alphSize * k) / det;
                        else
                            alg_dop[i][j] = (alg_dop[i][j] * det + alphSize * k) / det + alphSize;
                        if (roundf(alg_dop[i][j]) == alg_dop[i][j]) {
                            break;
                        }
                    }
                }
            }
        }
        for (int i = 0; i < KEYSIZE; i++) {
            for (int j = 0; j < KEYSIZE; j++) {
                result[i][j] = (int)alg_dop[i][j];
            }
        }
    }

    result = transpose(result);
    
    return result;
}

// Ділення за модулем кожного елементу матриці
void divideByAlphSize(int** matrix) {
    for (int i = 0; i < KEYSIZE; i++) {
        for (int j = 0; j < KEYSIZE; j++) {
            matrix[i][j] = matrix[i][j] % alphSize;
        }
    }
}
// Знаходження результату множенння рядка першої матриці на стопвчик другої
int multRowCol(int text[KEYSIZE][1], int** key, int row) {
    int result = 0;
    for (int i = 0; i < KEYSIZE; i++) {
        result += text[i][0] * key[row][i];
    }
    return result;
}
// Множення матриць та вивід тексту
void multiplyMatrixes(int text[KEYSIZE][(sizeof(encryptedText) / sizeof(encryptedText[0])) / KEYSIZE], int** key) {
    int result[KEYSIZE][1];

    for (int t = 0; t < sizeof(encryptedText) / sizeof(encryptedText[0]) / KEYSIZE; t++)
    {
        int currentRow[KEYSIZE][1];
        currentRow[0][0] = text[0][t];
        currentRow[1][0] = text[1][t];
        for (int i = 0; i < KEYSIZE; i++) {
            result[i][0] = multRowCol(currentRow, key, i);
        }

        
        for (int i = 0; i < KEYSIZE; i++) {
                result[i][0] = result[i][0] % alphSize;
        }
        for (int i = 0; i < KEYSIZE; i++) {
            if (textLength % 2 != 0 && t == countOfCols - 1 && i == KEYSIZE - 1)
                break;
            printf("%c", (char)(result[i][0] + 97));
        }
    }

}



int main() {
    int det = det2(keyMatrix);
    if (det == 0) {
        printf("Determinant = 0, decoding is impossible");
        return 0;
    }
    int** inversedMatrix;
    inversedMatrix = addition(keyMatrix);
    divideByAlphSize(inversedMatrix);
    printf("Inverse matrix\n");
    for (int i = 0; i < KEYSIZE; i++) {
        for (int j = 0; j < KEYSIZE; j++) {
            printf(" %d", inversedMatrix[i][j]);
        }
        printf("\n");
    }
    int textMatrix[KEYSIZE][sizeof(encryptedText) / sizeof(encryptedText[0]) / KEYSIZE];
    getTextMatrix(encryptedText, textMatrix);
    printf("Decrypted message\n");
    multiplyMatrixes(textMatrix, inversedMatrix);
    // Звільнення пам'яті
    for (int i = 0; i < KEYSIZE; i++) {
        free(inversedMatrix[i]);
    }
    free(inversedMatrix);

    return 0;
}
