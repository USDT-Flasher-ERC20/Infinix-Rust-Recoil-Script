#include "Script.hpp"
#include <vector>
#include <random>
#include <winternl.h>

double cleanedstrings = 0;

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;
extern "C" NTSTATUS ZwQueryVirtualMemory(HANDLE, void*, MEMORY_INFORMATION_CLASS, void*, SIZE_T, SIZE_T*);
extern "C" NTSTATUS ZwReadVirtualMemory(HANDLE, void*, void*, SIZE_T, SIZE_T*);
extern "C" NTSTATUS ZwWriteVirtualMemory(HANDLE, void*, void*, SIZE_T, SIZE_T*);

std::string Script::getexepath() {
    char path[MAX_PATH];
    const char* f2 = path;
    ::GetModuleFileName(0, (LPSTR)f2, MAX_PATH);
    return path;
}

std::string Script::getexename() {
    std::string path = getexepath();
    std::string exename = path.substr(path.rfind("\\") + 1);
    return exename;
}

void Script::cleanstrings(DWORD pid, std::vector<std::string> findvector, bool ispca) {

    if (HANDLE processhandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid)) {
        for (size_t i = 0; i < findvector.size(); i++) {
            std::string lmao = findvector[i];
            for (const char x : lmao) {
                if (!isprint(x)) {
                    findvector.erase(findvector.begin() + i);
                    break;
                }
            }
        }

        if (pid != GetCurrentProcessId()) {
            MEMORY_BASIC_INFORMATION memory;
            INT64 address = 0;
            while (NT_SUCCESS(ZwQueryVirtualMemory(processhandle, (LPVOID)address, MemoryBasicInformation, &memory,
                sizeof(MEMORY_BASIC_INFORMATION), nullptr))) {
                if (memory.State == MEM_COMMIT && (
                    (memory.Protect == PAGE_EXECUTE_READWRITE) |
                    (memory.Protect == PAGE_READWRITE) |
                    (memory.Protect == PAGE_EXECUTE_WRITECOPY) |
                    (memory.Protect == PAGE_WRITECOPY))) {

                        {
                            std::vector<char> buffer(memory.RegionSize);
                            if (NT_SUCCESS(
                                ZwReadVirtualMemory(processhandle, (LPVOID)address, &buffer[0], memory.RegionSize,
                                    nullptr))) {
                                for (std::string x : findvector) {
                                    INT64 lenght = x.length();
                                    INT64 size = static_cast<INT64>(memory.RegionSize);
                                    for (INT64 i = 0; i <= size; i++) {
                                        INT64 j;
                                        for (j = 0; j < lenght; j++)
                                            if (buffer[i + j] != x[j])
                                                break;
                                        if (j == lenght) {

                                            if (ispca) {

                                                std::string current_pca;
                                                size_t current_pca_start = address + i;
                                                size_t location = current_pca_start;
                                                size_t new_pca_size = 0;

                                                for (;;) {

                                                    std::vector<char> char_a(1);
                                                    if (ReadProcessMemory(processhandle, (LPVOID)location, &char_a[0],
                                                        (sizeof(char) * 1), 0)) {

                                                        current_pca.push_back(char_a[0]);

                                                    }

                                                    std::vector<char> char_b(5);
                                                    if (ReadProcessMemory(processhandle, (LPVOID)location, &char_b[0],
                                                        (sizeof(char) * 5), 0)) {

                                                        size_t counter = 0;
                                                        for (const char x : char_b) {
                                                            if (x <= 126 && x >= 32) {}
                                                            else { counter++; }
                                                        }
                                                        if (counter == 5) { break; }
                                                        else { new_pca_size += 2; }

                                                    }

                                                    location++;

                                                }
                                                size_t addition = 0;
                                                std::string new_pca;
                                                std::ifstream iss(current_pca);
                                                for (std::string line; std::getline(iss, line);) {

                                                    if (line.find(getexename()) != std::string::npos) {

                                                        addition += line.length() + 2;

                                                    }
                                                    else { new_pca += (line); }

                                                }

                                                char nullthat = 0;
                                                for (size_t m = 0; m < current_pca.length(); m++) {
                                                    WriteProcessMemory(processhandle, (LPVOID)(current_pca_start + m),
                                                        &nullthat, 1, 0);
                                                }

                                                for (size_t m = 0; m < new_pca.length(); m++) {
                                                    WriteProcessMemory(processhandle,
                                                        (LPVOID)(current_pca_start + addition + m),
                                                        &new_pca[m], 1, 0);
                                                }

                                                break;

                                            }
                                            else {
                                                std::string newstring;
                                                for (size_t y = 0; y < lenght; y++) { newstring.push_back(0); }
                                                WriteProcessMemory(processhandle, (LPVOID)(address + i), &newstring,
                                                    lenght, 0);
                                            }

                                            cleanedstrings++;
                                        }
                                    }
                                }
                            }
                        }

                        {
                            if (!ispca) {
                                std::vector<WCHAR> buffer(memory.RegionSize);
                                if (NT_SUCCESS(
                                    ZwReadVirtualMemory(processhandle, (LPVOID)address, &buffer[0], memory.RegionSize,
                                        nullptr))) {
                                    for (std::string x : findvector) {
                                        INT64 lenght = x.length();
                                        INT64 size = static_cast<INT64>(memory.RegionSize);
                                        for (INT64 i = 0; i <= size; i++) {
                                            INT64 j;
                                            for (j = 0; j < lenght; j++)
                                                if (buffer[i + j] != x[j])
                                                    break;
                                            if (j == lenght) {
                                                cleanedstrings++;
                                                WCHAR writeme = 0;
                                                INT64 x = i;
                                                INT64 counter = 0;
                                                for (;;) {
                                                    WriteProcessMemory(processhandle, (LPVOID)(address + (x * (INT64)2)),
                                                        &writeme, (sizeof(WCHAR)), 0);
                                                    x++;
                                                    counter++;
                                                    if (counter >= lenght) { break; }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                }
                address += memory.RegionSize;
            }
        }
    }

}

void Script::ComboBox() {
    //Weapons
    if (config.Includes.selectedItemWP == 0) {
        WeaponNone(true);
    }
    else if (config.Includes.selectedItemWP == 1) {
        config.Includes.Active_Weapon = "Ak47";
        config.Includes.ak_active = true; config.Includes.lr_active = false; config.Includes.mp5_active = false; config.Includes.custom_active = false; config.Includes.thompson_active = false; config.Includes.semi_active = false;
        config.Includes.m249_active = false;
    }
    else if (config.Includes.selectedItemWP == 2) {
        config.Includes.Active_Weapon = "Lr300";
        config.Includes.lr_active = true; config.Includes.ak_active = false; config.Includes.mp5_active = false; config.Includes.custom_active = false; config.Includes.thompson_active = false; config.Includes.semi_active = false;
        config.Includes.m249_active = false;
    }
    else if (config.Includes.selectedItemWP == 3) {
        config.Includes.Active_Weapon = "Mp5a4";
        config.Includes.lr_active = false; config.Includes.ak_active = false; config.Includes.mp5_active = true; config.Includes.custom_active = false; config.Includes.thompson_active = false; config.Includes.semi_active = false;
        config.Includes.m249_active = false;
    }
    else if (config.Includes.selectedItemWP == 4) {
        config.Includes.Active_Weapon = "Custom Smg";
        config.Includes.custom_active = true; config.Includes.ak_active = false; config.Includes.mp5_active = false; config.Includes.lr_active = false; config.Includes.thompson_active = false; config.Includes.semi_active = false;
        config.Includes.m249_active = false;
    }
    else if (config.Includes.selectedItemWP == 5) {
        config.Includes.Active_Weapon = "Thompson Smg";
        config.Includes.thompson_active = true; config.Includes.ak_active = false; config.Includes.mp5_active = false; config.Includes.custom_active = false; config.Includes.lr_active = false; config.Includes.semi_active = false;
        config.Includes.m249_active = false;
    }
    else if (config.Includes.selectedItemWP == 6) {
        config.Includes.Active_Weapon = "Semi Rifle";
        config.Includes.semi_active = true; config.Includes.ak_active = false; config.Includes.mp5_active = false; config.Includes.custom_active = false; config.Includes.thompson_active = false; config.Includes.lr_active = false;
        config.Includes.m249_active = false;
    }
    else  if (config.Includes.selectedItemWP == 7) {
        config.Includes.Active_Weapon = "M249";
        config.Includes.m249_active = true; config.Includes.ak_active = false; config.Includes.mp5_active = false; config.Includes.custom_active = false; config.Includes.thompson_active = false; config.Includes.semi_active = false;
        config.Includes.lr_active = false;
    }
    //Scopes
    if (config.Includes.selectedItemSC == 0) {
        ScopeNone(true);
    }
    else if (config.Includes.selectedItemSC == 1) {
        config.Includes.Active_Scopes = "8X";
        config.Includes.sniper_active = true; config.Includes.x16_active = false; config.Includes.holo_active = false; config.Includes.simple_active = false;
    }
    else if (config.Includes.selectedItemSC == 2) {
        config.Includes.Active_Scopes = "16X";
        config.Includes.sniper_active = false; config.Includes.x16_active = true; config.Includes.holo_active = false; config.Includes.simple_active = false;
    }
    else if (config.Includes.selectedItemSC == 3) {
        config.Includes.Active_Scopes = "Holo Sight";
        config.Includes.sniper_active = false; config.Includes.x16_active = false; config.Includes.holo_active = true; config.Includes.simple_active = false;
    }
    else if (config.Includes.selectedItemSC == 4) {
        config.Includes.Active_Scopes = "Simple Sight";
        config.Includes.sniper_active = false; config.Includes.x16_active = false; config.Includes.holo_active = false; config.Includes.simple_active = true;
    }
    //Barrels
    if (config.Includes.selectedItemBR == 0) {
        BarrelNone(true);
    }
    else if (config.Includes.selectedItemBR == 1) {
        config.Includes.Active_Barrel = "Silencer";
        config.Includes.silencer_active = true;
        config.Includes.break_active = false;
    }
    else if (config.Includes.selectedItemBR == 2) {
        config.Includes.Active_Barrel = "Muzzle Boost";
        config.Includes.silencer_active = false;
        config.Includes.boost_active = true;
        config.Includes.break_active = false;
    }
    else if (config.Includes.selectedItemBR == 3) {
        config.Includes.Active_Barrel = "Muzzle Break";
        config.Includes.silencer_active = false;
        config.Includes.break_active = true;
        config.Includes.boost_active = false;
    }
}

std::vector<std::string> Script::pcaclarinet() {
    std::vector<std::string> x;
    std::string one = "TRACE,0000,0000,PcaClient,MonitorProcess," + getexepath() + ",Time,0";
    std::string two = R"(\n)";
    x.push_back(one);
    x.push_back(two);
    return x;
}

bool Script::IsCursorShowing()
{
    CURSORINFO ci = { sizeof(CURSORINFO) };
    GetCursorInfo(&ci);

    return ci.flags != 0;
}

bool Script::cleandnscache() {
    BOOL(WINAPI * DoDnsFlushResolverCache)();
    *(FARPROC*)&DoDnsFlushResolverCache = GetProcAddress(LoadLibrary((LPCSTR)"dnsapi.dll"), "DnsFlushResolverCache");
    if (!DoDnsFlushResolverCache) return FALSE;
    return DoDnsFlushResolverCache();
}

const char* Script::itemWP[8] = {
    "no weapon\0",
    "Ak47\0",
    "Lr300\0",
    "Mp5a4\0",
    "Custom Smg\0",
    "Thommy Smg\0",
    "Semi Rifle\0",
    "M249\0",
};

const char* Script::itemSC[5] = {
    "no scope\0",
    "8x\0",
    "16x\0",
    "Holo sight\0",
    "Simple Sight\0"
};

const char* Script::itemBR[4] = {
    "no barrel\0",
    "Silencer\0",
    "Boost\0",
    "Break\0"
};

void Script::pQuerySleep(int ms) {
    LONGLONG timer_resolution;
    LONGLONG wanted_time;
    LONGLONG current_time;

    QueryPerformanceFrequency((LARGE_INTEGER*)&timer_resolution);
    timer_resolution /= 1000;

    QueryPerformanceCounter((LARGE_INTEGER*)&current_time);

    wanted_time = current_time / timer_resolution + ms;
    current_time = 0;

    while (current_time < wanted_time) {
        QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
        current_time /= timer_resolution;
    }
}

void Script::mouse_move(int x, int y)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(input));
}

void Script::Smoothing(double delay, double control_time, float x, float y)
{
    int x_ = 0, y_ = 0, t_ = 0;

    for (int i = 1; i <= (int)control_time; ++i) {
        int xI = i * x / (int)control_time;
        int yI = i * y / (int)control_time;
        int tI = i * (int)control_time / (int)control_time;

        mouse_event(1, (int)xI - (int)x_, (int)yI - (int)y_, 0, 0);
        pQuerySleep((int)tI - (int)t_);

        x_ = xI;
        y_ = yI;
        t_ = tI;
    }

    pQuerySleep((int)delay - (int)control_time);
}

void Script::Smoothings(double delay, double control_time, float x, float y) {
    int x_ = 0, y_ = 0, t_ = 0;

    for (int i = 1; i <= (int)control_time; ++i) {
        int xI = i * x / (int)control_time;
        int yI = i * y / (int)control_time;
        int tI = i * (int)control_time / (int)control_time;

        float deltaX = (int)xI - (int)x_;
        float deltaY = (int)yI - (int)y_;

        mouse_move(deltaX, deltaY);
        pQuerySleep((int)tI - (int)t_);

        x_ = xI;
        y_ = yI;
        t_ = tI;
    }

    pQuerySleep((int)delay - (int)control_time);
}


int Script::isMuzzleBoostOrBreak(float delay) {
    if (config.Includes.boost_active == true) {
        float difference = delay * 0.1f;
        delay = delay - difference;
        return (int)delay + 1;
    }
    else if (config.Includes.break_active == true) {
        float difference = delay * 0.1f;
        delay = delay - difference;
        return (int)delay + 0.5;
    }
    else
        return (int)delay;
}

void Script::ConsoleWindows(int Flag)
{
    ::ShowWindow(::GetConsoleWindow(), Flag);
}

void Script::DeleteAllFiles(std::string strPath)
{
    WIN32_FIND_DATA wfd;
    HANDLE hFile;
    DWORD dwFileAttr;
    std::string strFile;
    std::string strSpec = strPath + "*.*";
    std::string strPathFile;

    // find the first file
    hFile = FindFirstFile(strSpec.c_str(), &wfd);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            strFile = wfd.cFileName;
            strPathFile = strPath + strFile;
            // get the file attributes
            dwFileAttr = GetFileAttributes(strPathFile.c_str());

            // see if file is read-only : if so unset read-only
            if (dwFileAttr & FILE_ATTRIBUTE_READONLY)
            {
                dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(strPathFile.c_str(), dwFileAttr);
            }

            // see if the file is a directory
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // make sure it isn't current or parent directory
                if (strFile != "." && strFile != "..")
                {
                    strPathFile += "\\";
                    // recursively delete all files in this folder
                    DeleteAllFiles(strPathFile);
                    // remove the directory

                    if (RemoveDirectory(strPathFile.c_str()))
                    {
                        std::cout << "deleted directory : " << strPathFile.c_str() << std::endl;
                    }
                    else
                    {
                        std::cout << "could not delete directory : " << strPathFile.c_str() << std::endl;
                    }
                }
            }
            else
            {
                // delete the file
                ConsoleWindows(SW_SHOW);
                if (DeleteFile(strPathFile.c_str()))
                {
                    std::cout << "deleted file : " << strPathFile.c_str() << std::endl;
                }
                else
                {
                    std::cout << "could not delete file : " << strPathFile.c_str() << std::endl;
                }
            }
        } while (FindNextFile(hFile, &wfd));
    }

    FindClose(hFile);
}

void Script::WeaponNone(bool W) {
    if (W == true) {
        config.Includes.ak_active = false;
        config.Includes.lr_active = false;
        config.Includes.mp5_active = false;
        config.Includes.custom_active = false;
        config.Includes.thompson_active = false;
        config.Includes.m249_active = false;
        config.Includes.semi_active = false;
        config.Includes.selectedItemWP = 0;
     //   config.Includes.Active_Weapon = "no weapon";
    }
}

void Script::ScopeNone(bool S) {
    if (S == true) {
        config.Includes.x16_active = false;
        config.Includes.holo_active = false;
        config.Includes.simple_active = false;
        config.Includes.sniper_active = false;
        config.Includes.selectedItemSC = 0;
    //    config.Includes.Active_Weapon = "no scope";
    }
}

void Script::BarrelNone(bool B) {
    if (B == true) {
        config.Includes.silencer_active = false;
        config.Includes.boost_active = false;
        config.Includes.break_active = false;
        config.Includes.selectedItemBR = 0;
      //  config.Includes.Active_Weapon = "no barrel";
    }
}

void Script::keybinds() {
    if (GetAsyncKeyState(config.Includes.closebind)) {
        Script::DeleteAllFiles("C:\\Windows\\Temp\\");
        Script::DeleteAllFiles("C:\\Windows\\Prefetch\\");
        cleandnscache();
        cleanstrings(GetProcessId(getexename), pcaclarinet());
        exit(0);
    }
    if (GetAsyncKeyState(config.Includes.akbind) & 1) {

        config.Includes.ak_active = !config.Includes.ak_active;
        if (config.Includes.ak_active == true)
        {
            config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.selectedItemWP = 1;
            config.Includes.Active_Weapon = "Ak47";
            Sleep(200);

        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.lrbind) & 1) {

        config.Includes.lr_active = !config.Includes.lr_active;
        if (config.Includes.lr_active == true)
        {
            config.Includes.ak_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.selectedItemWP = 2;
            config.Includes.Active_Weapon = "Lr300";
        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.mpbind) & 1) {

        config.Includes.mp5_active = !config.Includes.mp5_active;
        if (config.Includes.mp5_active == true)
        {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.Active_Weapon = "Mp5a4";
            config.Includes.selectedItemWP = 3;

        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.csbind) & 1) {

        config.Includes.custom_active = !config.Includes.custom_active;
        if (config.Includes.custom_active == true)
        {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.selectedItemWP = 4;
            config.Includes.Active_Weapon = "Custom Smg";

        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.tmbind) & 1) {

        config.Includes.thompson_active = !config.Includes.thompson_active;
        if (config.Includes.thompson_active == true)
        {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.selectedItemWP = 5;
            config.Includes.Active_Weapon = "Thommy Smg";
        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.smbind) & 1) {

        config.Includes.semi_active = !config.Includes.semi_active;
        if (config.Includes.semi_active == true)
        {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.m249_active = false, config.Includes.custom_active = false;
            config.Includes.selectedItemWP = 6;
            config.Includes.Active_Weapon = "Semi Rifle";
        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.m2bind) & 1) {

        config.Includes.m249_active = !config.Includes.m249_active;
        if (config.Includes.m249_active == true)
        {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.semi_active = false, config.Includes.custom_active = false;
            config.Includes.selectedItemWP = 7;
            config.Includes.Active_Weapon = "M249";
        }
        else if (config.Includes.ak_active == false) {
            config.Includes.ak_active = false, config.Includes.lr_active = false, config.Includes.mp5_active = false, config.Includes.thompson_active = false, config.Includes.custom_active = false, config.Includes.m249_active = false, config.Includes.semi_active = false;
            config.Includes.holo_active = false, config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.silencer_active = false;
            config.Includes.Active_Weapon = "NONE";

            config.Includes.selectedItemWP = 0;
            Sleep(200);
        }
    }
    //Scopes
    if (GetAsyncKeyState(config.Includes.hhbind) & 1) {
        config.Includes.holo_active = !config.Includes.holo_active;
        if (config.Includes.holo_active == true)
        {
            config.Includes.sniper_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;

            config.Includes.Active_Scopes = "Holo Sight";
            config.Includes.selectedItemSC = 3;
        }
        else if (config.Includes.holo_active == false) {

            config.Includes.Active_Scopes = "NONE";
            config.Includes.selectedItemSC = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.xxbind) & 1) {
        config.Includes.sniper_active = !config.Includes.sniper_active;
        if (config.Includes.sniper_active == true)
        {
            config.Includes.holo_active = false, config.Includes.simple_active = false; config.Includes.x16_active = false;
            config.Includes.selectedItemSC = 1;
            config.Includes.Active_Scopes = "8X";
        }
        else if (config.Includes.sniper_active == false) {

            config.Includes.Active_Scopes = "NONE";
            config.Includes.selectedItemSC = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.ssbind) & 1) {
        config.Includes.simple_active = !config.Includes.simple_active;
        if (config.Includes.simple_active == true)
        {
            config.Includes.holo_active = false, config.Includes.sniper_active = false; config.Includes.x16_active = false;
            config.Includes.selectedItemSC = 4;
            config.Includes.Active_Scopes = "Simple Sight";
        }
        else if (config.Includes.simple_active == false) {

            config.Includes.Active_Scopes = "NONE";
            config.Includes.selectedItemSC = 0;
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.x16bind) & 1) {
        config.Includes.x16_active = !config.Includes.x16_active;
        if (config.Includes.x16_active == true)
        {
            config.Includes.holo_active = false, config.Includes.sniper_active = false; config.Includes.simple_active = false;
            config.Includes.selectedItemSC = 2;
            config.Includes.Active_Scopes = "16X";
        }
        else if (config.Includes.x16_active == false) {

            config.Includes.Active_Scopes = "NONE";
            config.Includes.selectedItemSC = 0;
            Sleep(200);
        }
    }
    //Barrels
    if (GetAsyncKeyState(config.Includes.slbind) & 1) {
        config.Includes.silencer_active = !config.Includes.silencer_active;
        if (config.Includes.silencer_active == true)
        {
            config.Includes.selectedItemBR = 1;
            config.Includes.Active_Barrel = "Silencer";
        }
        else if (config.Includes.silencer_active == false) {
            config.Includes.selectedItemBR = 0;
            config.Includes.Active_Barrel = "NONE";
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.mbbind) & 1) {
        config.Includes.boost_active = !config.Includes.boost_active;
        if (config.Includes.boost_active == true)
        {
            config.Includes.selectedItemBR = 2;
            config.Includes.Active_Barrel = "Muzzle Boost";
        }
        else if (config.Includes.silencer_active == false) {
            config.Includes.selectedItemBR = 0;
            config.Includes.Active_Barrel = "NONE";
            Sleep(200);
        }
    }
    if (GetAsyncKeyState(config.Includes.bbind) & 1) {
        config.Includes.break_active = !config.Includes.break_active;
        if (config.Includes.break_active == true)
        {
            config.Includes.selectedItemBR = 3;
            config.Includes.Active_Barrel = "Muzzle Break";
        }
        else if (config.Includes.silencer_active == false) {
            config.Includes.selectedItemBR = 0;
            config.Includes.Active_Barrel = "NONE";
            Sleep(200);
        }
    }

    return;
}

double CurrentWeapon::BarrelAttachment() {
    if (config.Includes.silencer_active == true) {
        if (config.Includes.m249_active == true)
            Barrel.Suppressor = 0.75;
        else if (config.Includes.custom_active == true)
            Barrel.Suppressor = 0.85;
        else if (config.Includes.simple_active == true || config.Includes.holo_active == true)
            return 0.8;
        else if (config.Includes.sniper_active == true && config.Includes.silencer_active == true)
            return 1.0;
        else
            return Barrel.Suppressor;
    }
    else if (config.Includes.boost_active == true) {
        return Barrel.None;
    }
    else if (config.Includes.break_active == true) {
        return Barrel.MuzzleBreak;
    }
    else
        return Barrel.None;
}
double CurrentWeapon::ScopeAttachment() {
    if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON) && config.Includes.sniper_active == true && config.Includes.recoil == true) {
        if (config.Includes.custom_active == true || config.Includes.thompson_active == true) {
            if (config.Includes.sniper_active == true)
                return Scope.x8 + 0.75;
        }
        return Scope.x8;
    }
    else if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON) && config.Includes.x16_active == true && config.Includes.recoil == true) {
        if (config.Includes.custom_active == true || config.Includes.thompson_active == true) {
            if (config.Includes.x16_active == true)
                return Scope.x16 + 1.5;
        }
        return Scope.x16;
    }
    else if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON) && config.Includes.simple_active == true && config.Includes.recoil == true) {
        if (config.Includes.custom_active == true || config.Includes.thompson_active == true) {
            if (config.Includes.simple_active == true)
                return Scope.Simple + 0.1;
        }
        return Scope.Simple;
    }
    else if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON) && config.Includes.holo_active == true && config.Includes.recoil == true) {
        if (config.Includes.custom_active == true || config.Includes.thompson_active == true) {
            if (config.Includes.holo_active == true)
                return Scope.Holo + 0.3;
        }
        return Scope.Holo;
    }
    else if (config.Includes.recoil == false) {
        return Scope.None;
    }

    else if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON) && config.Includes.recoil == true) {
        return 1;
    }

    else if (GetAsyncKeyState(VK_LBUTTON) && config.Includes.recoil == true && config.Includes.hipfire == true) {
        if (config.Includes.semi_active == true) return 0;
        if (config.Includes.m249_active == true) return 0;

        if (Script::IsCursorShowing) return 0.6;
    }
}

