#define IMGUI_DEFINE_MATH_OPERATORS
#include "Main.hpp"
//#include "PeakLogo.h"
float dpi_scale = 1.f;
ImDrawList* draw;
ImVec2 pos;

weapon_t get_weapon();

void RecoilControl() {
	while (true) {
		if (CurrentWeapon::WeaponActive() == true)
		{
			if (config.Includes.recoil == true)
			{
                for (int i = 0; i <= CurrentWeapon::Bullet() && GetAsyncKeyState(VK_LBUTTON) & 0x8000; i++)
                {
                    if (Script::IsCursorShowing()) continue;
                    Script::Smoothings((int)Script::isMuzzleBoostOrBreak((float)CurrentWeapon::Delay()), (int)Script::isMuzzleBoostOrBreak(CurrentWeapon::Control(i)), (int)CurrentWeapon::x(i), (int)CurrentWeapon::y(i));
                    CurrentWeapon::RecoilBreak(i);
                }
			}
		}
	}
}

int APIENTRY AutoDetection(HINSTANCE, HINSTANCE, LPSTR, int) {
    while (true) {
        if (config.Includes.autodetect) {
            switch (get_weapon().weapon) {
            case WEAPON_AK:
                config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
                config.Includes.selectedItemWP = 1;
                config.Includes.Active_Weapon = "Ak47";

                //std::cout << "Ak47" << std::endl;
                break;
            case WEAPON_LR:
                config.Includes.ak_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
                config.Includes.selectedItemWP = 2;
                config.Includes.Active_Weapon = "Lr300";

                //std::cout << "Lr300" << std::endl;
                break;
            case WEAPON_MP5:
                config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
                config.Includes.Active_Weapon = "Mp5a4";
                config.Includes.selectedItemWP = 3;

                //std::cout << "Mp5a4" << std::endl;
                break;
            case WEAPON_TOMMY:
                config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
                config.Includes.selectedItemWP = 5;
                config.Includes.Active_Weapon = "Thommy Smg";

                //std::cout << "Thommy Smg" << std::endl;
                break;
            case WEAPON_SMG:
                config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
                config.Includes.selectedItemWP = 4;
                config.Includes.Active_Weapon = "Custom Smg";

                //std::cout << "Custom Smg" << std::endl;
                break;
            case WEAPON_SEMI:
                config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.m249_active = false, config.Includes.custom_active = false;
                config.Includes.selectedItemWP = 6;
                config.Includes.Active_Weapon = "Semi Rifle";

                //std::cout << "Semi Rifle" << std::endl;
                break;
            case WEAPON_M249:
                config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.semi_active = false, config.Includes.custom_active = false;
                config.Includes.selectedItemWP = 7;
                config.Includes.Active_Weapon = "M249";

                //std::cout << "M249" << std::endl;
                break;
            default:
                break;
            }
        }
        Sleep(100);
    }
}

void createfile() {
    std::filesystem::create_directories("C:\\skeng\\rust gear\\Configs");
}

void check() {

    ImGui::SetCursorPos(ImVec2(195, 2));

 //   ImGui::BeginGroup();
 //   {
     //   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 4));
        if (ImGui::tab("recoil", config.Includes.currentColumn == 1)) config.Includes.currentColumn = 1;
        ImGui::SameLine();
    //    if (ImGui::tab("misc", config.Includes.currentColumn == 3)) config.Includes.currentColumn = 3;
        if (ImGui::tab("keys", config.Includes.currentColumn  == 2)) config.Includes.currentColumn = 2;
        ImGui::SameLine();
        if (ImGui::tab("config", config.Includes.currentColumn == 3)) config.Includes.currentColumn = 3;
     //   ImGui::PopStyleVar();
//    }
 //   ImGui::EndGroup();
}


