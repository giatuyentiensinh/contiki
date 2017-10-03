The Contiki Operating System
============================

[![Build Status](https://travis-ci.org/contiki-os/contiki.svg?branch=master)](https://travis-ci.org/contiki-os/contiki/branches)

Contiki is an open source operating system that runs on tiny low-power
microcontrollers and makes it possible to develop applications that
make efficient use of the hardware while providing standardized
low-power wireless communication for a range of hardware platforms.

Contiki is used in numerous commercial and non-commercial systems,
such as city sound monitoring, street lights, networked electrical
power meters, industrial monitoring, radiation monitoring,
construction site monitoring, alarm systems, remote house monitoring,
and so on.

For more information, see the Contiki website:

[http://contiki-os.org](http://contiki-os.org)


## IPv6 Configure router in Linux

### Setup address

- By command: `$ ip -6 addr add aaaa::1/64 dev eth0`

- By static config (global file config) `/ect/network/interfaces`

  ```
  iface eth0 inet6 static 
  pre-up modprobe ipv6 
  address aaaa::10 
  netmask 64 
  gateway aaaa::1 
  ```

### Setup router

- show router tabel: `$ ip -6 route show`
- set router link: `$ ip -6 route add bbbb::/64 via aaaa::`

#### Note

Enable forwarding by command `$ sudo sysctl -w net.ipv6.conf.all.forwarding=1`
