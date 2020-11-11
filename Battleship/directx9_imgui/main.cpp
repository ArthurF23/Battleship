// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#pragma disable(warning: 4996)



#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <fstream>
#include "Game.h"
#include <atomic>
//#include <cstdint>
using namespace std;

//#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#pragma comment (lib, "d3d9.lib") //Adds some thing that make d3d9.h work
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

static string IP;
static bool is_host = false;
/*
10x10 grid (technically 11x11)
What the array looks like as the buttons mapped

    0   1   2   3   4   5   6   7   8   9   10

    11  12  13  14  15  16  17  18  19  20  21

    22  23  24  25  26  27  28  29  30  31  32

    33  34  35  36  37  38  39  40  41  42  43

    44  45  46  47  48  49  50  51  52  53  54

    55  56  57  58  59  60  61  62  63  64  65

    66  67  68  69  70  71  72  73  74  75  76

    77  78  79  80  81  82  83  84  85  86  87

    88  89  90  91  92  93  94  95  96  97  98

    99  100 101 102 103 104 105 106 107 108 109

    110 111 112 113 114 115 116 117 118 119 120

5 ships



*/



// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
struct MAINWINDOW {
    int WIDTH = 1280;
    int HEIGHT = 800;
};

struct PLAYER_VIEW {
    
    int WIDTH = ImGui::GetIO().DisplaySize.x / 2;
    int HEIGHT = ImGui::GetIO().DisplaySize.y - 120;

    
    int X = ImGui::GetIO().DisplaySize.x - WIDTH;
    int Y = 0;
};

struct PLAYER_RADAR {

    int WIDTH = ImGui::GetIO().DisplaySize.x / 2;
    int HEIGHT = ImGui::GetIO().DisplaySize.y - 120;


    int X = 0;
    int Y = 0;
};

struct COLOR {
    ImVec4 HIT = ImVec4(255.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 MISS = ImVec4(0.255f, 0.255f, 0.255f, 1.0f);
    ImVec4 SUNK = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 BOAT = ImVec4(0.040f, 0.050f, 0.160f, 1.0f);
    ImVec4 DEFAULT = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
};

ImVec2 BUTTON_SIZE = ImVec2(50, 50);

struct PLACER_ {
    int LENGTH;
    int MAX_ARR_SIZE = 120;

    int FORBIDDEN_BUTTONS[21] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 22, 33, 44, 55, 66, 77, 88, 99, 110 }; // Buttons that arent supposed to be clicked

    PLACER_(int LENGTH_) {
        LENGTH = LENGTH_;        
    };

    enum ROTATION_ {
        HORIZONTAL,
        VERTICAL
    };    

    ROTATION_ ROTATION = ROTATION_::HORIZONTAL;
    int ROTATION_MATH[2] = {1, 11}; //+1=Horizontal, +11=Vertical
    
    string SMART_PLACER_POS[5];

    bool SMART_PLACER(int LOCATION) {

        if (LENGTH == 5) {

            if (ROTATION == ROTATION_::HORIZONTAL && (LOCATION + 4) <= 120 && (LOCATION + 3) <= 120 && (LOCATION + 2) <= 120 && (LOCATION + 1) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 1);
                SMART_PLACER_POS[2] = to_string(LOCATION + 2);
                SMART_PLACER_POS[3] = to_string(LOCATION + 3);
                SMART_PLACER_POS[4] = to_string(LOCATION + 4);
                return true;
            }

            else if (ROTATION == ROTATION_::VERTICAL && (LOCATION + 11) <= 120 && (LOCATION + 22) <= 120 && (LOCATION + 33) <= 120 && (LOCATION + 44) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 11);
                SMART_PLACER_POS[2] = to_string(LOCATION + 22);
                SMART_PLACER_POS[3] = to_string(LOCATION + 33);
                SMART_PLACER_POS[4] = to_string(LOCATION + 44);
                return true;
            }

