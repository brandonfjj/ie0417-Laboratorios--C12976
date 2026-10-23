#include <iostream>

int obtener_mayor(int* datos, int tamano) {
    // Se parte del primer elemento, no de 0, para que funcione con negativos
    int mayor = datos[0];

    // El índice llega hasta tamano - 1, así no se sale del arreglo
    for (int i = 1; i < tamano; i++) {
        if (datos[i] > mayor) {
            mayor = datos[i];
        }
    }

    return mayor;
}

double calcular_promedio(int* datos, int tamano) {
    int suma = 0;

    for (int i = 0; i < tamano; i++) {
        suma += datos[i];
    }

    // Se divide entre la cantidad de datos, convirtiendo a double para no truncar
    return static_cast<double>(suma) / tamano;
}

int main() {
    int tamano = 5;
    int* datos = new int[tamano];

    datos[0] = -10;
    datos[1] = -20;
    datos[2] = -5;
    datos[3] = -30;
    datos[4] = -15;

    int mayor = obtener_mayor(datos, tamano);
    double promedio = calcular_promedio(datos, tamano);

    std::cout << "Mayor: " << mayor << std::endl;
    std::cout << "Promedio: " << promedio << std::endl;

    // Se libera el arreglo reservado con new[]
    delete[] datos;

    return 0;
}
