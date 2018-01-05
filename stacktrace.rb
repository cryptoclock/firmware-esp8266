#

decoder = "C:/Users/snoutmate/Desktop/Downloads/EspStackTraceDecoder.jar"
addr2line = "C:/Users/snoutmate/.platformio/packages/toolchain-xtensa/bin/xtensa-lx106-elf-addr2line.exe"
firmware = ".pioenvs/nodemcuv2/firmware.elf"
dump = "dump.txt"

tool = "java -jar #{decoder} #{addr2line} #{firmware} #{dump}"
#java -jar EspStackTraceDecoder.jar C:\Users\snoutmate\.platformio\packages\toolchain-xtensa\bin\xtensa-lx106-elf-addr2line.exe firmware.bin dump.txt

puts `#{tool}`
