#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// Los mismos dos recursos de la versión con deadlock
std::mutex recurso1;
std::mutex recurso2;

// std::scoped_lock toma ambos mutex a la vez usando un mecanismo que evita
// el deadlock, y los libera automáticamente al salir de la función
void tarea(const std::string& nombre) {
    std::scoped_lock lock(recurso1, recurso2);
    std::cout << nombre << " obtuvo ambos recursos de forma segura." << std::endl;
}

int main() {
    // Los dos hilos piden los mismos recursos, pero ya no se bloquean
    std::thread a(tarea, "Hilo A");
    std::thread b(tarea, "Hilo B");

    a.join();
    b.join();

    return 0;
}
