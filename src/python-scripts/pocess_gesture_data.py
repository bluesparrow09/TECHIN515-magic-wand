import serial
import argparse
import os
import time
import csv

def collect_data(port, baudrate, gesture, person, samples=100):
    # 创建保存目录
    save_dir = f"data/{gesture.upper()}"
    os.makedirs(save_dir, exist_ok=True)

    # 文件名以时间戳区分
    timestamp = int(time.time())
    filename = f"{gesture.upper()}_{person}_{timestamp}.csv"
    filepath = os.path.join(save_dir, filename)

    print(f"Connecting to {port} at {baudrate} baud...")
    ser = serial.Serial(port, baudrate, timeout=2)
    time.sleep(2)  # 等待ESP32启动

    print(f"Capturing gesture '{gesture.upper()}' from '{person}'...")
    print("Sending trigger command...")
    ser.write(b'o')  # 向ESP32发送'o'字符开始采集
    ser.flush()

    lines = []
    while len(lines) < samples:
        line = ser.readline().decode('utf-8').strip()
        if line:
            parts = line.split(',')
            if len(parts) == 4:  # timestamp,x,y,z
                lines.append(parts)
                print(f"[{len(lines)}/{samples}] {parts}")

    ser.close()

    print(f"Saving data to {filepath}...")
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "x", "y", "z"])
        writer.writerows(lines)

    print("Done!")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', required=True, help='Serial port (e.g., COM3 or /dev/ttyUSB0)')
    parser.add_argument('--gesture', required=True, help='Gesture name (e.g., O, V, Z)')
    parser.add_argument('--person', required=True, help='Your name')
    parser.add_argument('--baudrate', type=int, default=115200)
    parser.add_argument('--samples', type=int, default=100)
    args = parser.parse_args()

    collect_data(args.port, args.baudrate, args.gesture, args.person, args.samples)
