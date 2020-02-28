#include "message.h"

extern engineData ENGINE_DATA;
extern worldData WORLD_DATA;

//helper function, handles messages directed at graphics code
void send_GraphicsMsg(std::vector<std::string> tokens, bool *validMsg){
    if(tokens[0] == "RESIZE_WINDOW"){
        ENGINE_DATA.windowWidth = std::stoi(tokens[1]);
        ENGINE_DATA.windowHeight = std::stoi(tokens[2]);
        SDL_SetWindowSize(ENGINE_DATA.window, ENGINE_DATA.windowWidth, ENGINE_DATA.windowHeight);
        *validMsg=true;
    }
    if(tokens[0] == "FULLSCREEN"){
        if(tokens[1] == "1"){
            SDL_SetWindowFullscreen(ENGINE_DATA.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else if(tokens[1] == "0"){
            SDL_SetWindowFullscreen(ENGINE_DATA.window, 0);
        }
        *validMsg=true;
    }
}

//helper function, handles messages directed at Creatures
void send_MaterialMsg(std::vector<std::string> tokens, bool *validMsg){
    if(tokens[0] == "LOAD_MATERIALS"){
        WORLD_DATA.allMats->loadMaterials(tokens[1]);
        *validMsg=true;
    }

    //debug
    if(tokens[0] == "PRINT_MATERIALS"){
        WORLD_DATA.allMats->printMaterials();
        *validMsg=true;
    }
}

//helper function, handles messages directed at Creatures
void send_ThingMsg(std::vector<std::string> tokens, bool *validMsg){
    
}

//helper function, handles messages directed at Creatures
void send_CreatureMsg(std::vector<std::string> tokens, bool *validMsg){
    
}

void send(std::string msg){
    bool validMsg = false;
    //parse message into tokens
    std::vector<std::string> tokens;
    if(msg.find(" _KEY_PRESSED") == std::string::npos){
        splitString(&tokens, msg, ' ');
    }
    else{
        tokens.push_back(msg);
    }
    //quit game
    if(tokens[0] == "QUIT_APP"){
        ENGINE_DATA.quitGame = 1;
        validMsg=true;
    }
    //developer console
    if(tokens[0] == "TOGGLE_DEBUG_MODE"){
        ENGINE_DATA.debugMode = !ENGINE_DATA.debugMode;
        validMsg=true;
    }
    if(ENGINE_DATA.debugMode){
        if(tokens[0] == "CONFIRM"){
            if(strlen(ENGINE_DATA.debugConsoleStr) > 0){
                send(ENGINE_DATA.debugConsoleStr);
            }
            memset(ENGINE_DATA.debugConsoleStr, 0, strlen(ENGINE_DATA.debugConsoleStr));
        }
        else if(tokens[0].find("_KEY_PRESSED") != std::string::npos){
            char tmp[2];
            tmp[0] = tokens[0][0];
            tmp[1] = '\0';
            strcat(ENGINE_DATA.debugConsoleStr, tmp);
        }
        validMsg=true;
    }

    //testing stuff
    if(tokens[0] == "TIME"){
        time_t result = time(NULL);
        printf("%s\n", asctime(gmtime(&result)));
        validMsg=true;
    }

    //helpers
    send_GraphicsMsg(tokens, &validMsg);
    send_MaterialMsg(tokens, &validMsg);
    send_ThingMsg(tokens, &validMsg);
    send_CreatureMsg(tokens, &validMsg);

    if(!validMsg){
        logError("send", "Could not process message: '" + std::string(msg) + "'");
    }
}
