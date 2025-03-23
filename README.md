[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WXeqVgks)

# final-project-skeleton

* Team Number:
* Team Name: Neuro
* Team Members: Nick Harty & Sophia Fu
* GitHub Repository URL: https://github.com/upenn-embedded/final-project-s25-neuro
* GitHub Pages Website URL: https://neuro.medibound.com

## Final Project Proposal

### 1. Abstract

*In a few sentences, describe your final project.*

> Our final project is Neuro, a wearable headband embedded with EEG electrodes that monitor brain activity and detect abnormal patterns such as stress or early seizure indicators. The device integrates with Medibound to store, analyze, and visualize data in real time, allowing caregivers to monitor patient activity remotely and receive timely alerts.

### 2. Motivation

*What is the problem that you are trying to solve? Why is this project interesting? What is the intended purpose?*

> We aim to address the lack of accessible and continuous neurological monitoring tools for individuals prone to cognitive stress or seizure activity. This project is particularly compelling because it empowers patients and caregivers with real-time insights into brain health—outside of hospital settings—promoting preventative care and faster response. The ultimate purpose is to create a non-invasive, low-friction monitoring system that prioritizes health equity and tech-enabled accessibility.

### 3. System Block Diagram

*Show your high level design, as done in WS1 and WS2. What are the critical components in your system? How do they communicate (I2C?, interrupts, ADC, etc.)? What power regulation do you need?*

> At a high level, Neuro is composed of EEG sensors, a microcontroller, and wireless BLE communication to the cloud. The EEG sensor interfaces with the microcontroller using SPI protocol. The captured data is processed and transmitted via Bluetooth for storage and visualization in Medibound. The system is powered by a rechargeable Li-ion battery with voltage regulation circuitry to ensure consistent power delivery.

![](block_diagram.png)

### 4. Design Sketches

*What will your project look like? Do you have any critical design features? Will you need any special manufacturing techniques to achieve your vision, like power tools, laser cutting, or 3D printing?*

The device will look like a lightweight, adjustable headband that fits comfortably across the user’s forehead and temples. It includes embedded EEG sensors positioned strategically on contact points. The electronics are housed in a flexible casing at the rear of the band. A design priority is minimal intrusion and comfort for continuous use. We will use 3D printing to produce ergonomic casings for the electronics and laser cutting for internal frame support. LCD display will go on the back.

![img](nueroband.png)

### 5. Software Requirements Specification (SRS)

*Formulate key software requirements here. Think deeply on the design: What must your device do? How will you measure this during validation testing? Create 4 to 8 critical system requirements.*

*These must be testable! See the Final Project Manual Appendix for details. Refer to the table below; replace these examples with your own.*

* The EEG signal must be sampled at 250 Hz with a resolution of at least 16 bits.
* BLE data transmission must occur at least once every 5 seconds for real-time updates.
* Abnormal neural patterns (defined thresholds for stress or seizure-like waveforms) should trigger alerts within 3 seconds.
* On user request, the system must be able to visualize a live 10-second rolling EEG graph in the Medibound app.
* The device must enter low-power mode after 5 minutes of inactivity and automatically wake upon signal detection.

**5.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

* EEG: Electroencephalogram
* BLE: Bluetooth Low Energy
* SPI: Serial Peripheral Interface

**5.2 Functionality:** *Collectivized in 6.2*

### 6. Hardware Requirements Specification (HRS)

*Formulate key hardware requirements here. Think deeply on the design: What must your device do? How will you measure this during validation testing? Create 4 to 8 critical system requirements.*

*These must be testable! See the Final Project Manual Appendix for details. Refer to the table below; replace these examples with your own.*

* The EEG sensor must be able to read microvolt-level signals with <1 µV noise.
* A 3-lead electrode cable will be used to collect forehead-based EEG signals.
* A rechargeable battery with minimum 6-hour battery life is required.
* A vibration motor and LED will serve as optional feedback/alert mechanisms.
* The system should be housed in a wearable form factor weighing less than 200g (more than that is way too crazy heavy to be on your head - might adjust this later).

