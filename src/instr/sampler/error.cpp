#include "sampler.h"

string Sampler::friendlyErr0(int e) {
  switch (e) {
    case 0: return "The operation was a success!";
    case 1: return "You're not able to do that.";
    case 2: return "That file/directory doesn't exist.";
    case 5: return "There was a I/O error!";
    case 6: return "That device doesn't exist.";
    case 7: return "You specified too many arguments.";
    case 12: return "Can't allocate memory!";
    case 13: return "You don't have permission.";
    case 16: return "That device is busy.";
    case 17: return "That file already exists.";
    case 19: return "That device doesn't exist.";
    case 20: return "That is not a directory.";
    case 21: return "That is a directory.";
    case 22: return "You have provided an invalid argument.";
    case 23: return "Too many files are open!";
    case 24: return "You have opened too many files!";
    case 27: return "That file is too large.";
    case 28: return "You don't have more device space!";
    case 30: return "That file system is read-only.";
    case 36: return "That file name is too long.";
    case 95: return "You can't do that.";
    case 121: return "There was a remote I/O error!";
    case 122: return "You have exceeded your disk quota!";
    case 256: return "That format can't be recognized.";
    case 257: return "That file is foul.";
    case 258: return "That file has an unsupported encoding.";
    default: return "An error has occured:";
  }
}

string Sampler::friendlyErr1(int e) {
  switch (e) {
    case 0: return "This couldn't happen though...";
    case 1: return "Try contacting your system administrator";
    case 2: return "Maybe you made a typo, or that file truly";
    case 5: return "Your device might be corrupted...";
    case 6: return "Specify a different device, and";
    case 7: return "Specify less, and try";
    case 12: return "Try closing some apps.";
    case 13: return "Try contacting your system administrator";
    case 16: return "Wait a little, then";
    case 17: return "Please use a different file";
    case 19: return "Try with a different";
    case 20: return "Specify a directory, then";
    case 21: return "Specify a non-directory, then";
    case 22: return "Maybe you made a typo.";
    case 23: return "Increase the system limits, then";
    case 24: return "Increase your limits, then";
    case 27: return "Try with a smaller";
    case 28: return "Make up some space, then";
    case 30: return "Remount read-write if possible, then";
    case 36: return "Shorten it please.";
    case 95: return "Try doing something else.";
    case 121: return "Contact that host's administrator.";
    case 122: return "Try contacting your system administrator";
    case 256: return "If it's a sound file, convert that file to";
    case 257: return "I don't think I can do anything";
    case 258: return "Convert that file to a different format,";
    default: return strerror(e);
  }
}

string Sampler::friendlyErr2(int e) {
  switch (e) {
    case 0: return "Report a bug at https://github.com/tildearrow/orchestrack";
    case 1: return "(if you are him/her, then I'm sorry).";
    case 2: return "doesn't exist.";
    case 5: return "Replace it as soon as possible!";
    case 6: return "try again.";
    case 7: return "again.";
    case 12: return "Then try again.";
    case 13: return "(if you are him/her, give yourself permission to that file).";
    case 16: return "try again.";
    case 17: return "or delete it.";
    case 19: return "device.";
    case 20: return "try again.";
    case 21: return "try again.";
    case 22: return "";
    case 23: return "try again.";
    case 24: return "try again.";
    case 27: return "file.";
    case 28: return "try again.";
    case 30: return "try again.";
    case 36: return "";
    case 95: return "";
    case 121: return "";
    case 122: return "(if you are him/her, increase your limits).";
    case 256: return "a supported format first.";
    case 257: return "to help you...";
    case 258: return "then try again.";
    default: return "";
  }
}
