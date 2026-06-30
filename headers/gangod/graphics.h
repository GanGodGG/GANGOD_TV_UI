
#pragma once

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <functional>
#include <gangod/filemgmt.h>
#include <curl/curl.h>
#include <json.hpp>
#include <cmath>
#include <memory>
#include <utility>
#include <algorithm>
class ObjectParenter;
struct WindowHandler{
    SDL_Window* window;
    SDL_Renderer* render;
};
static size_t imgwriteBuffer(void* ptr, size_t size, size_t nmemb, std::vector<char>* data){
    size_t total_size = size * nmemb;
    data->insert(data->end(), (char*)ptr, (char*)ptr + total_size);
    return total_size;
}
class uiResponse{
public:
    static float mouseX, mouseY;
    static float scrollX, scrollY;
    static void OnMouseMove(float x, float y) { mouseX = x; mouseY = y; };
    static void onScroll(float x, float y) { 
        scrollX = x, scrollY = y; 
    }

    static void update(){
        if(scrollX != 0)
            scrollX -= 0.01f;
        if(std::abs(scrollY) > 0.1)
           scrollY /= 1.25f;
        else
            scrollY = 0;
    }

    uiResponse() = delete;
    ~uiResponse() = delete;
    uiResponse(const uiResponse&) = delete;
    uiResponse& operator=(const uiResponse&) = delete;
};

class Object{
protected:
    nlohmann::json jfile;
    virtual void CreateElement() {}
    std::function<void()> onReload;
    void BasicUpdate(SDL_Renderer* rend){ 
SDL_SetRenderDrawColor(rend, Color.r, Color.g, Color.b, Color.a);
        SDL_RenderFillRect(rend, &rect);
    }
    void ChangeByJsonFile(){
        if(!jfile.empty()){
            rect = {jfile["rect"]["x"].get<float>(), jfile["rect"]["y"].get<float>(), jfile["rect"]["width"].get<float>(), jfile["rect"]["height"].get<float>()};
        if(jfile.contains("color")){
            Color = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
            }
        }
    }

public:
    SDL_FRect rect;
    SDL_Color Color;
    std::string name;
    
    virtual void Update(SDL_Renderer* rend){
        BasicUpdate(rend);
        
    };
    void ConstructFile(const nlohmann::json_abi_v3_12_0::basic_json<> file);
    Object(const char* name);
    Object();
    ~Object() = default;
};

class Button : public Object{
public:
    using Object::Object;
    std::function<void()> onclick;
    SDL_Color hoverColor;
    SDL_Color fineColor;
    bool ishover;

    Button(const nlohmann::json_abi_v3_12_0::basic_json<> file){
        ConstructFile(file);
        fineColor = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
        hoverColor = {jfile["hover_color"]["r"], jfile["hover_color"]["g"], jfile["hover_color"]["b"], jfile["hover_color"]["a"]};
    }

    void Update(SDL_Renderer* rend) override{
        BasicUpdate(rend);
        ishover =(uiResponse::mouseX >= rect.x && uiResponse::mouseX <= rect.x + rect.w &&
            uiResponse::mouseY >= rect.y && uiResponse::mouseY <= rect.y + rect.h);
        if(ishover){
            Color = hoverColor;
        }
        else{
            Color = fineColor;
        }
    }

    void Click(){
        if(ishover && onclick){
            onclick();
        }
    }

    ~Button();
};
template <class T>
class UIVerticalArray : public Object{
private:
    std::vector<T*> created;
    float currentScrollY = 0.0f;
    bool scrollable;
public:
using Object::Object;
    T* operator[](int idx){
        return dynamic_cast<T*>(created[idx]);
    }
    int size(){
        return created.size();
    }
    T* begin(){
        return dynamic_cast<T*>(created[0]);
    }
    T* end(){
        return dynamic_cast<T*>(created[created.size() - 1]);
    }
    template<typename... args>
    UIVerticalArray(const char* name, int size, bool scroll = false, args&&... arguments) : Object(name), scrollable(scroll) {
        for(int i = 0; i < size; ++i){
            T* b = new T(std::forward<args>(arguments)...);
            b->rect = { rect.x, rect.y - jfile["step"].get<int>() * i, b->rect.w, b->rect.h };
            //b->Color = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
            created.push_back(b);
        }
    }

