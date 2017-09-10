# TinyDTLS example. 

This is using the original TinyDTLS 0.8.2 (Sourceforge) examples. Yet, the 
Makefile has been modified and a new project-conf.h generated.

The 6BR is configured for connecting the exteriro to Cooja. 
So, should be possible to use an extern TinyDTLS client (like Linux) to connect
to the server.

The example are very simple: The server will display the data sent by the 
client.

## Howto

From the ../rpl-border-router :

	make connect-router-cooja

From firefox (Optional): 

   http://[aaaa::212:7401:1:101]:80/ (6BR Root)


Is possible to load two forks of TinyDTLs with the Makefile in this directory.
One is mine,  which has been tested with RIOT-OS. 
The other is from cetic/6lbr which have been using TinyDTLS with success.


### Normal configuration for TinyDTLS 

TinyDTLS need to be "configured" for Contiki, this will generated the special Makefiles and preprare #define WITH_CONTIKI. 

    ./configure --with-contiki --without-ecc
    ./configure --with-contiki
    ./configure --with-contiki --without-psk --without-debug


  __NOTE__: Seems that TLS_NULL_WITH_NULL_NULL is not loaded in the client nor the server, yet is in the code.
  The client is only able to load PSK and/or ECC and the server only identify those two.

  __NOTE__: You need to run first 'autoreconf'.
  
## Current situation 

The code is the most clean possible. 
However, with my current Contiki environment, there are too many retransmission by each packet.
This happens too with the ICMPv6.  I have been unable to identify the source of this behavior.

Additionally, the TinyDTLS 0.8.2 is the only one working with this model and only with PSK.
The issues are with the MEMB functions invoked by TinyDTLs and not the programs here.

Therefore, because all this problem I'm leaving this project alone and focus on RIOT only.