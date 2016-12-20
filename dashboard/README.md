# Symple WebRTC Video Chat Demo

The Symple video chat demo is an example of how to use Symple to build an instant messaging and WebRTC video chat application in about 100 lines of JavaScript. External projects used are AngularJS, Bootstrap, Node.js and Express.

See this blog post for more information about the demo: http://sourcey.com/symple-webrtc-video-chat-demo

## What is Symple?

Symple is a unrestrictive real time messaging and presence protocol that implements the minimum number of features required to build full fledged messaging applications with security, flexibility, performance and scalability in mind. These features include:

* Session sharing with any backend (via Redis)
* User rostering and presence
* Media streaming (via WebRTC, [see demo](http://symple.sourcey.com))
* Scoped messaging ie. direct, user and group scope
* Real-time commands and events
* Real-time forms

Symple currently has client implementations in [JavaScript](https://github.com/sourcey/symple-client), [Ruby](https://github.com/sourcey/symple-client-ruby) and [C++](https://github.com/sourcey/libsourcey/tree/master/src/symple), which make it ideal for a wide range of messaging requirements, such as building real-time games and applications that run in the web browser, desktop, and mobile phone.

## Usage

1. Install dependencies: `npm install`
2. Fire up the server: `node app`
3. And point your browser to: `http://localhost:4550`

## Hacking

Some key options are specified in the main HTML file located at `index.ejs`

**CLIENT_OPTIONS** This is the options hash for the Symple client. This is where you specify the server URL and Peer object. Note that we have disabled 'websocket' transport by default, but you will probably want to re-enable it in production.

**WEBRTC_CONFIG** This is the PeerConnection options hash. In production you will want to specify some TURN servers so as to ensure the p2p connection succeeds in all network topologies.

Other than that all relevant JavaScript is located in `assets/app.js` and `assets/helpers.js`. Enjoy!

## Contact

For more information please check out the Symple homepage: http://sourcey.com/symple/
If you have a bug or an issue then please use the Github issue tracker: https://github.com/sourcey/symple-webrtc-video-chat-demo/issues
