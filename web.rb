require 'sinatra'
require 'slim'

set :bind, '0.0.0.0'

get '/' do
  slim :index
end

get '/esp/update' do
  request.logger.info request.env
  if (request.env["HTTP_X_ESP8266_STA_MAC"]=="5C:CF:7F:86:2D:09" &&
    request.env["HTTP_X_ESP8266_VERSION"]=="0.0.1")
      headers({ 'x-MD5' => Digest::MD5.hexdigest(File.read("ota_0_0_2.ino.bin"))})
      send_file 'ota_0_0_2.ino.bin'
  else
    status 304
  end
end

__END__

@@layout
doctype html
html
  head
    meta charset="utf-8"
    title Just Do It
    link rel="stylesheet" media="screen, projection" href="/styles.css"
    /[if lt IE 9]
      script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"
  body
    h1 Just Do It
    == yield

@@index
h2 My Tasks
ul.tasks
  li Get Milk
