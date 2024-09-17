#include "Ethernet/Ethernet.hpp"
int main() {
    
    Ethernet eth("../Ethernet/EthernetConfig.txt");
    eth.generateFixed();
    return 0;
}