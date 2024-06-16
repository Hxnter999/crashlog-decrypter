### War thunder logs a lot of encrypted information, it does so by creating a file with a `.clog` extention, for the current instance of the game under the `.game_logs` directory inside the installation folder. We can decrypt these logs really easily:
## Installation

Clone the repository then just run cmake

```bash
git clone https://github.com/Hxnter999/crashlog-decrypter.git
cd crashlog-decrypter
cmake -B build -S .
```
open the solution inside the build folder and just compile it. 
## Usage
Provide an input file being the encrypted crash logs and it will decrypt and save it under the same directly with a different extention `.txt`

```bash
<crashlog-decrypter> <input_file>
```


## How to update
If you're interested on how to get the key yourself, heres how I did it:

Their custom engine `DagorEngine` has optional an crashlog encryption, heres a reference to a relevant snippet of their debugging interface [debug.cpp](https://github.com/GaijinEntertainment/DagorEngine/blob/8b340d10814f06aa364e75769d406ec7373c5574/prog/engine/kernel/debug.cpp#L551), the code looks a little bit like this

```C
if (last_char_ptr && 
    debug_fname && 
    last_char_ptr > debug_fname + 1 && 
    strcmp(last_char_ptr - 1, "/#") == 0) 
{
  crypt_debug_setup(get_dagor_log_crypt_key(), (60 << 20)); // <-- This is important
  crypted_logs = cryptKey != NULL;
  last_char_ptr--;
}
```

The highlighted line is important but it also gets inlined since it does such small operations, its responsible for copying a provided key get_dagor_log_crypt_key() into a secondary buffer.

This whole function gets called within the winmain which is where it also acts upon some command line arguments one of them being `copy_log_to_stdout` which comes right after the function call, we can leverage this by searching for that string and checking which function gets called before it.

Heres how the [original code](https://github.com/GaijinEntertainment/DagorEngine/blob/8b340d10814f06aa364e75769d406ec7373c5574/prog/dagorInclude/startup/dag_winMain.inc.cpp#L104) looks:

```C
static int dagor_program_exec(int nCmdShow, int debugmode)
{
  default_crt_init_kernel_lib();

#if defined(__DEBUG_FILEPATH)
  start_classic_debug_system(__DEBUG_FILEPATH); // <-- Important

  if (dgs_get_argv("copy_log_to_stdout"))
    ...
}
```

Once ida is open, like mentioned before just search for the specific command and you should find something like the following:

```C
LABEL_71:
sub_19F5220(dword_4C02320, 1, 0, 0, 0); // <-- this is start_classic_debug_system()
*&Filename[0].wYear = 1;
if ( sub_19F4150("copy_log_to_stdout", Filename, 0i64) )
```

Just like we saw before, a simple copying of the key to a secondary buffer right after a string comparison:
```C
sub_19F5220:
...

if ( v9 >= 3 && (v12 = &v8[v9 - 2], !strcmp(v12, "/#")) )
{
  key_copy_end = key_end;
  xmmword_4CB3600 = xmmword_45E76A0;
  xmmword_4CB35F0 = xmmword_45E7690;
  xmmword_4CB35E0 = xmmword_45E7680;
  xmmword_4CB35D0 = xmmword_45E7670;
  xmmword_4CB35C0 = xmmword_45E7660;
  xmmword_4CB35B0 = xmmword_45E7650;
  key_copy_beg = key_beg; // <-- Encryption key at key_beg
  ...
}
```

**Here are some relevant signatures aswell:**
- `7C ? 48 8D 5F`
- `E8 ? ? ? ? 48 85 C0 74 ? C7 84 24`
- `0F 84 ? ? ? ? 44 89 74 24 ? 4C 8D BC 24`