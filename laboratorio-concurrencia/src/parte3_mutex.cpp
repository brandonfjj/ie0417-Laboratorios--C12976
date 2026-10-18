#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Contador global compartido por todos los hilos
int contador = 0;

// Mutex que protege el acceso a contador
std::mutex mtx;

// Cada hilo suma 1 al contador la cantidad de veces indicada.
// El lock_guard bloquea el mutex al crearse y lo libera al terminar
// cada vuelta del ciclo, así solo un hilo incrementa a la vez.
void incrementar(int repeticiones) {
    for (int i = 0; i < repeticiones; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
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

    // Con el mutex ambos valores deberían coincidir
    std::cout << "Valor esperado: "
              << cantidad_hilos * repeticiones
              << std::endl;
    std::cout << "Valor obtenido: "
              << contador
              << std::endl;

    return 0;
}
