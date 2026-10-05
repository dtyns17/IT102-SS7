#include <stdio.h>
#include <string.h>

// Cac hang so nghiep vu
#define NGUONG_DONG_DIEN   30.0   // Nguong an toan dong dien (A)
#define NGUONG_VANG_NGUOI  15     // Nguong phong trong de tu dong tat dieu hoa (phut)
#define GIO_TOI_DA         744    // So gio toi da cua thang 31 ngay
// Bieu gia dien bac thang (VND/kWh)
#define GIA_BAC_1 1806.0
#define GIA_BAC_2 1866.0
#define GIA_BAC_3 2167.0
#define GIA_BAC_4 2729.0

// Thiet bi thong minh
struct SmartDevice {
    int deviceID;          // Ma thiet bi
    char deviceName[40];   // Ten thiet bi
    int status;            // 1: Bat, 0: Tat
    double powerWatt;      // Cong suat tieu thu (W)
};

// Cam bien hien dien / dien nang
struct EnergySensor {
    double voltageVolts;       // Dien ap van hanh (V)
    double currentAmperes;     // Dong dien do duoc (A)
    int idleMinutes;           // Thoi gian khong phat hien chuyen dong (phut)
};

// Bao cao tong hop
struct PowerReport {
    double totalPowerWatt;     // Tong cong suat tai (W)
    double totalCurrentAmp;    // Tong dong dien (A)
    double estimatedKWh;       // Dien nang tieu thu uoc tinh (kWh)
    int overloadAlert;         // 1: Vuot 30A, 0: An toan
};

