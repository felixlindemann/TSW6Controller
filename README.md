# OLE-TSW Controller

An open-source controller project for **Train Sim World (TSW)** built around the **ESP32 (Lolin32 Lite)** platform.  
It allows reading live data from TSW and controlling multiple train functions through custom hardware and a browser-based configuration tool.

I found a "documentation" to the api here: https://forums.dovetailgames.com/threads/train-sim-world-api-support.94488/

---

## ✨ Features
- ESP32-based controller with HID joystick and web interface  
- SD-card configuration system for profiles and presets  
- Live data reading from TSW via HTTP-GET  
- Supports AFB, throttle, train brake, loco brake, lights, wipers and more  
- Easy mapping between TSW values and real hardware controls  
- Works offline, LAN-compatible, and loco-independent  

---

## 🧩 Hardware
- Lolin32 Lite (ESP32) 
- Optional OLED display (SH1106/SSD1306)   

---

## ⚙️ Setup
1. Flash using **PlatformIO** or **Arduino IDE**  
2. Open Wi-Fi Access Point: `TSW-Controller`  
3. Connect to the Web Interface  
4. use Windows/Enable-TSW6-API-Access.cmd to open the Firewall for the controller
   and setup a proxy to forward api-request to localhost 
   (as it seems, the api cannot be made public on the client)
5. Follow the described steps to get the api-key from your TSW-6 installation


---

## 📁 Repository Structure
/src → main firmware
/data → web interface files
/assets → logos, screenshots, etc.

---

## 📄 License
Released under the **MIT License**.  
You can freely use, modify, and share this project — attribution is appreciated but not required.  

---

## 💡 Credits
Developed by **Felix Lindemann**  
Special thanks to the Arduino & Train Sim World modding communities for inspiration and support
Many thanks to COBRA-ONE - his approach (https://forums.dovetailgames.com/threads/ts-world-raildriver-and-joystick-interface.61440/) 
for modelling a controller with a joystick gave the idea to this project. 
