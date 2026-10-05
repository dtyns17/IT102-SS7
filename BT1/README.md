# Bài tập 01: Dò vết lỗi truy xuất thuộc tính struct thiết bị thông minh qua dấu chấm

## 1. Phân tích lỗi

### Dòng lệnh sai logic
```c
heater.power_watt = heater.voltage * heater.current;   // (A) tính công suất

if (heater.current > 30.0f) {                          // (B) kiểm tra quá tải
    heater.is_overloaded = 1;
    heater.is_active = 0;                              // ngắt điện
}
```

### Trace luồng dữ liệu (bản gốc)

| Bước | Lệnh | voltage | current | power_watt | is_active | is_overloaded |
|------|------|---------|---------|------------|-----------|---------------|
| 1 | Khởi tạo | 220.0 | 35.0 | (rác) | 1 | 0 |
| 2 | `heater.power_watt = heater.voltage * heater.current` | 220.0 | 35.0 | **7700.0** | 1 | 0 |
| 3 | `if (heater.current > 30.0f)` → đúng | 220.0 | 35.0 | 7700.0 | 1 | 0 |
| 4 | `heater.is_overloaded = 1; heater.is_active = 0;` | 220.0 | 35.0 | **7700.0** | **0** | 1 |
| 5 | In kết quả | | | **7700.00 W** | TAT | CÓ |

### Nguyên nhân kỹ thuật
- Chương trình chạy tuần tự từ trên xuống. Công suất `heater.power_watt` được tính **trước** khi kiểm tra quá tải, nên nó dùng trạng thái `is_active = 1` cũ.
- Sau đó `heater.is_active` bị đổi về 0 nhưng **không có lệnh nào cập nhật lại** `heater.power_watt`. Các member của struct độc lập nhau, đổi `heater.is_active` không tự làm đổi `heater.power_watt`.
- Hệ quả: trạng thái TẮT nhưng công suất vẫn là 7700 W, vi phạm quy tắc "TẮT ⇒ power_watt = 0.0 W".
- Ngoài ra, mã gốc tính công suất mà không xét `is_active`, nên thiết bị đang TẮT nhưng có dòng rò/nhiễu cũng bị tính ra công suất khác 0.

### Cách sửa
1. Kiểm tra quá tải **trước** (chỉ khi thiết bị đang BẬT), cập nhật `is_active`, `is_overloaded`.
2. Sau khi trạng thái đã chốt, mới cập nhật `heater.power_watt`:
   - `is_active == 1` → `P = V × I`
   - `is_active == 0` → `P = 0.0`

## 2. Bảng Test Cases đối chứng

| Trường hợp kiểm thử | Dữ liệu đầu vào | Kết quả sai thực tế (mã gốc) | Kết quả đúng mong đợi (mã sửa) |
|---|---|---|---|
| TC1: Quá tải, tự ngắt aptomat | `voltage = 220.0`, `current = 35.0`, `is_active = 1` | Trạng thái **TẮT**, công suất **7700.00 W**, cảnh báo CÓ | Trạng thái **TẮT**, công suất **0.00 W**, cảnh báo CÓ (ĐÃ NGẮT ĐIỆN) |
| TC2: Thiết bị TẮT nhưng có dòng rò | `voltage = 220.0`, `current = 2.5` (dòng rò), `is_active = 0` | Trạng thái **TẮT**, công suất **550.00 W** (= 220 × 2.5) | Trạng thái **TẮT**, công suất **0.00 W**, cảnh báo KHÔNG |

Kiểm tra thêm (hoạt động bình thường): `current = 25.0`, `is_active = 1` → BẬT, 5500.00 W, cảnh báo KHÔNG. Biên `current = 30.0` → BẬT, 6600.00 W (không quá tải vì điều kiện là `> 30.0`).

## 3. Kết quả chạy sau khi sửa
```
=== HETHONG GIAM SAT THIET BI SMART HOME ===
Ma thiet bi: 101
Trang thai hoat dong: TAT
Cong suat tieu thu: 0.00 W
Canh bao qua tai (>30A): CO (DA NGAT DIEN)
```

## 4. Biên dịch & chạy
```bash
gcc -Wall -Wextra -std=c11 main.c -o main
./main
```

Chỉ dùng `struct`, toán tử `.`, `if/else`, `printf` (đúng phạm vi bài học).
