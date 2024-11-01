#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
    char message[32]; // Use char array for incoming data
} struct_message;

struct_message incomingData;

// Definição dos pinos dos LEDs
const int ledOptionOne = 13; // Pino do LED para opção um
const int ledOptionTwo = 14; // Pino do LED para opção dois

void onDataReceive(const esp_now_recv_info* info, const uint8_t* incomingDataPtr, int len) {
    if (len == sizeof(incomingData)) {
        memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));
        Serial.print("Received: ");
        Serial.println(incomingData.message);

        // Lógica para controlar os LEDs
        if (strcmp(incomingData.message, "OP1-ON") == 0) {
            Serial.println("Action for Option One Activated");
            digitalWrite(ledOptionOne, LOW); // Liga o LED da opção um
        } else if (strcmp(incomingData.message, "OP1-OFF") == 0) {
            Serial.println("Action for Option One Deactivated");
            digitalWrite(ledOptionOne, HIGH); // Desliga o LED da opção um
        } else if (strcmp(incomingData.message, "OP2-ON") == 0) {
            Serial.println("Action for Option Two Activated");
            digitalWrite(ledOptionTwo, LOW); // Liga o LED da opção dois
        } else if (strcmp(incomingData.message, "OP2-OFF") == 0) {
            Serial.println("Action for Option Two Deactivated");
            digitalWrite(ledOptionTwo, HIGH); // Desliga o LED da opção dois
        } else {
            Serial.println("Unknown message received");
        }
    } else {
        Serial.println("Received data size mismatch");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Inicializa os pinos dos LEDs
    pinMode(ledOptionOne, OUTPUT);
    pinMode(ledOptionTwo, OUTPUT);


    digitalWrite(ledOptionOne, HIGH);
    digitalWrite(ledOptionTwo, HIGH);

    // Inicializa o ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Registra o callback de recebimento
    if (esp_now_register_recv_cb(onDataReceive) != ESP_OK) {
        Serial.println("Error registering receive callback");
        return;
    }

    Serial.println("Receiver setup complete. Waiting for messages...");
}

void loop() {
    // O loop pode permanecer vazio se não houver outra lógica a ser executada
}
