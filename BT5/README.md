# Bài tập 05: Giám sát nốt cảm biến môi trường SMART_HOME_IOT

## Mô tả
Chương trình C (C11) nhập thông tin một nốt cảm biến vào biến `struct EnvironmentSensor`, kiểm tra dung lượng pin, đánh giá mức an toàn và in báo cáo giám sát.

## Cấu trúc dữ liệu
```c
struct EnvironmentSensor {
    int sensor_id;        // Mã định danh
    char room_name[51];   // Tên phòng (tối đa 50 ký tự, không có khoảng trắng)
    float temperature;    // Nhiệt độ (°C)
    float humidity;       // Độ ẩm (%)
    int battery_level;    // Pin còn lại (%)
};
```

## Quy tắc đánh giá (kiểm tra theo thứ tự)
1. `battery_level` ngoài [0, 100] → in `Loi: Dung luong pin khong hop le!` và kết thúc.
2. **CRITICAL**: `battery_level < 10` **hoặc** `temperature > 60.0`.
3. **WARNING** (không thuộc CRITICAL): `10 <= battery_level <= 20` **hoặc** `40.0 <= temperature <= 60.0` **hoặc** `humidity > 85.0`.
4. **SAFE**: các trường hợp còn lại.

## Kết quả kiểm thử

| Case | Input (id, phòng, nhiệt độ, độ ẩm, pin) | Kết quả | Đạt |
|---|---|---|---|
| 1 | 101, LivingRoom, 28.5, 65.0, 85 | SAFE | ✔ |
| 2 | 102, Kitchen, 42.0, 88.0, 15 | WARNING | ✔ |
| 3 | 103, ServerRoom, 62.5, 50.0, 5 | CRITICAL | ✔ |
| 4 | 104, Bedroom, 25.0, 50.0, 120 | `Loi: Dung luong pin khong hop le!` | ✔ |
| 5 | Biên nhiệt độ 39.99 / 40.0 / 60.0 / 60.5 (pin 50, ẩm ≤ 85) | SAFE / WARNING / WARNING / CRITICAL | ✔ |
| 6 | Biên pin 100 / 21 / 10 / 9 / 0 | SAFE / SAFE / WARNING / CRITICAL / CRITICAL | ✔ |
| 7 | Biên độ ẩm 85.5 (pin 50, nhiệt 25) | WARNING | ✔ |
| 8 | Pin -1 | `Loi: Dung luong pin khong hop le!` | ✔ |

## Biên dịch & chạy
```bash
gcc -Wall -Wextra -std=c11 main.c -o main
./main
```

## Tuân thủ quy định
- Không dùng mảng struct, không định nghĩa hàm ngoài `main()`.
- Truy xuất mọi trường bằng toán tử `.`, tên biến theo `snake_case`.
- Số thực in với `%.2f`.