// Main code
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    config.run();
    createfile();

    Script::ComboBox();
    Script::keybinds();  
    //std::thread recoil_control(RecoilControl);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoDetection, 0, 0, 0);
   // CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AutoDetection, NULL, NULL, NULL);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;

    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style->WindowRounding = 0.0f;
        style->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui::StyleColorsDark();

    style->Alpha = 1.0f;
    style->WindowRounding = 2;
    style->FramePadding = ImVec2(4, 3);
    style->WindowPadding = ImVec2(0, 0);
    style->ItemInnerSpacing = ImVec2(6, 6);
    style->ItemSpacing = ImVec2(8, 10);
    style->FrameRounding = 12;
    style->PopupRounding = 0.f;
    style->Rounding = 11.f;
    style->FrameRounding = 0.0f;

    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 0.5f;

    style->Colors[ImGuiCol_CheckMark] = ImColor(230, 10, 93);
    style->Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0);
    style->Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0);
    style->Colors[ImGuiCol_Button] = ImColor(29, 31, 39);
    style->Colors[ImGuiCol_ButtonActive] = ImColor(48, 48, 47);
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 37);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(29, 31, 38);
    style->Colors[ImGuiCol_FrameBg] = ImColor(10, 10, 10);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(230, 10, 93);
    style->Colors[ImGuiCol_SliderGrab] = ImColor(230, 10, 93);
    style->Colors[ImGuiCol_Slider] = ImColor(98, 0, 255);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(56, 4, 179);
    style->Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(230, 10, 93);
    style->Colors[ImGuiCol_HeaderActive] = ImColor(71, 2, 120);

    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF,
        0x0400, 0x044F,
        0,
    };

   
    ImFont* iconfont = nullptr;
    ImFont* gilroy_big = nullptr;
    ImFont* gilroy_mini = nullptr;

    gilroy_mini = io.Fonts->AddFontFromMemoryTTF((void*)gilroyfont, sizeof(gilroyfont), 10, &font_config, ranges);
    gilroy_big = io.Fonts->AddFontFromMemoryTTF((void*)gilroyfont, sizeof(gilroyfont), 20, &font_config, ranges);
    iconfont = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 25, &font_config, ranges);

    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    DWORD window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    auto x = float(screen_rect.right - WINDOW_WIDTH) / 2.f;
    auto y = float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f;

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (CurrentWeapon::WeaponActive() == true)
        {
            if (config.Includes.recoil == true)
            {
                for (int i = 0; i <= CurrentWeapon::Bullet() && GetAsyncKeyState(VK_LBUTTON) & 0x8000; i++)
                {
                    if (Script::IsCursorShowing()) continue;
                    Script::Smoothings((int)Script::isMuzzleBoostOrBreak((float)CurrentWeapon::Delay()), (int)Script::isMuzzleBoostOrBreak(CurrentWeapon::Control(i)), (int)CurrentWeapon::x(i), (int)CurrentWeapon::y(i));
                    CurrentWeapon::RecoilBreak(i);
                }
            }
        }
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowBgAlpha(1.0f);

            if (GetAsyncKeyState(config.Includes.hsbind) & 1)
            {
                config.Includes.showing = !config.Includes.showing;
            }
            if (config.Includes.showing)
            {
                SetWindowPos(main_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ WINDOW_WIDTH, WINDOW_HEIGHT };
                ImGui::Begin("VechterBesting ", &loader_active, window_flags);
                {
                    {//Draw Line
                        static int x = 450 * dpi_scale, y = 288 * dpi_scale;
                        pos = ImGui::GetWindowPos();
                        draw = ImGui::GetWindowDrawList();

                        draw->AddRectFilled(pos, ImVec2(pos.x + 545, pos.y + 355), ImColor(38, 38, 42), 9);
                        draw->AddRectFilled(pos, ImVec2(pos.x + 545, pos.y + 355), ImGui::GetColorU32(ImGuiCol_WindowBg), 9);
                        draw->AddRectFilled(ImVec2(pos.x, pos.y + 30), ImVec2(pos.x + 130, pos.y + 355), ImGui::GetColorU32(ImGuiCol_TabBar), 9, 4);
                        draw->AddLine(ImVec2(pos.x, pos.y + 38), ImVec2(pos.x + 545, pos.y + 38), ImColor(230, 10, 93), 2.5f);
                    }

           

                    {//Script
                        Script::ComboBox();
                        check();
                        Script::keybinds();

                        switch (config.Includes.currentColumn) {
                            break;
                        case 0:
                            check();

                            Script::ComboBox();

                            Script::keybinds();

                            ImGui::SetCursorPos(ImVec2(5, 45));
                            ImGui::BeginChild("##scriptinfo", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

              //                  ImGui::PushFont(f7);

                                ImGui::Text("-----Script");
                                ImGui::Text("+ Better randomization");
                                ImGui::Text("+ New recoil tables");
                                ImGui::Text("+ Autodetection");
                                ImGui::Text("+ New menu");

              //                  ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorPos(ImVec2(227.5, 45));
                            ImGui::BeginChild("##loaderinfo", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

                //                ImGui::PushFont(f7);

                                ImGui::Text("-----Loader");
                                ImGui::Text("+ Better protection");
                                ImGui::Text("+ Auto update");

               //                 ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                            break;
                        case 1:
                            check();

                            Script::ComboBox();

                            Script::keybinds();

                            check();

                            ImGui::SetCursorPos(ImVec2(5, 45));
                            ImGui::BeginChild("##recoil", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

                 //               ImGui::PushFont(f7);

                                ImGui::SetCursorPos(ImVec2(10, 8));
                                ImGui::Checkbox("enable recoil", &config.Includes.recoil);

                                ImGui::SetNextItemWidth(200);
                                ImGui::Combo("##weapons", &config.Includes.selectedItemWP, Script::itemWP, 8, ImGuiComboFlags_NoArrowButton);
                                ImGui::SetNextItemWidth(200);
                                ImGui::Combo("##sights", &config.Includes.selectedItemSC, Script::itemSC, 5, ImGuiComboFlags_NoArrowButton);
                                ImGui::SetNextItemWidth(200);
                                ImGui::Combo("##barrels", &config.Includes.selectedItemBR, Script::itemBR, 4, ImGuiComboFlags_NoArrowButton);
                                ImGui::Text(" Sensitivity");
                                ImGui::SameLine();
                                ImGui::Text("                 FOV");
                                ImGui::SetNextItemWidth(50);
                                ImGui::InputFloat("##Sens", &config.Includes.game_sensitivity, 0.0f, 100.0f, "%0.3f", 0);
                                ImGui::SameLine(120);
                                ImGui::SetNextItemWidth(50);
                                ImGui::InputFloat("##Fov", &config.Includes.game_fov, 0.0f, 100.0f, "%0.3f", 0);

                                ImGui::Checkbox("autodetection (beta)", &config.Includes.autodetect);
                                ImGui::Checkbox("mini overlay", &config.Includes.show_info);
                                ImGui::Checkbox("legitmode", &config.Includes.legitmode);
                                ImGui::Checkbox("hipfire", &config.Includes.hipfire);



                                ImGui::Checkbox("Crosshair", &config.Includes.crosshair);
                                if (config.Includes.crosshair == true)
                                {
                                    ImGui::SameLine();
                                    ImGui::Spacing();
                                    ImGui::SameLine();
                                    ImGui::Spacing();
                                    ImGui::SameLine();
                                   // ImGui::KeyBind("##Cross Key", config.Includes.crossbind, ImVec2(55, 20));
                                 //   ImGui::Combo("##CrossColor", &config.Includes.selectedCross, Script::crossBR, 6);
                                    ImGui::Spacing();
                                }

                 //-               ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorPos(ImVec2(227.5, 45));
                            ImGui::BeginChild("##recoil1", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

                   //             ImGui::PushFont(f7);

                                ImGui::SetCursorPos(ImVec2(10, 10));
                                ImGui::Checkbox("Randomization", &config.Includes.randomxy_active);
                                if (config.Includes.randomxy_active == true) {
                                    ImGui::Text("control X");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##control X", &config.Includes.XCONTROL, 1, 10, "% .2f");
                                    ImGui::Text("control Y");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##control Y", &config.Includes.YCONTROL, 1, 10, "% .2f");
                                    ImGui::Text("randomization X");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##randomization X", &config.Includes.XSCALE, 1, 50, "% .2f");
                                    ImGui::Text("randomization Y");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##randomization Y", &config.Includes.YSCALE, 1, 50, "% .2f");
                                }
                                else {
                                    ImGui::Text("control X");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##control X", &config.Includes.XCONTROL, 1, 10, "% .2f");
                                    ImGui::Text("control Y");
                                    ImGui::SetNextItemWidth(200);
                                    ImGui::SliderFloat("##control Y", &config.Includes.YCONTROL, 1, 10, "% .2f");
                                }

                    //            ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                            break;
                        case 2:
                            Script::ComboBox();

                            Script::keybinds();

                            ImGui::SetCursorPos(ImVec2(5, 45));
                            ImGui::BeginChild("##keys", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

                                ImGui::PushFont(gilroy_mini);
                                ImGui::SetCursorPos(ImVec2(4, 5));
                                ImGui::KeyBind("Ak47", config.Includes.akbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 30));
                                ImGui::KeyBind("Lr300", config.Includes.lrbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 55));
                                ImGui::KeyBind("Mp5a4", config.Includes.mpbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 80));
                                ImGui::KeyBind("Custom", config.Includes.csbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 105));
                                ImGui::KeyBind("Thommy", config.Includes.tmbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 130));
                                ImGui::KeyBind("Semi Rifle", config.Includes.smbind, ImVec2(140, 20));
                                ImGui::SetCursorPos(ImVec2(4, 155));
                                ImGui::KeyBind("M249", config.Includes.m2bind, ImVec2(140, 20));

                                ImGui::SetCursorPos(ImVec2(4, 228 + 25));
                                ImGui::KeyBind("Close Bind", config.Includes.closebind, ImVec2(140, 18));

                                ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorPos(ImVec2(227.5, 45));
                            ImGui::BeginChild("##keys1", ImVec2(217.5, 248 + 25));
                            {
                                ImGui::BeginGroup();

                   //             ImGui::PushFont(f7);

                                ImGui::SetCursorPos(ImVec2(4, 5));
                                ImGui::KeyBind("8x", config.Includes.xxbind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 30));
                                ImGui::KeyBind("16x", config.Includes.x16bind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 55));
                                ImGui::KeyBind("Holo Sight", config.Includes.hhbind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 80));
                                ImGui::KeyBind("Simple Sight", config.Includes.ssbind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 130));
                                ImGui::KeyBind("Silencer", config.Includes.slbind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 155));
                                ImGui::KeyBind("Muzzle Boost", config.Includes.mbbind, ImVec2(125, 20));
                                ImGui::SetCursorPos(ImVec2(4, 180));
                                ImGui::KeyBind("Muzzle Break", config.Includes.bbind, ImVec2(125, 20));

                                ImGui::SetCursorPos(ImVec2(4, 228 + 25));
                                ImGui::KeyBind("Hide Bind", config.Includes.hsbind, ImVec2(125, 18));

                 //               ImGui::PopFont();

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                            break;

                        case 3:
                            ImGui::SetCursorPos(ImVec2(5, 45));
                            ImGui::BeginChild("##configs", ImVec2(440, 248 + 25));
                            {
                                ImGui::BeginGroup();

                                Script::ComboBox();

                                Script::keybinds();

                  //              ImGui::PushFont(f7);

                                constexpr auto& config_items = config.get_configs();
                                static auto current_config = -1;

                                if ((size_t)(current_config) >= config_items.size())
                                    current_config = -1;

                                static char buffer[32];

                             /*   ImGui::SetCursorPos(ImVec2(10, 10));
                                ImGui::SetNextItemWidth(134);
                                if (ImGui::ListBox("##Includes::config_list", &current_config, [](void* data, int idx, const char** out_text)
                                    {
                                        auto& vector = *(std::vector<std::string> *)(data);
                                        *out_text = vector[idx].c_str();
                                        return true;
                                    }, &config_items, (int)(config_items.size()), 5) && current_config != -1) strcpy_s(buffer, config_items[current_config].c_str());

                                    ImGui::SetNextItemWidth(134);
                                    if (ImGui::InputText("##Includes::config_name", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
                                    {
                                        if (current_config != -1)
                                            config.rename(current_config, buffer);
                                    }*/

                                    if (ImGui::Button("Create"))
                                        config.add(buffer);

                                    ImGui::SameLine();

                                    if (ImGui::Button("Reset"))
                                        config.reset();

                                    ImGui::SameLine();

                                    if (ImGui::Button("Refresh"))
                                        config.run2();

                                    if (current_config > -1)
                                    {
                                        if (ImGui::Button("Save"))
                                            config.save(current_config);

                                        ImGui::SameLine();

                                        if (ImGui::Button("Load"))
                                            config.load(current_config);

                                        ImGui::SameLine();

                                        if (ImGui::Button("Delete"))
                                            config.remove(current_config);
                                    }

                                    ImGui::SetCursorPos(ImVec2(371, 230 + 23));
                                    if (ImGui::Button("exit script")) {
                                        Script::DeleteAllFiles("C:\\Windows\\Temp\\");
                                        Script::DeleteAllFiles("C:\\Windows\\Prefetch\\");
                                        Script::cleandnscache();
                                        Script::cleanstrings(GetProcessId(Script::getexename), Script::pcaclarinet());
                                        Sleep(500);
                                        exit(0);
                                    }

                  //                  ImGui::PopFont();

                                    ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                            break;
                        }
                    }
                    
                }
                ImGui::End();
            }
        }
        if (config.Includes.show_info == true)
        {
            Script::keybinds();

            //  ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Once);
#define WINDOW_WIDTH  100
#define WINDOW_HEIGHT 54
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
        //    ::SetWindowPos(main_hwnd, HWND_TOPMOST, 0, 0, 0, 0 | SWP_NOSIZE);
            if (config.Includes.show_info == true)
            {
                DWORD flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
                ImGui::Begin("Vechterbesting - Info", &config.Includes.show_info, flags);
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Once);
                ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
                ImGui::SetNextWindowBgAlpha(1.0f);
                if (config.Includes.Active_Weapon == "NONE")
                {
                    ImGui::Text("Weapon:11");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(255, 0, 8), config.Includes.Active_Weapon);
                }
                else {
                    ImGui::Text("Weapon:");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(9, 255, 0), config.Includes.Active_Weapon);
                }
                if (config.Includes.Active_Scopes == "NONE")
                {
                    ImGui::Text("Scopes:");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(255, 0, 8), config.Includes.Active_Scopes);
                }
                else {
                    ImGui::Text("Scopes:");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(9, 255, 0), config.Includes.Active_Scopes);
                }
                if (config.Includes.Active_Barrel == "NONE")
                {
                    ImGui::Text("Barrel:");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(255, 0, 8), config.Includes.Active_Barrel);
                }
                else {
                    ImGui::Text("Barrel:");
                    ImGui::SameLine(58);
                    ImGui::TextColored(ImColor(9, 255, 0), config.Includes.Active_Barrel);
                }
                ImGui::End();
            }
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}