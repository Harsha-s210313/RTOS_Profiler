# RTOS Profiler — STM32F411RE

A real-time FreeRTOS task profiler built from scratch on the STM32F411RE microcontroller. Hooks into the FreeRTOS kernel using trace macros, measures per-task CPU usage with the ARM Cortex-M4 DWT cycle counter, and streams live statistics over UART as JSON for visualization on a Python dashboard.

---

## Overview

Understanding how an RTOS distributes CPU time across tasks is critical for debugging performance issues, detecting task starvation, and optimizing system behavior. This project implements a lightweight profiler that runs invisibly alongside your application — zero task overhead, no polling, no dedicated hardware timer.

Every time the FreeRTOS scheduler performs a context switch, the profiler records a timestamp using the DWT cycle counter. Over time it accumulates per-task CPU cycles, context switch counts, and average run times — then streams the data live over UART every second.

---

## How It Works

### Kernel Hook Integration

FreeRTOS exposes two trace macros that fire on every context switch:

```c
traceTASK_SWITCHED_IN()   // called when a task gains the CPU
traceTASK_SWITCHED_OUT()  // called when a task loses the CPU
```

These macros are overridden in `FreeRTOSConfig.h` to call the profiler:

```c
#define traceTASK_SWITCHED_IN()   profiler_task_switched_in()
#define traceTASK_SWITCHED_OUT()  profiler_task_switched_out()
```

### DWT Cycle Counter

The ARM Cortex-M4 has a built-in hardware cycle counter — the Data Watchpoint and Trace (DWT) unit. It counts CPU cycles since reset at full clock speed (100MHz = 10ns resolution). No timer peripheral required.

```c
CoreDebug->DEMCR |= (1U<<24);  // enable trace
DWT->CYCCNT = 0;                // reset counter
DWT->CTRL   |= (1U<<0);        // start counting
```

### Profiling Logic

```
traceTASK_SWITCHED_IN
    → read DWT cycle count
    → store as cycle_in for current task

traceTASK_SWITCHED_OUT
    → elapsed = read_cycles() - cycle_in
    → task.cycle_count += elapsed
    → task.context_switch_count++
    → total_cycles += elapsed

Every 1000ms (Stats Task)
    → CPU% = (task.cycle_count / total_cycles) * 100
    → send JSON over UART
```

### Data Flow

```
FreeRTOS Kernel
    ↓ traceTASK_SWITCHED_IN / OUT
rtos_profiler.c
    ↓ DWT cycle counter timestamps
    ↓ per-task stats accumulation
Stats Task (every 1s)
    ↓ profiler_calc_stats()
    ↓ profiler_print_stats()
UART (USART2 @ PA2)
    ↓ JSON stream
Python Dashboard
    ↓ live bar chart — CPU% per task
```

---

## Project Structure

```
RTOS_Profiler/
├── Inc/
│   ├── FreeRTOSConfig.h     # FreeRTOS kernel config + hook macro definitions
│   ├── dwt_counter.h        # DWT cycle counter driver declarations
│   ├── rtos_profiler.h      # TaskStats struct, profiler API declarations
│   └── uart.h               # UART driver declarations
├── Src/
│   ├── main.c               # Tasks, ISR, peripheral init, scheduler start
│   ├── dwt_counter.c        # DWT init, cycle read, cycle-to-microsecond conversion
│   ├── rtos_profiler.c      # Hook implementations, stats calculation, JSON output
│   └── uart.c               # Bare-metal USART2 driver — no HAL
├── FreeRTOS/
│   └── Source/
│       ├── *.c              # FreeRTOS kernel source files
│       ├── include/         # FreeRTOS headers
│       └── portable/
│           ├── GCC/         # ARM_CM4F port (port.c, portmacro.h)
│           └── MemMang/     # heap_4.c — 8KB heap
├── Dashboard/
│   └── dashboard.py         # Python live dashboard — pyserial + matplotlib
├── Startup/
│   └── startup_stm32f411retx.s
├── STM32F411RETX_FLASH.ld
└── STM32F411RETX_RAM.ld
```

