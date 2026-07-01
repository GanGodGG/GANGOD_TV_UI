#pragma once 
#include <iostream>
#include <string>
#include <gangod/gath.h>
#include <SDL3/SDL.h>
#include <cstdint>
namespace getch{
    
    namespace gamepad{
        
        enum class GamepadButton : std::uint8_t  {
            None = 99,
            
            // Face buttons
            A = 0,             // Cross / Bottom (SDL_GAMEPAD_BUTTON_SOUTH)
            B = 1,             // Circle / Right (SDL_GAMEPAD_BUTTON_EAST)
            X = 2,             // Square / Left  (SDL_GAMEPAD_BUTTON_WEST)
            Y = 3,             // Triangle / Top (SDL_GAMEPAD_BUTTON_NORTH)
            
            // Menu / Utility buttons
            Back = 4,          // View / Share / Select
            Guide = 5,         // Xbox Button / PS Button / Home
            Start = 6,         // Menu / Options
            
            // Thumbstick clicks 
            LeftThumb = 7,     // LS / L3
            RightThumb = 8,    // RS / R3
            
            // Bumpers / Shoulder buttons
            LeftBumper = 9,    // LB / L1
            RightBumper = 10,  // RB / R1
            
            // Directional Pad (D-Pad)
            DPadUp = 11,
            DPadDown = 12,
            DPadLeft = 13,
            DPadRight = 14,
            
            // Modern controllers extensions
            Misc1 = 15,         // Share (Xbox), Mic (PS5), Capture (Switch)
            RightPaddle1 = 16,  // (Xbox Elite P1, DualSense Edge RB)
            LeftPaddle1 = 17,   // (Xbox Elite P3, DualSense Edge LB)
            RightPaddle2 = 18,  // (Xbox Elite P2, DualSense Edge Fn)
            LeftPaddle2 = 19,   // (Xbox Elite P4, DualSense Edge Fn)
            Touchpad = 20,      // (PS4/PS5)
            
            // Additional Misc buttons
            Misc2 = 21,
            Misc3 = 22,   
            Misc4 = 23,      
            Misc5 = 24,
            Misc6 = 25,
    
            Count = 26 // valid buttons
        };
        enum class GamepadAxis : std::uint8_t {
            // Left Thumbstick
            LeftX = 0,        // Horizontal (-1.0 to 1.0)
            LeftY = 1,        // Vertical (-1.0 to 1.0)
            
            // Right Thumbstick
            RightX = 2,       // Horizontal (-1.0 to 1.0)
            RightY = 3,       // Vertical (-1.0 to 1.0)
            
            // Triggers
            LeftTrigger = 4,  // LT / L2 (0.0 to 1.0)
            RightTrigger = 5  // RT / R2 (0.0 to 1.0)
        };
        const int DEAD_ZONE = 8000;
        
        class Ganpad {
            private:
            // current loaded gamepad!
            SDL_Gamepad* current;
            SDL_JoystickID *gamepads;
            int lastNavigationTime = 0;
            public:
            bool GetButton(GamepadButton button);
            bool GetButtonDown(GamepadButton button);
            gath::v2d GetCurrentAxis();
            gath::v2d GetCurrentRightAxis();
            bool Open(int id);
            Ganpad();
            ~Ganpad();
        };
        extern Ganpad current;
        void Init();
    }

    
}