            else {
                return false;
            };

        }

        else if (LENGTH == 4) {

            if (ROTATION == ROTATION_::HORIZONTAL && (LOCATION + 3) <= 120 && (LOCATION + 2) <= 120 && (LOCATION + 1) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 1);
                SMART_PLACER_POS[2] = to_string(LOCATION + 2);
                SMART_PLACER_POS[3] = to_string(LOCATION + 3);
                SMART_PLACER_POS[4] = to_string(LOCATION + 4);
                return true;
            }

            else if (ROTATION == ROTATION_::VERTICAL && (LOCATION + 11) <= 120 && (LOCATION + 22) <= 120 && (LOCATION + 33) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 11);
                SMART_PLACER_POS[2] = to_string(LOCATION + 22);
                SMART_PLACER_POS[3] = to_string(LOCATION + 33);
                SMART_PLACER_POS[4] = to_string(LOCATION + 44);
                return true;
            }

            else {
                return false;
            };
        }

        else if (LENGTH == 3) {

            if (ROTATION == ROTATION_::HORIZONTAL && (LOCATION + 2) <= 120 && (LOCATION + 1) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 1);
                SMART_PLACER_POS[2] = to_string(LOCATION + 2);
                SMART_PLACER_POS[3] = to_string(LOCATION + 3);
                SMART_PLACER_POS[4] = to_string(LOCATION + 4);
                return true;
            }

            else if (ROTATION == ROTATION_::VERTICAL && (LOCATION + 11) <= 120 && (LOCATION + 22) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 11);
                SMART_PLACER_POS[2] = to_string(LOCATION + 22);
                SMART_PLACER_POS[3] = to_string(LOCATION + 33);
                SMART_PLACER_POS[4] = to_string(LOCATION + 44);
                return true;
            }

            else {
                return false;
            };
        }

        else if (LENGTH == 2) {

            if (ROTATION == ROTATION_::HORIZONTAL && (LOCATION + 1) <= 120 &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 1);
                SMART_PLACER_POS[2] = to_string(LOCATION + 2);
                SMART_PLACER_POS[3] = to_string(LOCATION + 3);
                SMART_PLACER_POS[4] = to_string(LOCATION + 4);
                return true;
            }

            else if (ROTATION == ROTATION_::VERTICAL && (LOCATION + 11) <= 120 && 
                LOCATION + 11 != FORBIDDEN_BUTTONS[0] && LOCATION + 11 != FORBIDDEN_BUTTONS[1] && LOCATION + 11 != FORBIDDEN_BUTTONS[2] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[3] && LOCATION + 11 != FORBIDDEN_BUTTONS[4] && LOCATION + 11 != FORBIDDEN_BUTTONS[5] && 
                LOCATION + 11 != FORBIDDEN_BUTTONS[6] && LOCATION + 11 != FORBIDDEN_BUTTONS[7] && LOCATION + 11 != FORBIDDEN_BUTTONS[8] && 
                LOCATION + 11 != FORBIDDEN_BUTTONS[9] && LOCATION + 11 != FORBIDDEN_BUTTONS[10] && LOCATION + 11 != FORBIDDEN_BUTTONS[11] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[12] && LOCATION + 11 != FORBIDDEN_BUTTONS[13] && LOCATION + 11 != FORBIDDEN_BUTTONS[14] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[15] && LOCATION + 11 != FORBIDDEN_BUTTONS[16] && LOCATION + 11 != FORBIDDEN_BUTTONS[17] &&
                LOCATION + 11 != FORBIDDEN_BUTTONS[18] && LOCATION + 11 != FORBIDDEN_BUTTONS[19] && LOCATION + 11 != FORBIDDEN_BUTTONS[20]) {
                SMART_PLACER_POS[0] = to_string(LOCATION);
                SMART_PLACER_POS[1] = to_string(LOCATION + 11);
                SMART_PLACER_POS[2] = to_string(LOCATION + 22);
                SMART_PLACER_POS[3] = to_string(LOCATION + 33);
                SMART_PLACER_POS[4] = to_string(LOCATION + 44);
                return true;
            }

            else {
                return false;
            };
        }
    };
};

