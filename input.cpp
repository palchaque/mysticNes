#include "input.h"

using namespace mysticNes;

void Input::enableDisableControllers(uint8_t value)
{

    bool controllersEnabled = (bool)value & 0x01;

    if(controllersEnabled)
    {
        controller1Enabled = true;
        //controller1PollInput = 0;
        controller2Enabled = true;
        //controller2PollInput = 0;
    }

}

uint8_t Input::getCurrentController1State()
{
    if(!controller1Enabled)
    {
        return 0;
    }

    SDL_PumpEvents();

    uint8_t* keys = (uint8_t*)SDL_GetKeyboardState(NULL);

    uint8_t keyboardValue = 0;
    switch(controller1PollInput)
    {
    case(0):
        keyboardValue = keys[SDL_SCANCODE_L];
        break;
    case(1):
        keyboardValue = keys[SDL_SCANCODE_K];
        break;
    case(2):
        keyboardValue = keys[SDL_SCANCODE_LCTRL];
        break;
    case(3):
        keyboardValue = keys[SDL_SCANCODE_LSHIFT];
        break;
    case(4):
        keyboardValue = keys[SDL_SCANCODE_W];
        break;
    case(5):
        keyboardValue = keys[SDL_SCANCODE_S];
        break;
    case(6):
        keyboardValue = keys[SDL_SCANCODE_A];
        break;
    case(7):
        keyboardValue = keys[SDL_SCANCODE_D];
        break;
    }

    controller1PollInput++;

    if(controller1PollInput > 7) controller1PollInput = 0;

    return keyboardValue | 0x40;
}


uint8_t Input::getCurrentController2State()
{
    if(controller2Enabled)
    {

    }
}
