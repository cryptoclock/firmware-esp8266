# remote debugginh via GDB stub
import subprocess
import os
import sys

# At top of main.cpp, add #include <internal/gdbstub.h> to include GDB stub in firmware
# After that, rebuild, reupload and change settings bellow before running this script
port = "/dev/ttyUSB0"
model = "3DA0100"
xtensa_dir = "~/.platformio/packages/toolchain-xtensa/bin/"

firmware_binary = ".pioenvs/" + model + "/firmware.elf"
gdb_command = os.path.expanduser(xtensa_dir) + "xtensa-lx106-elf-gdb" 
port_command = "target remote " + port
file_command = "file " + firmware_binary

subprocess.call([gdb_command, "-x", sys.path[0] + "/gdbcommands", "-ex", file_command, "-ex", port_command])
