# State format

orchestrack plugins use a format when loading/saving states, which is documented here.

The file type is `STAT`.

## Header chunk (head)

This should go first.

 Type | Value
------|---------
 int  | format version (currently `0`)
 char | number of audio in ports
 char | number of audio out ports
 char | number of MIDI in ports
 char | number of MIDI out ports
 char | plugin name/identifier, as C string
 char | assigned name, as C string

After this chunk, plugin-specific chunks follow.
