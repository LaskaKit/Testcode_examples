###  For the GxEPD2 library, the power setting section should be commented out — otherwise the image may appear distorted or faded.
<code>
// same POWER SETTING as from OTP
_writeCommand(0x01); // POWER SETTING
_writeData (0x07); // enable internal
_writeData (0x07); // VGH=20V,VGL=-20V
_writeData (0x3f); // VDH=15V
_writeData (0x3f); // VDL=-15V
<code>