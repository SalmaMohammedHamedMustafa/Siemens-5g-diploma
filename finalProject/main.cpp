#include "Ethernet/Ethernet.hpp"
int main() {
    
    Ethernet eth("../Ethernet/EthernetConfig.txt");
    eth.generateFixed();
    eth.print();

    return 0;
}