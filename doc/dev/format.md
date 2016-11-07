```C
typedef unsigned char uchar;
```

# orchestrack file format

orchestrack uses an IFF-like format for storing files.

All values are little-endian.

 Type | Value
------|-----------
 char | 'O'
 char | 'T'
 char | 'R'
 char | 'K'
 int  | total file length
 char | file type, as char[4]
 chunk| subchunks, see below

## Subchunk format

Same as IFF, but little-endian:
 
Type   | Value
-------|-----------
 char  | type, as char[4]
 int   | length
 uchar | data, unsigned char[length]

## File types

The following file types are used by orchestrack:

- `PROJ`: project

- `STAT`: plugin state

See `project/format.md` or `../instr/dev/format.md` for documentation on these types.
