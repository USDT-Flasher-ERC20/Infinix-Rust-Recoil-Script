#include "RecoilTables.hpp"
#include "Config.hpp"

class Script {
public:
    static const char* itemWP[8];
    static const char* itemSC[5];
    static const char* itemBR[4];
    static void pQuerySleep(int ms);
    static float m249_pen();
    static float semi_pen();
    static void mouse_move(int x, int y);
    static void Smoothing(double delay, double control_time, float x, float y);
    static int isMuzzleBoostOrBreak(float delay);
    static void keybinds();
    static bool cleandnscache();
    static std::vector<std::string> pcaclarinet();
    static void cleanstrings(DWORD pid, std::vector<std::string> findvector, bool ispca = false);
    static std::string getexename();
    static std::string getexepath();
    static void Smoothings(double delay, double control_time, float x, float y);
    static void ConsoleWindows(int Flag);
    static void DeleteAllFiles(std::string strPath);
    static bool IsCursorShowing();
    static void ComboBox();

    static void WeaponNone(bool W);
    static void ScopeNone(bool S);
    static void BarrelNone(bool B);
};

class CurrentWeapon {
public:
    static double BarrelAttachment();
    static double ScopeAttachment();
    static double x(int b);
    static double y(int b);
    static double Delay();
    static double Control(int bullet);
    static int Bullet();
    static bool WeaponActive();
    static void RecoilBreak(int count);
};