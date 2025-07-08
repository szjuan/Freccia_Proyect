import serial
import time
import random

# Cambia este COM por el que espera tu GUI (normalmente COM4 o COM5 en Windows)
puerto = '/dev/pts/3'
baudrate = 115200

# Abrir puerto
ser = serial.Serial(puerto, baudrate)
time.sleep(2)  # Esperar a que se estabilice la conexión

while True:
    # Generar datos simulados
    latitude = round(random.uniform(4.60, 4.70), 6)
    longitude = round(random.uniform(-74.10, -74.00), 6)
    fecha = "20245d0527"
    hora = "12.34"
    calidad = 1
    satelites = random.randint(6, 12)
    hdop = round(random.uniform(0.6, 1.2), 2)
    roll = round(random.uniform(-30, 30), 2)
    pitch = round(random.uniform(-30, 30), 2)
    yaw = round(random.uniform(-180, 180), 2)

    # Armar línea (mínimo 15 campos)
    linea = f"{latitude},{longitude},{fecha},{hora},{calidad},{satelites},{hdop},{roll},{pitch},{yaw},0,0,0,0,0\r\n"

    # Enviar al puerto serial
    ser.write(linea.encode())
    print("Enviado:", linea.strip())
    time.sleep(0.1)  # Espera 100 ms entre muestras
