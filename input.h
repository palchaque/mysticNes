#ifndef INPUT_H
#define INPUT_H


#include <SDL.h>
#include <cstdint>
#include <iostream>
#include <bitset>
#include <map>

namespace mysticNes {

    class Input
    {
    public:
        Input()
        {
            controller1Enabled = false;
            controller2Enabled = false;
            controller1PollInput = 0;
            controller2PollInput = 0;
        };

        uint8_t getCurrentController1State();
        uint8_t getCurrentController2State();

        void enableDisableControllers(uint8_t value);

        const uint16_t controller1Address = 0x4016;
        const uint16_t controller2Address = 0x4017;

    private:
        int controller1PollInput;
        int controller2PollInput;
        bool controller1Enabled;
        bool controller2Enabled;
    };

}

#endif // INPUT_H
