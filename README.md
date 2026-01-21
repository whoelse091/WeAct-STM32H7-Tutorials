

# WeAct-STM32H7-Tutorials

This project provides clear and practical examples of how to work with the WeAct STM32H7 development board. It explains how to configure the MCU, display, camera, and other modules step by step.
It is intended for beginners and hobbyists who want an easy and straightforward guide to using this board and learning embedded development on STM32H7.


## Getting Started

After you receive your WeAct STM32H7 board, the first thing you will probably want
to do is get the display working.  
To begin, you need to install the official STM32 development environment —
**STM32CubeIDE**, available from the STMicroelectronics website.  
This tool will allow you to configure the MCU, generate initialization code,
compile the project, and flash the firmware to your board.

<img width="2500" height="1400" alt="image" src="https://github.com/user-attachments/assets/c7f6ddf7-1dba-419f-910f-e4f9d60f75f3" />


### Creating a New Project

After installing STM32CubeIDE, the next step is to create a new project.
Open the IDE and navigate to:

**File → New → STM32 Project**

This will open the board and MCU selection window, where you can choose
the WeAct STM32H7 device and start configuring your project.

<img width="1213" height="849" alt="image" src="https://github.com/user-attachments/assets/65559cc1-03f7-41f8-9d58-31e83c908b6a" />



### Selecting the Microcontroller

After creating a new project, STM32CubeIDE will ask you to select the MCU.
In the field **“Commercial Part Number”** enter the exact name of your
microcontroller as written on the chip.

<img width="2500" height="1400" alt="image" src="https://github.com/user-attachments/assets/5e7ef81c-5c1f-48b5-aca1-799cd711d6c3" />

For this board, the correct part number is:

**STM32H7B0VBT6**

After typing the part number, select it from the list and click **Next** to continue.


### Project Name and Language Selection

The next window allows you to configure your project settings.
Here you should choose a **project name** and select the **programming language**.

For this repository, we use the **C language**, so make sure to set the
language option to **C** before continuing.

<img width="714" height="764" alt="image" src="https://github.com/user-attachments/assets/491f43e5-8c5b-4f79-bc6f-29853fc32e3a" />

Once everything is set, click **Finish** to generate the project.


### Pin Configuration and Interfaces

Now we move on to the most interesting part.
After the project is created, STM32CubeIDE will open the pinout configuration window.

Here you can see all available pins of the microcontroller.  
This is where we will configure every interface required for the project —  
SPI for the display, UART for debugging, GPIO control pins, and other peripherals.

<img width="2500" height="1400" alt="image" src="https://github.com/user-attachments/assets/95f2a1ec-a7e6-4f69-bd72-4fe2c89c304a" />


In this view you can enable modules, assign pins, and set the functions needed
for proper operation of the display and other connected hardware.


###  Display 

Before initializing the display, we must configure the pins it uses.
In the pinout view of STM32CubeIDE, locate the pins connected to the LCD module.
In our WeAct STM32H7 board design, the display is wired to the **SPI4** interface.

The correct pin mapping is:

- **PE12 → SPI4_SCK**  
- **PE14 → SPI4_MOSI**  
- **PE5  → SPI4_MISO** (not used in this project, but still part of SPI4)

Click on each of these pins in the pinout diagram and assign the corresponding
SPI4 functions exactly as listed above.

<img width="1215" height="731" alt="image" src="https://github.com/user-attachments/assets/ab8133cc-9e6b-49c0-8f36-9fa05b53c124" />

### Enabling the SPI4 Peripheral

After assigning the pins manually, we need to activate the SPI4 bus itself.
On the left sidebar, open the category **Connectivity**, then locate and select **SPI4**.

This enables the peripheral and allows you to configure parameters such as mode,
clock polarity, speed, DMA, and more.
<img width="2559" height="1528" alt="Без имени-1" src="https://github.com/user-attachments/assets/d2e6cee6-f65e-4888-98f1-4bc6bda33110" />


After enabling the SPI4 peripheral, a configuration panel will appear.
Here you must set the operating mode for the SPI bus.

In the **Mode** field, select:

**Full Duplex Master**

This mode allows the microcontroller to control the SPI bus while sending data
to the display.


<img width="1147" height="409" alt="image" src="https://github.com/user-attachments/assets/b764d200-cb4a-457a-8e0a-4fdfa5140c4f" />



Once the correct mode is selected, the SPI pins you assigned earlier
(SCK, MOSI, MISO) should highlight in green in the pinout view.
This indicates that the configuration is valid and the pins are now properly linked
to the SPI4 peripheral.

After selecting the operating mode, an additional SPI settings panel will appear
at the bottom of the window.  
This panel allows you to configure all necessary parameters for the SPI bus.

<img width="740" height="1217" alt="image" src="https://github.com/user-attachments/assets/c9741e6b-2508-483a-be5a-8650146b42bc" />

Set the values according to the reference image shown adove.  
Make sure your configuration matches the displayed settings before continuing.

### Display Control GPIO Pins (Required)

In addition to SPI configuration, several GPIO pins must be configured *manually* in STM32CubeIDE.
These pins are not part of the SPI peripheral and are mandatory for correct display operation.

GPIO Output Pins

The following pins MUST be configured as GPIO_Output in the .ioc file:

**PE10** — LCD_LED
Function: Backlight control
Enables or disables the display backlight.
Can optionally be driven using PWM for brightness control.

**PE11** — LCD_CS
Function: Chip Select (active low)
Selects the display controller during SPI communication.
Controlled by software as a GPIO output.

**PE13** — LCD_DC / LCD_RS
Function: Data / Command selection
Determines whether SPI data represents commands or display data.
Must be driven explicitly by firmware.

⚠️ ***Important Notice***
If PE10, PE11, or PE13 are not configured as GPIO outputs, the display may remain blank, even if SPI is configured and functioning correctly.

## Badges

![MCU](https://img.shields.io/badge/MCU-STM32H7-informational)
![Board](https://img.shields.io/badge/Board-WeAct%20STM32H7-blue)
![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-brightgreen)
![Framework](https://img.shields.io/badge/Framework-STM32CubeIDE%2FHAL-lightgrey)
![Status](https://img.shields.io/badge/Status-Work_in_Progress-orange)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)

## Project Structure

This repository follows the standard STM32CubeIDE folder structure. Here is a breakdown of the key files and directories:


## Project Structure

```
This repository follows the standard STM32CubeIDE folder structure. Here is a breakdown of the key files and directories:

```text
├── Core
│   ├── Inc
│   │   └── main.h           # Main header file (defines and includes)
│   └── Src
│       ├── main.c           # Main application code (Display driver & Logic)
│       ├── stm32h7xx_it.c   # Interrupt Service Routines (Cleaned up for this example)
│       └── system_stm32h7xx.c
├── Drivers                  # STM32 HAL & CMSIS Drivers (auto-generated)
├── *.ioc                    # STM32CubeMX Configuration file (Open this to see Pinout settings)
└── README.md                # Project Documentation
```

## FAQ

#### Question 1

Answer 1

#### Question 2

Answer 2
















