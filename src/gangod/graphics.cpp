#include <gangod/graphics.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
float uiResponse::mouseX = 0.0f;
float uiResponse::mouseY = 0.0f;

float uiResponse::scrollX = 0.0f;
float uiResponse::scrollY = 0.0f;

std::vector<Object*> ObjectParenter::alls = {};

void ObjectParenter::push(Object* o){
    alls.push_back(o);
}

void ObjectParenter::updButtonClick(){
    for(auto& o : alls){
        auto but = dynamic_cast<Button*>(o);
        if(but){
            but->Click(); 
        }
           
    }
}

void ObjectParenter::updAll(SDL_Renderer* rend){
    for(auto& o : alls){
        o->Update(rend); 
    }
}

void ObjectParenter::reload(SDL_Renderer* sdlrd){
    for(auto& o : alls){
        // deconstruct all pointers
        delete o;
        o = nullptr;
    }
    // reload array
    alls = {};
    std::string file = fileRead::OpenFILE("jsons/interface.json");
    auto parsed = nlohmann::json::parse(file);
    for(const auto& f : parsed["ui_elements"]){
        Object* pushable = nullptr;
        if(f["type"].get<std::string>() == "button"){
            pushable = new Button(f);
        }
        if(f["type"].get<std::string>() == "image"){
            pushable = new Image(f, sdlrd);
        }
        alls.push_back(pushable);
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
