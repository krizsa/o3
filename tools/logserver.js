var sys = require('sys');
var dgram = require('dgram');

var server = dgram.createSocket("udp4");

server.on("message", function (msg, rinfo) {
  sys.puts(msg+"\n");
});

server.on("listening", function () {
  sys.puts("Log server listening " + server.address().address);
})

server.bind(3333);