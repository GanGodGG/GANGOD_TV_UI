#include <gangod/graphics.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
float uiResponse::mouseX = 0.0f;
float uiResponse::mouseY = 0.0f;

float uiResponse::scrollX = 0.0f;
float uiResponse::scrollY = 0.0f;

std::unordered_map<std::string, std::function<void(gargs)>> GActions::actionDict;

std::vector<Object*> ObjectParenter::alls = {};
float ObjectParenter::lastNavTime = 0;
Object* ObjectParenter::currentSelect = nullptr;



void ObjectParenter::push(Object* o){
    alls.push_back(o);
}

void ObjectParenter::updButtonClick(){
    if(currentSelect){
        currentSelect->Point();
    }
}

void ObjectParenter::updAll(SDL_Renderer* rend){
    for(auto& o : alls){
        o->Update(rend); 
    }
    Uint64 currentTime = SDL_GetTicks();
    if (currentTime - lastNavTime < NAV_COLOLDOWN) {
        return; 
    }
    if(currentSelect != nullptr){
        gath::v2d axis = getch::gamepad::current.GetCurrentAxis(); 

        std::string targetName = "";

        if (axis.x > 0) targetName = currentSelect->nav.next_right;
        else if (axis.x < 0) targetName = currentSelect->nav.next_left;
        else if (axis.y > 0) targetName = currentSelect->nav.next_up;
        else if (axis.y < 0) targetName = currentSelect->nav.next_down;

        if (!targetName.empty()) {
            SelectByName(targetName);
            lastNavTime = currentTime;
        }
    }
}

void ObjectParenter::reload(SDL_Renderer* sdlrd){
    for(auto& o : alls){
        // deconstruct all pointers
        delete o;
        o = nullptr;
    }
    currentSelect = nullptr;
    // reload array
    alls = {};
    std::string file = fileRead::OpenFILE("jsons/interface.json");
    auto parsed = nlohmann::json::parse(file);
    Button* any = nullptr;
    for(const auto& f : parsed["ui_elements"]){
        std::cout << f["name"].get<std::string>() << std::endl;
        Object* pushable = nullptr;
        if(f["type"].get<std::string>() == "button"){
            pushable = new Button(f);
            if(any == nullptr){
                any = static_cast<Button*>(pushable);
            }
        }
        if(f["type"].get<std::string>() == "image"){
            pushable = new Image(f, sdlrd);
        }
        if(f["type"].get<std::string>() == "vertical array"){
            if(f["ex"]["type"].get<std::string>() == "button"){
                pushable = new UIVerticalArray<Button>(f, f["name"].get<std::string>().c_str(), f["size"].get<int>(), f["scroll"].get<bool>(), f["ex"]);
            }
            if(f["ex"]["type"].get<std::string>() == "image"){
                pushable = new UIVerticalArray<Image>(f, f["name"].get<std::string>().c_str(), f["size"].get<int>(), f["scroll"].get<bool>(), f["ex"], sdlrd);
            }
        }
        if(f["type"].get<std::string>() == "horizontal array"){
            
            if(f["ex"]["type"].get<std::string>() == "button"){
                std::cout << "loading button" << std::endl;
                pushable = new UIHorizontalArray<Button>(f, f["name"].get<std::string>().c_str(), f["size"].get<int>(), f["scroll"].get<bool>(), f["ex"]);
            }
            if(f["ex"]["type"].get<std::string>() == "image"){
                pushable = new UIHorizontalArray<Image>(f, f["name"].get<std::string>().c_str(), f["size"].get<int>(), f["scroll"].get<bool>(), f["ex"], sdlrd);
            }
        }
        alls.push_back(pushable);
    }
    SelectByName(any->name);
}

bool ObjectParenter::FindByName(std::string name, Object*& result){
    for(auto& o : alls){
        if(o->name == name){
            result = o;
            return true;
        }
    }
    std::cout << "No such object" << name << std::endl;
    return false;
}

void ObjectParenter::SelectByName(std::string name){
    if(currentSelect){
        currentSelect->deSelect();
    }
    if(FindByName(name, currentSelect)){
        currentSelect->Select();
    }
}

Object::Object(const char* name){
    //ObjectParenter::push(this);
    //ConstructFile(name);
    //BasicUpdate();
}

Object::Object(){
    //ObjectParenter::push(this);
}

void Object::ConstructFile(const nlohmann::json_abi_v3_12_0::basic_json<> file){
    jfile = file;
    this->name = file["name"].get<std::string>();
    ChangeByJsonFile();
}

void Button::Update(SDL_Renderer* rend){
    
    //color lerp
    Color.r = Color.r + (targetCol.r - Color.r) * lerpspeed;
    Color.g = Color.g + (targetCol.g - Color.g) * lerpspeed;
    Color.b = Color.b + (targetCol.b - Color.b) * lerpspeed;
    Color.a = Color.a + (targetCol.a - Color.a) * lerpspeed;

    //scaleLerp
    currentScale = currentScale + (targetScale - currentScale) * lerpScale;

    // 2. Применяем масштаб к размерам RECT перед рендером
    // Чтобы кнопка уменьшалась К ЦЕНТРУ, а не к левому верхнему углу,
    // нужно скорректировать её координаты x и y // Берем исходный rect из JSON
    
    SDL_FRect originalRect = rect; 

    // 2. Применяем масштаб к размерам оригинального rect
    float originalW = originalRect.w;
    float originalH = originalRect.h;

    rect.w = originalW * currentScale;
    rect.h = originalH * currentScale;

    // Смещаем x и y относительно оригинального центра
    rect.x = originalRect.x + (originalW - rect.w) / 2.0f;
    rect.y = originalRect.y + (originalH - rect.h) / 2.0f;

    // 3. Вызываем обновление и отрисовку с масштабированным rect
    BasicUpdate(rend); 

    // 4. ВОЗВРАЩАЕМ ИСХОДНЫЕ ДАННЫЕ НА МЕСТО
    // После того как отрисовка завершена, восстанавливаем чистые координаты из JSON
    rect = originalRect;
}

