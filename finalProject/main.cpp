#include "Ethernet/Ethernet.hpp"
int main() {
    
    Ethernet eth("../Ethernet/EthernetConfig.txt");
    eth.print();

    return 0;
}