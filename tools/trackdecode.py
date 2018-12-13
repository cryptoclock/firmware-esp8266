# analyze stack trace (exception) for ESP, using ESP stack trace decoder

import subprocess
import os

print os.getcwd()

xtensa_dir = ".platformio/packages/toolchain-xtensa/bin/"
cwd = "tools/"

# https://github.com/littleyoda/EspStackTraceDecoder
decoder = cwd + "EspStackTraceDecoder.jar"
firmware_elf = cwd + "firmware.elf"
trace_txt = cwd + "trace.txt"
addr2line = xtensa_dir + "xtensa-lx106-elf-addr2line"

subprocess.call(['java', '-jar', decoder, addr2line, firmware_elf, trace_txt ])
