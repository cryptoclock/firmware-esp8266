# analyze stack trace (exception) for ESP, using ESP stack trace decoder

import subprocess
import os

from pathlib import Path
home_dir = str(Path.home())
xtensa_dir = home_dir + "/" + ".platformio/packages/toolchain-xtensa/bin/"
cwd = "tools/"

build_dir = ".pio/build/3DA0100/"

# https://github.com/littleyoda/EspStackTraceDecoder
decoder = cwd + "EspStackTraceDecoder.jar"
firmware_elf = build_dir + "firmware.elf"
trace_txt = cwd + "trace.txt"
addr2line = xtensa_dir + "xtensa-lx106-elf-addr2line"

subprocess.call(['java', '-jar', decoder, addr2line, firmware_elf, trace_txt ])
