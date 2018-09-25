// these are your protocol messages
struct HelloRequest {
    uint32_t seq_no;
    // more stuff
};
struct HelloResponse {
    uint32_t seq_no;
    // more stuff
};

// Session callback for received messages
struct SessionReceiver {
    virtual void connected(Session*) = 0;
    virtual void receive(Session* from, HelloRequest msg) = 0;
    virtual void receive(Session* from, HelloResponse msg) = 0;
    virtual void disconnected(Session*) = 0;
};

// Session interface to send messages
struct Session {
    virtual void send(HelloRequest msg) = 0;
    virtual void send(HelloResponse msg) = 0;
};

// this connects asynchronously and then calls SessionReceiver::connected() with a newly established session
struct SessionInitiator {
    virtual void connect(SessionReceiver* cb, std::string peer) = 0;
};

// this accepts connections asynchronously and then calls SessionReceiver::connected() with a newly accepted session
struct SessionAcceptor {
    virtual void listen(SessionReceiver* cb, std::string port) = 0;
};