struct CARRIER_ {
    string NAME = "Carrier";
    const static int LENGTH = 5;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[5] = { false, false, false, false, false };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct BATTLESHIP_ {
    string NAME = "Battleship";
    const static int LENGTH = 4;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false, false };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct DESTROYER_ {
    string NAME = "Destroyer";
    const static int LENGTH = 3;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct SUBMARINE_ {
    string NAME = "Submarine";
    const static int LENGTH = 3;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct PATROL_BOAT_ {
    string NAME = "Patrol Boat";
    const static int LENGTH = 2;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false };
    PLACER_* PLACER = new PLACER_(LENGTH);
};



class SHIPS_CLASS {
public:    
    CARRIER_* CARRIER = new CARRIER_;

    BATTLESHIP_* BATTLESHIP = new BATTLESHIP_;

    DESTROYER_* DESTROYER = new DESTROYER_;

    SUBMARINE_* SUBMARINE = new SUBMARINE_;

    PATROL_BOAT_* PATROL_BOAT = new PATROL_BOAT_;

    bool VALID_POS[5] = { false, false, false, false, false };
};


struct RADAR_ {
    string ID;
    ImVec2 SIZE = ImVec2(50, 50);
    enum STATE_ {
        UNCLICKED,
        MISS,
        HIT,
        SUNK
    };

    int FORBIDDEN_BUTTONS[21] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 22, 33, 44, 55, 66, 77, 88, 99, 110}; // Buttons that arent supposed to be clicked

    STATE_ STATE = STATE_::UNCLICKED;
};

struct FLEET_ {
    string ID;
    ImVec2 SIZE = ImVec2(50, 50);
    enum STATE_ {
        UNCLICKED,
        MISS,
        HIT,
        BOAT
    };

    STATE_ STATE = STATE_::UNCLICKED;
};

class PLAYER {    
private:
    const static int RADAR_SIZE = 120;
public:
    atomic<bool> LOST = false;

    atomic<bool> is_turn = false;

    string name = "Player";

    SHIPS_CLASS* SHIPS = new SHIPS_CLASS;

    RADAR_* RADAR[121];

    FLEET_* FLEET[120];

    void BUTTON_FUNC(string BTN, int arrL) {
        bool is_forbidden = false;
        for (int i = 0; i < 21; i++) {
            if (arrL == RADAR[arrL]->FORBIDDEN_BUTTONS[i]) {
                is_forbidden = true;
                break;
            };
        };

        if (is_forbidden == false && is_turn == true && RADAR[arrL]->STATE == RADAR[arrL]->STATE_::UNCLICKED) {
            if (is_host == true) {
                thread sendth(SERVER::SEND, ("F@" + to_string(arrL)));
                sendth.join();
                cout << "waiting for response..." << endl;
                do {
                    Sleep(1000);
                } while (SERVER::RECENTMESSAGE == "");
                if (SERVER::RECENTMESSAGE.substr(0, 5) == "R@HIT") {
                    int index = stoi(SERVER::RECENTMESSAGE.substr(5, SERVER::RECENTMESSAGE.length()));
                    RADAR[index]->STATE = RADAR[index]->STATE_::HIT;
                    RADAR[index]->ID += "\nHIT";
                    SERVER::RECENTMESSAGE = "";
                }

                else if (SERVER::RECENTMESSAGE.substr(0, 6) == "R@MISS") {
                    int index = stoi(SERVER::RECENTMESSAGE.substr(6, SERVER::RECENTMESSAGE.length()));
                    RADAR[index]->STATE = RADAR[index]->STATE_::MISS;
                    RADAR[index]->ID += "\nMISS";
                    SERVER::RECENTMESSAGE = "";
                }

                else if (SERVER::RECENTMESSAGE.substr(0, 6) == "R@SUNK") {
                    int index = stoi(SERVER::RECENTMESSAGE.substr(6, SERVER::RECENTMESSAGE.length()));
                    RADAR[index]->STATE = RADAR[index]->STATE_::SUNK;
                    RADAR[index]->ID += "\nSUNK";
                    SERVER::RECENTMESSAGE = "";
                }
            }
        };        
    }

