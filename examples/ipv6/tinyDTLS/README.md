# Setup Tiny DTLS for ContikiOS

## Clone TinyDTLS and install

```
$ cd contiki/app
$ git clone https://github.com/cetic/tinydtls.git
$ cd tinydtls && sh reconf.sh
$ ./configure --without-ecc
$ cd test && make && cd ..
$ ./configure  --with-contiki --without-ecc
```

## Examples dtls-server and dtls-client with RE-Mote devices

- Go to the tinyDTLS folder

```
$ cd contiki/examples/ipv6/tinyDTLS && make TARGET=zoul savetarget
```

- Flash program into server

```
$ make dtls-server.upload MOTES=/dev/ttyUSB0 && make login MOTES=/dev/ttyUSB0

```

- Flash program into client. But edit ip address of server at function `set_connection_address(...)` on `dtls-client.c`

```
$ make dtls-client.upload MOTES=/dev/ttyUSB1 && make login MOTES=/dev/ttyUSB1
```

### Tip show log to console and then, save to file
`<command> |& tee  <outputFile>`

### 6lbr Mapping

| Module HR911105A | RE-Mote |
|---|---|
| VCC | 5V |
| GNC | GND |
| SCL | SCLK1 |
| CS | GPA7 |
| S0 | MOS01 |
| S1 | MOSI1 |

# Auth Tuyenng
