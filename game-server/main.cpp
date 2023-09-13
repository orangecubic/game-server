#include "net/Server.h"
#include "PacketProcess.h"
#include "MatchMaker.h"
#include "gflags/gflags.h"
#include "photon/photon.h"
#include "photon/thread/thread.h"

// main function
int main() {

    // photon::init(photon::INIT_EVENT_IOURING);
    photon::init(photon::INIT_EVENT_EPOLL);

    // start timestamp updater, it used for photon sleep
    photon::timestamp_updater_init();

    // photon::WorkPool workPool(8, photon::INIT_EVENT_IOURING, photon::INIT_IO_DEFAULT, 0);
    photon::WorkPool workPool(8, photon::INIT_EVENT_EPOLL, photon::INIT_IO_DEFAULT, 0);

    MatchMaker* matchMaker = new MatchMaker(&workPool);

    PacketProcess* packetprocessor = new PacketProcess(matchMaker);

    Server* server = new Server(&workPool, packetprocessor);

    if (server->init("0.0.0.0", 9988) == -1) {
        return -1;
    }

    matchMaker->start();

    server->start(true);
}