    void GENERATE_RADAR() {
        string LETTERS[10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
        cout << "Generating Radar" << endl;
        for (int i = 0, u = 0, L = -1; i != 121; i++) {
            RADAR[i] = new RADAR_;
            string ID;
            if (i == 0) {
                ID = "INV";
            }

            else if (u == 0) {
                ID = LETTERS[L];
            }

            else if (L == -1) {
                ID = to_string(u);
            }            

            else {             
                if (L != -1)
                    ID = LETTERS[L] + "-" + to_string(u);                              
            };

            

            RADAR[i]->ID = ID;

            if (u == 10) {
                u = 0;
                L++;
            }

            else {
                u++;
            }            
            cout << RADAR[i]->ID << endl;
        };
        
    }

    void GENERATE_FLEET() {
        string LETTERS[10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
        cout << "Generating Fleet..." << endl;
        for (int i = 0, u = 0, L = -1; i != 121; i++) {
            FLEET[i] = new FLEET_;
            string ID;
            if (i == 0) {
                ID = "INV";
            }

            else if (u == 0) {
                ID = LETTERS[L];
            }

            else if (L == -1) {
                ID = to_string(u);
            }
            //register carriers
            else if (SHIPS->CARRIER->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->CARRIER->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->CARRIER->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->CARRIER->LOCATION[3] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->CARRIER->LOCATION[4] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            //register battleships
            else if (SHIPS->BATTLESHIP->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[3] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            //register Destroyers
            else if (SHIPS->DESTROYER->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->DESTROYER->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->DESTROYER->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            //register Submarines
            else if (SHIPS->SUBMARINE->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->SUBMARINE->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->SUBMARINE->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            //register patrol boats
            else if (SHIPS->PATROL_BOAT->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nPATR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else if (SHIPS->PATROL_BOAT->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nPATR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
            }

            else {
                if (L != -1)
                    ID = LETTERS[L] + "-" + to_string(u);
            };





            FLEET[i]->ID = ID;

            if (u == 10) {
                u = 0;
                L++;
            }

            else {
                u++;
            }
            cout << FLEET[i]->ID << endl;
        };

    }
    
};

PLAYER* PLAYER_1;
PLAYER* PLAYER_2;
PLAYER* POINTER;
PLAYER* ENEMY_POINTER;

int setup() {
    cout << "Are you hosting a game or joining a game? h (host) / j (join)" << endl;
    char inp = ' ';
    cin >> inp;

    if (inp == 'j') {
        system("cls");
        cout << "What is the ip address of the host?" << endl;
        //192.168.86.195
        cin >> IP;
        return 0;
    }
    else if (inp == 'h') {
        system("cls");
        cout << "Setting up..." << endl;
        is_host = true;
        return 0;
    }
    else {
        system("cls");
        return setup();
    }
}

void launch_comms(bool host_client) {
    if (host_client == true) {
        thread host(SERVER::START);
        host.join();
    }

    else if (host_client == false) {
        thread client(CLIENT::START, IP);
        client.join();
    };
}

void get_name() {
    if (is_host == true) {
        SERVER::SEND("NAME:" + POINTER->name);
        do {
            Sleep(1000);
        } while (SERVER::RECENTMESSAGE == "");

        PLAYER_2->name = SERVER::RECENTMESSAGE.substr(5, SERVER::RECENTMESSAGE.length());
        SERVER::RECENTMESSAGE = "";
        cout << "Player 2 name: " << PLAYER_2->name << endl;
    }
    else {
        CLIENT::SEND("NAME:" + POINTER->name);
        do {
            Sleep(1000);
        } while (CLIENT::RECENTMESSAGE == "");

        PLAYER_2->name = CLIENT::RECENTMESSAGE.substr(5, CLIENT::RECENTMESSAGE.length());
        CLIENT::RECENTMESSAGE = "";
        cout << "Player 1 name: " << PLAYER_2->name << endl;
    };
}

// Main code
int main(int, char**)
{

    thread th(setup);
    th.join();

    PLAYER_1 = new PLAYER;
    PLAYER_1->is_turn = true;
    PLAYER_2 = new PLAYER;

    if (is_host == true) {
        POINTER = PLAYER_1;
        ENEMY_POINTER = PLAYER_2;
    }
    else if (is_host == false) {
        POINTER = PLAYER_2;
        ENEMY_POINTER = PLAYER_1;
    };

    cout << "Please enter a name" << endl;
    string name_inp;
    cin >> name_inp;

    POINTER->name = name_inp;

    thread comms(launch_comms, is_host);

    system("cls");
    if (is_host == true) {
        do {
            cout << "Waiting for connection";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            system("cls");
        } while (SERVER::CONNECTED_TO_CLIENT == false);
    }
    else if (is_host == false) {
        do {
            cout << "Waiting for connection";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            system("cls");
        } while (CLIENT::CONNECTED_TO_SERVER == false);
    }
    cout << "Connected!" << endl << "Starting..." << endl;
    cout << "Getting other player's name..." << endl;
    thread NAME_TH(get_name);
    NAME_TH.join();
    POINTER->GENERATE_RADAR();

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Battleship"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Battleship"), WS_OVERLAPPEDWINDOW, 100, 100, MAINWINDOW().WIDTH, MAINWINDOW().HEIGHT, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_save_window = false;
    bool show_game_window = false;
    bool show_config_window = true;
    bool placed_ships = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static char INPUT[5][1250] = { {"12"}, {"23"}, {"34"}, {"45"}, {"56"} };
    
    
    

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        /*
        ImGui::PushStyleColor(ImGuiCol_TitleBg, STYLE().color_background);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, STYLE().color_main);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, STYLE().color_slider);

        ImGui::PushStyleColor(ImGuiCol_Button, STYLE().color_main);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, STYLE().color_active);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, STYLE().color_slider);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, STYLE().color_slider);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, STYLE().color_slider);

        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, STYLE().color_main);

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, STYLE().color_main);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, STYLE().color_active);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, STYLE().color_main);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, STYLE().color_active);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, STYLE().color_active);

        ImGui::PushStyleColor(ImGuiCol_ResizeGrip, STYLE().color_main);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, STYLE().color_active);
        ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, STYLE().color_active);
        */

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window == false)
            ImGui::ShowDemoWindow(&show_demo_window);

        //Player's fleet show_game_window == true && show_config_window == false && 
        if (show_game_window == true && show_config_window == false && SERVER::IS_STARTED == true && CLIENT::IS_STARTED == true) {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowPos(ImVec2(PLAYER_VIEW().X, PLAYER_VIEW().Y));
            ImGui::SetNextWindowSize(ImVec2(PLAYER_VIEW().WIDTH, PLAYER_VIEW().HEIGHT));

            ImGui::Begin((POINTER->name + "'s fleet").c_str(), (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
            ImGui::Columns(11, NULL, false);

            string name;
            for (int i = 0; i != 121; i++) {
                name = POINTER->FLEET[i]->ID;
                if (name == "INV") {
                    ImGui::InvisibleButton(" ", POINTER->FLEET[i]->SIZE);
                }

                else {
                    if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::HIT) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().HIT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().HIT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().HIT);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {
                            if (POINTER->is_turn == true) {
                                POINTER->BUTTON_FUNC(POINTER->FLEET[i]->ID, i);
                            }
                        };
                        ImGui::PopStyleColor();
                    }

                    else if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::MISS) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().MISS);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().MISS);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().MISS);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {
                            if (POINTER->is_turn == true) {
                                POINTER->BUTTON_FUNC(POINTER->FLEET[i]->ID, i);
                            }
                        };
                        ImGui::PopStyleColor();
                    }

                    else if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::BOAT) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().BOAT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().BOAT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().BOAT);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {
                            if (POINTER->is_turn == true) {
                                POINTER->BUTTON_FUNC(POINTER->FLEET[i]->ID, i);
                            }
                        };
                        ImGui::PopStyleColor();
                    }

