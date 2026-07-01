#include <gangod/tech.h>
//GAMEPAD
using namespace getch::gamepad;

Ganpad::Ganpad() {
}

bool Ganpad::Open(int id) {
    if (!SDL_HasGamepad()) {
        std::cerr << "Gamepad isn't connected." << std::endl;
        return false;
    }

    int count = 0;
    gamepads = SDL_GetGamepads(&count);
    if (gamepads && count > id) {
        current = SDL_OpenGamepad(gamepads[id]);
        return current != nullptr;
    } else {
        std::cerr << "Gamepad isn't reachable." << std::endl;
        return false;
    }
}

Ganpad::~Ganpad() {
    if (current) {
        SDL_CloseGamepad(current);
    }
    if (gamepads) {
        SDL_free(gamepads);
    }
}

bool Ganpad::GetButton(GamepadButton button){
    return SDL_GetGamepadButton(current, static_cast<SDL_GamepadButton>(button));
}

bool Ganpad::GetButtonDown(GamepadButton button){
    return SDL_GetGamepadButton(current, static_cast<SDL_GamepadButton>(button));
}

gath::v2d Ganpad::GetCurrentAxis(){
    Sint16 left_x = SDL_GetGamepadAxis(current, SDL_GAMEPAD_AXIS_LEFTX);
        Sint16 left_y = SDL_GetGamepadAxis(current, SDL_GAMEPAD_AXIS_LEFTY);

        // 2. Filter Thumbsticks with Dead Zone
        if (SDL_abs(left_x) < DEAD_ZONE) left_x = 0;
        if (SDL_abs(left_y) < DEAD_ZONE) left_y = 0;

        return {
            left_x, left_y
        };
}
gath::v2d Ganpad::GetCurrentRightAxis(){
    Sint16 right_x = SDL_GetGamepadAxis(current, SDL_GAMEPAD_AXIS_RIGHTX);
    Sint16 right_y = SDL_GetGamepadAxis(current, SDL_GAMEPAD_AXIS_RIGHTY);

    // 2. Filter Thumbsticks with Dead Zone
    if (SDL_abs(right_x) < DEAD_ZONE) right_x = 0;
    if (SDL_abs(right_y) < DEAD_ZONE) right_y = 0;

    return {
        right_x, right_y
    };
}
Ganpad getch::gamepad::current;

void getch::gamepad::Init(){
    current.Open(0);
}
