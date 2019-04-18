require 'rubygems'
require 'serialport'
require 'json'

device = "/dev/ttyUSB0" 
speed = 115200

sp = SerialPort.new(device, speed, 8,1, SerialPort::NONE)

$sendbuffer = []
$linebuffer = ""
def readline(io)
    line = nil
    ch = io.getc
    if ch
        $linebuffer += ch
        if ch=="\n"
            line = $linebuffer
            $linebuffer = nil
        end
    end
    return line
end

msg = {"type":"welcome","text":"Hello"}
$sendbuffer << msg.to_json

while true
    line = readline(sp)
    if (line)
        puts "<< #{line}"

    end
    while !$sendbuffer.empty?
        puts ">> #{$sendbuffer.first}"
        sp.puts($sendbuffer.shift)
    end
end
