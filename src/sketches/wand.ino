#include <chunzhi-project-1_inferencing.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// ---------- 引脚定义 ----------
#define BUTTON_PIN 2     // D0 → GPIO2
#define LED_RED    3     // D1 → GPIO3
#define LED_GREEN  4     // D2 → GPIO4
#define LED_BLUE   5     // D3 → GPIO5

// ---------- 常量 ----------
#define SAMPLE_RATE_MS 10
#define CAPTURE_DURATION_MS 1000
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE

// ---------- 全局变量 ----------
Adafruit_MPU6050 mpu;
bool capturing = false;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;
float features[FEATURE_SIZE];

// ---------- 函数声明 ----------
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr);
void run_inference();
void capture_accelerometer_data();
void print_inference_result(ei_impulse_result_t result);
void set_led_color(bool r, bool g, bool b);

// ---------- 初始化 ----------
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 按钮使用内建上拉
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  set_led_color(false, false, false);

  Serial.println("Initializing MPU6050...");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized");
  Serial.println("Press the button to start gesture capture...");
}

// ---------- 主循环 ----------
void loop() {
  if (!capturing && digitalRead(BUTTON_PIN) == LOW) {
    // 检测到按键按下
    delay(10); // 去抖
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Starting gesture capture...");
      sample_count = 0;
      capturing = true;
      capture_start_time = millis();
      last_sample_time = millis();
      set_led_color(true, true, true); // 白灯表示采样中
    }
  }

  if (capturing) {
    capture_accelerometer_data();
  }
}

// ---------- 采样函数 ----------
void capture_accelerometer_data() {
  if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
    last_sample_time = millis();
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    if (sample_count < FEATURE_SIZE / 3) {
      int idx = sample_count * 3;
      features[idx] = a.acceleration.x;
      features[idx + 1] = a.acceleration.y;
      features[idx + 2] = a.acceleration.z;
      sample_count++;
    }

    if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
      capturing = false;
      Serial.println("Capture complete");
      run_inference();
    }
  }
}

// ---------- 推理函数 ----------
void run_inference() {
  if (sample_count * 3 < FEATURE_SIZE) {
    Serial.println("ERROR: Not enough data");
    return;
  }

  ei_impulse_result_t result = { 0 };
  signal_t features_signal;
  features_signal.total_length = FEATURE_SIZE;
  features_signal.get_data = &raw_feature_get_data;

  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
  if (res != EI_IMPULSE_OK) {
    Serial.print("ERR: Inference failed (");
    Serial.print(res);
    Serial.println(")");
    return;
  }

  print_inference_result(result);
}

// ---------- 打印结果并点灯 ----------
void print_inference_result(ei_impulse_result_t result) {
  float max_value = 0;
  int max_index = -1;

  for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    if (result.classification[i].value > max_value) {
      max_value = result.classification[i].value;
      max_index = i;
    }
  }

  if (max_index != -1) {
    const char* label = ei_classifier_inferencing_categories[max_index];
    Serial.print("Prediction: ");
    Serial.print(label);
    Serial.print(" (");
    Serial.print(max_value * 100);
    Serial.println("%)");

    // 控制 LED 颜色
    if (strcmp(label, "Z") == 0) {
      set_led_color(true, false, false); // 红
    } else if (strcmp(label, "O") == 0) {
      set_led_color(false, true, false); // 绿
    } else if (strcmp(label, "V") == 0) {
      set_led_color(false, false, true); // 蓝
    } else {
      set_led_color(false, false, false); // 没识别到
    }
  }
}

// ---------- 传感器数据接口 ----------
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

// ---------- LED 控制函数 ----------
void set_led_color(bool r, bool g, bool b) {
  digitalWrite(LED_RED, r ? HIGH : LOW);
  digitalWrite(LED_GREEN, g ? HIGH : LOW);
  digitalWrite(LED_BLUE, b ? HIGH : LOW);
}
