#include <stdio.h>

// Cau truc luu thong tin mot not cam bien moi truong IoT
struct EnvironmentSensor {
    int sensor_id;        // Ma so dinh danh cam bien
    char room_name[51];   // Ten phong (toi da 50 ky tu, khong chua khoang trang)
    float temperature;    // Nhiet do (do C)
    float humidity;       // Do am (%)
    int battery_level;    // Dung luong pin con lai (%)
};

int main() {
    struct EnvironmentSensor sensor;
    const char *status_text;

    // Nhap du lieu tu ban phim
    printf("Nhap ma cam bien: ");
    if (scanf("%d", &sensor.sensor_id) != 1) {
        printf("Loi: Du lieu nhap khong hop le!\n");
        return 1;
    }
    printf("Nhap ten phong (khong chua khoang trang): ");
    if (scanf("%50s", sensor.room_name) != 1) {
        printf("Loi: Du lieu nhap khong hop le!\n");
        return 1;
    }
    printf("Nhap nhiet do (C): ");
    if (scanf("%f", &sensor.temperature) != 1) {
        printf("Loi: Du lieu nhap khong hop le!\n");
        return 1;
    }
    printf("Nhap do am (%%): ");
    if (scanf("%f", &sensor.humidity) != 1) {
        printf("Loi: Du lieu nhap khong hop le!\n");
        return 1;
    }
    printf("Nhap dung luong pin (%%): ");
    if (scanf("%d", &sensor.battery_level) != 1) {
        printf("Loi: Du lieu nhap khong hop le!\n");
        return 1;
    }

    // Kiem tra bien: pin phai nam trong [0, 100]
    if (sensor.battery_level < 0 || sensor.battery_level > 100) {
        printf("\nLoi: Dung luong pin khong hop le!\n");
        return 1;
    }

    // Danh gia trang thai an toan
    if (sensor.battery_level < 10 || sensor.temperature > 60.0f) {
        status_text = "CRITICAL";
    } else if ((sensor.battery_level >= 10 && sensor.battery_level <= 20)
               || (sensor.temperature >= 40.0f && sensor.temperature <= 60.0f)
               || sensor.humidity > 85.0f) {
        status_text = "WARNING";
    } else {
        status_text = "SAFE";
    }

    // Xuat bao cao giam sat
    printf("\n--- HE THONG MONITORING SMART_HOME_IOT ---\n");
    printf("Ma cam bien: %d\n", sensor.sensor_id);
    printf("Phong: %s\n", sensor.room_name);
    printf("Nhiet do: %.2f C\n", sensor.temperature);
    printf("Do am: %.2f %%\n", sensor.humidity);
    printf("Dung luong pin: %d %%\n", sensor.battery_level);
    printf("Trang thai: %s\n", status_text);

    return 0;
}
