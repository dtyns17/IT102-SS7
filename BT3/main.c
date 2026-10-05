#include <stdio.h>
#include <string.h>

// ===== PHUONG AN GOC: struct phang CHUA toi uu (thu tu truong tu phat) =====
struct DeviceUnoptimized {
    char status;           // 1 byte  -> theo sau la 7 byte dem
    double voltage;        // 8 byte
    char errorFlag;        // 1 byte  -> theo sau la 7 byte dem
    double current;        // 8 byte
    short deviceID;        // 2 byte  -> theo sau la 6 byte dem
    double measuredPower;  // 8 byte
    char name[10];         // 10 byte -> theo sau la 2 byte dem
    int runMinutes;        // 4 byte
};

// ===== GIAI PHAP 1: struct phang, sap xep kich thuoc GIAM DAN =====
struct DeviceFlatOptimized {
    double voltage;        // 8 byte
    double current;        // 8 byte
    double measuredPower;  // 8 byte
    int runMinutes;        // 4 byte
    short deviceID;        // 2 byte
    char name[10];         // 10 byte
    char status;           // 1 byte
    char errorFlag;        // 1 byte
};

// ===== GIAI PHAP 2: struct long nhau (nhom theo chuc nang) =====
struct ElectricalData {    // Nhom du lieu do dien
    double voltage;
    double current;
    double measuredPower;
};

struct DeviceInfo {        // Nhom dinh danh & trang thai
    int runMinutes;
    short deviceID;
    char name[10];
    char status;
    char errorFlag;
};

struct DeviceNested {
    struct ElectricalData elec;
    struct DeviceInfo info;
};

