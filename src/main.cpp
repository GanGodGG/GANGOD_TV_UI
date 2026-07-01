#include <iostream>
#include <gangod/graphics.h>
#include <gangod/actiondictionary.h>
#include <gangod/tech.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <future>
void quit(WindowHandler& winh){
    SDL_DestroyRenderer(winh.render);
    SDL_DestroyWindow(winh.window);
    SDL_Quit();
}



int main() {
    //gserver::startServer();
    // Set up locals
    // connect to the service
    //SDL start
    GANGOD_REGISTER_NAME::setAllRegisters();
    bool running;
    if(!SDL_Init(SDL_INIT_VIDEO)){
        //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Error while starting sdl3, check is there SDL3.dll file with .exe file in programm, love, GANGOD.", nullptr);
        return -1;
    }
    if(!TTF_Init()){
        //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Error while starting sdl3 ttf, check is there SDL3.dll (ttf) file with .exe file in programm, love, GANGOD.", nullptr);
        return -1;
    }
    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        // Handle initialization error
        std::cerr << "Gamepad find error!" << std::endl;
        return -1;
    }
    WindowHandler wh;
    wh.window = SDL_CreateWindow("GANGOD UI!", 800, 600, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    
    if(!wh.window){
        //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "Fail with WIN start, love, GANGOD.", nullptr);
        quit(wh);
    }
    wh.render = SDL_CreateRenderer(wh.window, nullptr);
    if(!SDL_SetWindowFullscreen(wh.window, true)){
        std::cout << "Cannot create fullscreen window!" << std::endl;
    }
    std::cout << "Settled!" << std::endl;
    
    running = true;
    ObjectParenter::reload(wh.render);
    std::cout << "starting run..." << std::endl;
    getch::gamepad::Init();
    SDL_SetRenderLogicalPresentation(wh.render, 1920, 1080, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    while(running){
        SDL_Event event { 0 };
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_EVENT_QUIT:
                    running = false;
                break;

                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    if (getch::gamepad::current.GetButton(getch::gamepad::GamepadButton::A)) {
                        ObjectParenter::updButtonClick();
                    }
                break;

                case SDL_EVENT_MOUSE_MOTION:
                    uiResponse::OnMouseMove(event.motion.x, event.motion.y);
                break;

                case SDL_EVENT_MOUSE_WHEEL:
                    uiResponse::onScroll(event.wheel.x, event.wheel.y);
                break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        ObjectParenter::updButtonClick();
                    }
                break;

                case SDL_EVENT_GAMEPAD_ADDED:
                    getch::gamepad::Init();
                break;

                case SDL_EVENT_KEY_DOWN:

                    if(event.key.key == SDLK_F5){
                        std::cout << "reload!" << std::endl;
                        ObjectParenter::reload(wh.render);
                    }
                break;
            }
        }

        // rendering
        SDL_SetRenderDrawColor(wh.render, 233, 233, 233, 255);
        SDL_RenderClear(wh.render);
        uiResponse::update();
        ObjectParenter::updAll(wh.render);

        SDL_RenderPresent(wh.render);

        SDL_Delay(16);
    }

    quit(wh);
    return 0;
}