---

## Hardware

| Component | Detail |
|---|---|
| MCU | STM32F411RE (Nucleo-64) |
| Architecture | ARM Cortex-M4F @ 100MHz |
| Profiling Timer | DWT Cycle Counter (no hardware timer used) |
| UART | USART2 — PA2 TX → USB Virtual COM Port |
| LED | PA5 — on-board |
| Button | PC13 — on-board USER button |

---

## FreeRTOS Configuration

| Parameter | Value |
|---|---|
| Port | GCC / ARM_CM4F |
| Heap | heap_4 — 8KB |
| Tick Rate | 1000 Hz (1ms tick) |
| Max Priorities | 5 |
| CPU Clock | 100 MHz |
| Preemption | Enabled |

---

## Tasks

| Task | Priority | Behavior | Purpose |
|---|---|---|---|
| `vTask1` | 1 | Sends counter to queue every 1s | Low CPU — queue producer |
| `vTask2` | 1 | Receives from queue, logs via UART | Blocked most of the time |
| `vTask3` | 2 | Periodic UART message every 100ms | Medium CPU — preempts Task1/2 |
| `vTask4` | 3 | Waits on button semaphore, toggles LED | Event-driven — near zero CPU |
| `vStatsTask` | 4 | Calculates and prints stats every 1s | Highest priority — profiler output |

---

## Profiler Data

Per task the profiler tracks:

| Metric | Description |
|---|---|
| `task_name` | FreeRTOS task name string |
| `cycle_count` | Total CPU cycles accumulated |
| `cycle_in` | DWT timestamp at last SWITCHED_IN |
| `context_switch_count` | Total number of context switches |
| `CPU_percentage` | (task cycles / total cycles) × 100 |

---

## UART Output Format

Stats are streamed as JSON every 1 second:

```json
{"task":"Task1","cpu":12.34,"switches":1024}
{"task":"Task2","cpu":8.91,"switches":876}
{"task":"Task3","cpu":45.67,"switches":3201}
{"task":"Task4","cpu":0.12,"switches":14}
{"task":"Stats","cpu":2.30,"switches":60}
```

Connect via any serial terminal at **9600 baud** on the Nucleo's Virtual COM Port (USART2).

---

## DWT Driver

```c
void     dwt_init(void);                 // enable and reset cycle counter
uint32_t read_cycles(void);              // return current cycle count
uint32_t cast_to_usecs(uint32_t cycles); // convert cycles to microseconds
```

At 100MHz: `microseconds = cycles / 100`

---

## Build Instructions

1. Clone the repository
2. Open STM32CubeIDE → `File → Import → Existing Projects into Workspace`
3. Select this folder
4. Ensure FreeRTOS source folders are not excluded from build
5. Press `Ctrl+B`

**Expected output:**
```
Build Finished. 0 errors, 1 warning. (FreeRTOS internal — not application code)
```

---

## Python Dashboard

Install dependencies:
```bash
pip install pyserial matplotlib
```

Run:
```bash
cd Dashboard
python dashboard.py
```

The dashboard reads the serial port, parses the JSON stream, and displays a live updating bar chart showing CPU usage per task.

---

## Key Concepts Demonstrated

- FreeRTOS kernel trace hook integration
- ARM Cortex-M4 DWT cycle counter for high-resolution timing
- Zero-overhead profiling — no dedicated task, no polling
- Bare-metal USART2 driver without HAL
- JSON serialization on embedded target
- Live data visualization from embedded serial stream

---

## Build Output

```
text     data      bss      dec
26004     460    10540    37004    RTOS_Profiler.elf

Build Finished. 0 errors, 1 warning.
```

---

*Hardware testing pending STM32F411RE Nucleo board arrival. Firmware verified to compile and link cleanly.*
