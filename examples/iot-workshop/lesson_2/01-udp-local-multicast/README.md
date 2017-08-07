# Link-Local UDP multicast example

The following examples will show how to read and set radio parameters, such as:

* RSSI (Received signal strength indication) and LQI (Link quality indicator)
* Radio channel
* PAN ID (network identifier)

We will also learn how to use the Simple-UDP library, which allows to create wireless applications on top of IPv6/UDP, and transmit data such as sensor readings and system information.

## Requirements

You will need at least two Zolertia motes, the `RE-Mote` and `Z1` can be used together in the same network.

## Compile and program the example

````
make 01-udp-local-multicast.upload && make login
````

The command above should open a serial connection right after thenode is flashed, showing:

````
Zolertia RE-Mote revision B platform
CC2538: ID: 0xb964, rev.: PG2.0, Flash: 512 KiB, SRAM: 32 KiB, AES/SHA: 1, ECC/RSA: 1
System clock: 16000000 Hz
I/O clock: 16000000 Hz
Reset cause: CLD or software reset
Rime configured with address 00:12:4b:00:09:df:4f:53
 Net: sicslowpan
 MAC: CSMA
 RDC: nullrdc

* Radio parameters:
   Channel 15 (Min: 11, Max: 26)
   Tx Power   3 dBm (Min: -24 dBm, Max:   7 dBm)
   PAN ID: 0xABCD

***
Sending packet to multicast adddress ff02::1
ID: 171, core temp: 25.238, ADC1: 2316, ADC2: 0, ADC3: 272, batt: 3277, counter: 1
````