**6.1 Definitions, Abbreviations**

Here, you will define any special terms, acronyms, or abbreviations you plan to use for hardware

* Bitalino EEG: Biomedical sensor used for EEG monitoring
* UC-E6 Cable: Sensor cable for Arduino/Bitalino compatibility

**6.2 Functionality Combined Software & Hardware**

| ID       | Description                                                                                                                                                                                                                                       |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| NEURO-01 | The EEG sensor must continuously sample brain activity at 250 Hz with a 16-bit resolution. The system will collect this data every 4 milliseconds, processing and transmitting it via Bluetooth Low Energy (BLE) for visualization in real-time. |
| NEURO-02 | The system must transmit collected EEG data to the Medibound platform at least once every 5 seconds for real-time monitoring by caregivers.                                                                                                       |
| NEURO-03 | Upon detecting abnormal brain activity (defined as seizure-like waveforms or stress indicators), the device will trigger an alert within 3 seconds to notify caregivers via the Medibound platform or connected application.                      |
| NEURO-04 | Upon a request from the user, the system shall visualize a 10-second rolling EEG graph in the Medibound app, with live updates displaying the most recent EEG data.                                                                               |
| NEURO-05 | When no brain activity is detected for more than 5 minutes, the device shall enter low-power mode and automatically wake up upon detecting signal changes.                                                                                        |
| NEURO-06 | The system must provide haptic or visual feedback when abnormal activity is detected, offering immediate cues to the user or caregiver.                                                                                                           |

### 7. Bill of Materials (BOM)

*What major components do you need and why? Try to be as specific as possible. Your Hardware & Software Requirements Specifications should inform your component choices.*

*In addition to this written response, copy the Final Project BOM Google Sheet and fill it out with your critical components (think: processors, sensors, actuators). Include the link to your BOM in this section.*