int main() {
    // Phuong an duoc chon cho he thong: Giai phap 2 (long nhau, moi nhom da sap xep giam dan)
    struct DeviceNested dev;

    double pCalc, pDiff, pLimit, kWh;
    int hasError, c;

    // ---------- 1. SO SANH BO NHO ----------
    printf("=== PHAN TICH BO NHO (Memory Padding) ===\n");
    printf("Tong kich thuoc cac truong thuc te : %d byte\n",
           (int)(3 * sizeof(double) + sizeof(int) + sizeof(short) + 10 * sizeof(char) + 2 * sizeof(char)));
    printf("sizeof(struct DeviceUnoptimized)   : %d byte\n", (int)sizeof(struct DeviceUnoptimized));
    printf("sizeof(struct DeviceFlatOptimized) : %d byte (Giai phap 1)\n", (int)sizeof(struct DeviceFlatOptimized));
    printf("sizeof(struct DeviceNested)        : %d byte (Giai phap 2)\n", (int)sizeof(struct DeviceNested));
    printf("  - sizeof(struct ElectricalData)  : %d byte\n", (int)sizeof(struct ElectricalData));
    printf("  - sizeof(struct DeviceInfo)      : %d byte\n", (int)sizeof(struct DeviceInfo));
    printf("Tiet kiem so voi ban goc (G1)      : %d byte/thiet bi\n",
           (int)(sizeof(struct DeviceUnoptimized) - sizeof(struct DeviceFlatOptimized)));
    printf("Tiet kiem so voi ban goc (G2)      : %d byte/thiet bi\n",
           (int)(sizeof(struct DeviceUnoptimized) - sizeof(struct DeviceNested)));

    // ---------- 2. NHAP DU LIEU ----------
    printf("\n=== NHAP THONG SO THIET BI ===\n");
    printf("Ma thiet bi (so nguyen): ");
    {
        int idTam;
        if (scanf("%d", &idTam) != 1) {
            printf("Loi: Ma thiet bi khong hop le!\n");
            return 1;
        }
        dev.info.deviceID = (short)idTam;
    }
    while ((c = getchar()) != '\n' && c != EOF); // Xoa bo dem truoc khi dung fgets

    printf("Ten thiet bi: ");
    if (fgets(dev.info.name, sizeof(dev.info.name), stdin) == NULL) {
        printf("Loi: Khong doc duoc ten thiet bi!\n");
        return 1;
    }
    if (strlen(dev.info.name) > 0 && dev.info.name[strlen(dev.info.name) - 1] == '\n') {
        dev.info.name[strlen(dev.info.name) - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF); // Ten qua dai: bo phan du
    }

    {
        int statusTam;
        printf("Trang thai (1: Bat, 0: Tat): ");
        if (scanf("%d", &statusTam) != 1) {
            printf("Loi: Trang thai khong hop le!\n");
            return 1;
        }
        if (statusTam != 0 && statusTam != 1) {
            printf("Loi: Trang thai chi nhan 0 hoac 1. Dung tinh toan!\n");
            return 1;
        }
        dev.info.status = (char)statusTam;
    }
    printf("Dien ap U (V): ");
    if (scanf("%lf", &dev.elec.voltage) != 1) {
        printf("Loi: Dien ap khong hop le!\n");
        return 1;
    }
    printf("Dong dien I (A): ");
    if (scanf("%lf", &dev.elec.current) != 1) {
        printf("Loi: Dong dien khong hop le!\n");
        return 1;
    }
    printf("Cong suat do duoc tu thiet bi (W): ");
    if (scanf("%lf", &dev.elec.measuredPower) != 1) {
        printf("Loi: Cong suat do duoc khong hop le!\n");
        return 1;
    }
    printf("Thoi gian hoat dong t (phut): ");
    if (scanf("%d", &dev.info.runMinutes) != 1) {
        printf("Loi: Thoi gian hoat dong khong hop le!\n");
        return 1;
    }

    // ---------- 3. KIEM TRA DU LIEU RAC ----------
    hasError = 0;
    if (dev.elec.voltage <= 0) {
        printf("[LOI] Dien ap U <= 0 la du lieu rac/loi cam bien.\n");
        hasError = 1;
    }
    if (dev.elec.current < 0) {
        printf("[LOI] Dong dien I < 0 la du lieu rac/loi cam bien.\n");
        hasError = 1;
    }
    if (dev.info.runMinutes < 0) {
        printf("[LOI] Thoi gian hoat dong t < 0 la du lieu rac/loi cam bien.\n");
        hasError = 1;
    }
    if (dev.elec.measuredPower < 0) {
        printf("[LOI] Cong suat do duoc < 0 la du lieu rac/loi cam bien.\n");
        hasError = 1;
    }
    if (hasError) {
        printf("Dung tinh toan do du lieu dau vao bat hop le!\n");
        return 1;
    }

    // ---------- 4. TINH TOAN & KIEM TRA AN TOAN ----------
    printf("\n=== KET QUA PHAN TICH THIET BI '%s' (ID %d) ===\n", dev.info.name, dev.info.deviceID);
    printf("Trang thai: %s\n", dev.info.status == 1 ? "BAT" : "TAT");

    // 4.1 Cong suat thuc te P_calc = U x I va kiem tra lech cam bien > 5%
    pCalc = dev.elec.voltage * dev.elec.current;
    pDiff = dev.elec.measuredPower - pCalc;
    if (pDiff < 0) {
        pDiff = -pDiff; // Tri tuyet doi (khong dung math.h)
    }
    pLimit = pCalc * 0.05;
    printf("Cong suat thuc te P_calc = U x I = %.2f W | Cong suat do duoc = %.2f W\n",
           pCalc, dev.elec.measuredPower);
    if (pCalc == 0.0) {
        if (dev.elec.measuredPower > 0.0) {
            printf("[CANH BAO] Cam bien cong suat bao sai lech (P_calc = 0 nhung do duoc > 0)\n");
        }
    } else if (pDiff > pLimit) {
        printf("[CANH BAO] Cam bien cong suat bao sai lech (lech %.2f W, vuot 5%% cua P_calc)\n", pDiff);
    }

    // 4.2 Qua tai dong dien / cong suat (dung cong suat thuc te P_calc)
    if (dev.elec.current > 30.0 || pCalc > 6600.0) {
        printf("[CANH BAO] QUA TAI NGUY HIEM (I = %.2f A, P = %.2f W)\n", dev.elec.current, pCalc);
    }

    // 4.3 Ro ri dien nang khi thiet bi dang TAT
    if (dev.info.status == 0 && dev.elec.current > 0.05) {
        printf("[CANH BAO] RO RI DIEN HOAC TAI AN (thiet bi TAT nhung I = %.3f A)\n", dev.elec.current);
    }

    // 4.4 Dien nang tieu thu: kWh = P x (t / 60.0) / 1000.0 (phep chia so thuc)
    if (dev.info.status == 1) {
        kWh = pCalc * (dev.info.runMinutes / 60.0) / 1000.0;
        printf("Dien nang tieu thu: %.4f kWh (t = %d phut)\n", kWh, dev.info.runMinutes);
    } else {
        kWh = 0.0;
        printf("Dien nang tieu thu: %.4f kWh (thiet bi dang TAT nen khong tinh dien nang van hanh)\n", kWh);
    }

    return 0;
}