int main() {
    struct SmartDevice airConditioner;
    struct SmartDevice lighting;
    struct EnergySensor sensor;
    struct PowerReport report;

    double shiftHours;          // So gio tinh toan dien nang
    double kwhBac1, kwhBac2, kwhBac3, kwhBac4;
    double tienDien;
    int autoOffTriggered = 0;   // Co danh dau da tu dong tat dieu hoa
    char line[100];             // Bo dem de can le noi dung dashboard

    // ---------- 1. KHOI TAO DU LIEU MO PHONG ----------
    airConditioner.deviceID = 1001;
    strcpy(airConditioner.deviceName, "Dieu hoa nhiet do");
    lighting.deviceID = 1002;
    strcpy(lighting.deviceName, "He thong den chieu sang");

    printf("=== SMART HOME IOT EDGE CONTROLLER - KHU VUC PHONG KHACH ===\n");
    printf("--- Nhap du lieu mo phong ---\n");

    printf("[%s] Trang thai (1: Bat, 0: Tat): ", airConditioner.deviceName);
    if (scanf("%d", &airConditioner.status) != 1 || (airConditioner.status != 0 && airConditioner.status != 1)) {
        printf("Loi: Trang thai chi nhan 0 hoac 1!\n");
        return 1;
    }
    printf("[%s] Cong suat (W): ", airConditioner.deviceName);
    if (scanf("%lf", &airConditioner.powerWatt) != 1 || airConditioner.powerWatt < 0) {
        printf("Loi: Cong suat phai la so khong am!\n");
        return 1;
    }
    printf("[%s] Trang thai (1: Bat, 0: Tat): ", lighting.deviceName);
    if (scanf("%d", &lighting.status) != 1 || (lighting.status != 0 && lighting.status != 1)) {
        printf("Loi: Trang thai chi nhan 0 hoac 1!\n");
        return 1;
    }
    printf("[%s] Cong suat (W): ", lighting.deviceName);
    if (scanf("%lf", &lighting.powerWatt) != 1 || lighting.powerWatt < 0) {
        printf("Loi: Cong suat phai la so khong am!\n");
        return 1;
    }
    printf("[Cam bien] Dien ap van hanh (V): ");
    if (scanf("%lf", &sensor.voltageVolts) != 1 || sensor.voltageVolts <= 0) {
        printf("Loi: Dien ap phai lon hon 0!\n");
        return 1;
    }
    printf("[Cam bien] Thoi gian khong co chuyen dong (phut): ");
    if (scanf("%d", &sensor.idleMinutes) != 1 || sensor.idleMinutes < 0) {
        printf("Loi: Thoi gian khong co chuyen dong phai la so nguyen khong am!\n");
        return 1;
    }
    printf("So gio hoat dong tinh dien nang (0 < gio <= %d): ", GIO_TOI_DA);
    if (scanf("%lf", &shiftHours) != 1 || shiftHours <= 0 || shiftHours > GIO_TOI_DA) {
        printf("Loi: So gio hoat dong khong hop le!\n");
        return 1;
    }

    // Thiet bi dang TAT thi khong tieu thu cong suat
    if (airConditioner.status == 0) {
        airConditioner.powerWatt = 0.0;
    }
    if (lighting.status == 0) {
        lighting.powerWatt = 0.0;
    }

    // ---------- 2. KICH BAN TU DONG HOA: TAT DIEU HOA KHI PHONG TRONG ----------
    if (sensor.idleMinutes > NGUONG_VANG_NGUOI && airConditioner.status == 1) {
        airConditioner.status = 0;
        airConditioner.powerWatt = 0.0;
        autoOffTriggered = 1;
    }

    // ---------- 3. TONG HOP VAO PowerReport ----------
    report.totalPowerWatt = airConditioner.powerWatt + lighting.powerWatt;
    // Dong dien tung thiet bi I = P / U (tai thuan tro), cong lai thanh tong dong dien
    report.totalCurrentAmp = airConditioner.powerWatt / sensor.voltageVolts
                           + lighting.powerWatt / sensor.voltageVolts;
    sensor.currentAmperes = report.totalCurrentAmp; // Cam bien do lai sau khi tu dong hoa
    report.estimatedKWh = report.totalPowerWatt * shiftHours / 1000.0;
    report.overloadAlert = (report.totalCurrentAmp > NGUONG_DONG_DIEN) ? 1 : 0;

    // ---------- 4. TINH TIEN DIEN BAC THANG ----------
    kwhBac1 = 0; kwhBac2 = 0; kwhBac3 = 0; kwhBac4 = 0;
    if (report.estimatedKWh <= 50) {
        kwhBac1 = report.estimatedKWh;
    } else if (report.estimatedKWh <= 100) {
        kwhBac1 = 50;
        kwhBac2 = report.estimatedKWh - 50;
    } else if (report.estimatedKWh <= 200) {
        kwhBac1 = 50;
        kwhBac2 = 50;
        kwhBac3 = report.estimatedKWh - 100;
    } else {
        kwhBac1 = 50;
        kwhBac2 = 50;
        kwhBac3 = 100;
        kwhBac4 = report.estimatedKWh - 200;
    }
    tienDien = kwhBac1 * GIA_BAC_1 + kwhBac2 * GIA_BAC_2 + kwhBac3 * GIA_BAC_3 + kwhBac4 * GIA_BAC_4;

    // ---------- 5. DASHBOARD ----------
    printf("\n");
    printf("+================================================================+\n");
    printf("| %-62s |\n", "SMART HOME DASHBOARD - PHONG KHACH (RkHome)");
    printf("+================================================================+\n");
    printf("| %-62s |\n", "1. TRANG THAI THIET BI");
    printf("+------+---------------------------+------------+----------------+\n");
    printf("| %-4s | %-25s | %-10s | %-14s |\n", "ID", "Ten thiet bi", "Trang thai", "Cong suat (W)");
    printf("+------+---------------------------+------------+----------------+\n");
    printf("| %-4d | %-25s | %-10s | %-14.1f |\n", airConditioner.deviceID, airConditioner.deviceName,
           airConditioner.status == 1 ? "ON" : "OFF", airConditioner.powerWatt);
    printf("| %-4d | %-25s | %-10s | %-14.1f |\n", lighting.deviceID, lighting.deviceName,
           lighting.status == 1 ? "ON" : "OFF", lighting.powerWatt);
    printf("+------+---------------------------+------------+----------------+\n");

    printf("| %-62s |\n", "2. CHI SO CAM BIEN (REAL-TIME)");
    printf("+----------------------------------------------------------------+\n");
    sprintf(line, "  Dien ap van hanh     : %.1f V", sensor.voltageVolts);
    printf("| %-62s |\n", line);
    sprintf(line, "  Dong dien do duoc    : %.2f A", sensor.currentAmperes);
    printf("| %-62s |\n", line);
    sprintf(line, "  Phong khong co nguoi : %d phut", sensor.idleMinutes);
    printf("| %-62s |\n", line);
    printf("+----------------------------------------------------------------+\n");

    printf("| %-62s |\n", "3. BANG TONG HOP TIEU THU DIEN NANG");
    printf("+----------------------------------------------------------------+\n");
    sprintf(line, "  Tong cong suat tai   : %.1f W", report.totalPowerWatt);
    printf("| %-62s |\n", line);
    sprintf(line, "  Tong dong dien       : %.2f A", report.totalCurrentAmp);
    printf("| %-62s |\n", line);
    sprintf(line, "  Thoi gian tinh toan  : %.1f gio", shiftHours);
    printf("| %-62s |\n", line);
    sprintf(line, "  Dien nang uoc tinh   : %.3f kWh", report.estimatedKWh);
    printf("| %-62s |\n", line);
    sprintf(line, "  Chi phi uoc tinh     : %.0f VND (chua VAT)", tienDien);
    printf("| %-62s |\n", line);
    printf("+----------------------------------------------------------------+\n");

    printf("| %-62s |\n", "4. CANH BAO / SU KIEN");
    printf("+----------------------------------------------------------------+\n");
    if (autoOffTriggered) {
        sprintf(line, "[TU DONG HOA] Phong trong %d phut (> %d phut)", sensor.idleMinutes, NGUONG_VANG_NGUOI);
        printf("| %-62s |\n", line);
        sprintf(line, "  -> Da TAT %s, cong suat ve 0 W", airConditioner.deviceName);
        printf("| %-62s |\n", line);
    }
    if (report.overloadAlert == 1) {
        printf("| %-62s |\n", "[!!! CANH BAO QUA TAI NGUY HIEM !!!]");
        sprintf(line, "  Tong dong dien %.2f A vuot nguong %.1f A", report.totalCurrentAmp, NGUONG_DONG_DIEN);
        printf("| %-62s |\n", line);
    }
    if (!autoOffTriggered && report.overloadAlert == 0) {
        printf("| %-62s |\n", "He thong hoat dong an toan, khong co canh bao.");
    }
    printf("+================================================================+\n");

    return 0;
}