> We will use the Bitalino EEG sensor, 3-lead electrode cable, and UC-E6 cable. These are compatible with Arduino and offer sufficient resolution and flexibility for our signal needs. Other components include a BLE-capable microcontroller (e.g., ESP32), a rechargeable battery pack, a vibration motor for alerts, and materials for casing (3D printed PLA). The complete BOM spreadsheet can be found [here](https://docs.google.com/spreadsheets/d/1D1r-qtnwWtG0i4U08g_JSRCN8SvMN2fPsdjjQd8AklY/edit?usp=sharing).

### 8. Final Demo Goals

*How will you demonstrate your device on demo day? Will it be strapped to a person, mounted on a bicycle, require outdoor space? Think of any physical, temporal, and other constraints that could affect your planning.*

> For demo day, we will showcase the headband on a test subject (team member), measuring and displaying real-time EEG signals using Medibound’s visualization module. Simulated stress signals will be used to trigger alerts, and we will demonstrate feedback via vibration/LED output. This will be an indoor demo, requiring a laptop and Bluetooth-enabled phone.

### 9. Sprint Planning

*You've got limited time to get this project done! How will you plan your sprint milestones? How will you distribute the work within your team? Review the schedule in the final project manual for exact dates.*

| Milestone  | Functionality Achieved                                                                                                                                                                                                                                                                                                                                                                   | Distribution of Work                                                                                                                                                                                                                                                      |
| ---------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Sprint #1  | Initial setup of the EEG sensor system (Bitalino EEG sensor) - If recieved.<br />Communication setup between the EEG sensor and microcontroller (ESP32).<br />Basic data collection from EEG sensor, sending data to the microcontroller via SPI.<br />Setup of Bluetooth Low Energy (BLE) communication for data transfer                                                               | Nick Harty: Setup and calibration of EEG sensor (Bitalino) and microcontroller (ESP32). Implement SPI communication between the components.<br />Sophia Fu: Work on BLE communication setup and initial data transmission to the cloud (Medibound integration)            |
| Sprint #2  | Integration of EEG data with Medibound platform for real-time visualization.<br />Setup of alert system for abnormal brain patterns (e.g., stress or seizure-like patterns).<br />Power management implementation (battery usage, low-power mode).<br />Initial user interface (UI) prototype in the Medibound app for visualizing EEG data.                                             | Nick Harty: Work on integrating Medibound platform with real-time EEG data, implementing low-power mode.<br />Sophia Fu: Develop the alert system for abnormal brain activity and begin UI development for data visualization in Medibound.                               |
| MVP Demo   | Fully functional EEG sensor system with real-time data transmission and visualization.<br />Basic alert system operational when abnormal brain activity is detected.<br />Demonstrate the device's ability to transmit data to Medibound and provide caregiver alerts.                                                                                                                   | Nick Harty: Finalize integration with Medibound, implement full BLE communication for data transfer, and ensure data visualization.<br />Sophia Fu: Complete the alert system and demonstrate it working with real-time data. Ensure the app is displaying live EEG data. |
| Final Demo | Fully functional wearable EEG system with real-time brain activity monitoring.<br />Demonstrate full features including:<br />- Data transmission to Medibound<br />- Alerts triggered by abnormal brain patterns<br />- Haptic or LED feedback when abnormal activity is detected<br />- Display of EEG data on the Medibound app<br />Final device assembled in a wearable form factor | Nick Harty: Lead the final testing, finalize device assembly, ensure the device functions seamlessly with the cloud platform.<br />Sophia Fu: Polish the UI, conduct final tests for abnormal brain pattern detection, and ensure the alert system is fully functional.   |

**This is the end of the Project Proposal section. The remaining sections will be filled out based on the milestone schedule.**

## Sprint Review #1

### Last week's progress

### Current state of project

### Next week's plan

## Sprint Review #2

### Last week's progress

### Current state of project

### Next week's plan

## MVP Demo

1. Show a system block diagram & explain the hardware implementation.
2. Explain your firmware implementation, including application logic and critical drivers you've written.
3. Demo your device.
4. Have you achieved some or all of your Software Requirements Specification (SRS)?

   1. Show how you collected data and the outcomes.
5. Have you achieved some or all of your Hardware Requirements Specification (HRS)?

   1. Show how you collected data and the outcomes.
6. Show off the remaining elements that will make your project whole: mechanical casework, supporting graphical user interface (GUI), web portal, etc.
7. What is the riskiest part remaining of your project?

   1. How do you plan to de-risk this?
8. What questions or help do you need from the teaching team?

## Final Project Report

Don't forget to make the GitHub pages public website!
If you’ve never made a GitHub pages website before, you can follow this webpage (though, substitute your final project repository for the GitHub username one in the quickstart guide):  [https://docs.github.com/en/pages/quickstart](https://docs.github.com/en/pages/quickstart)

### 1. Video

[Insert final project video here]

* The video must demonstrate your key functionality.
* The video must be 5 minutes or less.
* Ensure your video link is accessible to the teaching team. Unlisted YouTube videos or Google Drive uploads with SEAS account access work well.
* Points will be removed if the audio quality is poor - say, if you filmed your video in a noisy electrical engineering lab.

### 2. Images

[Insert final project images here]

*Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!).*

### 3. Results

*What were your results? Namely, what was the final solution/design to your problem?*

#### 3.1 Software Requirements Specification (SRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

Reflect on your project. Some questions to address:

* What did you learn from it?
* What went well?
* What accomplishments are you proud of?
* What did you learn/gain from this experience?
* Did you have to change your approach?
* What could have been done differently?
* Did you encounter obstacles that you didn’t anticipate?
* What could be a next step for this project?

## References

Fill in your references here as you work on your final project. Describe any libraries used here.
