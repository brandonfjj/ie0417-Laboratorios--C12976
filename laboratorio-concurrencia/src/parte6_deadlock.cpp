#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

// Dos recursos compartidos, cada uno protegido por su propio mutex
std::mutex recurso1;
std::mutex recurso2;

// El hilo A toma primero el recurso 1 y después el recurso 2
void hilo_a() {
    std::cout << "Hilo A intentando tomar recurso 1..." << std::endl;
    recurso1.lock();

    // La pausa le da tiempo al hilo B de tomar el recurso 2
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Hilo A intentando tomar recurso 2..." << std::endl;
    recurso2.lock();

    std::cout << "Hilo A obtuvo ambos recursos." << std::endl;
    recurso2.unlock();
    recurso1.unlock();
}

// El hilo B toma los recursos en el orden contrario: primero el 2 y después el 1
void hilo_b() {
    std::cout << "Hilo B intentando tomar recurso 2..." << std::endl;
    recurso2.lock();

    // La pausa le da tiempo al hilo A de tomar el recurso 1
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Hilo B intentando tomar recurso 1..." << std::endl;
    recurso1.lock();

    std::cout << "Hilo B obtuvo ambos recursos." << std::endl;
    recurso1.unlock();
    recurso2.unlock();
}

int main() {
    // Se lanzan los dos hilos, que terminan esperándose mutuamente
    std::thread a(hilo_a);
    std::thread b(hilo_b);

    a.join();
    b.join();

    return 0;
}
