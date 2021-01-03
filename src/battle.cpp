#include "battle.h"


void startBattle(gameData *dt){
    dt->inBattle = true;
    createTurnQueue(dt);
}

void processBattle(gameData *dt){
    if(dt->inBattle){
        //PC turn
        if(ac(dt, dt->turnQueue.front())->isPC){
            //create menus
            if(dt->weaponSelector.selectorButtons.size() == 0){
                createWeaponSelectorMenu(dt, dt->turnQueue.front());
            }
            //handle world clicks
            GraphObjID bBoxId = dt->grph->getLeftClickedBoundingBox();
            if(bBoxId != 0){
                Vec3 clickedLoc;
                if(dt->boundingBoxToCreature.find(bBoxId) != dt->boundingBoxToCreature.end()){
                    ID selecCrt = dt->boundingBoxToCreature.at(bBoxId);
                    if(dt->crtGroup.at(selecCrt).isPC){
                        dt->selectedPC = selecCrt;
                    }
                    else{
                        dt->selectedNPC = selecCrt;
                    }
                    createTargetSelectorMenu(dt, selecCrt);
                    std::cout << "got creature: " << selecCrt << std::endl;
                }
                else{
                    clickedLoc = dt->boundingBoxToLocation.at(bBoxId).loc;
                }
                //std::cout << "Clicked the node at: " << clickedLoc.x << "," << clickedLoc.y << "," << clickedLoc.z << " which is type: " << getNode(&dt->loadedChunk, clickedLoc) << std::endl;
            }
            //handle button clicks
            ID btnId = getLeftClickedButtonID();
            for(auto wpn : dt->weaponSelector.selectorButtons){
                if(btnId == wpn.second){
                    std::cout << "You selected the " << ao(dt,wpn.first)->name << std::endl;
                    dt->weaponSelector.selectedObject = wpn.first;
                    resetLeftClickedButtonID();
                }
            }
            for(auto trg : dt->targetSelector.selectorButtons){
                if(btnId == trg.second){
                    std::cout << "You targeted the " << ao(dt,trg.first)->name << std::endl;
                    dt->targetSelector.selectedObject = trg.first;
                    resetLeftClickedButtonID();
                }
            }
            if(getKeyPressed() == GLFW_KEY_SPACE){
                if(dt->weaponSelector.selectedObject != NULL_ID){
                    attackObject(dt, dt->weaponSelector.selectedObject, dt->targetSelector.selectedObject);
                }
                dt->weaponSelector.selectedObject = NULL_ID;
                resetKeyPressed();
                advanceTurnQueue(dt);
            }
        }
        //NPC turn
        else{
            //basic target-picking NPC behavior
            for(auto crt : dt->crtGroup){
                if(crt.second.isPC){
                    setBattleTarget(&dt->crtGroup, dt->turnQueue.front(), crt.first);
                    break;
                }
            }
            Creature crt = *ac(dt, dt->turnQueue.front());
            ID targetPart = getLinkedObjs(dt, ac(dt, crt.battleTarget)->body, _ANY, FUNCTIONAL, "", false)[0];
            attackObject(dt, getPhysWeapons(dt, crt.body)[0], targetPart);
            advanceTurnQueue(dt);
        }
    }
}

void createTurnQueue(gameData *dt){
    dt->turnQueue = {};
    for(auto crt : dt->crtGroup){
        dt->turnQueue.push(crt.first);
    }
}

void advanceTurnQueue(gameData *dt){
    dt->turnQueue.push(dt->turnQueue.front());
    dt->turnQueue.pop();
}

void createStackSelector(gameData *dt, GUI_StackSelector *selector, std::vector<ID> objects, double x, double y){
    selector->selectorButtons.clear();
    selector->selectedObject = NULL_ID;
    int iter = 1;
    for(auto obj : objects){
        GraphObjID btn = dt->grph->createButton(onButtonLeftClick, ao(dt, obj)->name, x, 1.0-(iter*y));
        iter++;
        selector->selectorButtons.push_back(std::make_pair(obj, btn));
    }
}

void createWeaponSelectorMenu(gameData *dt, ID playerChar){
    std::vector<ID> weapons = getPhysWeapons(dt, ac(dt, playerChar)->body);
    std::pair<uint, uint> dimen = dt->grph->getScreenDimensions();
    createStackSelector(dt, &dt->weaponSelector, weapons, 20.0/dimen.first, 50.0/dimen.second);
}

void createTargetSelectorMenu(gameData *dt, ID character){
    std::vector<ID> targets = getLinkedObjs(dt, ac(dt,character)->body, _ANY, FUNCTIONAL, "", false);
    targets.push_back(ac(dt,character)->body);
    std::pair<uint, uint> dimen = dt->grph->getScreenDimensions();
    createStackSelector(dt, &dt->targetSelector, targets, 1.0-(300.0/dimen.first), 50.0/dimen.second);
}