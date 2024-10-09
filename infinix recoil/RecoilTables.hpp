#include <vector>

typedef struct Vec1 {
    double x;
    double y;
} vec1;

typedef struct Vec2 {
    double x;
    double y;
} vec2;

class RecoilTables {
public:
    // old std::vector<vec1> Recoil_Ak47 = { {-69.0, 100.0},{10.0, 92.0},{-110.0, 83.0},{-85.0, 75.0},{0.0, 67.0},{33.0, 57.0},{58.0, 48.0},{76.0, 39.0},{84.0, 29.0},{85.0, 19.0},{76.0, 20.0},{60.0, 37.0},{34.0, 50.0},{2.0, 59.0},{-30.0, 65.0},{-55.0, 67.0},{-74.0, 64.0},{-86.0, 59.0},{-92.0, 49.0},{-91.0, 34.0},{-84.0, 17.0},{-70.0, 10.0},{-49.0, 28.0},{-22.0, 42.0},{24.0, 51.0},{72.0, 56.0},{97.0, 57.0},{98.0, 51.0},{77.0, 43.0},{35.0, 35.0}, {0, 0} };
    static std::vector<vec1> Recoil_Ak47;
    static std::vector<vec1> Recoil_Lr300;
    static std::vector<vec1> Recoil_Mp5a4;
    static std::vector<vec1> Recoil_Custom_g;
    static std::vector<vec1> Recoil_Thompson_g;
    static std::vector<vec1> Recoil_Semi_r;
    static std::vector<vec2> Recoil_M249;
    static std::vector<double> ControlTime_Ak47;
    static std::vector<double> control_lr;
    static std::vector<double> control_mp5;
    static std::vector<double> control_custom;
    static std::vector<double> control_thompson;
};

struct {
    const double Delay = 133.3300018310547f;//133.333333333
}AK47;
struct {
    const int Delay = 120;
}LR300;
struct {
    const int Delay = 100;
}MP5;
struct {
    const int Delay = 100;
}CUSTOM_SMG;
struct {
    const int Delay = 129;
}THOMPSON;
struct {
    const int Delay = 150;
}SEMI;
struct {
    const int Delay = 103;
}M249;

struct {
    double x8 = 3.84;
    double x16 = 7.68;
    double Holo = 1.2;
    double Simple = 0.8;
    double None = 1.0;
}Scope;

struct {
    double MuzzleBoost = 1.0;
    double Suppressor = 0.80;
    double MuzzleBreak = 0.5;
    double None = 1.0;
}Barrel;

struct {
    int crouch = 2;
    int none = 1;
}Crouch;