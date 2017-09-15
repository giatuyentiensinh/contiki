## Exercise

### Exercise 1
Upload program `exercise1` into RE-Mote by command

```
$ make exercise1.upload
```

### Exercise 2

- Upload `border-router` program into RE-Mote as the border-router.

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="bbbb::1/64"
 ```
- Upload `exercise2` program into RE-Mote 2 as the client.

 ```
 $ make exercise2.upload MOTES=/dev/ttyUSB1 && make login MOTES=/dev/ttyUSB1
 ```

- Compile and run `UDPServer` program on the computer and connected to the border-router.

 ```
 $ javac javacode/*.java && java javacode.UDPServer 

 ```
- Plug in `DHT22` sensor into RE-Mote and signal data of sensor maching with PIN PA5 of RE-Mote.

### Exercise 3

- Upload `border-router` program into RE-Mote as the border-router.

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="bbbb::1/64"
 ```
- Upload `coap server` program into RE-Mote as the coap server.

 ```
 $ make exercise3.upload MOTES=/dev/ttyUSB1
 ```
 
- Plug in `DHT22` sensor into RE-Mote and signal data of sensor maching with PIN PA5 of RE-Mote.

- Open browser and enter the IPv6 of the border-router to get IPv6 address of the node running the coap server. eg: `http://[bbbb::212:4b00:9df:4f53]`. And then enter IPv6 of the coap server in `firefox` (Extension Copper available). eg: `coap://[bbbb::212:4b00:615:a974]:5683/iot/contiki-remote`. And then enable `Debug Control` and type `application/json` on `Accept`. Finally, Click `GET` button to check the results.

### Exercise 4

- Upload `border-router` program into RE-Mote as the border-router.

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="cccc::1/64"
 ```

- Upload `exercise4` program into 2 RE-Mote as the client.
 
 ```
 $ make exercise4.upload MOTES=/dev/ttyUSB1 && make login MOTES=/dev/ttyUSB1
 $ make exercise4.upload MOTES=/dev/ttyUSB2 && make login MOTES=/dev/ttyUSB2
 ```

- Set up multi-hop topology like lesson 4 in the document.
- Compile and run `UDPServer` program on the computer and connected to the border-router.

 ```
 $ javac javacode/*.java && java javacode.UDPServer 

 ```