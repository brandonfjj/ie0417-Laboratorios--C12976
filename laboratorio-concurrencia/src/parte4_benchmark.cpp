#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

// Suma todos los elementos del vector en un solo hilo
long long suma_secuencial(const std::vector<int>& datos) {
    long long suma = 0;
    for (int valor : datos) {
        suma += valor;
    }
    return suma;
}

// Suma solo el bloque [inicio, fin) del vector y guarda el resultado
// en la variable que recibe por referencia
void suma_parcial(
    const std::vector<int>& datos,
    size_t inicio,
    size_t fin,
    long long& resultado
) {
    long long suma = 0;
    for (size_t i = inicio; i < fin; ++i) {
        suma += datos[i];
    }
    resultado = suma;
}

// Divide el vector en bloques, suma cada bloque en un hilo distinto
// y al final junta las sumas parciales
long long suma_paralela(const std::vector<int>& datos, int cantidad_hilos) {
    std::vector<std::thread> hilos;

    // Cada hilo escribe en su propia posición, así no comparten variable
    std::vector<long long> resultados(cantidad_hilos, 0);

    size_t tamano = datos.size();
    size_t bloque = tamano / cantidad_hilos;

    for (int i = 0; i < cantidad_hilos; ++i) {
        size_t inicio = i * bloque;
        // El último hilo toma también los elementos que sobran de la división
        size_t fin = (i == cantidad_hilos - 1) ? tamano : inicio + bloque;

        // std::cref y std::ref evitan que el hilo copie el vector o el resultado
        hilos.emplace_back(
            suma_parcial,
            std::cref(datos),
            inicio,
            fin,
            std::ref(resultados[i])
        );
    }

    // Se espera a que todos los hilos terminen su bloque
    for (auto& hilo : hilos) {
        hilo.join();
    }

    // Se combinan las sumas parciales
    long long suma_total = 0;
    for (long long parcial : resultados) {
        suma_total += parcial;
    }
    return suma_total;
}

int main() {
    // Tamaño del vector y cantidad de hilos para la versión paralela
    const size_t cantidad_elementos = 100000000;
    const int cantidad_hilos = 4;

    // Vector lleno de unos, así la suma esperada es igual a la cantidad de elementos
    std::vector<int> datos(cantidad_elementos, 1);

    // Medición de la versión secuencial
    auto inicio_sec = std::chrono::high_resolution_clock::now();
    long long resultado_sec = suma_secuencial(datos);
    auto fin_sec = std::chrono::high_resolution_clock::now();
    auto duracion_sec = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_sec - inicio_sec
    ).count();

    // Medición de la versión paralela
    auto inicio_par = std::chrono::high_resolution_clock::now();
    long long resultado_par = suma_paralela(datos, cantidad_hilos);
    auto fin_par = std::chrono::high_resolution_clock::now();
    auto duracion_par = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_par - inicio_par
    ).count();

    std::cout << "Resultado secuencial: " << resultado_sec << std::endl;
    std::cout << "Tiempo secuencial: " << duracion_sec << " ms" << std::endl;
    std::cout << "Resultado paralelo: " << resultado_par << std::endl;
    std::cout << "Tiempo paralelo: " << duracion_par << " ms" << std::endl;

    return 0;
}
