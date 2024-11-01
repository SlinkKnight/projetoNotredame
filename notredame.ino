#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <esp_now.h>
#include <WiFi.h>

Adafruit_MPU6050 mpu;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Define addresses
#define MPU6050_ADDRESS 0x68  // or 0x69
#define VL53L0X_ADDRESS 0x29   // or another address if necessary

float totalRotationZ = 0;

const float deadzone = 0.3;
const float deadzoneSelect = 10.0;
const float deadzoneOption = 50.0;

int lastMessage;

// Structure for data to be sent via ESP-NOW
typedef struct struct_message {
    char message[32]; // Use char array instead of String for compatibility
} struct_message;

struct_message data;

// MAC address of the receiver
uint8_t receiverAddress[] = {0xD0, 0xEF, 0x76, 0x33, 0x6E, 0x40}; // Use the specific MAC address

void setup(void) {
    Serial.begin(115200);

    // Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Add peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;  // Use default channel
    peerInfo.encrypt = false; // No encryption

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    if (!lox.begin(VL53L0X_ADDRESS)) {
        Serial.println("Failed to find VL53L0X chip");
        while (1);
    }

    if (!mpu.begin(MPU6050_ADDRESS)) {
        Serial.println("Failed to find MPU6050 chip");
        while (1);
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    delay(100);
}

float measureDistance() {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false); // Execute measurement

    if (measure.RangeStatus != 4) {  // Check for phase failures
        return measure.RangeMilliMeter; // Return distance
    }
    return -1; // Return a default value in case of error
}

float getRotationZ() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float gyroZ = g.gyro.z * (180.0 / PI) * 0.1; // Convert to degrees

    // Accumulate rotation if outside deadzone
    if (fabs(gyroZ) > deadzone) {
        totalRotationZ += gyroZ;
    }

    return totalRotationZ; // Return total accumulated rotation
}

void sendMessage(const char* message) {
    strcpy(data.message, message);
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &data, sizeof(data));
    if (result == ESP_OK) {
        Serial.println("Message sent successfully");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(result);
    }
}

void loop() {

    float eixoZ = getRotationZ(); // Get accumulated rotation
    float distance = measureDistance(); // Get distance

    if (eixoZ > deadzoneSelect) {
        if (distance <= deadzoneOption && lastMessage != 1) { // Validate distance
            Serial.println("Option one activated");
            lastMessage = 1;
            sendMessage("OP1-ON");
        } else if (lastMessage != 2){
            Serial.println("Option one deactivated");
            lastMessage = 2;
            sendMessage("OP1-OFF");
        } else {
            Serial.println("Nothing was sent");
        }
    } else if (eixoZ < -deadzoneSelect) {
        if (distance <= deadzoneOption && lastMessage != 3) { // Validate distance
            Serial.println("Option two activated");
            lastMessage = 3;
            sendMessage("OP2-ON");
        } else if (lastMessage != 4){
            Serial.println("Option two deactivated");
            lastMessage = 4;
            sendMessage("OP2-OFF");
        } else {
            Serial.println("Nothing was sent");
        }
    }

    delay(100);
}