                    else {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().DEFAULT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().DEFAULT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().DEFAULT);
                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {
                            if (POINTER->is_turn == true) {
                                POINTER->BUTTON_FUNC(POINTER->FLEET[i]->ID, i);
                            }
                        };
                        ImGui::PopStyleColor();
                    };
                }
                ImGui::NextColumn();
            };
            ImGui::End();
        }


        //Radar
        if (show_game_window == true && show_config_window == false && SERVER::IS_STARTED == true && CLIENT::IS_STARTED == true) {
        ImGui::SetNextWindowPos(ImVec2(PLAYER_RADAR().X, PLAYER_RADAR().Y));
        ImGui::SetNextWindowSize(ImVec2(PLAYER_RADAR().WIDTH, PLAYER_RADAR().HEIGHT));

        ImGui::Begin("Radar", (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
        ImGui::Columns(11, NULL, false);

        string name;
        for (int i = 0; i != 121; i++) {
            name = POINTER->RADAR[i]->ID;
            if (name == "INV") {
                ImGui::InvisibleButton("INV", POINTER->RADAR[i]->SIZE);
            }

            else {
                if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::HIT) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().HIT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().HIT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().HIT);

                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i);
                        }
                    };
                    ImGui::PopStyleColor();
                }

                else if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::MISS) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().MISS);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().MISS);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().MISS);

                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i);
                        }
                    };
                    ImGui::PopStyleColor();
                }

                else {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().DEFAULT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().DEFAULT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().DEFAULT);
                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i);
                        }
                    };
                    ImGui::PopStyleColor();
                };
            }
            ImGui::NextColumn();
        };

        ImGui::End();

        }

        //Waiting window
        if (show_game_window == true && show_config_window == false && (SERVER::IS_STARTED == false || CLIENT::IS_STARTED == false)) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, (ImGui::GetIO().DisplaySize.y / 2) - 100));
            ImGui::SetNextWindowSize(ImVec2(400, 100));

            ImGui::Begin("Waiting...", (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            string waiting = "Waiting for " + ENEMY_POINTER->name + " to start...";
            ImGui::Text(waiting.c_str());
            ImGui::End();
        }

        //Config window
        if (show_config_window) {
            ImGui::SetNextWindowPos(ImVec2(PLAYER_RADAR().X, PLAYER_RADAR().Y));
            ImGui::SetNextWindowSize(ImVec2(PLAYER_VIEW().WIDTH, PLAYER_VIEW().HEIGHT));
            ImGui::Begin("Config");            
            if (placed_ships == false) {
                ImGui::Text((POINTER->name + "'s ships").c_str());
                ImGui::Text((POINTER->SHIPS->CARRIER->NAME).c_str());
                ImGui::InputText("Carrier pos", INPUT[0], IM_ARRAYSIZE(INPUT[0]));
                if (ImGui::Button("Horizontal")) {
                    POINTER->SHIPS->CARRIER->PLACER->ROTATION = POINTER->SHIPS->CARRIER->PLACER->ROTATION_::HORIZONTAL;
                }

                if (ImGui::Button("Vertical")) {
                    POINTER->SHIPS->CARRIER->PLACER->ROTATION = POINTER->SHIPS->CARRIER->PLACER->ROTATION_::VERTICAL;
                }
                //##############################
                ImGui::InputText("Battleship pos", INPUT[1], IM_ARRAYSIZE(INPUT[1]));
                if (ImGui::Button("Horizontal ")) {
                    POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION = POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION_::HORIZONTAL;
                }

                if (ImGui::Button("Vertical ")) {
                    POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION = POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION_::VERTICAL;
                }
                //##############################
                ImGui::InputText("Destroyer pos", INPUT[2], IM_ARRAYSIZE(INPUT[2]));
                if (ImGui::Button("Horizontal  ")) {
                    POINTER->SHIPS->DESTROYER->PLACER->ROTATION = POINTER->SHIPS->DESTROYER->PLACER->ROTATION_::HORIZONTAL;
                }

                if (ImGui::Button("Vertical  ")) {
                    POINTER->SHIPS->DESTROYER->PLACER->ROTATION = POINTER->SHIPS->DESTROYER->PLACER->ROTATION_::VERTICAL;
                }
                //##############################
                ImGui::InputText("Submarine pos", INPUT[3], IM_ARRAYSIZE(INPUT[3]));
                if (ImGui::Button("Horizontal   ")) {
                    POINTER->SHIPS->SUBMARINE->PLACER->ROTATION = POINTER->SHIPS->SUBMARINE->PLACER->ROTATION_::HORIZONTAL;
                }

                if (ImGui::Button("Vertical   ")) {
                    POINTER->SHIPS->SUBMARINE->PLACER->ROTATION = POINTER->SHIPS->SUBMARINE->PLACER->ROTATION_::VERTICAL;
                }
                //##############################
                ImGui::InputText("Patrol Boat pos", INPUT[4], IM_ARRAYSIZE(INPUT[4]));
                if (ImGui::Button("Horizontal    ")) {
                    POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION = POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION_::HORIZONTAL;
                }

                if (ImGui::Button("Vertical    ")) {
                    POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION = POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION_::VERTICAL;
                }
                
                

                if (ImGui::Button("Done?")) {
                    
                    if (POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER(stoi(INPUT[0])) == true) {
                        POINTER->SHIPS->VALID_POS[0] = true;
                        POINTER->SHIPS->CARRIER->LOCATION[0] = stoi(POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER_POS[0]);
                        POINTER->SHIPS->CARRIER->LOCATION[1] = stoi(POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER_POS[1]);
                        POINTER->SHIPS->CARRIER->LOCATION[2] = stoi(POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER_POS[2]);
                        POINTER->SHIPS->CARRIER->LOCATION[3] = stoi(POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER_POS[3]);
                        POINTER->SHIPS->CARRIER->LOCATION[4] = stoi(POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER_POS[4]);
                    }
                    else {                        
                        POINTER->SHIPS->VALID_POS[0] = false;
                        cout << "INVALID POSITION @ CARRIER" << endl;
                    };                 
                    //#####################
                    if (POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER(stoi(INPUT[1])) == true) {
                        POINTER->SHIPS->VALID_POS[1] = true;
                        POINTER->SHIPS->BATTLESHIP->LOCATION[0] = stoi(POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER_POS[0]);
                        POINTER->SHIPS->BATTLESHIP->LOCATION[1] = stoi(POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER_POS[1]);
                        POINTER->SHIPS->BATTLESHIP->LOCATION[2] = stoi(POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER_POS[2]);
                        POINTER->SHIPS->BATTLESHIP->LOCATION[3] = stoi(POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER_POS[3]);
                    }
                    else {
                        POINTER->SHIPS->VALID_POS[1] = false;
                        cout << "INVALID POSITION @ BATTLESHIP" << endl;
                    };
                    //#####################
                    if (POINTER->SHIPS->DESTROYER->PLACER->SMART_PLACER(stoi(INPUT[2])) == true) {
                        POINTER->SHIPS->VALID_POS[2] = true;
                        POINTER->SHIPS->DESTROYER->LOCATION[0] = stoi(POINTER->SHIPS->DESTROYER->PLACER->SMART_PLACER_POS[0]);
                        POINTER->SHIPS->DESTROYER->LOCATION[1] = stoi(POINTER->SHIPS->DESTROYER->PLACER->SMART_PLACER_POS[1]);
                        POINTER->SHIPS->DESTROYER->LOCATION[2] = stoi(POINTER->SHIPS->DESTROYER->PLACER->SMART_PLACER_POS[2]);
                    }
                    else {
                        POINTER->SHIPS->VALID_POS[2] = false;
                        cout << "INVALID POSITION @ DESTROYER" << endl;
                    };
                    //#####################
                    if (POINTER->SHIPS->SUBMARINE->PLACER->SMART_PLACER(stoi(INPUT[3])) == true) {
                        POINTER->SHIPS->VALID_POS[3] = true;
                        POINTER->SHIPS->SUBMARINE->LOCATION[0] = stoi(POINTER->SHIPS->SUBMARINE->PLACER->SMART_PLACER_POS[0]);
                        POINTER->SHIPS->SUBMARINE->LOCATION[1] = stoi(POINTER->SHIPS->SUBMARINE->PLACER->SMART_PLACER_POS[1]);
                        POINTER->SHIPS->SUBMARINE->LOCATION[2] = stoi(POINTER->SHIPS->SUBMARINE->PLACER->SMART_PLACER_POS[2]);
                    }
                    else {
                        POINTER->SHIPS->VALID_POS[3] = false;
                        cout << "INVALID POSITION @ SUBMARINE" << endl;
                    };
                    //#####################
                    if (POINTER->SHIPS->PATROL_BOAT->PLACER->SMART_PLACER(stoi(INPUT[4])) == true) {
                        POINTER->SHIPS->VALID_POS[4] = true;
                        POINTER->SHIPS->PATROL_BOAT->LOCATION[0] = stoi(POINTER->SHIPS->PATROL_BOAT->PLACER->SMART_PLACER_POS[0]);
                        POINTER->SHIPS->PATROL_BOAT->LOCATION[1] = stoi(POINTER->SHIPS->PATROL_BOAT->PLACER->SMART_PLACER_POS[1]);
                    }
                    else {
                        POINTER->SHIPS->VALID_POS[4] = false;
                        cout << "INVALID POSITION @ PATROL_BOAT" << endl;
                    };
                    //#####################
                    if (POINTER->SHIPS->VALID_POS[0] == true && POINTER->SHIPS->VALID_POS[1] == true &&
                        POINTER->SHIPS->VALID_POS[2] == true && POINTER->SHIPS->VALID_POS[3] == true && POINTER->SHIPS->VALID_POS[4] == true) {
                        placed_ships = true;
                        POINTER->GENERATE_FLEET();
                    }
                    
                };
            }

            else if (placed_ships == true) {
                if (ImGui::Button("Start?")) {
                    if (is_host == true) {
                        SERVER::SEND("START");
                        SERVER::IS_STARTED = true;
                    }
                    else if (is_host == false) {
                        CLIENT::SEND("START");
                        CLIENT::IS_STARTED = true;
                    }
                    show_game_window = true;
                    show_config_window = false;
                };
            };
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    comms.join();
    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}