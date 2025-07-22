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

// Hàm để hiển thị hướng dẫn nhập bảng màu
void printPaletteInputInstructions()
{
    Serial.println("\n--- Nhap Bang Mau ---");
    Serial.println("Vui long nhap 16 ma mau hex (vi du: 0xFF0000, 0x00FF00, ...):");
    Serial.println("Cac ma mau phai duoc phan cach bang dau phay hoac dau cach.");
    Serial.println("Ban co the su dung tien to '0x' hoac khong. Nhan Enter sau khi nhap xong.");
    Serial.println("De nhap lai bang mau bat ky luc nao, nhap '111' va nhan Enter.");
    Serial.println("Dang cho du lieu tu Serial...");

    // Tắt tất cả các LED và bật LED đầu tiên nhấp nháy để báo hiệu đang chờ input
    FastLED.clear();
    leds[0] = CRGB::White; // Bật LED 0 màu trắng
    FastLED.show();
}

// Hàm để phân tích chuỗi màu đã nhận được
void parsePaletteString(String inputString)
{
    inputString.trim(); // Xoa khoang trang o dau/cuoi

    Serial.print("Chuoi dau vao da doc: '");
    Serial.print(inputString);
    Serial.println("'");
    Serial.print("Do dai chuoi dau vao: ");
    Serial.println(inputString.length());

    char charArray[200]; // Đảm bảo kích thước đủ lớn
    inputString.toCharArray(charArray, sizeof(charArray));
    charArray[inputString.length()] = '\0'; // Đảm bảo chuỗi kết thúc bằng null

    Serial.print("Kich thuoc charArray (bo dem): ");
    Serial.println(sizeof(charArray));

    char *token;
    int colorCount = 0;

    token = strtok(charArray, ", ");

    while (token != NULL && colorCount < 16)
    {
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

    if (colorCount == 16)
    {
        Serial.println("Da nhap thanh cong 16 ma mau!");
        paletteInputNeeded = false; // Đặt cờ thành false khi bảng màu đã được nhập
    }
    else
    {
        Serial.print("Loi: Chi nhan duoc ");
        Serial.print(colorCount);
        Serial.println(" ma mau. Vui long nhap lai du 16 ma mau.");
        // Đảm bảo các màu còn lại là đen nếu không đủ
        for (int i = colorCount; i < 16; i++)
        {
            my_custom_16_color_palette_data[i] = CRGB::Black;
        }
        // paletteInputNeeded vẫn là true, nên chương trình sẽ tiếp tục chờ input.
        // In lại hướng dẫn để người dùng biết cần nhập lại.
        printPaletteInputInstructions();
    }
    Serial.println("-------------------------");
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    FastLED.setCorrection(TypicalLEDStrip); // Hiển thị màu trung thực hơn
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(80);

    // Hiển thị hướng dẫn nhập bảng màu khi khởi động
    printPaletteInputInstructions();
}

void loop()
{
    // Luôn kiểm tra dữ liệu Serial đến
    while (Serial.available())
    {
        char incomingChar = Serial.read();
        if (incomingChar == '\n')
        {                             // Nếu nhận được ký tự xuống dòng
            serialInputBuffer.trim(); // Xóa khoảng trắng ở đầu/cuối của buffer

            // Kiểm tra nếu lệnh là "111" để reset
            if (serialInputBuffer.equals("111"))
            {
                Serial.println("\nLenh '111' da nhan! Quay lai che do nhap bang mau.");
                paletteInputNeeded = true; // Đặt lại cờ để yêu cầu nhập bảng màu
                serialInputBuffer = "";    // Xóa buffer
                FastLED.clear();           // Tắt đèn LED
                FastLED.show();
                printPaletteInputInstructions(); // In lại hướng dẫn
            }
            else if (paletteInputNeeded)
            {                                          // Nếu đang ở chế độ chờ nhập bảng màu
                parsePaletteString(serialInputBuffer); // Phân tích chuỗi đã xây dựng
                serialInputBuffer = "";                // Xóa buffer cho lần nhập tiếp theo
            }
            else
            { // Nếu đang chạy hiệu ứng và nhận được input không phải lệnh reset
                Serial.print("Lenh khong hop le hoac du lieu thua: '");
                Serial.print(serialInputBuffer);
                Serial.println("'");
                serialInputBuffer = ""; // Xóa buffer
            }
        }
        else if (incomingChar != '\r')
        {                                      // Bỏ qua ký tự Carriage Return ('\r')
            serialInputBuffer += incomingChar; // Thêm ký tự vào buffer
        }
    }

    // Chạy hiệu ứng LED chỉ khi bảng màu đã được nhập thành công
    if (!paletteInputNeeded)
    {
        CRGBPalette16 currentPalette = my_custom_16_color_palette_data;
        CRGB currentColor = ColorFromPalette(currentPalette, gHue, 128, LINEARBLEND);

        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = currentColor;
        }

        FastLED.show();

        gHue += gDirection;

        if (gHue == 255 || gHue == 0)
        {
            gDirection *= -1;
            if (gHue == 255)
            {
                Serial.println("--- is 255 and back ---");
                leds[0] == CRGB::Red;
                FastLED.show();
                FastLed.delay(3000); // Dừng lại một chút khi đạt đến 255
            }
            else
            {
                Serial.println("--- is 0 and forward ---");
                leds[0] = CRGB::Green; // Đặt LED 0 thành màu xanh lá cây khi quay lại
                FastLED.show();
                FastLed.delay(3000); // Dừng lại một chút khi đạt đến 255}
            }

            delay(25);
        }
        else
        {
            // Nếu chưa nhận đủ input, không chạy hiệu ứng LED
            // Hiển thị nhấp nháy LED 0 để báo hiệu đang chờ input
            static unsigned long lastBlinkTime = 0;
            if (millis() - lastBlinkTime > 500)
            { // Nhấp nháy mỗi 500ms
                lastBlinkTime = millis();
                if (leds[0] == CRGB::White)
                {
                    leds[0] = CRGB::Black;
                }
                else
                {
                    leds[0] = CRGB::White;
                }
                FastLED.show();
            }
        }
    }
