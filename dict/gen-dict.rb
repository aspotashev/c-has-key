#!/usr/bin/ruby

File.open("generated-dict.txt", "w") do |f|
    6e6.to_i.times do
        f.puts rand
    end
end