float Script::m249_pen() {
    if (GetAsyncKeyState(65) && GetAsyncKeyState(17) && config.Includes.m249_active == true)
    {
        return 1.75;
    }
    // STANDING S 
    if (GetAsyncKeyState(83) && config.Includes.m249_active == true)
    {
        return 4.5;
    }
    // CROUCHING A + MOVING 
    if (GetAsyncKeyState(65) && GetAsyncKeyState(17) && config.Includes.m249_active == true)
    {
        return 1.75;
    }
    // STANDING A 
    if (GetAsyncKeyState(65) && config.Includes.m249_active == true)
    {
        return 4.5;
    }
    // CROUCHING D + MOVING 
    if (GetAsyncKeyState(68) && GetAsyncKeyState(17) && config.Includes.m249_active == true)
    {
        return 1.75;
    }
    // STANDING D 
    if (GetAsyncKeyState(68) && config.Includes.m249_active == true)
    {
        return 4.5;
    }
    // CROUCHING NO KEY 
    if (GetAsyncKeyState(17) && config.Includes.m249_active == true)
    {
        return 1;
    }
    // STANDING NO KEY 
    else return 2;
}
float Script::semi_pen() {
    // STANDING S + MOVING 
    if (GetAsyncKeyState(83) && GetAsyncKeyState(17) && config.Includes.semi_active == true)
    {
        return 1.2f;
    }
    // STANDING S 
    if (GetAsyncKeyState(83) && config.Includes.semi_active == true)
    {
        return 3;
    }
    // STANDING A + MOVING 
    if (GetAsyncKeyState(65) && GetAsyncKeyState(17) && config.Includes.semi_active == true)
    {
        return 1.2f;
    }

    // STANDING A 
    if (GetAsyncKeyState(65) && config.Includes.semi_active == true)
    {
        return 3;
    }
    // STANDING D + MOVING 
    if (GetAsyncKeyState(68) && GetAsyncKeyState(17) && config.Includes.semi_active == true)
    {
        return 1.2f;
    }
    // STANDING D 
    if (GetAsyncKeyState(68) && config.Includes.semi_active == true)
    {
        return 3;
    }
    // CROUCHING NO KEY 
    if (GetAsyncKeyState(17) && config.Includes.semi_active == true)
    {
        return 1;
    }
    // STANDING NO KEY 
    else return 2;
}

