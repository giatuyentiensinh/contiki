6LoWPAN (HC06 Compression) with UDP and CoAP
===================================================

General examples
-------------

This is a general testbed with CoAP and the intended hardware configuration for
CoAPS (see the project_conf and Makefile). The clients and server here are 
identical to those of the 
../../er-example.

In the simulation you have:
- 1 6LoWPAN Border Router (6BR)
- 2 clients CoAP based on the "Erbium Example Client"
- 3 clients CoAP based on the observer client
- 1 server CoAP. 

PRELIMINARIES
-------------

TODO
- Get the Copper (Cu) CoAP user-agent from
  
[https://addons.mozilla.org/en-US/firefox/addon/copper-270430](https://addons.mozilla.org/en-US/firefox/addon/copper-270430)
- Optional: Save your target as default target
      make TARGET=sky savetarget

COOJA HOWTO
-----------

TODO (though ../../er-example/README.md is a good start.)

  http://[aaaa::212:7401:1:101]:80/ (6BR Root)
  coap://[aaaa::212:7402:2:202]:5683/ (Un cliente)

  **NOTE**: The 6BR do not has CoAP implemented, thus HTTP.

  make connect-router-cooja

  **__WARNING__**: The client will try to locate servers using multicast.
  If this fail, will be reflected by a big amount of NDP NS without their
  counterpart (NDP NA). However, both motes will answer to Firefox with Cu.