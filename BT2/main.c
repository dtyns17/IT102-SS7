#include <stdio.h>
#include <string.h>

// Cau truc thiet bi thong minh
struct SmartDevice {
    int deviceID;          // Ma dinh danh thiet bi
    char deviceName[50];   // Ten thiet bi
    int status;            // 1: Dang bat, 0: Da tat
    float ratedPowerKW;    // Cong suat dinh muc (kW)
};

// Cau truc cam bien dien nang & hien dien
struct EnergySensor {
    float voltageVolts;       // Dien ap do duoc (V)
    float currentAmperes;     // Dong dien do duoc (A)
    int unoccupancyMinutes;   // Thoi gian vang nguoi (phut)
};

int main() {
    struct SmartDevice device;
    struct EnergySensor sensor;

    float operatingHours;     // So gio van hanh trong thang (nguoi dung nhap)
    float kWh;                // Tong dien nang tieu thu
    double tienDien;          // Tong tien dien chua VAT
    double tien1, tien2, tien3, tien4; // Tien tung bac
    float kwhBac1, kwhBac2, kwhBac3, kwhBac4;
    int c;

    printf("=== MODULE QUAN LY THIET BI PHONG THONG MINH (SMART GATEWAY) ===\n");

    // ---------- BUOC 1: NHAP DU LIEU ----------
    printf("--- Thong tin thiet bi ---\n");
    printf("Ma dinh danh thiet bi (so nguyen): ");
    if (scanf("%d", &device.deviceID) != 1) {
        printf("Loi: Ma thiet bi khong hop le!\n");
        return 1;
    }
    while ((c = getchar()) != '\n' && c != EOF); // Xoa bo dem truoc khi dung fgets

    printf("Ten thiet bi: ");
    if (fgets(device.deviceName, sizeof(device.deviceName), stdin) == NULL) {
        printf("Loi: Khong doc duoc ten thiet bi!\n");
        return 1;
    }
    if (strlen(device.deviceName) > 0 && device.deviceName[strlen(device.deviceName) - 1] == '\n') {
        device.deviceName[strlen(device.deviceName) - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF); // Ten qua dai: bo phan du
    }

    printf("Trang thai hien tai (1: Dang bat, 0: Da tat): ");
    if (scanf("%d", &device.status) != 1 || (device.status != 0 && device.status != 1)) {
        printf("Loi: Trang thai chi nhan gia tri 0 hoac 1!\n");
        return 1;
    }
    printf("Cong suat dinh muc (kW): ");
    if (scanf("%f", &device.ratedPowerKW) != 1 || device.ratedPowerKW <= 0) {
        printf("Loi: Cong suat dinh muc phai lon hon 0!\n");
        return 1;
    }

    printf("--- Du lieu cam bien ---\n");
    printf("Dien ap do duoc (V): ");
    if (scanf("%f", &sensor.voltageVolts) != 1) {
        printf("Loi: Du lieu dien ap khong hop le!\n");
        return 1;
    }
    printf("Dong dien do duoc (A): ");
    if (scanf("%f", &sensor.currentAmperes) != 1) {
        printf("Loi: Du lieu dong dien khong hop le!\n");
        return 1;
    }
    printf("Thoi gian vang nguoi (phut): ");
    if (scanf("%d", &sensor.unoccupancyMinutes) != 1) {
        printf("Loi: Thoi gian vang nguoi khong hop le!\n");
        return 1;
    }
    printf("So gio van hanh trong thang (gio): ");
    if (scanf("%f", &operatingHours) != 1) {
        printf("Loi: So gio van hanh khong hop le!\n");
        return 1;
    }

    // ---------- BUOC 2: KIEM TRA DU LIEU CAM BIEN (LOI THUONG GAP 1) ----------
    if (sensor.voltageVolts < 0 || sensor.currentAmperes < 0 || sensor.unoccupancyMinutes < 0) {
        printf("\n[LOI CAM BIEN] Phat hien thong so am (dien ap/dong dien/thoi gian vang nguoi).\n");
        printf("Day la dau hieu loi phan cung cam bien. Huy thao tac xu ly!\n");
        return 1;
    }

    printf("\n--- KET QUA XU LY ---\n");
    printf("Thiet bi: %s (ID: %d) | Trang thai ban dau: %s\n",
           device.deviceName, device.deviceID, device.status == 1 ? "DANG BAT" : "DA TAT");

    // ---------- BUOC 3: KIEM SOAT AN TOAN DIEN (UU TIEN 1) ----------
    if (sensor.currentAmperes > 30.0f) {
        device.status = 0; // Ngat khan cap
        printf("[CANH BAO] Dong dien %.2f A vuot nguong an toan 30.0 A! Nguy co chay no.\n", sensor.currentAmperes);
        printf("          -> NGAT KHAN CAP thiet bi.\n");
    }

    // ---------- BUOC 4: TU DONG HOA TIET KIEM NANG LUONG (UU TIEN 2) ----------
    if (device.status == 1 && sensor.unoccupancyMinutes >= 15) {
        device.status = 0;
        printf("[TIET KIEM] Phong trong lien tuc %d phut (>= 15 phut) -> Tu dong TAT thiet bi.\n",
               sensor.unoccupancyMinutes);
    }

    printf("Trang thai thiet bi sau xu ly: %s\n", device.status == 1 ? "DANG BAT" : "DA TAT");

    // ---------- BUOC 5: KIEM TRA SO GIO VAN HANH (LOI THUONG GAP 2) ----------
    if (operatingHours <= 0 || operatingHours > 744) {
        printf("\n[LOI] So gio van hanh (%.2f) khong hop le. Phai lon hon 0 va khong vuot qua 744 gio.\n", operatingHours);
        printf("Tu choi tinh toan hoa don tien dien!\n");
        return 1;
    }

    // ---------- BUOC 6: TINH DIEN NANG & TIEN DIEN BAC THANG EVN ----------
    kWh = device.ratedPowerKW * operatingHours;

    kwhBac1 = 0; kwhBac2 = 0; kwhBac3 = 0; kwhBac4 = 0;
    if (kWh <= 50) {
        kwhBac1 = kWh;
    } else if (kWh <= 100) {
        kwhBac1 = 50;
        kwhBac2 = kWh - 50;
    } else if (kWh <= 200) {
        kwhBac1 = 50;
        kwhBac2 = 50;
        kwhBac3 = kWh - 100;
    } else {
        kwhBac1 = 50;
        kwhBac2 = 50;
        kwhBac3 = 100;
        kwhBac4 = kWh - 200;
    }
    tien1 = kwhBac1 * 1806.0;
    tien2 = kwhBac2 * 1866.0;
    tien3 = kwhBac3 * 2167.0;
    tien4 = kwhBac4 * 2729.0;
    tienDien = tien1 + tien2 + tien3 + tien4;

    // ---------- BUOC 7: XUAT BAO CAO ----------
    printf("\n=========== BAO CAO DIEN NANG & CHI PHI ===========\n");
    printf("Cong suat dinh muc : %.2f kW\n", device.ratedPowerKW);
    printf("So gio van hanh    : %.2f gio\n", operatingHours);
    printf("Tong dien nang     : %.2f kWh\n", kWh);
    printf("-----------------------------------------------------\n");
    printf("Bac 1 (1.806 VND/kWh): %8.2f kWh -> %12.0f VND\n", kwhBac1, tien1);
    printf("Bac 2 (1.866 VND/kWh): %8.2f kWh -> %12.0f VND\n", kwhBac2, tien2);
    printf("Bac 3 (2.167 VND/kWh): %8.2f kWh -> %12.0f VND\n", kwhBac3, tien3);
    printf("Bac 4 (2.729 VND/kWh): %8.2f kWh -> %12.0f VND\n", kwhBac4, tien4);
    printf("-----------------------------------------------------\n");
    printf("TONG TIEN DIEN (chua VAT): %.0f VND\n", tienDien);
    printf("=====================================================\n");

    return 0;
}
