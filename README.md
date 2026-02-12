# Xianru's e-paper sticky note
description: a personal project that uses a 4in2 e-paper display by waveshare, as a sticky note that can display messages for other home members.

# E-paper Sticky Note Project – Current Status Overview

This repository contains the **Usages and firmware** for the sticky note.  
The focus so far has been on **hardware connections and u8g2 font library instantiation**.

---

## Project Scope (So Far)

- Target application: **magnetic sticky note on Fridge**
- Focus: **hardware connections and u8g2 font library instantiation**

---

## Key Features Implemented

### Firmware

- **MQTT subscription and info updates**
  - Learnt about MQTT and uses a free service called EMQX, broker.emqx.io
  - managed in the code using PubSubClient.h
  - better than using just wifi and bluetooth

- **Screen Refresh and Title**
  - refreshes the screen
  - refined display format so new lines recognises /n sign start aligned


See:
- `Architecture.txt`

---

### Hardware

- **Self powered through TPU4065**
  - the dev board is connected to a lithum battery with a TPU4065 module for charging


See:
- `Images/Connection_Schematic.pdf`

---

## Repository Structure Guide

├── Connection_schematic/ # hardware connections through cables and jumper wires

├── Architecture/ # firmware Architecture for understanding code structure

└── README.md


Use this as the **entry point** when reviewing or onboarding.

---

## TODO / Next Steps

### Hardware

- Peer review of schematics (power + safety paths)

### Manufacturing

- Finalize BOM substitutions (availability check)
- Panelization (if required by fab)
- Assembly notes for sensitive components (MOSFETs, shunts)

### Bring-Up Preparation

- Define test points and probing plan
- Power-up checklist (pre- and post-relay)
- Fault-injection tests (E-stop, lost IRQ, brownout)

---

## Notes for Reviewers

- This is a **hardware-focused milestone**
- Functional firmware behavior is **not required** to evaluate safety or layout correctness
- Pay particular attention to:
  - Power cut-off logic
  - Grounding and current paths
  - Clear separation of noisy vs sensitive signals
