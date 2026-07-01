
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
#include <gangod/gath.h>
#include <gangod/tech.h>
using gargs = std::vector<std::string>;
const int NAV_COLOLDOWN = 300;
struct Navigation {
    std::string next_right;  // +x
    std::string next_left;   // -x
    std::string next_up;     // +y
    std::string next_down;   // -y
};
class GActions{
private:
    static std::unordered_map<std::string, std::function<void(gargs)>> actionDict;
public:
    
    GActions() = delete;
    ~GActions() = delete;
    GActions(const GActions&) = delete;
    GActions& operator=(const GActions&) = delete;
    static void REGISTER(std::function<void(gargs)> func, std::string funcName){
        actionDict[funcName] = func;
    }

    static std::function<void(gargs)> GETCALL(std::string funcName){
        if (actionDict.find(funcName) != actionDict.end()) {
            return actionDict[funcName];
        } else {
            std::cerr << "Warning: Action " << funcName << " not found!\n";
        }
        return nullptr;
    }
};

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
    static gath::v2d currentVector;
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

    static void NavigateVector(gath::v2d vec){
        currentVector.x = vec.x;
        currentVector.y = vec.y;
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
        float scaleX = (float)SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay())->w / 800.0f;
        float scaleY = (float)SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay())->h / 600.0f;
        float log_x = jfile["rect"]["x"].get<float>() * scaleX;
        float log_y = jfile["rect"]["y"].get<float>() * scaleY;
        float log_w = jfile["rect"]["width"].get<float>() * scaleX;
        float log_h = jfile["rect"]["height"].get<float>() * scaleY;

        float halfx = (float)SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay())->w / 2.0f; 
        float halfy = (float)SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay())->h / 2.0f; 

        float phys_x = halfx + log_x;
        float phys_y = halfy - log_y;

        phys_x -= log_w / 2.0f;
        phys_y -= log_h / 2.0f;
        if(!jfile.empty()){
            rect = {phys_x, phys_y, log_w, log_h};
        if(jfile.contains("color")){
            Color = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
            }
        }
    }

public:
    Navigation nav;
    SDL_FRect rect;
    SDL_Color Color;
    std::string name;
    virtual void Select() {}
    virtual void deSelect() {}
    virtual void Point() {}
    virtual void Update(SDL_Renderer* rend){
        BasicUpdate(rend);
        
    };
    void ConstructFile(const nlohmann::json_abi_v3_12_0::basic_json<> file);
    Object(const char* name);
    Object();
    ~Object() = default;
};

class Button : public Object{
private:
    SDL_Color targetCol;
    float lerpspeed = 0.25f;

public:
    using Object::Object;
    
    std::function<void(gargs)> onclick;
    SDL_Color hoverColor;
    SDL_Color fineColor;
    float targetScale = 1.0f;
    float currentScale = 1.0f;
    float lerpScale = 0.1f;
    bool ishover;

    Button(const nlohmann::json_abi_v3_12_0::basic_json<> file){
        ConstructFile(file);
        
        nav.next_up = jfile["nextnav"]["+y"];
        nav.next_down = jfile["nextnav"]["-y"];
        nav.next_left = jfile["nextnav"]["-x"];
        nav.next_right = jfile["nextnav"]["+x"];
        onclick = GActions::GETCALL(file["onclick"]["action"]);
        fineColor = {jfile["color"]["r"], jfile["color"]["g"], jfile["color"]["b"], jfile["color"]["a"]};
        hoverColor = {jfile["hover_color"]["r"], jfile["hover_color"]["g"], jfile["hover_color"]["b"], jfile["hover_color"]["a"]};
        targetCol = fineColor;
    }

    void Update(SDL_Renderer* rend) override;

    void Point() override {
        currentScale = 0.95f;
        if(onclick){
            onclick(jfile["onclick"]["args"]);
        }
    }

