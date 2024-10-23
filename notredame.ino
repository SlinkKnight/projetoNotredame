#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float totalRotationZ = 0;

// Define a deadzone em graus
const float deadzone = 0.3; // ajuste este valor conforme necessário
const int deadzoneSelect = 10

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause até que o console serial esteja aberto

  Serial.println("Adafruit MPU6050 test!");

  // Tenta inicializar o MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.println("");
  delay(100);
}

// Função para pegar o valor do eixo Z e acumular a rotação
float pegarEixoZ() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float gyroZ = g.gyro.z * (180.0 / PI) * 0.1; // Converte para graus

  // Acumula a rotação se estiver fora da deadzone
  if (fabs(gyroZ) > deadzone) {
    totalRotationZ += gyroZ;
  }

  return totalRotationZ; // Retorna o total acumulado
}

void loop() {
  float eixoZ = pegarEixoZ(); // Chama a função e obtém o total acumulado

  if (eixoZ > deadzoneSelect) {
    Serial.print("op1"); // Retorna op1 se eixoZ for maior que 20
  } else if (eixoZ < -deadzoneSelect) {
    Serial.println("op2"); // Retorna op2 se eixoZ for menor que -20
  }

  delay(100); // Atraso em milissegundos
}
