#include <FastLED.h>

// Định nghĩa số lượng đèn LED trên dải của bạn
#define NUM_LEDS 10 

// Định nghĩa chân DATA mà dải LED của bạn được kết nối
#define DATA_PIN 9  

// Khai báo mảng CRGB để lưu trữ màu sắc của từng LED
CRGB leds[NUM_LEDS]; 

// Biến toàn cục để theo dõi sắc độ (hue) hiện tại cho hiệu ứng chạy màu
uint8_t gHue = 0; 

// Biến toàn cục để xác định hướng chuyển màu: 1 là tiến, -1 là lùi
int8_t gDirection = 1; 

// =====================================================================
// BẢNG MÀU TÙY CHỈNH CỦA BẠN (16 MÃ MÀU)
// =====================================================================
CRGB my_custom_16_color_palette_data[16]; // Mảng CRGB để lưu trữ 16 màu

// Biến trạng thái để kiểm soát khi nào cần đọc input Serial
bool paletteInputNeeded = true; 
String serialInputBuffer = ""; // Buffer để xây dựng chuỗi từ Serial

// Hàm để phân tích chuỗi màu đã nhận được
void parsePaletteString(String inputString) {
  inputString.trim(); // Xóa khoảng trắng ở đầu/cuối

  Serial.print("Chuoi dau vao da doc: '");
  Serial.print(inputString);
  Serial.println("'");
  Serial.print("Do dai chuoi dau vao: ");
  Serial.println(inputString.length()); 

  char charArray[inputString.length() + 1]; 
  inputString.toCharArray(charArray, sizeof(charArray));
  charArray[inputString.length()] = '\0'; // Đảm bảo chuỗi kết thúc bằng null

  Serial.print("Kich thuoc charArray (bo dem): ");
  Serial.println(sizeof(charArray));

  char *token;
  int colorCount = 0;

  token = strtok(charArray, ", "); 

  while (token != NULL && colorCount < 16) {
    Serial.print("Phan tich token [");
    Serial.print(colorCount);
    Serial.print("]: '");
    Serial.print(token);
    Serial.println("'");

    long hexValue = strtol(token, NULL, 16);

    Serial.print("  Gia tri hex: 0x");
    Serial.println(hexValue, HEX);

    my_custom_16_color_palette_data[colorCount] = hexValue; 
    
    colorCount++;
    token = strtok(NULL, ", "); 
  }

  Serial.print("--- Ket Thuc Phan Tich ---");
  Serial.print("Tong so mau nhan duoc: ");
  Serial.println(colorCount);

  if (colorCount == 16) {
    Serial.println("Da nhap thanh cong 16 ma mau!");
    paletteInputNeeded = false; // Đặt cờ thành false khi bảng màu đã được nhập
  } else {
    Serial.print("Loi: Chi nhan duoc ");
    Serial.print(colorCount);
    Serial.println(" ma mau. Vui long nhap lai du 16 ma mau.");
    for (int i = colorCount; i < 16; i++) {
        my_custom_16_color_palette_data[i] = CRGB::Black;
    }
    // Giữ paletteInputNeeded là true để yêu cầu nhập lại
  }
  Serial.println("-------------------------");
}

void setup() {
  Serial.begin(115200); 
  while (!Serial); 

FastLED.setCorrection(TypicalLEDStrip);        // Hiển thị màu trung thực hơn
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128); 

  // Hiển thị hướng dẫn nhập bảng màu khi khởi động
  Serial.println("\n--- Nhap Bang Mau ---");
  Serial.println("Vui long nhap 16 ma mau hex (vi du: 0xFF0000, 0x00FF00, ...):");
  Serial.println("Cac ma mau phai duoc phan cach bang dau phay hoac dau cach.");
  Serial.println("Ban co the su dung tien to '0x' hoac khong. Nhan Enter sau khi nhap xong.");
  Serial.println("Dang cho du lieu tu Serial...");
}

void loop() {
  // Nếu cần nhập bảng màu, hãy xử lý input Serial
  if (paletteInputNeeded) {
    while (Serial.available()) {
      char incomingChar = Serial.read();
      if (incomingChar == '\n') { // Nếu nhận được ký tự xuống dòng
        parsePaletteString(serialInputBuffer); // Phân tích chuỗi đã xây dựng
        serialInputBuffer = ""; // Xóa buffer cho lần nhập tiếp theo
        // Nếu parsePaletteString thành công, paletteInputNeeded sẽ được đặt thành false
        // Nếu không, nó vẫn là true để yêu cầu nhập lại
      } else if (incomingChar != '\r') { // Bỏ qua ký tự Carriage Return ('\r')
        serialInputBuffer += incomingChar; // Thêm ký tự vào buffer
      }
    }
    // Nếu chưa nhận đủ input, không chạy hiệu ứng LED
    // Bạn có thể thêm một hiệu ứng chờ ở đây (ví dụ: nhấp nháy đèn báo)
    FastLED.clear();
    FastLED.show();
    return; // Thoát khỏi loop để chờ input
  }

  // Chạy hiệu ứng LED chỉ khi bảng màu đã được nhập thành công
  CRGBPalette16 currentPalette = my_custom_16_color_palette_data; 

  CRGB currentColor = ColorFromPalette(currentPalette, gHue, 128, LINEARBLEND); 

  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = currentColor;
  }
  
  FastLED.show();
  
  gHue += gDirection;

  if (gHue == 255 || gHue == 0) {
    gDirection *= -1; 
     Serial.println("--- change direction ---");
  }
  
  delay(50); 
}
