//
/// Setup the Symple server

var Symple = require('symple');
var sy = new Symple();
sy.loadConfig(__dirname + '/symple.json'); // see symple.json for options
sy.init();
console.log('Symple server listening on port ' + sy.config.port);


//
/// Setup the client web server

var express = require('express'),
  path = require('path'),
  redis = require('redis'),
  client = redis.createClient(),
  app = express(),
  serverPort = parseInt(sy.config.port)
  clientPort = serverPort - 1;

app.set('port', clientPort);
app.set('view engine', 'ejs');
app.set('views', __dirname + '/');
app.use(express.static(__dirname + '/assets'));
app.use(express.static(__dirname + '/node_modules/symple-client/src'));
app.use(express.static(__dirname + '/node_modules/symple-client-player/src'));

app.get('/', function (req, res) {
  // Create a random token to identify this client
  // NOTE: This method of generating unique tokens is not secure, so don't use
  // it in production ;)
  var token = '' + Math.random();

  // Create the arbitrary user session object here
  var session = {
    // user: 'demo',
    // name: 'Demo User',
    group: 'public'
  }

  // Store the user session on Redis
  // This will be sent to the Symple server to authenticate the session
  // client.set('symple:session:' + token, JSON.stringify(session), redis.print);

  // Render the response
  res.render('index', {
    port: serverPort,
    token: token,
    peer: session });
});

app.listen(app.get('port'), function () {
  console.log('Express server listening on port ' + app.get('port'));
});
