# Lesson 3: Create a CoAP server and MQTT protocol

## CoAP

The following examples will show how to create an embedded CoAP server.

CoAP - the Constrained Application Protocol - is a specialized web transfer protocol for use with constrained nodes and constrained networks in the Internet of Things.

For more information about CoAP visit the following page:

http://coap.technology


### Lesson objectives

The objective of the lessons are:

* Learn how to create a CoAP resource and build a CoAP server
* Show how to use the Copper client plugin to interact with our CoAP server
* Learn how to discover the resources our CoAP server exposes
* Learn how to use the `GET` method to retrieve sensor data from the CoAP server
* Subscribe to events (be notified each time the user button is pushed)
* Request sensor readings in different formatting (application/json, text/plain, etc.)
* Control the on-board LEDs remotely using `PUT/POST` requests

## MQTT

The following examples will show how to create a MQTT node able to publish and subscribe to a MQTT broker

For more information about MQTT visit the following page:

http://mqtt.org/

### Lesson objectives

The objective of the lessons are:

* Familiarize with the MQTT API
* Publish sensor data and node statistics to a MQTT broker
* Control the on-board LEDs remotely by publishing to the MQTT broker and receiving the command notification
* Implement a local MQTT broker (mosquitto)
