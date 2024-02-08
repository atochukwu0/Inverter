# ⚡Inverter⚡
80kW (2x40kW), 600V, 3 phase inverter with FOC 

This is my Electronics Engineering bachelor thesis, a dual 40kW, 600V, 3 phase inverter. The goal is to design and implement a dual bidirectional inverter. The inverter will use field-oriented control, as well as having autotuning and testing features. The target power densities for the inverter are 50 kW/liter and 30 kW/kg, and it will be designed with a variety of control and power requirements in mind.

## 📁 Repository Contents
This repository contains the following files and directories:

* [🕹️ Control/](https://github.com/dweggg/Inverter/blob/main/Control/): This directory contains the MATLAB / Simulink & PLECS files used to undestand and develop the FOC.
* [🛠️ HW/](https://github.com/dweggg/Inverter/blob/main/HW/): This directory contains all the design documents, such as schematics, PCB layouts, and BOM.
* [💾 SW/](https://github.com/dweggg/Inverter/blob/main/SW/): This directory contains the firmware code for the inverter, which is written in C and uses a STM32 microcontroller.

- [🧠 Wiki](https://github.com/dweggg/Inverter/wiki): Here we explain everything related to the development of the inverter, from the mathematical basis, hardware design, software implementation and car integration.

## 🛠️ Built With
### 🕹️ Control study
* [MATLAB 2022b & Simulink](https://www.mathworks.com/products/matlab.html) - Design of the vector control algorithm and general understanding of FOC
* [PLECS](https://www.plexim.com/products/plecs) - Validation of the control algorithm with thermals and switching

### 🛠️ HW
* [Altium Designer](https://www.altium.com/es/altium-designer) - Schematics and PCB
* [SOLIDWORKS 2022](https://help.solidworks.com/2022/spanish/WhatsNew/c_wn_install.htm?id=7ddf806d182541ffa399d655bb1f99a2#Pg0) - Mechanical assembly

### 💾 SW
* [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) - IDE for STM32 MCUs


## 👤 Author

* **David Redondo** - *Initial work* - [dweggg](https://github.com/dweggg)

See also the list of [contributors](https://github.com/dweggg/Inverter/contributors) who participated in this project.


## 👏 Acknowledgments

* e-Tech Racing
* UPC EEBE
* CITCEA
