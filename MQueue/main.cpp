#include "broker/Broker.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        // Initialize broker with storage path for persistence
        Broker broker("./MessageStore");

        // Start broker on port 8080
        std::cout << "Starting message queue broker..." << std::endl;
        broker.start(8080);
    } catch (const std::exception& e) {
        std::cerr << "Broker failed to start: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Broker failed due to an unknown error." << std::endl;
        return 1;
    }
    return 0;
}