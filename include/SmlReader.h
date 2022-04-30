#include "sml.h"

#define MAX_STR_MANUF 5

class SmlReader
{
public:

        SmlReader();

        void HandleByte (unsigned char currentChar);

protected:

    sml_states_t m_curState;   
    
    unsigned char m_manuf[MAX_STR_MANUF];
    double m_T1Wh = -2;
    double m_T2Wh = -2;
    double m_SumWh = -2;
    double m_CurW = -2;

    typedef struct
    {
        const unsigned char OBIS[6];
        void (*Handler)();
    } OBISHandler;

    OBISHandler OBISHandlers[6];

    void Manufacturer()
    {
        smlOBISManufacturer(m_manuf, MAX_STR_MANUF);
    }

    void PowerT1()
    {
        smlOBISWh(m_T1Wh);
    }

    void PowerT2()
    {
        smlOBISWh(m_T2Wh);
    }

    void PowerSum()
    {
        smlOBISWh(m_SumWh);
    }

    void EnergyCur()
    {
        smlOBISW(m_CurW);
    }

};