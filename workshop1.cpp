#include <iostream>
#include <vector>
#include <cmath>

class base
{
   public:
   void printFrame(int a, int b , int c , std::vector<char>& d);
   void printFlex(int a , int b ,int c,int d , int e , std::vector<char>& f, int g);
   void calculateM_ref(int x, double z);

   protected:
   constexpr static double TDDFrame = 20;
    constexpr static int NoSym = 14;
   int subcarrierSpacing;
   void setM_ref(int m_ref) {
       M_ref = m_ref;
   }
   int getM_ref() const {
       return M_ref;
   }
   private:
    int M_ref;
   
};

class pattern1 : public base
{
    public:
    void setPeriodicity(double periodicity) {
        this->periodicity = periodicity;
    }
    double getPeriodicity() const {
        return periodicity;
    }
    void setNumberOfSlots(int numberOfSlots) {
        this->numberOfSlots = numberOfSlots;
    }
    int getNumberOfSlots() const {
        return numberOfSlots;
    }
    void setUpLinkSlots(int upLinkSlots) {
        UpLinkSlots = upLinkSlots;
    }
    int getUpLinkSlots() const {
        return UpLinkSlots;
    }
    void setDownLinkSlots(int downLinkSlots) {
        DownLinkSlots = downLinkSlots;
    }
    int getDownLinkSlots() const {
        return DownLinkSlots;
    }
    void setUpLinkSym(int upLinkSym) {
        UpLinkSym = upLinkSym;
    }
    int getUpLinkSym() const {
        return UpLinkSym;
    }
    void setDownLinkSym(int downLinkSym) {
        DownLinkSym = downLinkSym;
    }
    int getDownLinkSym() const {
        return DownLinkSym;
    }
    std::vector<char>& getFrame() {
        return Frame;
    }
    std::vector<char>& getFlexSymbols() {
        return flexSymbols;
    }
    void setNumberOfFlexSlots(int numberOfFlexSlots) {
        this->numberOfFlexSlots = numberOfFlexSlots;
    }
    int getNumberOfFlexSlots() const {
        return numberOfFlexSlots;
    }

    void calculateNumberOfSlots();

    private:
    double periodicity;
    int numberOfSlots;
    int numberOfFlexSlots;
    int UpLinkSlots;
    int DownLinkSlots;
    int UpLinkSym;
    int DownLinkSym;
    std::vector<char> Frame;       // Vector to store frame structure (D for downlink, U for uplink, F for flexible)
    std::vector<char> flexSymbols; // Vector to store flexible symbols structure (D, U, F)
};

class pattern2 : public base
{
   public:
   void setPeriodicity(double periodicity) {
       periodicity_2 = periodicity;
   }
   double getPeriodicity() const {
       return periodicity_2;
   }
   void setNumberOfSlots(int numberOfSlots) {
       numberOfSlots_2 = numberOfSlots;
   }
   int getNumberOfSlots() const {
       return numberOfSlots_2;
   }
   void setUpLinkSlots(int upLinkSlots) {
       UpLinkSlots_2 = upLinkSlots;
   }
   int getUpLinkSlots() const {
       return UpLinkSlots_2;
   }
   void setDownLinkSlots(int downLinkSlots) {
       DownLinkSlots_2 = downLinkSlots;
   }
   int getDownLinkSlots() const {
       return DownLinkSlots_2;
   }
   void setUpLinkSym(int upLinkSym) {
       UpLinkSym_2 = upLinkSym;
   }
   int getUpLinkSym() const {
       return UpLinkSym_2;
   }
   void setDownLinkSym(int downLinkSym) {
       DownLinkSym_2 = downLinkSym;
   }
   int getDownLinkSym() const {
       return DownLinkSym_2;
   }
   std::vector<char>& getFrame() {
       return Frame;
   }
   std::vector<char>& getFlexSymbols() {
       return flexSymbols;
   }
   void setNumberOfFlexSlots(int numberOfFlexSlots) {
       numberOfFlexSlots_2 = numberOfFlexSlots;
   }
   int getNumberOfFlexSlots() const {
       return numberOfFlexSlots_2;
   }

   void calculateNumberOfSlots_2(double y);

   private:
   double periodicity_2;
   int numberOfSlots_2;
   int UpLinkSlots_2;
   int DownLinkSlots_2;
   int UpLinkSym_2;
   int DownLinkSym_2;
   int numberOfFlexSlots_2;
   std::vector<char> Frame;       // Vector to store frame structure (D for downlink, U for uplink, F for flexible)
   std::vector<char> flexSymbols; // Vector to store flexible symbols structure (D, U, F)
};

// calculateM_ref function used to get the corresponding M_ref value of sub-carrier spacing given from user
void base::calculateM_ref(int x, double z)
{
    switch (x) {
        case 30:
            if (z == 2.5)
                setM_ref(1);
            else
                std::cout << "Invalid sub-carrier spacing!\n";
            break;
        case 60:
            if (z == 1.25 || z == 2.5)
                setM_ref(2);
            else
                std::cout << "Invalid sub-carrier spacing!\n";
            break;
        case 120:
            if (z == 0.625 || z == 1.25 || z == 2.5)
                setM_ref(3);
            else
                std::cout << "Invalid sub-carrier spacing!\n";
            break;
        default:
            std::cout << "Invalid sub-carrier spacing!\n";
    }
}

