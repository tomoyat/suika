#include <thread>
#include <chrono>
#include <gtest/gtest.h>

#include "Udp.h"
#include "UdpControlBlock.h"

TEST(UDP, OPEN) {
    for (int i = 0; i < 100; i++) {
        const auto sock = suika::protocol::udp::UdpSocket::open();
        EXPECT_TRUE(sock.has_value());
    }
    const auto sock = suika::protocol::udp::UdpSocket::open();
    EXPECT_FALSE(sock.has_value());
}

TEST(UDP, BIND) {
    const auto sock1 = suika::protocol::udp::UdpSocket::open();
    EXPECT_TRUE(sock1.has_value());
    auto s1 = sock1.value();
    const auto r1 = s1.bind("127.0.0.1", 8080);
    EXPECT_TRUE(r1.has_value());

    const auto sock2 = suika::protocol::udp::UdpSocket::open();
    EXPECT_TRUE(sock2.has_value());
    auto s2 = sock2.value();
    const auto r2 = s2.bind("127.0.0.1", 8081);
    EXPECT_TRUE(r2.has_value());

    const auto sock3 = suika::protocol::udp::UdpSocket::open();
    EXPECT_TRUE(sock3.has_value());
    auto s3 = sock3.value();
    const auto r3 = s3.bind("127.0.0.1", 8081);
    EXPECT_FALSE(r3.has_value());
}


TEST(UDP, RECV) {
    const auto sockWrap = suika::protocol::udp::UdpSocket::open();
    auto sock = sockWrap.value();
    auto result = sock.bind("127.0.0.1", 8080);


    auto pcbId = sock.pcbId;
    auto t1 = std::thread([&]() {
        const auto mgr = suika::protocol::udp::pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> mgrLck(mgr->mutex);
        auto &pcb = mgr->get(pcbId);
        mgrLck.unlock();

        std::lock_guard<std::mutex> lck(pcb.mutex);
        std::vector<std::uint8_t> vec{0, 1, 2, 3};
        suika::protocol::udp::pcb::UdpPcbPayload payload{vec, suika::protocol::ipv4::IpEndpoint{0, 0}};
        pcb.que.push(payload);
        pcb.cv.notify_one();
    });

    auto t2 = std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        const auto mgr = suika::protocol::udp::pcb::UdpPcbManager::getInstance();
        std::unique_lock<std::mutex> mgrLck(mgr->mutex);
        auto &pcb = mgr->get(pcbId);
        mgrLck.unlock();

        std::lock_guard<std::mutex> lck(pcb.mutex);
        std::vector<std::uint8_t> vec{3, 2, 1, 0};
        suika::protocol::udp::pcb::UdpPcbPayload payload{vec, suika::protocol::ipv4::IpEndpoint{0, 0}};
        pcb.que.push(payload);
        pcb.cv.notify_one();
    });
    std::vector<std::uint8_t> buf(4, 0);
    std::uint32_t ipv4;
    std::uint16_t port;

    sock.recvFrom(buf, ipv4, port);

    std::vector<std::uint8_t> expected1{0, 1, 2, 3};
    ASSERT_EQ(buf, expected1);
    ASSERT_EQ(ipv4, 0);
    ASSERT_EQ(port, 0);

    sock.recvFrom(buf, ipv4, port);

    std::vector<std::uint8_t> expected2{3, 2, 1, 0};
    ASSERT_EQ(buf, expected2);
    ASSERT_EQ(ipv4, 0);
    ASSERT_EQ(port, 0);

    t1.join();
    t2.join();
}
