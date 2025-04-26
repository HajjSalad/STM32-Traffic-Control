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
                                     |  |  │  |  |
                          south-bound|  |  │  |  |
                            traffic  |  |⬇️│  |⬆️| 
                                     |  |  │  |  |
                                     |⬇️|  │⬆️|  |
                                     ── ── ── ── ─ 
                                      🚥 
               _ _ _ _ _ _ _ .       Light 2           ._ _ _ _ _ _ _ _ _ west-bound
               _ _ _ ⬅️ _  _ |                       🚦|_⬅️ _ _ _ _ _ _ _   traffic
               ___________⬅️_|                 Light 3 |_______⬅️________
    east-bound _ _ _➡️ _ _ _ |🚦                       |_➡️ _ _ _ _ _ _ _
      traffic  _ _ _ _ _➡️ _ |Light 1                  |_ _ _ _ ➡️_ _ _ _
                                             🚥 Light 4
                                     ── ── ── ── ─ 
                                     |⬇️|  │  |⬆️|
                                     |  |  │  |  | north-bound 
                                     |  |⬇️│⬆️|  |   traffic
                                     |  |  │  |  |
                                     |  |  │  |  |
```

🔄 **Traffic Light Synchronization**
`Light 1` and `Light 3` are synchronized, operating in tandem to manage east-west traffic flow.
`Light 2` and `Light 4` are synchronized, controlling north-south traffic flow.

💡 **Example Serial Terminal Output**
```bash
other outputs above...
Light 1-3: RED (east-west traffic stopped)   
Light 2-4: GREEN (north-south traffic allowed) 

Light 1 car detected: 1
Light 1 car detected: 2
Light 1 car detected: 3

Light 2-4: YELLOW (caution, changing soon)
Light 2-4: RED (north-south traffic stopped)
Light 1-3: GREEN (east-west traffic allowed)
... continues with other outputs
```

### Demo
View the demo in action.
