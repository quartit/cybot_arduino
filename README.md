# cybot_arduino
Basic arduino project to run steppers with arduino

This project is a very simple setup to drive steppers for the cybot robot arm.
Hardware you need :

- Power supply 24V
- 6 stepper motors (3 nema 17 + 3 nema 23 ) (+ 1 nema 17 for the gripper)
- Arduino MEGA
- Arduino MEGA Sensor Shield V2.0 https://www.aliexpress.com/w/wholesale-Arduino-MEGA-Sensor-Shield-V2.0.html?spm=a2g0o.detail.search.0
- Stepper Motor Driver Expension Board (1 per stepper) https://www.aliexpress.com/w/wholesale-42-Stepper-Motor-Driver-Expansion-Board.html?spm=a2g0o.productlist.search.0
- Stepper driver TMC2209 (best) or DRV8825 or A4988 (1 per stepper)
- Dupont connectors

Install arduino on your computer and open the testCybotWithSpeedyStepper.ino sketch.
Add SpeedyStepper in Library Manager or from here https://github.com/Stan-Reifel/SpeedyStepper

1) Plug the Shield to the arduino mega
2) plug the driver to the driver expension board (check the direction ; ground at the opposite of the vin)
3) connect with dupont wire the V and G of the driver expension to a 5v and ground of the arduino shield
4) connect EN + STEP + DIR to MEGA shield pin 14 + 15 + 16
5) connect stepper motor wires to the driver expension
6) connect power supply + and - to Driver expension board + and -
7) connect arduino USB to computer and upload the project.
8) IMPORTANT ! check vref of all drivers , see page https://all3dp.com/2/vref-calculator-tmc2209-tmc2208-a4988/
9) Open serial monitor and launch the following command : move X1000/X

You should now have the stepper moving.
Now repeat the process with all the steppers you have and the new connection pin :)
You should be able to add new code very easily and implement other commands  
--> try first with stepper motors not assembled to the robot !
