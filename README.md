## 🚦 Traffic Control System

This project implements a smart Traffic Light Control System on STM32 microcontroller, desgined to manage a 4-way intersection. The system intelligently controls 4 traffic lights, dynamically adjusting light states based on real-time vehicle detection and traffic density. 

By leveraging external interrupts, queue-based scheduling, and SysTick timer, the controller ensures efficient traffic flow, minimal waiting times, and safe transitions between lights.

**Documentation**: The project includes **comprehensive Doxygen documentation** covering modules, functions, classes and detailed usage.       
👉 Explore the generated docs: [Doxygen Documentation](https://hajjsalad.github.io/STM32-Traffic-Control/)

### 🔑 Key Features
1. **Event-Driven Architecture**  ·  `Low-Power` · `Interrupts`
- The system remains in a low-power idle state until a vehicle is detected, reducing unnecessary CPU usage.
- All events are interrupt-driven, ensuring responsive traffic management without continous polling. 
2. **GPIO External Interrupts (EXTI)**  ·  `GPIO` · `Interrupts`  · `Vehicle Detection`
- Each traffic lane has a button-simulated vehicle sensor connected to a GPIO pin.
- External interrupts immediately detect vehicle presence, triggering the control logic efficiently.
3. **Efficient Queue System**  ·  `Circular Queue` · `Scheduling`
- Uses a circular queue to manage requests for green signals from different lanes.
- Guarantees first-come, first-served priority while preventing lost requests.
- Optimized for multiple simultaneous requests.
4. **Dynamic Signal Timing**  ·  `Adaptive Control` · `Timing`
- Adjust green signal duration based on the number of vehicles detected.    
    Example: 1 car -> 2 seconds, 2 cars -> 3 seconds, >3 cars -> 5 seconds.
- Ensures shorter waits for low-traffic lanes and longer green phases for high-traffic lanes.
5. **SysTick Timer**  ·  `Timers` · `Scheduling` · `Precision`
- Implements a millisecond-precision timer for scheduling light transitions and timeouts.
- Enables precise delay management and time-based vehicle detection logic.
6. **UART Communication**  ·  `UART` · `Debugging` · `Monitoring`
- UART outputs provide a detailed, real-time log of system operations, enabling effective debugging, state monitoring, and timing analysis.
- Displays traffic light states, vehicle counts, transitions, and timing information in real-time.
7. **LED Traffic Light Control**  ·  `GPIO` ·  `Embedded Sytems`
- Uses GPIO outputs to drive LEDs representing traffic lights (RED, GREEN, YELLOW).
- Provides accurate visual simulation of real-world trffic lights.
8. **Bare-Metal Firmware**  ·  `Direct Register Access` · `Embedded` · `C Programming`
- Written entirely in C, using direct register access for maximum efficiency.
- No operating system overhead; fully bare-metal for predictable timing and low latency.
9. **Modular Design Architecture**  ·  `Modularity` · `Maintainability`
- Firmware divided into clear modules: `controller`, `lights`, `exti`, `queue`, `uart`, `systick` encouraging reuse and scalability for future traffic projects.
- Each module handles a specific responsibility, making code easy to maintain and extend.
10. **Doxygen Documentation**  ·  `Documentation` · `Maintainability`
- Fully documented using Doxygen with clear function, module, and data structure description.
- Generate browsable HTML documentation published via GitHub Pages from the `docs/` directory.

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

#### 🔄 **Traffic Light Synchronization**   
`Light 1` and `Light 3` are synchronized, operating in tandem to manage east-west traffic flow.  
`Light 2` and `Light 4` are synchronized, controlling north-south traffic flow.  

#### 💡 **Example Serial Terminal Output**
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

### 🛠️ Tools & Software
🕹️ **Microcontroller Development**   
- **VS Code** - Primary development environment for STM32 firmware, used for editing, building, and debugging.       
- **OpenOCD** - Used for flashing firmware and debugging the STM32 over SWD.    
- **Makefile** - Manages compilation, linking, and build automation for the project.    

⚙️ **Hardware**    
- STM32 MCU - Microcontroller responsible for controlling traffic light logic and timing.  
- RGB LEDs - Used to simulate the traffic lights.  
  - Used Red and Green LEDs; Yellow is achieved by activating both red and green LEDs simultaneously.  
- Resistors - Limit LED current and protect GPIO pins.
- Breadboards - Enables rapid prototyping and testing of the traffic light system.  

### ⛓️ Hardware Connection  
Traffic Light LED Connections  
- LEDs are configured in an active-low setup, with the common anode connected to ground.
- Red and green channels driven by dedicated GPIO outputs.  
- Yellow state achieved by simultaneously activating both red and green channels.          

Button Connections:  
- Tactile push buttons are connected to GPIO input pins configured with internal pull-up resistors.  
- Pressing a button pulls the input low, generating a GPIO external interrupt (EXTI) used to simulate vehicle detection.  

#### 📍 Pin Assignments
|   LIGHT   |   RED     |   GREEN   |   BUTTON |
|-----------|-----------|-----------|----------|
|  `Light 1`  |   `PB10`    |    `PB4`    |   `PC10`   |
|  `Light 2`  |   `PB5`    |    `PB3`    |   `PC11`   |
|  `Light 3`  |   `PB2`     |    `PB1`    |   `PC12`   |
|  `Light 4`  |   `PB14`   |    `PB13`   |   `PC13`   |

### Demo
![Demo 1](./demo.gif)
