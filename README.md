<div align="center" style="text-align:center;">

## KSControl
### WiFi-controlled and autonomous gadget to turn on and off the Klarstein NB series A/C

</div><br>

### Disclaimer:
#### This was only tested with the NB9 model but it'll most likely work with other NB series aircons

<br>

### Parts list:
- ESP8266 board
- Infrared LED
<!-- TODO: maybe transistor + series resistor? -->

<br>

### Software installation:

1. Set up your ESP board in the Arduino IDE
2. Clone or download and extract this project and open it with the Arduino IDE
3. Rename the file `settings.h.template` to `settings.h` and edit it to your liking
4. Upload the sketch to the ESP board

<br>

### Schematic:
Coming soon™

<br>

### Server:
This sketch spins up an HTTP server on your ESP's WiFi.  
It can be used to remote control the A/C from anywhere.  
Use the following routes to talk to the ESP.  

> ### POST `/state`
> Since all control data (temp, mode, fan speed, etc) are all simultaneously transmitted whenever only one value changes, the ESP needs to keep track of the current state of the A/C.  
> This is done through this route. Whenever the state updates, it is sent to the A/C unit via infrared.  
>   
> Some values depend on your A/C model's capabilities (some can't heat for example) and will just not influence the actual A/C's state.  
> Note that the ESP's state will be updated with whatever you throw at it though!  
>   
> To update the state with new values, send a POST request to this route with the following JSON data:
> 
> ```jsonc
> {
>     "enabled": true, // used to turn the A/C on or off
>     "temp": 20,      // can be 16-30 °C
>     "mode": "AUTO",  // can be AUTO, COOL, DRY, FAN or HEAT
>     "fan": "AUTO",   // can be AUTO, LOW, MEDIUM, HIGH
> }
> ```

<br>

> ### GET `/state`
> This route returns the current state that's set in the ESP.  
> The returned JSON object has the same structure as in [POST `/state`](#post-state)  
> Exception being when no state update has been received by the ESP yet, then it will return an empty object.  
>   
> If an error occurs, the properties `error` (boolean) and `message` (string) will appear.

<br><br><br>

<div align="center" style="text-align: center;">

Made with ❤️ by [Sv443](https://github.com/Sv443)  
If you like this project, please consider [supporting me](https://github.com/sponsors/Sv443)  
  
© 2022 Sv443 - [MIT license](./LICENSE.txt)

</div>