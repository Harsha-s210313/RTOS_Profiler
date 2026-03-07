# FreeRTOS Scheduler — STM32F411RE

A bare-metal FreeRTOS project for the **STM32F411RE (Nucleo-F411RE)** board, built entirely from scratch without STM32CubeMX code generation. Demonstrates core RTOS concepts including task scheduling, inter-task communication, ISR integration, and shared resource protection.

> ⚠️ **Status:** Awaiting hardware — built and verified to compile with 0 errors. Ready to flash and test on Nucleo-F411RE.

---

## 📋 Hardware

| Component | Detail |
|---|---|
| MCU | STM32F411RE (Cortex-M4F @ 100MHz) |
| Board | Nucleo-F411RE |
| LED | PA5 (onboard) |
| Button | PC13 (onboard USER button) |
| UART | USART2 via PA2 (ST-Link Virtual COM Port) |

---

## 🏗️ Project Structure

```
FreeRTOS_SCHEDULER/
├── Src/
│   └── main.c              # Application code — all tasks, ISR, UART
├── Inc/
│   └── FreeRTOSConfig.h    # FreeRTOS kernel configuration
├── FreeRTOS/
│   └── Source/
│       ├── *.c             # FreeRTOS kernel source files
│       ├── include/        # FreeRTOS headers
│       └── portable/
│           ├── GCC/        # ARM_CM4F port (port.c, portmacro.h)
│           └── MemMang/    # heap_4.c memory manager
├── Startup/
│   └── startup_stm32f411retx.s
├── STM32F411RETX_FLASH.ld  # Linker script
└── .cproject / .project    # STM32CubeIDE project files
```

---

## ⚙️ FreeRTOS Configuration

| Parameter | Value |
|---|---|
| Port | GCC / ARM_CM4F |
| Heap | `heap_4` (8 KB) |
| Tick Rate | 1000 Hz (1ms tick) |
| Max Priorities | 5 |
| CPU Clock | 100 MHz |
| Preemption | Enabled |

---

## 🧵 Tasks

| Task | Priority | Function |
|---|---|---|
| `vTask1` | 1 | Queue **producer** — sends incrementing counter every 1s |
| `vTask2` | 1 | Queue **consumer** — receives counter, prints via UART |
| `vTask3` | 2 | Periodic UART message every 100ms (preemption demo) |
| `vTask4` | 3 | Button handler — takes semaphore from ISR, toggles LED |

---

## 🔌 FreeRTOS Concepts Demonstrated

### Queue
- `vTask1` sends an incrementing `int` into a 5-item queue every second
- `vTask2` blocks on `xQueueReceive()` and prints the value via UART using `sprintf()`

### Binary Semaphore + ISR
- `EXTI15_10_IRQHandler` fires on PC13 button press (falling edge)
- ISR calls `xSemaphoreGiveFromISR()` to signal `vTask4`
- `vTask4` blocks on `xSemaphoreTake()` — zero CPU usage while waiting
- LED toggles on each button press

### Mutex
- `vTask2` and `vTask3` both use UART
- `xUARTmutex` prevents garbled output from simultaneous access
- Pattern: `xSemaphoreTake()` → send → `xSemaphoreGive()`

### Preemption
- `vTask3` (priority 2) preempts `vTask1/vTask2` (priority 1) every 100ms
- `vTask4` (priority 3) immediately preempts all others on button press

---

## 🔧 UART Configuration

| Parameter | Value |
|---|---|
| Peripheral | USART2 |
| TX Pin | PA2 (AF7) |
| Baud Rate | 9600 |
| Baud Register | `BRR = 0x0683` |

Connect via any serial terminal (e.g. PuTTY, Tera Term) at **9600 baud** on the Nucleo's Virtual COM Port.

---

## 🛠️ Build Instructions

1. Open **STM32CubeIDE**
2. Import project: `File → Import → Existing Projects into Workspace`
3. Select this folder
4. Ensure FreeRTOS source folders are **not excluded from build**
   - Right-click each `FreeRTOS/` subfolder → Resource Configurations → Exclude from Build → Deselect all
5. Press **Ctrl+B** to build

---

## 📚 Learning Notes

This project was built as a learning exercise — every line of code was written manually to understand:
- How FreeRTOS integrates at the bare-metal level (no HAL abstraction)
- How the ARM Cortex-M4 NVIC interacts with the FreeRTOS scheduler
- The difference between Queues, Semaphores, and Mutexes
- Priority inversion and how Mutex priority inheritance prevents it
