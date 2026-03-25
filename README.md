# Ubiquitous Computing Lab Exercises

This repository contains my solutions for the **Ubiquitous Computing** laboratory exercises at the [FER course](https://www.fer.unizg.hr/en/course/ubicom_a).

## 📚 Course Info

- **Course:** Ubiquitous Computing
- **Faculty:** Faculty of Electrical Engineering and Computing (FER), University of Zagreb
- **Focus:** Embedded systems, sensors, actuators, and real-world interaction using microcontrollers

---

## 🧪 Exercise 1 – Button Handling (CButton)

### 📝 Task Description

The goal of this exercise is to implement a `CButton` class that handles a physical button connected to a GPIO pin.

The class must:

- Accept a **GPIO pin number** during instantiation
- Detect different types of button interactions:
  - **Single press**
  - **Double press**
  - **Long press**
- Call **user-defined callback functions** depending on the type of press

In the main program:

- Instantiate the `CButton` class
- Register **three callback functions**:
  - One for single press
  - One for double press
  - One for long press

---

## ⚙️ Implementation Overview

Typical responsibilities of the `CButton` class:

- Debouncing the button input
- Measuring time between presses
- Detecting press duration
- Triggering appropriate callbacks

`CButton` interface

```cpp
typedef void (*ButtonEventHandler)(void);

class CButton {
  CButton(int port);
  void attachSingleClick(ButtonEventHandler method);
  void attachDoubleClick(ButtonEventHandler method);
  void attachLongPress(ButtonEventHandler method);

  void tick();
}
```

`CButton` State Diagram:

![`CButton` State Diagram](./img/CButtonState.png "`CButton` State Diagram")

Example usage:

```cpp
CButton button(BUTTON_GPIO_PIN);

button.attachSinglePress(onButtonClicked);
button.attachDoublePress(onButtonDoubleClicked);
button.attachLongPress(onButtonLongPressed);

void loop() {
    button.tick();
}
```