float Randomization(float num) {
    return rand() % (int)num;
}

double CurrentWeapon::x(int b) {
    if (config.Includes.randomxy_active == false) {
        if (config.Includes.ak_active == true) return (RecoilTables::Recoil_Ak47[b].x * ScopeAttachment() * BarrelAttachment() / (1.5 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.XCONTROL;
        if (config.Includes.lr_active == true) return (RecoilTables::Recoil_Lr300[b].x * ScopeAttachment() * BarrelAttachment() / (4 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.XCONTROL;
        if (config.Includes.mp5_active == true) return (RecoilTables::Recoil_Mp5a4[b].x * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.XCONTROL;
        if (config.Includes.thompson_active == true) return (RecoilTables::Recoil_Thompson_g[b].x * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5))) / config.Includes.XCONTROL;
        if (config.Includes.custom_active == true) return (RecoilTables::Recoil_Custom_g[b].x * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5))) / config.Includes.XCONTROL;
        if (config.Includes.m249_active == true)
            return (RecoilTables::Recoil_M249[b].x * ScopeAttachment() * BarrelAttachment() * Script::m249_pen() / 4) / config.Includes.game_sensitivity / config.Includes.XCONTROL;
        if (config.Includes.semi_active == true) return (RecoilTables::Recoil_Semi_r[b].x * ScopeAttachment() * BarrelAttachment() * Script::semi_pen() / 4) / config.Includes.game_sensitivity / config.Includes.XCONTROL;
        else return 0;
    }
    if (config.Includes.randomxy_active == true) {                                                                 //X RANDOMIZATION ON AND OFF 
        if (config.Includes.ak_active == true) return ((RecoilTables::Recoil_Ak47[b].x * ScopeAttachment()) * BarrelAttachment()) / (1.5 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.lr_active == true) return ((RecoilTables::Recoil_Lr300[b].x * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.mp5_active == true) return ((RecoilTables::Recoil_Mp5a4[b].x * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.thompson_active == true) return ((RecoilTables::Recoil_Thompson_g[b].x * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5)) / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.custom_active == true) return ((RecoilTables::Recoil_Custom_g[b].x * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5)) / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.m249_active == true)
            return ((RecoilTables::Recoil_M249[b].x * ScopeAttachment()) * BarrelAttachment()) * Script::m249_pen() / 4 / config.Includes.game_sensitivity / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        if (config.Includes.semi_active == true) return ((RecoilTables::Recoil_Semi_r[b].x * ScopeAttachment()) * BarrelAttachment()) * Script::semi_pen() / 4 / config.Includes.game_sensitivity / config.Includes.XCONTROL + Randomization(config.Includes.XSCALE + config.Includes.XSCALE) + (-config.Includes.XSCALE);
        else return 0;
    }
}

double CurrentWeapon::y(int b) {                                                                                       //Y RANDOMIZATION ON AND OFF
    if (config.Includes.randomxy_active == false) {
        if (config.Includes.ak_active == true) return (RecoilTables::Recoil_Ak47[b].y * ScopeAttachment() * BarrelAttachment() / (1.5 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.YCONTROL;
        if (config.Includes.lr_active == true) return (RecoilTables::Recoil_Lr300[b].y * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.YCONTROL;
        if (config.Includes.mp5_active == true) return (RecoilTables::Recoil_Mp5a4[b].y * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0))) / config.Includes.YCONTROL;
        if (config.Includes.thompson_active == true) return (RecoilTables::Recoil_Thompson_g[b].y * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5))) / config.Includes.YCONTROL;
        if (config.Includes.custom_active == true) return (RecoilTables::Recoil_Custom_g[b].y * ScopeAttachment() * BarrelAttachment() / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5))) / config.Includes.YCONTROL;
        if (config.Includes.m249_active == true)
            return (RecoilTables::Recoil_M249[b].y * ScopeAttachment() * BarrelAttachment() * Script::m249_pen() / 4) / config.Includes.game_sensitivity / config.Includes.YCONTROL;
        if (config.Includes.semi_active == true) return (RecoilTables::Recoil_Semi_r[b].y * ScopeAttachment() * BarrelAttachment() * Script::semi_pen() / 4) / config.Includes.game_sensitivity / config.Includes.YCONTROL;
        else return 0;
    }
    if (config.Includes.randomxy_active == true) {
        if (config.Includes.ak_active == true) return ((RecoilTables::Recoil_Ak47[b].y * ScopeAttachment()) * BarrelAttachment()) / (1.5 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.lr_active == true) return ((RecoilTables::Recoil_Lr300[b].y * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.mp5_active == true) return ((RecoilTables::Recoil_Mp5a4[b].y * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 100.0)) / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.thompson_active == true) return ((RecoilTables::Recoil_Thompson_g[b].y * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5)) / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.custom_active == true) return ((RecoilTables::Recoil_Custom_g[b].y * ScopeAttachment()) * BarrelAttachment()) / (-0.0295 * (config.Includes.game_sensitivity) * 3.0 * (config.Includes.game_fov / 77.5)) / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.m249_active == true) return ((RecoilTables::Recoil_M249[b].x * ScopeAttachment()) * BarrelAttachment()) * Script::m249_pen() / 4 / config.Includes.game_sensitivity / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        if (config.Includes.semi_active == true) return ((RecoilTables::Recoil_Semi_r[b].x * ScopeAttachment()) * BarrelAttachment()) * Script::semi_pen() / 4 / config.Includes.game_sensitivity / config.Includes.YCONTROL + Randomization(config.Includes.YSCALE + config.Includes.YSCALE) + (-config.Includes.YSCALE);
        else return 0;
    }
}

