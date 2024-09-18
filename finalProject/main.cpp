#include "Ethernet/Ethernet.hpp"
int main() {
    
    Ethernet eth("../Ethernet/EthernetConfig.txt");
    eth.generate();
    return 0;
}