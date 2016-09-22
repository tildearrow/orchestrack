# langs

langs is a very simple C++ library that provides language support via text files.

## Usage

`string* lsLoad(const char* dir, const char* lang)`: load all strings. returns a string array on success, and NULL on failure.

## Language file format

tab-separated values (.tsv). first column is string ID, and second one is value.