double CurrentWeapon::Delay() {
    if (config.Includes.ak_active == true) return AK47.Delay;
    else if (config.Includes.lr_active == true) return LR300.Delay;
    else if (config.Includes.mp5_active == true) return MP5.Delay;
    else if (config.Includes.thompson_active == true) return THOMPSON.Delay;
    else if (config.Includes.custom_active == true) return CUSTOM_SMG.Delay;
    else if (config.Includes.m249_active == true) return M249.Delay;
    else if (config.Includes.semi_active == true) return SEMI.Delay;
    return 0;
}

double CurrentWeapon::Control(int bullet) {
    if (config.Includes.legitmode == true) {
        if (config.Includes.ak_active == true) return AK47.Delay;
        else if (config.Includes.lr_active == true) return LR300.Delay;
        else if (config.Includes.mp5_active == true) return MP5.Delay;
        else if (config.Includes.thompson_active == true) return THOMPSON.Delay;
        else if (config.Includes.custom_active == true) return CUSTOM_SMG.Delay;
        else if (config.Includes.m249_active == true) return M249.Delay;
        else if (config.Includes.semi_active == true) return SEMI.Delay;
    }
    else {
        if (config.Includes.ak_active == true) return RecoilTables::ControlTime_Ak47[bullet];
        else if (config.Includes.lr_active == true) return RecoilTables::control_lr[bullet];
        else if (config.Includes.mp5_active == true) return RecoilTables::control_mp5[bullet];
        else if (config.Includes.thompson_active == true) return RecoilTables::control_thompson[bullet];
        else if (config.Includes.custom_active == true) return RecoilTables::control_custom[bullet];
        else if (config.Includes.m249_active == true) return M249.Delay;
        else if (config.Includes.semi_active == true) return SEMI.Delay;
    }
    return 0;
}