    void Select() override{
        targetScale = 1.08f;
        targetCol = hoverColor;
    }

    void deSelect() override {
        targetScale = 1.0f;
        targetCol = fineColor;
    }

};


class ObjectParenter{
private:
    static std::vector<Object*> alls;
    static float lastNavTime;
public:
    
    static Object* currentSelect;
    ObjectParenter() = delete;
    ~ObjectParenter() = delete;
    ObjectParenter(const ObjectParenter&) = delete;
    ObjectParenter& operator=(const ObjectParenter&) = delete;

    static void updAll(SDL_Renderer* rend);

    static void updButtonClick();
    static void push(Object* o);

    static void reload(SDL_Renderer* sdlrd);

    static bool FindByName(std::string name, Object*& result);
    static void SelectByName(std::string name);
};

template <class T>
class UIVerticalArray : public Object {
private:
    std::vector<T*> created;
    T* last;
    bool scrollable;
    float currentScrollY = 0.0f;
    float targetScrollY = 0.0f; // Для плавной анимации скролла через LERP
    float scrollLerpSpeed = 0.1f;
public:
    using Object::Object;

    T* operator[](int idx) {
        return created[idx]; // dynamic_cast не нужен, вектор уже хранит T*
    }

    int size() { return created.size(); }
    
    // Исправленные итераторы для поддержки циклов C++
    auto begin() { return created.begin(); }
    auto end()   { return created.end(); }

    template<typename... args>
    UIVerticalArray(const nlohmann::json_abi_v3_12_0::basic_json<> file, const char* name, int size, bool scroll = false, args&&... arguments) 
        : Object(name), scrollable(scroll) {
        
        ConstructFile(file);
        float step = jfile["step"].get<float>();

        for(int i = 0; i < size; ++i){
            T* b = new T(std::forward<args>(arguments)...);
            std::string origName = b->name;
            b->name = origName + std::to_string(i);
            
            b->rect = { rect.x, rect.y - (step * i), b->rect.w, b->rect.h };

            created.push_back(b);
            ObjectParenter::push(b);

            Button* conv = dynamic_cast<Button*>(b);
            if(conv){
                if(i == 0){
                    conv->nav.next_down = origName + std::to_string(i + 1); 
                    conv->nav.next_up = origName + std::to_string(size - 1); 
                    continue;
                }
                if(i == size-1){
                    conv->nav.next_down = origName + std::to_string(0); 
                    conv->nav.next_up = origName + std::to_string(i - 1); 
                    continue;
                }

                conv->nav.next_down = origName + std::to_string(i + 1); 
                conv->nav.next_up = origName + std::to_string(i - 1); 

            }
            
        }
    }

    void Update(SDL_Renderer* rend) override {
        if (scrollable) {
            for (int i = 0; i < created.size(); ++i) {
                if (ObjectParenter::currentSelect == created[i]) {
                    float elementRightEdge = (jfile["step"].get<float>() * i) + created[i]->rect.w;
                    if (elementRightEdge - targetScrollY > rect.w) {
                        targetScrollY = elementRightEdge - rect.w;
                    }
                    float elementLeftEdge = jfile["step"].get<float>() * i;
                    if (elementLeftEdge < targetScrollY) {
                        targetScrollY = elementLeftEdge; 
                    }
                    last = static_cast<T*>(ObjectParenter::currentSelect);
                }
            }
            float maxScroll = std::max(0.0f, (created.size() * jfile["step"].get<float>()) - rect.w);
            targetScrollY = std::clamp(targetScrollY, 0.0f, maxScroll);

            currentScrollY = currentScrollY + (targetScrollY - currentScrollY) * scrollLerpSpeed;
        }

        float step = jfile["step"].get<float>();

        for (int i = 0; i < created.size(); ++i){
            auto& b = created[i];
            
            b->rect = { rect.x, rect.y - (step * i) - currentScrollY, b->rect.w, b->rect.h };
        }
    }
    void Select() override{
        if(last){
            ObjectParenter::SelectByName(last->name);
            return;
        }
        ObjectParenter::SelectByName(created[0]->name);
    }
    ~UIVerticalArray() {
        
        for(auto& c : created){
            delete c;
            c = nullptr;
        }
        created.clear();
    }
};