    void Update(SDL_Renderer* rend) override{
        for (int i = 0; i < created.size(); ++i){
            if(scrollable){
                currentScrollY -= uiResponse::scrollY * 4;
                currentScrollY = std::max(0.0f, currentScrollY);
                currentScrollY = std::min((created.size() * jfile["step"].get<float>()) / rect.h, currentScrollY);
            }
            auto& b = created[i];
            b->rect = { rect.x, rect.y - ((jfile["step"].get<float>() * i) + (currentScrollY / rect.h)), b->rect.w, b->rect.h };
            //b->Color = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
        }
    }

    ~UIVerticalArray(){
        for(auto& c : created){
            delete c;
            c = nullptr;
        }
        created = {};
    }
};

class Image : public Object {
    private:
    SDL_Texture* tex;
    float origX, origY;
    SDL_Renderer* rendptr;
    public:
    using Object::Object;

    Image(const nlohmann::json_abi_v3_12_0::basic_json<> file, SDL_Renderer* rnd){
        ConstructFile(file);
        rendptr = rnd;
        gLoadImage(rendptr, jfile["source"].get<std::string>());
    }

    void gLoadImage(SDL_Renderer* rnd,const std::string file){
        if(fileRead::isFileExist(file) == false) { std::cout << "Cannot find texture!" << std::endl; }
        tex = IMG_LoadTexture(rnd, std::filesystem::path(file).string().c_str());
        if(!tex){
            std::cout << "Cannot load texture!" << std::endl;
        }
    }

    void Update(SDL_Renderer* rend) override{
        SDL_RenderTexture(rend, tex, NULL, &rect);
    }

    ~Image(){
        SDL_DestroyTexture(tex);
        rendptr = nullptr;
    }
};

class Text : public Object{

    private:
    SDL_Texture* tex;
    SDL_Renderer* rendptr;
    public:
    using Object::Object;
    std::string content;

    Text() : Object(){
        //nothing here...
    }

    Text(const char* name, SDL_Renderer* rend) : Object(name)
    {
        BuildText(rend);
    } 

    void ChangeText(const char* text){
        BuildText(rendptr, text);
    }

    void ChangeText(SDL_Renderer* rend, const char* text){
        BuildText(rendptr, text);
    }

    void BuildText(SDL_Renderer* rend, std::string text = ""){
        rendptr = rend;
        content = text.empty() ? (std::string)jfile["Text"] : text;
        onReload = [&](){
            std::cout << content << std::endl;
            ChangeText(rendptr, content.c_str());
        };
        if(tex){
            SDL_DestroyTexture(tex);
        }
        if(!fileRead::isFileExist(jfile["FontSource"])) {std::cerr << "no such font. error source: " << jfile["name"] << std::endl; return;}
        TTF_Font *font = TTF_OpenFont(jfile["FontSource"].get<std::string>().c_str(), jfile["FontSize"]);
        SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, content.c_str(), 0, Color, jfile["rect"]["width"]);
        if(!surf){
            TTF_CloseFont(font);
            SDL_DestroySurface(surf);
            std::cerr << "Cannot create surface. error source: " << jfile["name"] << std::endl; return;
        }
        tex = SDL_CreateTextureFromSurface(rend, surf);

        rect.h = surf->h;
        rect.w = surf->w;
        TTF_CloseFont(font);
        SDL_DestroySurface(surf);
    }

    void Update(SDL_Renderer* rend) override {
        //BasicUpdate();
        SDL_RenderTexture(rend, tex, 0, &rect);
    }

    ~Text(){
        SDL_DestroyTexture(tex);
        rendptr = nullptr;
    }
};

class Scroll : public Object{
    public:
    float value = 0;
    Object o1;
    Object o2;
    Scroll(const char* name) : Object(name)
    {
        o1.Color = {0, 0, 0, 0};
        o2.Color = {255, 255, 255, 5};
    }

    void Update(SDL_Renderer* rend) override{
        value = std::max(1.0f, value);
        value = std::min(0.0f, value);
        o1.rect = rect;
        o2.rect = rect;
        o2.rect.w = o1.rect.w * value;
        o1.rect.w = o1.rect.w * value;
        //o2.rect.x = rect.x - o2.rect.w / 2;
    }
};

class ObjectParenter{
    private:
    static std::vector<Object*> alls;
public:
    ObjectParenter() = delete;
    ~ObjectParenter() = delete;
    ObjectParenter(const ObjectParenter&) = delete;
    ObjectParenter& operator=(const ObjectParenter&) = delete;

    static void updAll(SDL_Renderer* rend);

    static void updButtonClick();
    static void push(Object* o);

    static void reload(SDL_Renderer* sdlrd);
};



