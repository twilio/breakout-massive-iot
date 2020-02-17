# Twilio Breakout SDK for Massive IoT
Twilio Massive SDK is a library that enables you to connect your cellular-connected devices to Twilio and your own back end services.

The SDK is designed to be cross-platform, so certain integration work is needed to make it work on your device. If you want to get started right away, you can use our example port:

  * [Twilio Breakout SDK for Massive IoT - with Arduino support](https://github.com/twilio/Breakout_Massive_SDK_Arduino) for [Twilio Narrowband Alfa developer kit](https://www.twilio.com/docs/wireless/quickstart/alfa-developer-kit). Demonstrates communicating to an MQTT broker over TLS with Certificates or Username/Password.

The library consist of the following parts arranged in respective directories under `src`:

 * modem - the core of the library. Two classes in this part, `OwlModemRN4` and `OwlModemBG96` are interfacing U-Blox SARA-R4/SARA-N4 and Quectel BG96 modems respectively, containing other `OwlModem*` instances as their members.
 * utils - contains helper functions for the rest of the library.
 * platform - headers for functions to be implemented in ports to different platforms. Porting will be explained shortly.
 * shims - example IP shims for third party libraries and platforms.

## Porting the SDK

`src/platform` directory contains headers with declaration of functions to implement in your port. These include:

  * log.h - logging functions
  * power.h - powering on and off certain modules
  * time.h - measuring time and sleeping
  * mem.h - allocating and freeing memory

Depending on what your target application is you might not need to implement all of these functions, let linker errors guide you.

You will also want to implement at least one subclass of `IOwlSerial` to enable `OwlModem*` classes talk to your modem.
