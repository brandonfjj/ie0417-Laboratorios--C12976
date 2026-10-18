#include <iostream>
#include <thread>
#include <vector>

// Contador global compartido por todos los hilos, sin ninguna protección
int contador = 0;

// Cada hilo suma 1 al contador la cantidad de veces indicada.
// Como contador++ no es atómico, los hilos se pueden pisar entre sí.
void incrementar(int repeticiones) {
    for (int i = 0; i < repeticiones; ++i) {
        contador++;
    }
}

int main() {
    // Cantidad de hilos y de incrementos que hace cada uno
    const int cantidad_hilos = 4;
    const int repeticiones = 1000000;

    // Vector para guardar los hilos y poder esperarlos después
    std::vector<std::thread> hilos;

    // Se crean todos los hilos, cada uno ejecuta incrementar
    for (int i = 0; i < cantidad_hilos; ++i) {
        hilos.emplace_back(incrementar, repeticiones);
    }

    // Se espera a que todos los hilos terminen antes de leer el contador
    for (auto& hilo : hilos) {
        hilo.join();
    }

    // Si no hubiera condición de carrera, ambos valores serían iguales
    std::cout << "Valor esperado: "
              << cantidad_hilos * repeticiones
              << std::endl;
    std::cout << "Valor obtenido: "
              << contador
              << std::endl;

    return 0;
}
