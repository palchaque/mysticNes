#ifndef INTERRUPTS_H
#define INTERRUPTS_H


namespace mysticNes {

    class Interrupts
    {
    public:
        Interrupts()
        {
            nmiOccurred = false;
            resetOccurred = false;
            brkOccurred = false;
        };
        void setNmiOccurred(bool value);
        bool isNmiOccurred();
    private:
        bool nmiOccurred;
        bool resetOccurred;
        bool brkOccurred;
    };


}

#endif // INTERRUPTS_H