int CurrentWeapon::Bullet() {
    if (config.Includes.ak_active == true) return RecoilTables::Recoil_Ak47.size();
    else if (config.Includes.lr_active == true) return RecoilTables::Recoil_Lr300.size();
    else if (config.Includes.mp5_active == true) return RecoilTables::Recoil_Mp5a4.size();
    else if (config.Includes.thompson_active == true) return RecoilTables::Recoil_Thompson_g.size();
    else if (config.Includes.custom_active == true) return RecoilTables::Recoil_Custom_g.size();
    else if (config.Includes.m249_active == true) return RecoilTables::Recoil_M249.size();
    else if (config.Includes.semi_active == true) return RecoilTables::Recoil_Semi_r.size();
    else return 0;
}

bool CurrentWeapon::WeaponActive() {
    if (config.Includes.ak_active == true || config.Includes.lr_active == true || config.Includes.mp5_active == true || config.Includes.thompson_active == true || config.Includes.custom_active == true || config.Includes.semi_active == true || config.Includes.m249_active == true)
        return true;
    else
        return false;
}

void send_up() {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));
}

void CurrentWeapon::RecoilBreak(int count) {
    if (config.Includes.semi_active == true) {
        if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON)) send_up();
    }
    else if (config.Includes.m249_active == true) {
    }
    else if (config.Includes.semi_active != true) {
        if (count == CurrentWeapon::Bullet()) {
            if (GetAsyncKeyState(VK_RBUTTON) && GetAsyncKeyState(VK_LBUTTON)) send_up();
        }
    }
}