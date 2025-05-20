# TECHIN515 Magic Wand Project

This repository contains all necessary files for our gesture-controlled magic wand, built using ESP32 and Edge Impulse.

## Project Structure

```plaintext
TECHIN515-magic-wand/
├── src/
│   ├── sketches/
│   │   ├── wand.ino                  # Final inference code
│   │   └── gesture_capture.ino       # Data collection sketch
│   ├── python-scripts/
│   │   └── process_gesture_data.py   # Script to capture labeled data
│   └── dataset/                      # Collected gesture data in CSV format
├── docs/
│   └── report.pdf                    # Final report with all required sections
├── media/
│   └── demo.mp4                      # Demo video of working wand
├── enclosure/
│   ├── final-enclosure-images/      # Photos and STL files of the 3D-printed enclosure
│   └── notes.md                      # Design choices, materials, and hardware notes
└── README.md                         # This file
```

## Setup Instructions

1. Clone this repository:
```bash
git clone https://github.com/yourusername/TECHIN515-magic-wand.git
cd TECHIN515-magic-wand
```

2. Set up the Python virtual environment:
```bash
cd src/python-scripts
python3 -m venv .venv
source .venv/bin/activate      # On Windows use `.venv\Scripts\activate`
pip install -r requirements.txt
```

3. Run the data capture script:
```bash
cd src/python-scripts
python process_gesture_data.py --gesture "Z" --person "your_name"
```

Follow on-screen instructions and move the wand accordingly.

---

##  Model Training (Edge Impulse)

1. Go to [Edge Impulse Studio](https://studio.edgeimpulse.com)
2. Create a new project and upload your dataset
3. Design an Impulse:
   - Input: 100Hz, 1s window
   - DSP Block: Flatten
   - Learning Block: Neural Network (Classifier)
4. Train and evaluate your model
5. Export as Arduino Library (Quantized – Int8)

---

## Hardware Connections

### **MPU6050**
- VCC → 3.3V
- GND → GND
- SDA → D4 (GPIO6)
- SCL → D5 (GPIO7)

### **Button**
- One leg → D0
- Other leg → GND

### **RGB LED (Common Cathode)**
- R (Red) → D1 through resistor
- G (Green) → D2 through resistor
- B (Blue) → D3 through resistor
- Common Cathode → GND

---

## 🎥 Demo

Watch our demo in `media/demo.mp4` to see the wand in action.


