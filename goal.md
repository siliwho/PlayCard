- Finish C course by **10 March**
- Can give:
  - **3 hrs × 5 weekdays = 15 hrs**
  - **5 hrs × 2 weekend = 10 hrs**

- Total = **~25 hrs/week**

Now we design timeline properly around:

- C completion: 10 March
- Exams: 21 Feb – 1 March (so mostly revision mode before 10 March anyway)
- End-April exams

---

# 🧠 Realistic Total Duration With 25 hrs/week

At 30 hrs/week:

This project becomes:

> ~14–18 weeks total (3.5–4.5 months)

That’s strong and manageable.

---

# 🗓 Full Structured Roadmap (Starting 10 March)

We’ll break into 5 major phases.

---

# 🟢 PHASE 1 — Bare Metal Mastery (3 Weeks)

**(10 March – 31 March)**

### Goal:

No HAL. Full register confidence.

### Week 1

- Clock setup (RCC)
- GPIO input/output
- Button interrupt
- LED toggle
- Basic delay using SysTick

### Week 2

- UART driver (interrupt based)
- SPI driver (polling first)
- Send raw data to display

### Week 3

- Display init sequence
- Draw pixel
- Draw rectangle
- Simple text rendering

End of Phase 1:
You have:

- Working drivers
- Display output
- Clean project structure

---

⚠ End of April exams approaching
So don’t start RTOS yet.

---

# 🟡 PHASE 2 — Light Study During Exams (Mid-April)

During exam period:

- No heavy coding
- Read:
  - Context switching basics
  - ARM Cortex-M exception model
  - PendSV and SysTick

- Sketch RTOS architecture on paper

---

# 🟢 PHASE 3 — Custom RTOS (4–5 Weeks)

**(May Full Month)**

This is your core firmware identity.

### Week 1

- Task Control Block (TCB)
- Stack per task
- Cooperative scheduler

### Week 2

- Preemptive scheduler
- SysTick
- PendSV context switch
- Save/restore registers

### Week 3

- Priority scheduling
- Task delay
- Idle task

### Week 4

- Mutex
- Simple queue
- Inter-task communication

Optional Week 5:

- Stack overflow detection
- Measure context switch cycles

End of Phase 3:
You now have:

> Your own working RTOS.

That alone is resume-worthy.

---

# 🔵 PHASE 4 — USB Stack From Scratch (5–6 Weeks)

**(June – mid July)**

Hardest phase.

### Week 1

- Understand USB hardware
- Enable control endpoint
- Handle reset interrupt
- Capture setup packets

### Week 2

- Implement Device Descriptor
- Handle GET_DESCRIPTOR
- SET_ADDRESS

### Week 3

- Configuration descriptor
- SET_CONFIGURATION
- Successful enumeration

Huge milestone here.

### Week 4

- HID report descriptor
- Interrupt IN endpoint
- Send button states

### Week 5

- Stability
- Handle edge cases
- Debug enumeration failures

End:
USB controller works on PC.

---

# 🟣 PHASE 5 — BLE Integration (3–4 Weeks)

**(Mid July – August)**

### Week 1

- BLE HID on ESP32-C3
- Advertising
- Pairing

### Week 2

- Define STM32 ↔ ESP protocol
- UART interrupt-based driver
- Packet framing + CRC

### Week 3

- Mode switching
- BLE + USB dual support
- Display status info

---

# 🏁 Final Integration (2 Weeks)

- Clean architecture
- Code refactor
- Documentation
- Block diagrams
- Write technical blog (very important)

---

# 📊 Total Timeline

| Phase       | Duration  |
| ----------- | --------- |
| Bare metal  | 3 weeks   |
| RTOS        | 4–5 weeks |
| USB         | 5–6 weeks |
| BLE         | 3–4 weeks |
| Integration | 2 weeks   |
| **Total**   | ~17 weeks |

At 25 hrs/week → Very realistic.

---

# 🏆 What You’ll Have By August

A:

- Custom RTOS
- USB stack from scratch
- BLE HID device
- SPI display driver
- Dual MCU system
- Full documentation

That’s elite-level embedded portfolio.

---
