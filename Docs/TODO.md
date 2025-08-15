### 🗂️ Core Systems

- [ ] **Camera System**  
  - [ ] Implement camera interface to monitor animatronic locations.  
  - [ ] Ensure camera usage increases power consumption.  
  - [ ] Design camera system to prevent animatronic movement when observed. 

- [ ] **Door Mechanics**  
  - [ ] Enable doors to block animatronic entry effectively.  
  - [ ] Ensure doors consume power when closed.  
  - [ ] Add visual and audio feedback for door interactions.

---

### ⚡ Power Management System

- [ ] **Power Consumption Rates**  
  - [ ] Implement a base power drain rate (e.g., 1% every 9.6 seconds).  
  - [ ] Increase power drain based on active systems:
    - [ ] Lights: +1 bar
    - [ ] Doors: +1 bar per closed door
    - [ ] Camera: +1 bar when active
  - [ ] Display power usage bars to indicate current consumption level. 

- [ ] **Foxy's Power Drain Mechanic**  
  - [ ] Program Foxy to drain additional power upon door impact:
    - [ ] 1st impact: -1% power
    - [ ] 2nd impact: -6% power
    - [ ] 3rd impact: -11% power
    - [ ] Subsequent impacts: increase drain by 5% each time
  - [ ] Reset or continue drain escalation based on game design choice. 

- [ ] **Power Outage Consequences**  
  - [ ] Trigger office blackout when power reaches 0%.  
  - [ ] Disable all systems (doors, lights, cameras) during blackout.  
  - [ ] Initiate Freddy's endgame sequence post-blackout. 

---

### 🤖 Animatronic AI

- [ ] **Basic AI Behavior**  
  - [ ] Program animatronics to patrol and attempt office entry.  
  - [ ] React to player actions (e.g., door closures, camera monitoring).

- [ ] **Foxy's Unique Behavior**  
  - [ ] Design Foxy to become more aggressive with frequent camera checks.  
  - [ ] Implement running sequence towards the office when triggered.  
  - [ ] Ensure Foxy's attacks correlate with power drain mechanics.
  - [ ] When doing its jumpscare the office uses a different texture than normal. (all monitors in the office will reflect)

---

### 🔊 Audio & Visual Feedback

- [ ] **Audio Cues**  
  - [ ] Add sound effects for footsteps, door interactions, and jumpscares.  
  - [ ] Include ambient sounds to enhance atmosphere.

- [ ] **Visual Effects**  
  - [ ] Implement screen flickers or static during power fluctuations.  
  - [ ] Design visual indicators for power usage levels.


Issues:
- [ ] If you open the camera while your using the lights from the doors it should turn off the lights.