# Sampler

## Technical information

This document contains information about Sampler's state format, and its sampling routine.

### State format

#### Header (smph)

 type | value
------|--------------
 int  | format version
 char | git commit hash (as char[20])
 int  | total number of samples
 int  | total number of envelopes
 int  | total number of LFOs, future use

#### Sample (smps)

 type  | value
-------|--------------
 int   | sample number
 char  | sample name, as C string
 float | sample rate
 void  | 4-byte padding, since I might use double in a future
 char  | minimum note
 char  | maximum note
 char  | minimum velocity
 char  | maximum velocity
 char  | minimum param1
 char  | maximum param1
 char  | minimum param2
 char  | maximum param2
 void  | 8-byte padding for future use
 int   | volume envelope
 float | volume envelope amount
 float | volume envelope handicap
 int   | reserved for future use
 int   | panning envelope
 float | panning envelope amount
 float | panning envelope handicap
 int   | reserved for future use
 int   | pitch envelope
 float | pitch envelope amount
 float | pitch envelope handicap
 int   | reserved for future use
 int   | cutoff envelope
 float | cutoff envelope amount
 float | cutoff envelope handicap
 int   | reserved for future use
 int   | resonance envelope
 float | resonance envelope amount
 float | resonance envelope handicap
 int   | reserved for future use
 int   | filter mode
 int   | loop start
 int   | loop end
 int   | loop type
 int   | sample length
 int   | sample channels
 int   | flags, see below
 uchar | sample data, as unsigned char[length*channels], or sample path, as C
       | string, if remote flag is on

##### Sample flags

```
bit    1f-1    0
    +------------+
    | unused |rem|
    +------------+
```

- rem: remote mode. If this is true, sample will be loaded from a file.

#### Envelope (smpe)

 type | value
------|--------------
 int  | envelope number
 char | envelope name, as C string
 int  | start
 int  | sustain start
 int  | sustain end
 int  | loop start
 int  | loop end
 int  | envelope precision (in Hz), default 65536
 int  | flags, see below
 int  | point count
 envp | envelope points, as envp[count], see below

##### Envelope flags

```
bit    1f-4    3   2   1   0
    +------------------------+
    | unused |relMode|per|syn|
    +------------------------+
```

- relMode: release mode. Possible values:

  - `0`: wait (when released, do nothing)
  
  - `1`: jump (when released, jump to sustain loop end)
  
  - `2`: magic (when released, jump to sustain loop end, while scaling to match current value)
  
  - `3`: reserved

- per: persistent, envelope keeps running

- syn: sync to tempo

##### Envelope point

 type  | value
-------|--------------
 int   | time
 float | value
 int   | type, future use
 float | value2, future use

### Audio routine

to be added.
