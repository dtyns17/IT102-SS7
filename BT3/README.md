# Bài tập 03: Phân tích Trade-off – Biến đơn lẻ rời rạc vs Gom nhóm dữ liệu qua Struct

> Kết quả `sizeof` dưới đây đo bằng GCC trên nền tảng x86-64 (`double` căn chỉnh 8 byte, `int` 4 byte, `short` 2 byte).

## 1. Phân tích Input / Output

**Input**

| Dữ liệu | Kiểu | Ràng buộc |
|---|---|---|
| Mã thiết bị `deviceID` | `short` | Số nguyên |
| Tên thiết bị `name` | `char[10]` | Nhập bằng `fgets()`, tối đa 9 ký tự |
| Trạng thái `status` | `char` | Chỉ nhận 0 (Tắt) hoặc 1 (Bật) |
| Điện áp `voltage` U (V) | `double` | U > 0 |
| Dòng điện `current` I (A) | `double` | I >= 0 |
| Công suất đo được `measuredPower` (W) | `double` | >= 0 |
| Thời gian hoạt động `runMinutes` t (phút) | `int` | t >= 0 |

**Output**
- `sizeof` của struct chưa tối ưu và các struct đã tối ưu.
- Công suất thực tế `P_calc = U × I`.
- Các cảnh báo: cảm biến sai lệch, quá tải nguy hiểm, rò rỉ điện.
- Điện năng `kWh = P × (t / 60.0) / 1000.0`.
- Thông báo lỗi và dừng tính toán khi dữ liệu rác.

## 2. Nguyên nhân lãng phí bộ nhớ: Padding

Trình biên dịch căn chỉnh mỗi trường tại địa chỉ chia hết cho kích thước căn chỉnh của nó, và đệm cuối struct cho chia hết kích thước căn chỉnh lớn nhất (ở đây là 8). Khi trường nhỏ (`char`, `short`) nằm xen kẽ giữa các `double`, trình biên dịch chèn byte đệm.

**Struct gốc (chưa tối ưu)**

| Trường | Offset | Kích thước | Byte đệm theo sau |
|---|---|---|---|
| `char status` | 0 | 1 | 7 |
| `double voltage` | 8 | 8 | 0 |
| `char errorFlag` | 16 | 1 | 7 |
| `double current` | 24 | 8 | 0 |
| `short deviceID` | 32 | 2 | 6 |
| `double measuredPower` | 40 | 8 | 0 |
| `char name[10]` | 48 | 10 | 2 |
| `int runMinutes` | 60 | 4 | 0 |

Dữ liệu thực chỉ 42 byte nhưng `sizeof` = **64 byte** (đệm 22 byte, lãng phí khoảng 34%).

## 3. Hai giải pháp tái cấu trúc

### Giải pháp 1: Struct phẳng, sắp xếp kích thước giảm dần
```c
struct DeviceFlatOptimized {
    double voltage, current, measuredPower;  // 24 byte
    int runMinutes;                           // 4
    short deviceID;                           // 2
    char name[10];                            // 10
    char status, errorFlag;                   // 2
};                                            // 42 byte + 6 byte đệm cuối = 48
```
Các trường lớn đứng trước nên không còn đệm ở giữa. Chỉ còn 6 byte đệm cuối struct để `sizeof` chia hết cho 8.

### Giải pháp 2: Struct lồng nhau, nhóm theo chức năng
```c
struct ElectricalData { double voltage, current, measuredPower; };          // 24 byte
struct DeviceInfo     { int runMinutes; short deviceID; char name[10];
                        char status, errorFlag; };                          // 18 + 2 đệm = 20 byte
struct DeviceNested   { struct ElectricalData elec; struct DeviceInfo info; }; // 44 -> đệm 4 -> 48
```
Mỗi nhóm con cũng được sắp xếp giảm dần nên đệm bên trong gần như bằng 0.

## 4. Bảng so sánh

| Tiêu chí | Gốc (chưa tối ưu) | Giải pháp 1: Phẳng tối ưu | Giải pháp 2: Lồng nhau |
|---|---|---|---|
| `sizeof` (đo thực tế) | 64 byte | **48 byte** | **48 byte** |
| Tiết kiệm so với gốc | – | 16 byte (25%) | 16 byte (25%) |
| Cách truy xuất | `dev.voltage` | `dev.voltage` (1 dấu chấm) | `dev.elec.voltage` (2 dấu chấm) |
| Độ phức tạp truy xuất | Thấp | Thấp nhất | Dài hơn một mức, vẫn đơn giản |
| Tính dễ bảo trì | Kém: trường lẫn lộn | Trung bình: thứ tự bị quyết định bởi kích thước, không theo ý nghĩa nên khó đọc | Tốt: mỗi nhóm một mục đích rõ ràng |
| Khả năng mở rộng | Kém: thêm trường dễ phát sinh đệm mới | Trung bình: thêm trường phải chèn đúng chỗ | Tốt: thêm trường vào đúng nhóm, tái sử dụng nhóm cho thiết bị khác |
| Rủi ro | Lãng phí RAM | Dễ bị phá thứ tự khi sửa code | Nếu sắp xếp nhóm con không cẩn thận, đệm xuất hiện ở ranh giới giữa các nhóm |

