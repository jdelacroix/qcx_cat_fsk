# Frequency Key Shifting via CAT on the QCX transceiver

## Introduction 
This software is inspired by [qcx_f](https://github.com/olgierd/qcx_f), which showed that it is possible to transmit FT8 using the [QCX 5W CW transceiver](https://www.qrp-labs.com/qcx.html) with a some creative modifications. It differs from _qcx_f_ in one important aspect: **`qcx_cat_fsk` will work with the QCX's original firmware (1.03+) by using the CAT control interface over serial.** Consequently, one only needs to add the pin header for the serial port (underneath the LCD) on the QCX and a USB-to-serial converter to connect it to a computer. The QCX's earphones audio port is also connected to the computer for receiving signals.

1. `qcx_cat_fsk/audio_decoder.h`: Listens to the audio output from WSJT-X, determines the tone frequency, and accordingly sets VFOB and turns the QCX transmitter on (and off whenever no tones are played.)
2. `qcx_cat_fsk/cat_interface.h`: Implements the Kenwood TS-480-like CAT commands to control the QCX.

This software is currently a proof-of-concept tested with hard-coded values for FT8 on 40m at 7.074MHz. Audio decoding from WSJT-X and frequency shifting via CAT control tested. Despite its narrow CW filter (200Hz) receiving FT8 across portions of the band is also achievable in practice. Confirmed functionality with several QSOs. Next steps are to provide some configurability (e.g., other bands, other FSK-type digital modes), as well as, provide much needed documentation.

## Usage

```
# qcx_cat_fsk --help
usage: qcx_cat_ft8 [--version] [--help] [-l <logfile>]
                   <device name> <frequency>

Controls a QRP Labs QCX CW radio via CAT to transmit a FSK signal decoded from
WSTJ-X output audio. Specify a device name (e.g., /dev/ttyACM0) and the FT8
frequency used in WSJT-X in MHz (e.g., 7.074).

Mandatory arguments to long options are mandatory for short options too.
  -h [ --help ]                      show help
  -v [ --version ]                   show version
  -O [ --offset ] <frequency> (=700) offset in CW frequency in Hz, by default 
                                     set to 700 Hz on the QCX
  -l [ --log ] <logfile>             write log to a specific file, otherwise by
                                     default the log will be written to 
                                     qcx_cat_ft8.log

Report bugs at <https://github.com/jdelacroix/qcx_cat_fsk/issues>.
```
