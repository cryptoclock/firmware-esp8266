require 'sinatra'

set :bind, '0.0.0.0'

get '/esp/update' do
  request.logger.info request.env
#  if (request.env["HTTP_X_ESP8266_STA_MAC"]=="5C:CF:7F:86:2D:09" &&
  filename = 'platformio/assets/firmware.bin'
  md5 = Digest::MD5.hexdigest(File.read(filename))
  puts "ahoj #{md5}"
  unless params["md5"]==md5
      headers({ 'x-MD5' => md5})
      send_file filename
  else
    status 304
  end
end