void base::printFrame(int a, int b, int c, std::vector<char>& d)
{
    if (a + b <= c)
    {
        d.resize(c);  // Resize the Frame vector to match the number of slots
        for (int i = 0; i < b; i++)
        {
            d[i] = 'D';
        }
        for (int i = b; i < c - a; i++)
        {
            d[i] = 'F';
        }
        for (int i = c - a; i < c; i++)
        {
            d[i] = 'U';
        }

        std::cout << "Frame: ";
        for (int i = 0; i < c; i++) {
            std::cout << d[i] << " ";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "invalid number for uplink and downlink slots " << "\n";
    }
}

void base::printFlex(int a, int b, int c, int d, int e, std::vector<char>& f, int g)
{
    int flexSym = (a - b - c) * NoSym - d - e; // a= No. of slots , b= uplink slots , c= downlink slots , d= uplink sym , e= downlink sym , g= no. of flex. slots
    if (d + e <= a * NoSym)
    {
        f.resize(g * NoSym);  // Resize the flexSymbols vector to match the number of flexible symbols
        for (int i = 0; i < e; i++)
        {
            f[i] = 'D';
        }
        for (int i = e; i < g * NoSym - d; i++)
        {
            f[i] = 'F';
        }
        for (int i = g * NoSym - d; i < g * NoSym; i++)
        {
            f[i] = 'U';
        }

        // Print the flexible symbols structure
        std::cout << "Flexible symbols: ";
        for (int i = 0; i < g * NoSym; i++) {
            std::cout << f[i] << " ";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "invalid uplink and downlink numbers\n";
    }

}

// the function checks the validation (20/P) then calculates the no. of slots from S = P * 2^μ_ref
void pattern1::calculateNumberOfSlots() {
    if ((fmod(TDDFrame, getPeriodicity()) == 0.0))
    {
        setNumberOfSlots(getPeriodicity() * pow(2, getM_ref()));
        std::cout << "Number of slots (S): " << getNumberOfSlots() << "\n";
    }
    else
    {
        std::cout << "Invalid periodicity!" << "\n";
    }

}

void pattern2::calculateNumberOfSlots_2(double y) {
    if (fmod(TDDFrame, y) == 0.0)
    {
;        setNumberOfSlots(getPeriodicity() * pow(2, getM_ref())); 
        std::cout << "Number of slots (S_2): " << getNumberOfSlots() << "\n";
    }
    else
    {
        std::cout << "Invalid periodicity!" << "\n";
    }
}

int main()
{
    pattern1 first;
    pattern2 second;

    double periodicity1, periodicity2;
    int subcarrierSpacing;
    int downLinkSlots, upLinkSlots;
    int downLinkSym, upLinkSym;

    std::cout << "For pattern 1, Enter periodicity (P) in msec (0.625 or 1.25 or 2.5): ";
    std::cin >> periodicity1;
    first.setPeriodicity(periodicity1);

    std::cout << "Enter sub-carrier spacing in KHz (30, 60, 120): ";
    std::cin >> subcarrierSpacing;

    first.calculateM_ref(subcarrierSpacing, first.getPeriodicity());
    first.calculateNumberOfSlots();

    std::cout << "Enter downlink number of slots: ";
    std::cin >> downLinkSlots;
    first.setDownLinkSlots(downLinkSlots);

    std::cout << "Enter uplink number of slots: ";
    std::cin >> upLinkSlots;
    first.setUpLinkSlots(upLinkSlots);

    first.printFrame(first.getUpLinkSlots(), first.getDownLinkSlots(), first.getNumberOfSlots(), first.getFrame());
    first.setNumberOfFlexSlots(first.getNumberOfSlots() - (first.getUpLinkSlots() + first.getDownLinkSlots()));

    std::cout << "Enter downlink number of symbols: ";
    std::cin >> downLinkSym;
    first.setDownLinkSym(downLinkSym);

    std::cout << "Enter uplink number of symbols: ";
    std::cin >> upLinkSym;
    first.setUpLinkSym(upLinkSym);

    first.printFlex(first.getNumberOfSlots(), first.getUpLinkSlots(), first.getDownLinkSlots(),
                    first.getUpLinkSym(), first.getDownLinkSym(), first.getFlexSymbols(), first.getNumberOfFlexSlots());

    std::cout << "For pattern 2, Enter periodicity (P_2) in msec (0.625 or 1.25 or 2.5): ";
    std::cin >> periodicity2;
    second.setPeriodicity(periodicity2);

    double slot_config_period = periodicity1 + periodicity2;
    second.calculateM_ref(subcarrierSpacing, second.getPeriodicity());
    second.calculateNumberOfSlots_2(slot_config_period);

    std::cout << "Enter downlink number of slots: ";
    std::cin >> downLinkSlots;
    second.setDownLinkSlots(downLinkSlots);

    std::cout << "Enter uplink number of slots: ";
    std::cin >> upLinkSlots;
    second.setUpLinkSlots(upLinkSlots);

    second.printFrame(second.getUpLinkSlots(), second.getDownLinkSlots(), second.getNumberOfSlots(), second.getFrame());
    second.setNumberOfFlexSlots(second.getNumberOfSlots() - (second.getUpLinkSlots() + second.getDownLinkSlots()));

    std::cout << "Enter downlink number of symbols: ";
    std::cin >> downLinkSym;
    second.setDownLinkSym(downLinkSym);

    std::cout << "Enter uplink number of symbols: ";
    std::cin >> upLinkSym;
    second.setUpLinkSym(upLinkSym);

    second.printFlex(second.getNumberOfSlots(), second.getUpLinkSlots(), second.getDownLinkSlots(),
                     second.getUpLinkSym(), second.getDownLinkSym(), second.getFlexSymbols(), second.getNumberOfFlexSlots());

    std::cout << "Pattern 1 + Pattern 2 :" << std::endl;
    for (int i = 0; i < first.getNumberOfSlots(); i++)
    {
        std::cout << first.getFrame()[i] << " ";
    }
    for (int i = 0; i < second.getNumberOfSlots(); i++)
    {
        std::cout << second.getFrame()[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
