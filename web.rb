require 'sinatra'

set :bind, '0.0.0.0'

get '/esp/update' do
  request.logger.info request.env
  filename = 'assets/firmware.bin'
  md5 = Digest::MD5.hexdigest(File.read(filename))
  unless params["md5"]==md5
      headers({ 'x-MD5' => md5})
      send_file filename
  else
    status 304
  end
end
