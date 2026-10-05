#include <stdio.h>

// Khai bao cau truc quan ly thiet bi thong minh
struct SmartDevice {
    int id;
    float voltage;     // Dien ap (V)
    float current;     // Dong dien (A)
    float power_watt;  // Cong suat tieu thu (W)
    int is_active;     // Trang thai thiet bi: 1 - Bat, 0 - Tat
    int is_overloaded; // Canh bao qua tai: 1 - Qua tai, 0 - An toan
};

int main() {
    // Khoi tao thong tin thiet bi Binh Nong Lanh
    struct SmartDevice heater;
    heater.id = 101;
    heater.voltage = 220.0f;
    heater.current = 35.0f; // Cam bien ghi nhan dong dien 35A (vuot nguong 30A)
    heater.is_active = 1;   // Nguoi dung bam BAT thiet bi
    heater.is_overloaded = 0;
    heater.power_watt = 0.0f;

    // Buoc 1: Kiem tra quy tac an toan qua tai (chi xet khi thiet bi dang BAT)
    if (heater.is_active == 1 && heater.current > 30.0f) {
        heater.is_overloaded = 1;
        heater.is_active = 0; // Tu dong ngat dien thiet bi
    }

    // Buoc 2: Tinh cong suat SAU KHI trang thai da duoc chot
    if (heater.is_active == 1) {
        heater.power_watt = heater.voltage * heater.current; // P = V x I
    } else {
        heater.power_watt = 0.0f; // Thiet bi TAT -> cong suat bat buoc bang 0
    }

    // In thong so thiet bi ra man hinh ung dung IoT
    printf("=== HETHONG GIAM SAT THIET BI SMART HOME ===\n");
    printf("Ma thiet bi: %d\n", heater.id);
    printf("Trang thai hoat dong: %s\n", heater.is_active ? "BAT" : "TAT");
    printf("Cong suat tieu thu: %.2f W\n", heater.power_watt);
    printf("Canh bao qua tai (>30A): %s\n", heater.is_overloaded ? "CO (DA NGAT DIEN)" : "KHONG");

    return 0;
}
