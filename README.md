## 🚦 Traffic Control System

This project implements a Traffic Light Control System on STM32 which controls 4 traffic lights at an intersection. The system dynamically adapts to vehicle presence and count.

### 🔑 Features
🔹 **Finite-State Machine (FSM):** Manages traffic light states and transitions for real-time control.  
🔹 **Dynamic Signal Timing:** Adjusts signal timing based on vehicle presence and count.  
🔹 **GPIO Interrupts:** Utilizes interrupt-driven button presses for vehicle counting.  
🔹 **RGB LED Control:** Drives traffic lights using GPIO outputs.    
🔹 **Bare-Metal Firmware:** Implemented in C using direct register access for efficiency.  

### 🏗 System Architecture
```
                                      |
                                      |
                                      |
                                      |

│
│
│
│
│

               _ _ _ _ _ _ _ .
               _ _ _ ⬅️ _  _ |
               ___________⬅️_| 
               _ _ _➡️ _ _ _ |🚦
               _ _ _ _ _➡️ _ |Light 1
```
⬇️⬇️⬆️⬆️➡️➡️⬅️⬅️

### Demo
View the demo in action.
