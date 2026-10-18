#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

// Suma solo el bloque [inicio, fin) del vector y guarda el resultado
// en la variable que recibe por referencia (igual que en la parte 4)
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
// y al final junta las sumas parciales (igual que en la parte 4)
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
    // Tamaño del vector, lleno de unos
    const size_t cantidad_elementos = 100000000;
    std::vector<int> datos(cantidad_elementos, 1);

    // Cantidades de hilos que se van a probar
    std::vector<int> pruebas = {1, 2, 4, 8, 16};

    for (int cantidad_hilos : pruebas) {
        // Se mide cuánto tarda la suma paralela con esta cantidad de hilos
        auto inicio = std::chrono::high_resolution_clock::now();
        long long resultado = suma_paralela(datos, cantidad_hilos);
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(
            fin - inicio
        ).count();

        std::cout << "Hilos: " << cantidad_hilos
                  << " | Resultado: " << resultado
                  << " | Tiempo: " << duracion << " ms"
                  << std::endl;
    }

    return 0;
}
