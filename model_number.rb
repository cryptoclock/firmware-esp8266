#!/usr/bin/env ruby
model = ENV["X_MODEL_NUMBER"] || "3DA0100"
if model
  puts "-DX_MODEL_#{model}"
  puts "-DX_MODEL_NUMBER=\\\"#{model}\\\""
end
ARGV.each {|arg| puts arg; }
