#include <iostream>
#include <thread>
#include <vector>

// Función que ejecuta cada hilo. Recibe el identificador lógico del hilo
// e imprime un mensaje indicando cuál hilo se está ejecutando.
void saludar(int id) {
    std::cout << "Hola, soy el hilo " << id << " y estoy en ejecución." << std::endl;
}

int main() {
    // Cantidad de hilos que se van a crear
    const int cantidad_hilos = 5;

    // Vector para guardar los hilos y poder esperarlos después
    std::vector<std::thread> hilos;

    // Se crean los hilos, cada uno ejecuta saludar con su propio id
    for (int i = 0; i < cantidad_hilos; ++i) {
        hilos.emplace_back(saludar, i);
    }

    // El programa principal espera a que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }

    std::cout << "Todos los hilos finalizaron." << std::endl;
    return 0;
}