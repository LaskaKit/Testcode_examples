### For library GxEPD2 need to koment power setting, owervise picture can be shady
<code>
// same POWER SETTING as from OTP
_writeCommand(0x01); // POWER SETTING
_writeData (0x07); // enable internal
_writeData (0x07); // VGH=20V,VGL=-20V
_writeData (0x3f); // VDH=15V
_writeData (0x3f); // VDL=-15V
<code>