
### To see the Design click here: 
[Link to the 3D case designed in Fusion 360](https://a360.co/2QQmXdH)

### The goal of the Prototype is: 
  1. To measure Temperature, Humidity, Light Intensity, and Vibrations to check for Fall Detection of an Object in Cold Chain Supply System and alert us if the measured quantities are above the threshold values set by us.
  2. We use **Thingspeak Channel** to store the quantities in real time. 
  3. And then we link the *Thingspeak Channel* with **IFTTT** to produce alerts in real time by comparing the values with the Threshold values we set in IFTTT.
  4. The alerts are sent to the person in-charge.
  5. And design a PCB for dimensions and design a 3D box to fit that in.

### Steps followed in developing the prototype are:
  
  The initial step is to check each sensor DHT11(Temperature and Humidity Sensor), LDR(Light Sensor), MPU6050(Vibration Sensor), ESP8266 12e WiFi Module individually to check their working condition.
  - Next step is we integrate each sensor one by one and check for working. 
  - Then we create a Thingspeak Channel.
  - We do code for sending the values to the Thingspeak Channel.
  - We create IFTTT setup and set UPPER Limits.
  - We code for invoking the IFTTT alerts if the measured values go beyond the Threshold Values.
  - At last using a PCB design and the dimensions, design a 3D box in FUSION360.

### To see the Working Videos, below is the Google Drive Link:
[Click here to see the Working Videos](https://drive.google.com/drive/folders/1iGdLHke2VM7GPJ6VkCGJTwRCgi-lxLVa?usp=sharing)

### For more details, the Documentation and Presentation we have given to the Company are added below:
[Click here to access them](https://drive.google.com/drive/folders/1egTVYgo1f8i9j1zpCDQ5hPAyHhzT4-QA?usp=sharing)