## 5. Lựa chọn: Giải pháp 2 (Nested Struct, mỗi nhóm đã sắp xếp giảm dần)

Lý do:
1. **Tiết kiệm RAM tương đương**: 48 byte, giảm 25% so với bản gốc. Với 100 thiết bị, tiết kiệm 1.600 byte RAM.
2. **Đóng gói tốt hơn**: dữ liệu đo điện (`elec`) tách khỏi thông tin định danh và trạng thái (`info`). Đọc code là biết trường nào thuộc nhóm nào.
3. **Dễ mở rộng và bảo trì**: thêm tham số đo mới thì chỉ sửa `ElectricalData`, còn mã xử lý thông tin thiết bị không bị ảnh hưởng.
4. **Chi phí thấp**: chỉ thêm một dấu `.` khi truy xuất, không tốn thêm bộ nhớ hay thời gian chạy (offset được tính lúc biên dịch).

Giải pháp 1 vẫn là lựa chọn hợp lý khi struct rất nhỏ và không cần tách nhóm. Lưu ý: chỉ nhóm theo chức năng mà không sắp xếp bên trong thì vẫn bị đệm, nên cả hai giải pháp đều phải áp dụng quy tắc "trường lớn đứng trước".

## 6. Thiết kế xử lý (mã giả)

```
BẮT ĐẦU
  In sizeof: struct gốc, Giải pháp 1, Giải pháp 2
  Nhập: deviceID, name, status, U, I, measuredPower, t
  NẾU status khác 0 và khác 1 -> báo lỗi, DỪNG
  NẾU U <= 0 HOẶC I < 0 HOẶC t < 0 HOẶC measuredPower < 0 -> báo lỗi, DỪNG
  P_calc = U * I
  NẾU |measuredPower - P_calc| > 5% * P_calc -> cảnh báo "Cảm biến công suất báo sai lệch"
  NẾU I > 30.0 HOẶC P_calc > 6600.0          -> cảnh báo "QUÁ TẢI NGUY HIỂM"
  NẾU status == 0 VÀ I > 0.05                 -> cảnh báo "RÒ RỈ ĐIỆN HOẶC TẢI ẨN"
  NẾU status == 1: kWh = P_calc * (t / 60.0) / 1000.0, NGƯỢC LẠI kWh = 0
  In kết quả
KẾT THÚC
```

Các điểm cần lưu ý:
- **Chia số thực**: dùng `t / 60.0` (không phải `t / 60`) để tránh phép chia số nguyên làm mất phần thập phân.
- **Giá trị tuyệt đối**: tính tay bằng `if (x < 0) x = -x`, không cần `math.h`.
- **Công suất P dùng để cảnh báo quá tải và tính kWh**: là công suất thực tế `P_calc = U × I`, theo cách gọi "công suất thực tế" trong đề.
- **Thiết bị TẮT**: không tính điện năng vận hành (kWh = 0). Dòng rò chỉ được phát cảnh báo riêng.

## 7. Kết quả kiểm thử

| # | Đầu vào | Kết quả |
|---|---|---|
| 1 | Bật, 220 V, 10 A, đo 2200 W, 90 phút | Không cảnh báo; 3.3000 kWh |
| 2 | Bật, 220 V, 35 A, đo 7000 W, 120 phút | Cảm biến sai lệch (lệch 700 W); QUÁ TẢI NGUY HIỂM; 15.4000 kWh |
| 3 | Tắt, 220 V, 0.2 A, đo 44 W, 30 phút | RÒ RỈ ĐIỆN HOẶC TẢI ẨN; 0 kWh |
| 4 | Bật, 380 V, 20 A, đo 7600 W, 60 phút | QUÁ TẢI NGUY HIỂM do P = 7600 W > 6600 W (dù I < 30 A); 7.6000 kWh |
| 5 | U = 0 | Báo lỗi, dừng tính toán |
| 6 | I = -1, t = -5 | Báo cả hai lỗi, dừng tính toán |
| 7 | status = 2 | Báo lỗi trạng thái, dừng tính toán |

## 8. Biên dịch và chạy
```bash
gcc -Wall -Wextra -std=c11 main.c -o main
./main
```

## 9. Nộp bài
```bash
git add .
git commit -m "refactor(struct): optimize memory layout for smart device struct"
git push
```
