# Twilio Massive SDK
Twilio Massive SDK is a library that enables you to connect your narrowband-powered devices to Twilio and your own back ends.

The SDK is designed to be cross-platform, so certain integration work is needed to make it work on your device. If you want to get started right away, you can use one of our example ports:

  * Arduino library (**TODO: LINK**) for [Twilio Narrowband Alfa developer kit](https://www.twilio.com/docs/wireless/quickstart/alfa-developer-kit). Connects over DTLS-protected CoAP to Twilio Commands back end.
  * Sample Zephyr application (**TODO: LINK**) for [FRDM-F64X](https://www.nxp.com/support/developer-resources/evaluation-and-development-boards/freedom-development-boards/mcu-boards/freedom-development-platform-for-kinetis-k64-k63-and-k24-mcus:FRDM-K64F) with [Dragino NB-IoT Shield](https://wiki.dragino.com/index.php?title=NB-IoT_Shield). Connects over TLS-protected MQTT to a MQTT broker.

The library consist of the following parts arranged in respective directories under `src`:

 * modem - the core of the library. Two classes in this part, `OwlModemRN4` and `OwlModemBG96` are interfacing U-Blox SARA-R4/SARA-N4 and Quectel BG96 modems respectively, containing other `OwlModem*` instances as their members.
 * CoAP - is a CoAP client using `OwlModemRN4` modem.
 * DTLS - is a small wrapper for tinydtls.
 * utils - contains helper functions for the rest of the library.
 * platform - headers for functions to be implemented in ports to different platforms. Porting will be explained shortly.

## Porting the SDK

`src/platform` directory contains headers with declaration of funtions to implement in your port. These include:

  * log.h - logging functions
  * power.h - powering on and off certain modules
  * time.h - measuring time and sleeping
  * mem.h - allocating and freeing memory
  * random.h - generating random numbers

Depending on what your target application is you might not need to implement all of these functions, let linker errors guide you.

You will also want to implement at least one subclass of `IOwlSerial` to enable `OwlModem*` classes talk to your modem.
