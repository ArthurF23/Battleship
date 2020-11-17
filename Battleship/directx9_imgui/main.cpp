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

struct INFO_WINDOW {
    int WIDTH = ImGui::GetIO().DisplaySize.x - PLAYER_RADAR().WIDTH;
    int HEIGHT = ImGui::GetIO().DisplaySize.y - PLAYER_RADAR().HEIGHT;

    int X = 0;
    int Y = PLAYER_RADAR().HEIGHT + 10;
};

struct COLOR {
    ImVec4 HIT = ImVec4(255.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 MISS = ImVec4(0.255f, 0.255f, 0.255f, 1.0f);
    ImVec4 SUNK = ImVec4(0.053f, 0.053f, 0.105f, 1.0f);
    ImVec4 BOAT = ImVec4(0.040f, 0.050f, 0.169f, 1.0f);
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

    bool OVERLAP_FORBIDDEN_DETECT(int LOCATION, int INCREMENT_1, int INCREMENT_2, int INCREMENT_3, int INCREMENT_4, int LENGTH_) {
        bool BAD_POS = false;
        
        for (int i = 0; i < 21; i++) {
            if (LOCATION + INCREMENT_1 == FORBIDDEN_BUTTONS[i]) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_2 == FORBIDDEN_BUTTONS[i] && LENGTH_ >= 3) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_3 == FORBIDDEN_BUTTONS[i] && LENGTH_ >= 4) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_4 == FORBIDDEN_BUTTONS[i] && LENGTH_ == 5) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_1 > 120) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_2 > 120 && LENGTH_ >= 3) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_3 > 120 && LENGTH_ >= 4) {
                BAD_POS = true;
                break;
            }

            else if (LOCATION + INCREMENT_4 > 120 && LENGTH_ == 5) {
                BAD_POS = true;
                break;
            }
        };       

        return BAD_POS;
    }

    bool COLLISION_DETECTION(int LOCATION, int INCREMENT, int LENGTH_,
        int CARRIER_POS[5], int BATTLESHIP_POS[4], int DESTROYER_POS[3], int SUBMARINE_POS[3], int PATROL_BOAT_POS[2], string SHIP_NAME) {
        //This is to check to make sure that the ships are not overlapping
        /*I can use almost LENGTH_ to figure out what ship it is... but the destroyer and submarine have the same length so i may have to add something
        to determine what boat im dealing with if needed*/
        cout << SHIP_NAME << " INCREMENT = " << INCREMENT << endl;
        bool COLLISION = false;


        if (SHIP_NAME == "Carrier") {
            cout << SHIP_NAME << endl;

            for (int i = 0, inc = 0; i < 5; i++, inc += INCREMENT) {

                cout << LOCATION + inc << endl;
                for (int a = 0; a < 4; a++) {
                    if (LOCATION + inc == BATTLESHIP_POS[a]) {
                        cout << "CARRIER INTERSECTS BATTLESHIP" << endl;
                        COLLISION = true;
                        break;
                    };
                };

                for (int b = 0; b < 3; b++) {
                    if (LOCATION + inc == DESTROYER_POS[b]) {
                        cout << "CARRIER INTERSECTS DESTROYER" << endl;
                        COLLISION = true;
                    }
                    else if (LOCATION + inc == SUBMARINE_POS[b]) {
                        cout << "CARRIER INTERSECTS SUBMARINE" << endl;
                        COLLISION = true;
                        break;
                    };
                };

                for (int c = 0; c < 2; c++) {
                    if (LOCATION + inc == PATROL_BOAT_POS[c]) {
                        cout << "CARRIER INTERSECTS PATROL BOAT" << endl;
                        COLLISION = true;
                        break;
                    };
                };

                if (COLLISION == true) {
                    break;
                };

            };
        }

        if (SHIP_NAME == "Battleship") {
            cout << SHIP_NAME << endl;
            for (int i = 0, inc = 0; i < 4; i++, inc += INCREMENT) {
                for (int a = 0; a < 5; a++) {

                    cout << LOCATION + inc << endl;

                    if (LOCATION + inc == CARRIER_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS CARRIER" << endl;
                        COLLISION = true;
                        break;
                    }
                }
                

                for (int a = 0; a < 3; a++) {
                    if (LOCATION + inc == DESTROYER_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS DESTROYER" << endl;
                        COLLISION = true;
                        break;
                    }
                    else if (LOCATION + inc == SUBMARINE_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS SUBMARINE" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 2; a++) {
                    if (LOCATION + inc == PATROL_BOAT_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS PATROL BOAT" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                if (COLLISION == true) {
                    break;
                }
            }

        }

        if (SHIP_NAME == "Destroyer") {
            cout << SHIP_NAME << endl;
            for (int i = 0, inc = 0; i < 3; i++, inc += INCREMENT) {
                for (int a = 0; a < 5; a++) {

                    cout << LOCATION + inc << endl;

                    if (LOCATION + inc == CARRIER_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS CARRIER" << endl;
                        COLLISION = true;
                        break;
                    }                   
                }

                for (int a = 0; a < 4; a++) {
                    if (LOCATION + inc == BATTLESHIP_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS BATTLESHIP" << endl;
                        COLLISION = true;
                        break;
                    };
                };

                for (int a = 0; a < 3; a++) {                    
                    if (LOCATION + inc == SUBMARINE_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS SUBMARINE" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 2; a++) {
                    if (LOCATION + inc == PATROL_BOAT_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS PATROL BOAT" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                if (COLLISION == true) {
                    break;
                }
            }

        }

        if (SHIP_NAME == "Submarine") {
            cout << SHIP_NAME << endl;
            for (int i = 0, inc = 0; i < 3; i++, inc += INCREMENT) {
                for (int a = 0; a < 5; a++) {

                    cout << LOCATION + inc << endl;

                    if (LOCATION + inc == CARRIER_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS CARRIER" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 4; a++) {
                    if (LOCATION + inc == BATTLESHIP_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS BATTLESHIP" << endl;
                        COLLISION = true;
                        break;
                    };
                };

                for (int a = 0; a < 3; a++) {
                    if (LOCATION + inc == DESTROYER_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS DESTROYER" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 2; a++) {
                    if (LOCATION + inc == PATROL_BOAT_POS[a]) {
                        cout << SHIP_NAME << " INTERSECTS PATROL BOAT" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                if (COLLISION == true) {
                    break;
                }
            }

        }
        
        
        if (SHIP_NAME == "Patrol Boat") {
            cout << SHIP_NAME << endl;
            for (int i = 0, inc = 0; i < 2; i++, inc += INCREMENT) {
                for (int a = 0; a < 5; a++) {

                    cout << LOCATION + inc << endl;

                    if (LOCATION + inc == CARRIER_POS[a]) {
                        cout << "PATROL BOAT INTERSECTS CARRIER" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 4; a++) {
                    if (LOCATION + inc == BATTLESHIP_POS[a]) {
                        cout << "PATROL BOAT INTERSECTS CARRIER" << endl;
                        COLLISION = true;
                        break;
                    }
                }

                for (int a = 0; a < 3; a++) {
                    if (LOCATION + inc == DESTROYER_POS[a]) {
                        cout << "PATROL BOAT INTERSECTS DESTROYER" << endl;
                        COLLISION = true;
                        break;
                    }
                    else if (LOCATION + inc == SUBMARINE_POS[a]) {
                        cout << "PATROL BOAT INTERSECTS SUBMARINE" << endl;
                        COLLISION = true;
                        break;
                    }
                }
                if (COLLISION == true) {
                    break;
                }
            }
        };


        return COLLISION;
    };


    bool SMART_PLACER(int LOCATION, int CARRIER_POS[5], int BATTLESHIP_POS[4],
        int DESTROYER_POS[3], int SUBMARINE_POS[3], int PATROL_BOAT_POS[2], string SHIP_NAME) {
        if (ROTATION == ROTATION_::HORIZONTAL && 
            OVERLAP_FORBIDDEN_DETECT(LOCATION, 1, 2, 3, 4, LENGTH) == false &&
            COLLISION_DETECTION(LOCATION, 1, LENGTH, CARRIER_POS, BATTLESHIP_POS, DESTROYER_POS, SUBMARINE_POS, PATROL_BOAT_POS, SHIP_NAME) == false) {
            SMART_PLACER_POS[0] = to_string(LOCATION);
            SMART_PLACER_POS[1] = to_string(LOCATION + 1);
            SMART_PLACER_POS[2] = to_string(LOCATION + 2);
            SMART_PLACER_POS[3] = to_string(LOCATION + 3);
            SMART_PLACER_POS[4] = to_string(LOCATION + 4);
            return true;
        }

        else if (ROTATION == ROTATION_::VERTICAL && OVERLAP_FORBIDDEN_DETECT(LOCATION, 11, 22, 33, 44, LENGTH) == false &&
            COLLISION_DETECTION(LOCATION, 11, LENGTH, CARRIER_POS, BATTLESHIP_POS, DESTROYER_POS, SUBMARINE_POS, PATROL_BOAT_POS, SHIP_NAME) == false) {
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
    };
};

struct CARRIER_ {
    string NAME = "Carrier";
    const static int LENGTH = 5;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false, false, false };
    bool SUNK_DISPLAYED = false;

    bool CHECK_SUNK() {
        int counter = 0;

        for (int i = 0; i < LENGTH; i++) {
            if (HOLES_HIT[i] == true) {
                counter++;
            }
        }

        if (counter == LENGTH) {
            return true;
        }
        else {
            return false;
        };
    };

    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct BATTLESHIP_ {
    string NAME = "Battleship";
    const static int LENGTH = 4;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false, false };
    bool SUNK_DISPLAYED = false;

    bool CHECK_SUNK() {
        int counter = 0;

        for (int i = 0; i < LENGTH; i++) {
            if (HOLES_HIT[i] == true) {
                counter++;
            }
        }

        if (counter == LENGTH) {
            return true;
        }
        else {
            return false;
        };
    };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct DESTROYER_ {
    string NAME = "Destroyer";
    const static int LENGTH = 3;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false };
    bool SUNK_DISPLAYED = false;

    bool CHECK_SUNK() {
        int counter = 0;

        for (int i = 0; i < LENGTH; i++) {
            if (HOLES_HIT[i] == true) {
                counter++;
            }
        }

        if (counter == LENGTH) {
            return true;
        }
        else {
            return false;
        };
    };

    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct SUBMARINE_ {
    string NAME = "Submarine";
    const static int LENGTH = 3;

    int LOCATION[LENGTH];
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false, false };
    bool SUNK_DISPLAYED = false;

    bool CHECK_SUNK() {
        int counter = 0;

        for (int i = 0; i < LENGTH; i++) {
            if (HOLES_HIT[i] == true) {
                counter++;
            }
        }

        if (counter == LENGTH) {
            return true;
        }
        else {
            return false;
        };
    };
    PLACER_* PLACER = new PLACER_(LENGTH);
};

struct PATROL_BOAT_ {
    string NAME = "Patrol Boat";
    const static int LENGTH = 2;

    int LOCATION[LENGTH] = { 0, 0 };
    bool IS_DESTROYED = false;
    bool HOLES_HIT[LENGTH] = { false, false };
    bool SUNK_DISPLAYED = false;

    bool CHECK_SUNK() {
        int counter = 0;

        for (int i = 0; i < LENGTH; i++) {
            if (HOLES_HIT[i] == true) {
                counter++;
            }
        }

        if (counter == LENGTH) {
            return true;
        }
        else {
            return false;
        };
    };
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

    atomic<STATE_> STATE = STATE_::UNCLICKED;
};

struct FLEET_ {
    string ID;
    string SHIP_NAME;
    int SHIP_CHUNK;
    ImVec2 SIZE = ImVec2(50, 50);
    enum STATE_ {
        UNCLICKED,
        MISS,
        HIT,
        BOAT,
        SUNK
    };

    atomic<STATE_> STATE = STATE_::UNCLICKED;
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


    static int UPDATE_SHIP(PLAYER* PTR, int arr_pos) {
        //Update the holes hit...

        //Carrier
        if (PTR->FLEET[arr_pos]->SHIP_NAME == PTR->SHIPS->CARRIER->NAME) {
            PTR->SHIPS->CARRIER->HOLES_HIT[PTR->FLEET[arr_pos]->SHIP_CHUNK] = true;
        }
        //Battleship
        else if (PTR->FLEET[arr_pos]->SHIP_NAME == PTR->SHIPS->BATTLESHIP->NAME) {
            PTR->SHIPS->BATTLESHIP->HOLES_HIT[PTR->FLEET[arr_pos]->SHIP_CHUNK] = true;
        }
        //Destroyer
        else if (PTR->FLEET[arr_pos]->SHIP_NAME == PTR->SHIPS->DESTROYER->NAME) {
            PTR->SHIPS->DESTROYER->HOLES_HIT[PTR->FLEET[arr_pos]->SHIP_CHUNK] = true;
        }
        //Submarine
        else if (PTR->FLEET[arr_pos]->SHIP_NAME == PTR->SHIPS->SUBMARINE->NAME) {
            PTR->SHIPS->SUBMARINE->HOLES_HIT[PTR->FLEET[arr_pos]->SHIP_CHUNK] = true;
        }
        //Patrol Boat
        else if (PTR->FLEET[arr_pos]->SHIP_NAME == PTR->SHIPS->PATROL_BOAT->NAME) {
            PTR->SHIPS->PATROL_BOAT->HOLES_HIT[PTR->FLEET[arr_pos]->SHIP_CHUNK] = true;
        }

        //Check Gameover...
        if (PTR->SHIPS->CARRIER->CHECK_SUNK() == true && PTR->SHIPS->BATTLESHIP->CHECK_SUNK() == true &&
            PTR->SHIPS->DESTROYER->CHECK_SUNK() == true && PTR->SHIPS->SUBMARINE->CHECK_SUNK() == true &&
            PTR->SHIPS->PATROL_BOAT->CHECK_SUNK() == true) {
            PTR->LOST = true;
            return -1;
        }
        //Check to see if anything is sunk...
        //Carrier
        if (PTR->SHIPS->CARRIER->CHECK_SUNK() == true && PTR->SHIPS->CARRIER->SUNK_DISPLAYED != true) {
            for (int i = 0; i < 5; i++) {
                PTR->FLEET[PTR->SHIPS->CARRIER->LOCATION[i]]->STATE = PTR->FLEET[PTR->SHIPS->CARRIER->LOCATION[i]]->STATE_::SUNK;
            };
            return PTR->SHIPS->CARRIER->LOCATION[0];
        }
        //Battleship
        else if (PTR->SHIPS->BATTLESHIP->CHECK_SUNK() == true && PTR->SHIPS->BATTLESHIP->SUNK_DISPLAYED != true) {
            for (int i = 0; i < 4; i++) {
                PTR->FLEET[PTR->SHIPS->BATTLESHIP->LOCATION[i]]->STATE = PTR->FLEET[PTR->SHIPS->BATTLESHIP->LOCATION[i]]->STATE_::SUNK;
            };
            return PTR->SHIPS->BATTLESHIP->LOCATION[0];
        }
        //Destroyer
        else if (PTR->SHIPS->DESTROYER->CHECK_SUNK() == true && PTR->SHIPS->DESTROYER->SUNK_DISPLAYED != true) {
            for (int i = 0; i < 3; i++) {
                PTR->FLEET[PTR->SHIPS->DESTROYER->LOCATION[i]]->STATE = PTR->FLEET[PTR->SHIPS->DESTROYER->LOCATION[i]]->STATE_::SUNK;
            };
            return PTR->SHIPS->DESTROYER->LOCATION[0];
        }
        //Submarine
        else if (PTR->SHIPS->SUBMARINE->CHECK_SUNK() == true && PTR->SHIPS->SUBMARINE->SUNK_DISPLAYED != true) {
            for (int i = 0; i < 3; i++) {
                PTR->FLEET[PTR->SHIPS->SUBMARINE->LOCATION[i]]->STATE = PTR->FLEET[PTR->SHIPS->SUBMARINE->LOCATION[i]]->STATE_::SUNK;
            };
            return PTR->SHIPS->SUBMARINE->LOCATION[0];
        }
        //Patrol Boat
        else if (PTR->SHIPS->PATROL_BOAT->CHECK_SUNK() == true && PTR->SHIPS->PATROL_BOAT->SUNK_DISPLAYED != true) {
            PTR->SHIPS->PATROL_BOAT->SUNK_DISPLAYED = true;
            for (int i = 0; i < 2; i++) {
                PTR->FLEET[PTR->SHIPS->PATROL_BOAT->LOCATION[i]]->STATE = PTR->FLEET[PTR->SHIPS->PATROL_BOAT->LOCATION[i]]->STATE_::SUNK;
            };
            return PTR->SHIPS->PATROL_BOAT->LOCATION[0];
        }

        else { return 0; };
    };

    static void WAIT_TURN(PLAYER* PTR) {
        if (is_host == true) {
            do {
                Sleep(1000);
            } while (SERVER::RECENTMESSAGE.substr(0, 2) != "F@");
            cout << SERVER::RECENTMESSAGE.substr(2, SERVER::RECENTMESSAGE.length()) << endl;
            int arr_pos = stoi(SERVER::RECENTMESSAGE.substr(2, SERVER::RECENTMESSAGE.length()));

            if (PTR->FLEET[arr_pos]->STATE == PTR->FLEET[arr_pos]->STATE_::BOAT) {
                cout << "HIT" << endl;                

                int update_ships = PTR->UPDATE_SHIP(PTR, arr_pos);

                if (update_ships == -1) {
                    SERVER::SEND("R@LOST");
                }

                else if (update_ships == 0) {
                    SERVER::SEND("R@HIT" + to_string(arr_pos));
                    PTR->FLEET[arr_pos]->STATE = PTR->FLEET[arr_pos]->STATE_::HIT;
                }

                else {
                    cout << "SUNK" << endl;
                    int length;
                    string rotation;
                    string name = PTR->FLEET[arr_pos]->SHIP_NAME;
                    if (name == "Carrier") {
                        length = 5;
                        PTR->SHIPS->CARRIER->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->CARRIER->PLACER->ROTATION == PTR->SHIPS->CARRIER->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Battleship") {
                        length = 4;
                        PTR->SHIPS->BATTLESHIP->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->BATTLESHIP->PLACER->ROTATION == PTR->SHIPS->BATTLESHIP->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Destroyer") {
                        length = 3;
                        PTR->SHIPS->DESTROYER->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->DESTROYER->PLACER->ROTATION == PTR->SHIPS->DESTROYER->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Submarine") {
                        length = 3;
                        PTR->SHIPS->SUBMARINE->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->SUBMARINE->PLACER->ROTATION == PTR->SHIPS->SUBMARINE->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else {
                        length = 2;
                        PTR->SHIPS->PATROL_BOAT->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->PATROL_BOAT->PLACER->ROTATION == PTR->SHIPS->PATROL_BOAT->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    SERVER::SEND("R@SUNK@" + rotation + to_string(length) + to_string(update_ships));
                }                
            }

            else if (PTR->FLEET[arr_pos]->STATE == PTR->FLEET[arr_pos]->STATE_::UNCLICKED) {
                cout << "MISS" << endl;
                SERVER::SEND("R@MISS" + to_string(arr_pos));
                PTR->FLEET[arr_pos]->STATE = PTR->FLEET[arr_pos]->STATE_::MISS;
                PTR->UPDATE_SHIP(PTR, arr_pos);
            }

            SERVER::RECENTMESSAGE = "";
        }
        else {
            do {
                Sleep(1000);
            } while (CLIENT::RECENTMESSAGE.substr(0, 2) != "F@");
            cout << CLIENT::RECENTMESSAGE.substr(2, CLIENT::RECENTMESSAGE.length()) << endl;
            int arr_pos = stoi(CLIENT::RECENTMESSAGE.substr(2, CLIENT::RECENTMESSAGE.length()));

            if (PTR->FLEET[arr_pos]->STATE == PTR->FLEET[arr_pos]->STATE_::BOAT) {
                cout << "HIT" << endl;
                PTR->FLEET[arr_pos]->STATE = PTR->FLEET[arr_pos]->STATE_::HIT;
                int update_ships = PTR->UPDATE_SHIP(PTR, arr_pos);
                if (update_ships == -1) {
                    CLIENT::SEND("R@LOST");
                }
                else if (update_ships == 0) {
                    CLIENT::SEND("R@HIT" + to_string(arr_pos));
                }
                else {
                    int length; //Length of ship 
                    string rotation; //Rotation of ship
                    string name = PTR->FLEET[arr_pos]->SHIP_NAME;
                    if (name == "Carrier") {
                        length = 5;
                        PTR->SHIPS->CARRIER->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->CARRIER->PLACER->ROTATION == PTR->SHIPS->CARRIER->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Battleship") {
                        length = 4;
                        PTR->SHIPS->BATTLESHIP->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->BATTLESHIP->PLACER->ROTATION == PTR->SHIPS->BATTLESHIP->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Destroyer") {
                        length = 3;
                        PTR->SHIPS->DESTROYER->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->DESTROYER->PLACER->ROTATION == PTR->SHIPS->DESTROYER->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else if (name == "Submarine") {
                        length = 3;
                        PTR->SHIPS->SUBMARINE->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->SUBMARINE->PLACER->ROTATION == PTR->SHIPS->SUBMARINE->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    else {
                        length = 2;
                        PTR->SHIPS->PATROL_BOAT->SUNK_DISPLAYED = true;
                        if (PTR->SHIPS->PATROL_BOAT->PLACER->ROTATION == PTR->SHIPS->PATROL_BOAT->PLACER->ROTATION_::HORIZONTAL) {
                            rotation = "H";
                        }
                        else {
                            rotation = "V";
                        };
                    }
                    CLIENT::SEND("R@SUNK@" + rotation + to_string(length) + to_string(update_ships));
                }
            }

            else if (PTR->FLEET[arr_pos]->STATE == PTR->FLEET[arr_pos]->STATE_::UNCLICKED) {
                cout << "MISS" << endl;
                CLIENT::SEND("R@MISS" + to_string(arr_pos));
                PTR->FLEET[arr_pos]->STATE = PTR->FLEET[arr_pos]->STATE_::MISS;
                PTR->UPDATE_SHIP(PTR, arr_pos);
            }
            CLIENT::RECENTMESSAGE = "";
        }


        PTR->is_turn = true;
    }

    void BUTTON_FUNC(string BTN, int arrL, PLAYER* PTR, PLAYER* E_PTR) {
        bool is_forbidden = false;
        for (int i = 0; i < 21; i++) {
            if (arrL == RADAR[arrL]->FORBIDDEN_BUTTONS[i]) {
                is_forbidden = true;
                break;
            };
        };

        if (is_forbidden == false && is_turn == true && RADAR[arrL]->STATE == RADAR[arrL]->STATE_::UNCLICKED) {

            //Server
            if (is_host == true) {
                SERVER::RECENTMESSAGE = "";
                thread sendth(SERVER::SEND, ("F@" + to_string(arrL)));
                sendth.join();
                cout << "waiting for response..." << endl;
                do {
                    Sleep(1000);
                } while (SERVER::RECENTMESSAGE == "");
                cout << "&&& " << SERVER::RECENTMESSAGE << " &&&&" << endl;
                if (SERVER::RECENTMESSAGE.substr(0, 5) == "R@HIT") {
                    int index = stoi(SERVER::RECENTMESSAGE.substr(5, SERVER::RECENTMESSAGE.length()));
                    PTR->RADAR[index]->STATE = PTR->RADAR[index]->STATE_::HIT;
                    PTR->RADAR[index]->ID += "\nHIT";
                }

                else if (SERVER::RECENTMESSAGE.substr(0, 6) == "R@MISS") {
                    int index = stoi(SERVER::RECENTMESSAGE.substr(6, SERVER::RECENTMESSAGE.length()));
                    PTR->RADAR[index]->STATE = PTR->RADAR[index]->STATE_::MISS;
                    PTR->RADAR[index]->ID += "\nMISS";
                }

                else if (SERVER::RECENTMESSAGE.substr(0, 6) == "R@LOST") {
                    E_PTR->LOST = true;
                }


                else if (SERVER::RECENTMESSAGE.substr(0, 6) == "R@SUNK") {
                    //Decode key// roatation = @V and @H defines if vertical or horizontal
                    //R@SUNK@{rotation}{length}{location} // R@SUNK@H0521 // @H0521
                    int increment;
                    string msg = SERVER::RECENTMESSAGE.substr(6, SERVER::RECENTMESSAGE.length());
                    string rotation = msg.substr(0, 2);

                    string length_and_location = msg.substr(2, 3);
                    int length_of_sunk = stoi(length_and_location.substr(0, 1));
                    int location_of_sunk = stoi(length_and_location.substr(1, length_and_location.length()));
                    cout << "message = " << SERVER::RECENTMESSAGE << " rotation = " << rotation << " length = " << length_of_sunk << " location = " << location_of_sunk << endl;

                    if (rotation == "@V") {
                        increment = 11;
                    }

                    else if (rotation == "@H") {
                        increment = 1;
                    };

                    for (int i = 0, inc = location_of_sunk; i < length_of_sunk; i++, inc += increment) {
                        PTR->RADAR[inc]->STATE = PTR->RADAR[inc]->STATE_::SUNK;
                        PTR->RADAR[inc]->ID += "\nSUNK";
                        cout << inc << " " << PTR->RADAR[inc]->ID;
                    };

                };
                SERVER::RECENTMESSAGE = "";
                is_turn = false;
                SERVER::SEND("TURN");
                //thread T(WAIT_TURN, PTR);
                //T.join();
            }
            //Client
            else {
                thread sendth(CLIENT::SEND, ("F@" + to_string(arrL)));
                sendth.join();
                cout << "waiting for response..." << endl;
                do {
                    Sleep(1000);
                } while (CLIENT::RECENTMESSAGE == "");
                if (CLIENT::RECENTMESSAGE.substr(0, 5) == "R@HIT") {
                    int index = stoi(CLIENT::RECENTMESSAGE.substr(5, CLIENT::RECENTMESSAGE.length()));
                    RADAR[index]->STATE = RADAR[index]->STATE_::HIT;
                    RADAR[index]->ID += "\nHIT";
                }

                else if (CLIENT::RECENTMESSAGE.substr(0, 6) == "R@MISS") {
                    int index = stoi(CLIENT::RECENTMESSAGE.substr(6, CLIENT::RECENTMESSAGE.length()));
                    RADAR[index]->STATE = RADAR[index]->STATE_::MISS;
                    RADAR[index]->ID += "\nMISS";
                }


                else if (CLIENT::RECENTMESSAGE.substr(0, 6) == "R@SUNK") {
                    //Decode key// roatation = @V and @H defines if vertical or horizontal
                    //R@SUNK@{rotation}{length}{location} // R@SUNK@H0521 // @H0521
                    int increment;
                    string msg = CLIENT::RECENTMESSAGE.substr(6, CLIENT::RECENTMESSAGE.length());
                    string rotation = msg.substr(0, 2);

                    string length_and_location = msg.substr(2, 3);
                    int length_of_sunk = stoi(length_and_location.substr(0, 1));
                    int location_of_sunk = stoi(length_and_location.substr(1, length_and_location.length()));
                    cout << "message = " << CLIENT::RECENTMESSAGE << " rotation = " << rotation << " length = " << length_of_sunk << " location = " << location_of_sunk << endl;

                    if (rotation == "@V") {
                        increment = 11;
                    }

                    else if (rotation == "@H") {
                        increment = 1;
                    };

                    for (int i = 0, inc = location_of_sunk; i < length_of_sunk; i++, inc += increment) {
                        PTR->RADAR[inc]->STATE = PTR->RADAR[inc]->STATE_::SUNK;
                    };

                };
                CLIENT::RECENTMESSAGE = "";
                is_turn = false;
                CLIENT::SEND("TURN");
                //thread T(WAIT_TURN, PTR);
                //T.join();
                //is_turn = false;
            }
        };        
    };

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
            cout << "ARR POS -> " << i << " ID ->  " << RADAR[i]->ID << endl;
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
                FLEET[i]->SHIP_NAME = SHIPS->CARRIER->NAME;
                FLEET[i]->SHIP_CHUNK = 0;
            }

            else if (SHIPS->CARRIER->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->CARRIER->NAME;
                FLEET[i]->SHIP_CHUNK = 1;
            }

            else if (SHIPS->CARRIER->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->CARRIER->NAME;
                FLEET[i]->SHIP_CHUNK = 2;
            }

            else if (SHIPS->CARRIER->LOCATION[3] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->CARRIER->NAME;
                FLEET[i]->SHIP_CHUNK = 3;
            }

            else if (SHIPS->CARRIER->LOCATION[4] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nCARR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->CARRIER->NAME;
                FLEET[i]->SHIP_CHUNK = 4;
            }

            //register battleships
            else if (SHIPS->BATTLESHIP->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->BATTLESHIP->NAME;
                FLEET[i]->SHIP_CHUNK = 0;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->BATTLESHIP->NAME;
                FLEET[i]->SHIP_CHUNK = 1;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->BATTLESHIP->NAME;
                FLEET[i]->SHIP_CHUNK = 2;
            }

            else if (SHIPS->BATTLESHIP->LOCATION[3] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nBATT-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->BATTLESHIP->NAME;
                FLEET[i]->SHIP_CHUNK = 3;
            }

            //register Destroyers
            else if (SHIPS->DESTROYER->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->DESTROYER->NAME;
                FLEET[i]->SHIP_CHUNK = 0;
            }

            else if (SHIPS->DESTROYER->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->DESTROYER->NAME;
                FLEET[i]->SHIP_CHUNK = 1;
            }

            else if (SHIPS->DESTROYER->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nDEST-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->DESTROYER->NAME;
                FLEET[i]->SHIP_CHUNK = 2;
            }

            //register Submarines
            else if (SHIPS->SUBMARINE->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->SUBMARINE->NAME;
                FLEET[i]->SHIP_CHUNK = 0;
            }

            else if (SHIPS->SUBMARINE->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->SUBMARINE->NAME;
                FLEET[i]->SHIP_CHUNK = 1;
            }

            else if (SHIPS->SUBMARINE->LOCATION[2] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nSUB-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->SUBMARINE->NAME;
                FLEET[i]->SHIP_CHUNK = 2;
            }

            //register patrol boats
            else if (SHIPS->PATROL_BOAT->LOCATION[0] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nPATR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->PATROL_BOAT->NAME;
                FLEET[i]->SHIP_CHUNK = 0;
            }

            else if (SHIPS->PATROL_BOAT->LOCATION[1] == i) {
                ID = LETTERS[L] + "-" + to_string(u) + "\nPATR-";
                FLEET[i]->STATE = FLEET[i]->STATE_::BOAT;
                FLEET[i]->SHIP_NAME = SHIPS->PATROL_BOAT->NAME;
                FLEET[i]->SHIP_CHUNK = 1;
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

        ENEMY_POINTER->name = SERVER::RECENTMESSAGE.substr(5, SERVER::RECENTMESSAGE.length());
        SERVER::RECENTMESSAGE = "";
        cout << "Player 2 name: " << ENEMY_POINTER->name << endl;
    }
    else {
        int inc = 0;
        CLIENT::SEND("NAME:" + POINTER->name);
        do {
            inc++;
            Sleep(1000);
            if (inc == 30) {
                break;
            };
        } while (CLIENT::RECENTMESSAGE == "");

        ENEMY_POINTER->name = CLIENT::RECENTMESSAGE.substr(5, CLIENT::RECENTMESSAGE.length());
        CLIENT::RECENTMESSAGE = "";
        cout << "Player 1 name: " << ENEMY_POINTER->name << endl;
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
            system("ipconfig");
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
    bool pass_def = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static char INPUT[5][1250] = { {"A-1"}, {"B-1"}, {"C-1"}, {"D-1"}, {"E-1"} };
    
    
    

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

        if (POINTER->LOST == true || ENEMY_POINTER->LOST == true) {
            ImGui::Begin("Gameover", (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            string TEXT;
            if (POINTER->LOST == true) {
                TEXT = "You lost...";
            }
            else {
                TEXT = ENEMY_POINTER->name + " lost...";
            }
            ImGui::Text(TEXT.c_str());
            ImGui::End();
        }

        //Turn Window
        if (show_game_window == true && show_config_window == false && SERVER::IS_STARTED == true && CLIENT::IS_STARTED == true && POINTER->LOST == false && ENEMY_POINTER->LOST == false) {
            ImGui::SetNextWindowPos(ImVec2(INFO_WINDOW().X, INFO_WINDOW().Y));
            ImGui::SetNextWindowSize(ImVec2(INFO_WINDOW().WIDTH, INFO_WINDOW().HEIGHT));
            ImGui::Begin("Info", (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            string TURN;
            if (POINTER->is_turn == true) {
                TURN = "Your turn...";
            }
            else {
                TURN = ENEMY_POINTER->name + "'s turn...";
            };
            ImGui::Text(TURN.c_str());
            ImGui::End();
        };

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

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {};
                        ImGui::PopStyleColor();
                    }

                    else if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::MISS) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().MISS);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().MISS);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().MISS);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {};
                        ImGui::PopStyleColor();
                    }

                    else if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::BOAT) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().BOAT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().BOAT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().BOAT);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {};
                        ImGui::PopStyleColor();
                    }

                    else if (POINTER->FLEET[i]->STATE == POINTER->FLEET[i]->STATE_::SUNK) {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().SUNK);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().SUNK);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().SUNK);

                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {};
                        ImGui::PopStyleColor();
                    }

                    else {
                        ImGui::PushStyleColor(ImGuiCol_Button, COLOR().DEFAULT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().DEFAULT);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().DEFAULT);
                        if (ImGui::Button(name.c_str(), POINTER->FLEET[i]->SIZE)) {};
                        ImGui::PopStyleColor();
                    };
                }
                ImGui::NextColumn();
            };
            ImGui::End();
        }
        
        //Radar
        if (show_game_window == true && show_config_window == false && SERVER::IS_STARTED == true && CLIENT::IS_STARTED == true && POINTER->LOST == false && ENEMY_POINTER->LOST == false) {
        ImGui::SetNextWindowPos(ImVec2(PLAYER_RADAR().X, PLAYER_RADAR().Y));
        ImGui::SetNextWindowSize(ImVec2(PLAYER_RADAR().WIDTH, PLAYER_RADAR().HEIGHT));

        ImGui::Begin("Radar", (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
        ImGui::Columns(11, NULL, false);       

        //This makes it so the radar updates before i make the player wait for the other player
        if (POINTER->is_turn == false && pass_def == true) {
            thread T((POINTER->WAIT_TURN), POINTER);
            T.join();
            cout << "Other player has taken turn..." << endl;
            pass_def = false;
        }
        else if (POINTER->is_turn == false && pass_def == false) {
            pass_def = true;
        };

        string name;
        for (int i = 0; i != 121; i++) {
            name = POINTER->RADAR[i]->ID;

            

            if (name == "INV") {
                ImGui::InvisibleButton("INV", POINTER->RADAR[i]->SIZE);
            }

            else {                

                if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::MISS) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().MISS);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().MISS);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().MISS);

                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            //POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i, POINTER);
                        }
                    };
                    ImGui::PopStyleColor();
                }

                else if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::SUNK) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().SUNK);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().SUNK);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().SUNK);

                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            //POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i, POINTER);
                        }
                    };
                    ImGui::PopStyleColor();
                }

                else if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::HIT) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().HIT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().HIT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().HIT);

                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            //POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i, POINTER);
                        }
                    };
                    ImGui::PopStyleColor();
                }

                else if (POINTER->RADAR[i]->STATE == POINTER->RADAR[i]->STATE_::UNCLICKED) {
                    ImGui::PushStyleColor(ImGuiCol_Button, COLOR().DEFAULT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR().DEFAULT);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR().DEFAULT);
                    if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                        if (POINTER->is_turn == true) {
                            POINTER->BUTTON_FUNC(POINTER->RADAR[i]->ID, i, POINTER, ENEMY_POINTER);          
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
        if (show_game_window == true && show_config_window == false && (SERVER::IS_STARTED == false || CLIENT::IS_STARTED == false) && POINTER->LOST == false && ENEMY_POINTER->LOST == false) {
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
            ImGui::Begin("Config", (bool*)false, ImGuiWindowFlags_NoResize);            
            if (placed_ships == false) {
                ImGui::Text((POINTER->name + "'s ships").c_str());

                if (ImGui::TreeNode("Carrier")) {
                    //Get the input of where the ship goes
                    ImGui::InputText("Carrier pos", INPUT[0], IM_ARRAYSIZE(INPUT[0]));
                    ImGui::SameLine();
                    HelpMarker(("Please input the tile of which you would like the ship to be on...\nWhen horizontal, ship goes from LEFT TO RIGHT...\nWhen vertical, ship goes from UP TO DOWN\nPlease use the buttons to toggle between horizontal and vertical..."));
                    //Buttons to toggle between Horizontal and Vertical
                    if (ImGui::Button("Horizontal")) {
                        //Set the rotation
                        POINTER->SHIPS->CARRIER->PLACER->ROTATION = POINTER->SHIPS->CARRIER->PLACER->ROTATION_::HORIZONTAL;
                    }

                    if (ImGui::Button("Vertical")) {
                        POINTER->SHIPS->CARRIER->PLACER->ROTATION = POINTER->SHIPS->CARRIER->PLACER->ROTATION_::VERTICAL;
                    }
                    ImGui::TreePop();
                }
                //##############################
                if (ImGui::TreeNode("Battleship")) {
                    ImGui::InputText("Battleship pos", INPUT[1], IM_ARRAYSIZE(INPUT[1]));
                    if (ImGui::Button("Horizontal ")) {
                        POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION = POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION_::HORIZONTAL;
                    }

                    if (ImGui::Button("Vertical ")) {
                        POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION = POINTER->SHIPS->BATTLESHIP->PLACER->ROTATION_::VERTICAL;
                    }
                    ImGui::TreePop();
                }
                //##############################
                if (ImGui::TreeNode("Destroyer")) {
                    ImGui::InputText("Destroyer pos", INPUT[2], IM_ARRAYSIZE(INPUT[2]));
                    if (ImGui::Button("Horizontal  ")) {
                        POINTER->SHIPS->DESTROYER->PLACER->ROTATION = POINTER->SHIPS->DESTROYER->PLACER->ROTATION_::HORIZONTAL;
                    }

                    if (ImGui::Button("Vertical  ")) {
                        POINTER->SHIPS->DESTROYER->PLACER->ROTATION = POINTER->SHIPS->DESTROYER->PLACER->ROTATION_::VERTICAL;
                    }
                    ImGui::TreePop();
                }
                //##############################
                if (ImGui::TreeNode("Submarine")) {
                    ImGui::InputText("Submarine pos", INPUT[3], IM_ARRAYSIZE(INPUT[3]));
                    if (ImGui::Button("Horizontal   ")) {
                        POINTER->SHIPS->SUBMARINE->PLACER->ROTATION = POINTER->SHIPS->SUBMARINE->PLACER->ROTATION_::HORIZONTAL;
                    }

                    if (ImGui::Button("Vertical   ")) {
                        POINTER->SHIPS->SUBMARINE->PLACER->ROTATION = POINTER->SHIPS->SUBMARINE->PLACER->ROTATION_::VERTICAL;
                    }
                    ImGui::TreePop();
                }
                //##############################
                if (ImGui::TreeNode("Patrol Boat")) {
                    ImGui::InputText("Patrol Boat pos", INPUT[4], IM_ARRAYSIZE(INPUT[4]));
                    if (ImGui::Button("Horizontal    ")) {
                        POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION = POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION_::HORIZONTAL;
                    }

                    if (ImGui::Button("Vertical    ")) {
                        POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION = POINTER->SHIPS->PATROL_BOAT->PLACER->ROTATION_::VERTICAL;
                    }                    
                    ImGui::TreePop();
                }
                
                

                if (ImGui::Button("Done?")) {
                    
                    if (POINTER->SHIPS->CARRIER->PLACER->SMART_PLACER(GAME::POS_TO_ARR(INPUT[0]), 
                        POINTER->SHIPS->CARRIER->LOCATION, POINTER->SHIPS->BATTLESHIP->LOCATION, POINTER->SHIPS->DESTROYER->LOCATION,
                        POINTER->SHIPS->SUBMARINE->LOCATION, POINTER->SHIPS->PATROL_BOAT->LOCATION, POINTER->SHIPS->CARRIER->NAME) == true) {


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
                    if (POINTER->SHIPS->BATTLESHIP->PLACER->SMART_PLACER(GAME::POS_TO_ARR(INPUT[1]),
                        POINTER->SHIPS->CARRIER->LOCATION, POINTER->SHIPS->BATTLESHIP->LOCATION, POINTER->SHIPS->DESTROYER->LOCATION,
                        POINTER->SHIPS->SUBMARINE->LOCATION, POINTER->SHIPS->PATROL_BOAT->LOCATION, POINTER->SHIPS->BATTLESHIP->NAME) == true) {

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
                    if (POINTER->SHIPS->DESTROYER->PLACER->SMART_PLACER(GAME::POS_TO_ARR(INPUT[2]),
                        POINTER->SHIPS->CARRIER->LOCATION, POINTER->SHIPS->BATTLESHIP->LOCATION, POINTER->SHIPS->DESTROYER->LOCATION,
                        POINTER->SHIPS->SUBMARINE->LOCATION, POINTER->SHIPS->PATROL_BOAT->LOCATION, POINTER->SHIPS->DESTROYER->NAME) == true) {
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
                    if (POINTER->SHIPS->SUBMARINE->PLACER->SMART_PLACER(GAME::POS_TO_ARR(INPUT[3]),
                        POINTER->SHIPS->CARRIER->LOCATION, POINTER->SHIPS->BATTLESHIP->LOCATION, POINTER->SHIPS->DESTROYER->LOCATION,
                        POINTER->SHIPS->SUBMARINE->LOCATION, POINTER->SHIPS->PATROL_BOAT->LOCATION, POINTER->SHIPS->SUBMARINE->NAME) == true) {
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
                    if (POINTER->SHIPS->PATROL_BOAT->PLACER->SMART_PLACER(GAME::POS_TO_ARR(INPUT[4]),
                        POINTER->SHIPS->CARRIER->LOCATION, POINTER->SHIPS->BATTLESHIP->LOCATION, POINTER->SHIPS->DESTROYER->LOCATION,
                        POINTER->SHIPS->SUBMARINE->LOCATION, POINTER->SHIPS->PATROL_BOAT->LOCATION, POINTER->SHIPS->PATROL_BOAT->NAME) == true) {
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