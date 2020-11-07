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
#include "Converter_table.h"
#pragma comment (lib, "d3d9.lib") //Adds some thing that make d3d9.h work
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

static string IP;
/*
10x10 grid

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

struct STYLE {
    ImVec4 color_main = ImVec4(255.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 color_active = ImVec4(1.255f, 0.30f, 0.30f, 1.0f);
    ImVec4 color_slider = ImVec4(0.400f, 0.0f, 0.0f, 1.0f);
    ImVec4 color_background = ImVec4(0.300f, 0.0f, 0.0f, 1.0f);
    ImGuiStyle& style_ptr = ImGui::GetStyle();
};

ImVec2 BUTTON_SIZE = ImVec2(50, 50);

struct CARRIER_ {
    string name = "Carrier";
    bool destroyed = false;
    bool holes_hit[5] = { false, false, false, false, false };
    int POS_X = 0;
    int POS_Y = 0;
};

struct BATTLESHIP_ {
    string name = "Battleship";
    bool destroyed = false;
    bool holes_hit[4] = { false, false, false, false };
    int POS_X = 0;
    int POS_Y = 0;
};

struct DESTROYER_ {
    string name = "Destroyer";
    bool destroyed = false;
    bool holes_hit[3] = { false, false, false };
    int POS_X = 0;
    int POS_Y = 0;
};

struct SUBMARINE_ {
    string name = "Submarine";
    bool destroyed = false;
    bool holes_hit[3] = { false, false, false };
    int POS_X = 0;
    int POS_Y = 0;
};

struct PATROL_BOAT_ {
    string name = "Patrol Boat";
    bool destroyed = false;
    bool holes_hit[2] = { false, false };
    int POS_X = 0;
    int POS_Y = 0;
};



class SHIPS_CLASS {
public:
    CARRIER_* CARRIER = new CARRIER_;

    BATTLESHIP_* BATTLESHIP = new BATTLESHIP_;

    DESTROYER_* DESTROYER = new DESTROYER_;

    SUBMARINE_* SUBMARINE = new SUBMARINE_;

    PATROL_BOAT_* PATROL_BOAT = new PATROL_BOAT_;
};


struct RADAR_ {
    string ID;
    ImVec2 SIZE = ImVec2(50, 50);
    enum STATE {
        UNCLICKED,
        MISS,
        HIT
    };
};

class PLAYER {    
public:
    atomic<bool> LOST = false;

    atomic<bool> is_turn = false;

    string name = "Player";

    SHIPS_CLASS* SHIPS = new SHIPS_CLASS;

    RADAR_* RADAR[120];

    void GENERATE_RADAR() {
        string LETTERS[10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
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
    
};

PLAYER* PLAYER_1;
PLAYER* PLAYER_2;
PLAYER* POINTER;

static bool is_host = false;

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
        cout << "Starting..." << endl;
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
    }
    else if (is_host == false) {
        POINTER = PLAYER_2;
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
    cout << "Connected!" << endl << "Starting..." << endl;;
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

        //Player's arrangement
        if (show_game_window == true && show_config_window == false && SERVER::IS_STARTED == true && CLIENT::IS_STARTED == true) {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowPos(ImVec2(PLAYER_VIEW().X, PLAYER_VIEW().Y));
            ImGui::SetNextWindowSize(ImVec2(PLAYER_VIEW().WIDTH, PLAYER_VIEW().HEIGHT));

            ImGui::Begin((POINTER->name + "'s fleet").c_str(), (bool*)false, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
            ImGui::Columns(11, NULL, false);
            

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
                ImGui::InvisibleButton(" ", POINTER->RADAR[i]->SIZE);
            }

            else {
                if (ImGui::Button(name.c_str(), POINTER->RADAR[i]->SIZE)) {
                    if (POINTER->is_turn == true) {                        
                        cout << POINTER->RADAR[i]->ID << endl;
                    }                    
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

            ImGui::Text("Waiting for other player to start...");
            ImGui::End();
        }

        //Config window
        if (show_config_window) {
            ImGui::SetNextWindowPos(ImVec2(PLAYER_VIEW().X, PLAYER_VIEW().Y));
            ImGui::SetNextWindowSize(ImVec2(PLAYER_VIEW().WIDTH, PLAYER_VIEW().HEIGHT));
            ImGui::Begin("Config");            
            if (placed_ships == true) {
                ImGui::Text((POINTER->name + "'s ships").c_str());

            }

            else if (placed_ships == false) {
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