template <class T>
class UIHorizontalArray : public Object {
private:
    std::vector<T*> created;
    T* last;
    bool scrollable;
    float currentScrollX = 0.0f;
    float targetScrollX = 0.0f; // Для плавной анимации скролла через LERP
    float scrollLerpSpeed = 0.1f;
public:
    using Object::Object;

    T* operator[](int idx) {
        return created[idx]; // dynamic_cast не нужен, вектор уже хранит T*
    }

    int size() { return created.size(); }
    
    // Исправленные итераторы для поддержки циклов C++
    auto begin() { return created.begin(); }
    auto end()   { return created.end(); }

    template<typename... args>
    UIHorizontalArray(const nlohmann::json_abi_v3_12_0::basic_json<> file, const char* name, int size, bool scroll = false, args&&... arguments) 
        : Object(name), scrollable(scroll) {
        std::cout << "loading construct" << std::endl;
        ConstructFile(file);
        float step = jfile["step"].get<float>();

        for(int i = 0; i < size; ++i){
            std::cout << "loading button construct" << std::endl;
            T* b = new T(std::forward<args>(arguments)...);
            std::string origName = b->name;
            b->name = origName + std::to_string(i);
            
            b->rect = { rect.x + (step * i), rect.y, b->rect.w, b->rect.h };

            created.push_back(b);
            ObjectParenter::push(b);

            Button* conv = dynamic_cast<Button*>(b);
            if(conv){
                if(i == 0){
                    conv->nav.next_right = origName + std::to_string(i + 1); 
                    conv->nav.next_left = origName + std::to_string(size - 1); 
                    continue;
                }
                if(i == size-1){
                    conv->nav.next_right = origName + std::to_string(0); 
                    conv->nav.next_left = origName + std::to_string(i - 1); 
                    continue;
                }

                conv->nav.next_right = origName + std::to_string(i + 1); 
                conv->nav.next_left = origName + std::to_string(i - 1); 

                std::cout << origName + std::to_string(i + 1) << std::endl;
            }
            
        }
    }

    void Update(SDL_Renderer* rend) override {
        if (scrollable) {
            for (int i = 0; i < created.size(); ++i) {
                if (ObjectParenter::currentSelect == created[i]) {
                    float elementRightEdge = (jfile["step"].get<float>() * i) + created[i]->rect.w;
                    if (elementRightEdge - targetScrollX > rect.w) {
                        targetScrollX = elementRightEdge - rect.w;
                    }
                    float elementLeftEdge = jfile["step"].get<float>() * i;
                    if (elementLeftEdge < targetScrollX) {
                        targetScrollX = elementLeftEdge; 
                    }
                    last = static_cast<T*>(ObjectParenter::currentSelect);
                }
            }
            float maxScroll = std::max(0.0f, (created.size() * jfile["step"].get<float>()) - rect.w);
            targetScrollX = std::clamp(targetScrollX, 0.0f, maxScroll);

            currentScrollX = currentScrollX + (targetScrollX - currentScrollX) * scrollLerpSpeed;
        }

        float step = jfile["step"].get<float>();

        for (int i = 0; i < created.size(); ++i){
            auto& b = created[i];
            
            b->rect = { rect.x + (step * i) - currentScrollX, rect.y, b->rect.w, b->rect.h };
        }
    }
    void Select() override{
        if(last){
            ObjectParenter::SelectByName(last->name);
            return;
        }
        ObjectParenter::SelectByName(created[0]->name);
    }
    ~UIHorizontalArray() {
        
        for(auto& c : created){
            delete c;
            c = nullptr;
        }
        created.clear();
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







