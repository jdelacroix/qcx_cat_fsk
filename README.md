# Frequency Key Shifting via CAT on the QCX transceiver

This software is inspired by [qcx_f](https://github.com/olgierd/qcx_f), which showed that it is possible to transmit FT8 using the [QCX 5W CW transceiver](https://www.qrp-labs.com/qcx.html) with a some creative modifications. It differs from _qcx_f_ in one important aspect: **`qcx_cat_fsk` will work with the QCX's original firmware (1.03+) by using the CAT control interface over serial.** Consequently, one only needs to add the pin header for the serial port (underneath the LCD) on the QCX and a USB-to-serial converter to connect it to a computer. The QCX's earphones audio port is also connected to the computer for receiving signals.

```
qcx_cat_fsk/audio_decoder.h: Listens to the audio output from WSJT-X, determines the tone frequency, and accordingly sets VFOB and turns the QCX transmitter on (and off whenever no tones are played.)
qcx_cat_fsk/cat_interface.h: Implements the Kenwood TS-480-like CAT commands to control the QCX.
```

This software is currently a proof-of-concept using hard-coded values for FT8 on 40m at 7.074MHz. Audio decoding from WSJT-X and frequency shifting via CAT control tested. Despite its narrow CW filter (200Hz) receiving FT8 across portions of the band is also achievable in practice. Confirmed functionality with several QSOs. Next steps are to provide some configurability (e.g., other bands, other FSK-type digital modes), as well as, provide much needed documentation.
