///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <http://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#include "signaler.h"

#include "scy/av/codec.h"
#include "scy/av/format.h"
#include "scy/filesystem.h"
#include "scy/util.h"

#include <iostream>
#include <string>


#define OUTPUT_FILENAME "webrtcrecorder.mp4"
#define OUTPUT_FORMAT                                                                      \
    av::Format("MP4", "mp4",                                                               \
               av::VideoCodec("H.264", "libx264", 400, 300, 25, 48000, 128000, "yuv420p"), \
               av::AudioCodec("AAC", "libfdk_aac", 2, 44100, 64000, "s16"));


using std::endl;


namespace scy {


Signaler::Signaler(const smpl::Client::Options& options)
    : _client(options)
{
    _client.StateChange += slot(this, &Signaler::onClientStateChange);
    _client.roster().ItemAdded += slot(this, &Signaler::onPeerConnected);
    _client.roster().ItemRemoved += slot(this, &Signaler::onPeerDiconnected);
    _client += packetSlot(this, &Signaler::onPeerCommand);
    _client += packetSlot(this, &Signaler::onPeerEvent);
    _client += packetSlot(this, &Signaler::onPeerMessage);
    _client.connect();
}


Signaler::~Signaler()
{
}


void Signaler::sendSDP(PeerConnection* conn, const std::string& type,
                       const std::string& sdp)
{
    assert(type == "offer" || type == "answer");
    smpl::Event e;
    e.setName("ice:sdp");
    auto& desc = e["sdp"];
    desc[kSessionDescriptionTypeName] = type;
    desc[kSessionDescriptionSdpName] = sdp;

    postMessage(e);
}


void Signaler::sendCandidate(PeerConnection* conn, const std::string& mid,
                             int mlineindex, const std::string& sdp)
{
    smpl::Event e;
    e.setName("ice:candidate");
    auto& desc = e["candidate"];
    desc[kCandidateSdpMidName] = mid;
    desc[kCandidateSdpMlineIndexName] = mlineindex;
    desc[kCandidateSdpName] = sdp;

    postMessage(e);
}


void Signaler::onPeerConnected(smpl::Peer& peer)
{
    if (peer.id() == _client.ourID())
        return;
    DebugL << "Peer connected: " << peer.id() << endl;

    if (PeerConnectionManager::exists(peer.id())) {
        DebugL << "Peer already has a session: " << peer.id() << endl;
    }
}


void Signaler::onPeerCommand(smpl::Command& c)
{
    DebugL << "Peer command: " << c.from().toString() << endl;

    // List available files for streaming
    if (c.node() == "streaming:files") {
        json::Value files;
        StringVec nodes;
        fs::readdir(getDataDirectory(), nodes);
        for (auto node : nodes) {
            files.append(node);
        }

        c.setData("files", files);
        c.setStatus(200);
        _client.respond(c);
    }

    // Start a streaming session
    else if (c.node() == "streaming:start") {
        std::string file = c.data("file").asString();
        std::string filePath(getDataDirectory());
        fs::addnode(filePath, file);
        auto conn = new StreamingPeerConnection(this, c.from().id, c.id(), filePath);
        auto stream = conn->createMediaStream();
        conn->createConnection();
        conn->createOffer();
        PeerConnectionManager::add(c.id(), conn);

        c.setStatus(200);
        _client.respond(c);
        // _client.persistence().add(c.id(), reinterpret_cast<smpl::Message *>(c.clone()), 0);
    }

    // Start a recording session
    else if (c.node() == "recording:start") {

        av::EncoderOptions options;
        options.ofile = OUTPUT_FILENAME;
        options.oformat = OUTPUT_FORMAT;

        auto conn = new RecordingPeerConnection(this, c.from().id, c.id(), options);
        conn->constraints().SetMandatoryReceiveVideo(true);
        conn->constraints().SetMandatoryReceiveAudio(true);
        conn->createConnection();
        PeerConnectionManager::add(c.id(), conn);

        c.setStatus(200);
        _client.respond(c);
        // _client.persistence().add(c.id(), reinterpret_cast<smpl::Message *>(c.clone()), 0);
    }
}


void Signaler::onPeerEvent(smpl::Event& e)
{
    DebugL << "Peer event: " << e.from().toString() << endl;

    if (e.name() == "ice:sdp") {
        recvSDP(e.from().id, e["sdp"]);
    }
    else if (e.name() == "ice:candidate") {
        recvCandidate(e.from().id, e["candidate"]);
    }
}


void Signaler::onPeerMessage(smpl::Message& m)
{
    DebugL << "Peer message: " << m.from().toString() << endl;
}


void Signaler::onPeerDiconnected(const smpl::Peer& peer)
{
    DebugL << "Peer disconnected" << endl;

    // TODO: Loop all and close for peer

    // auto conn = get(peer.id());
    // if (conn) {
    //     DebugL << "Closing peer connection: " << peer.id() << endl;
    //     conn->closeConnection(); // will be deleted via callback
    // }
}


void Signaler::onClientStateChange(void* sender, sockio::ClientState& state,
                                   const sockio::ClientState& oldState)
{
    DebugL << "Client state changed from " << oldState << " to " << state << endl;

    switch (state.id()) {
        case sockio::ClientState::Connecting:
            break;
        case sockio::ClientState::Connected:
            break;
        case sockio::ClientState::Online:
            break;
        case sockio::ClientState::Error:
            throw std::runtime_error("Cannot connect to Symple server. "
                                     "Did you start the demo app and the "
                                     "Symple server is running on port 4551?");
    }
}


void Signaler::onAddRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream)
{
}


void Signaler::onRemoveRemoteStream(PeerConnection* conn, webrtc::MediaStreamInterface* stream)
{
    assert(0 && "free streams");
}


void Signaler::onStable(PeerConnection* conn)
{
}


void Signaler::onClosed(PeerConnection* conn)
{
    // _recorder.reset(); // shutdown the recorder
    PeerConnectionManager::onClosed(conn);
}


void Signaler::onFailure(PeerConnection* conn, const std::string& error)
{
    // _recorder.reset(); // shutdown the recorder
    PeerConnectionManager::onFailure(conn, error);
}


void Signaler::postMessage(const smpl::Message& m)
{
    _ipc.push(new ipc::Action(
        std::bind(&Signaler::syncMessage, this, std::placeholders::_1),
        m.clone()));
}


void Signaler::syncMessage(const ipc::Action& action)
{
    auto m = reinterpret_cast<smpl::Message*>(action.arg);
    _client.send(*m);
    delete m;
}


std::string Signaler::getDataDirectory() const
{
    // TODO: Make configurable
    std::string dir(getCwd());
    fs::addnode(dir, "data");
    return dir;
}


} // namespace scy
