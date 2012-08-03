#include <algorithm>
#include <gtest/gtest.h>
#include "ethernetII.h"
#include "utils.h"

using namespace Tins;

class EthernetIITest : public ::testing::Test {
public:
    static const uint8_t expected_packet[];
    static const uint8_t s_addr[];
    static const uint8_t d_addr[];
    static const uint8_t empty_addr[6];
    static const uint16_t p_type;
    static const uint32_t iface;

    void test_equals(const EthernetII &eth1, const EthernetII &eth2);
};

const uint8_t EthernetIITest::expected_packet[] = {
'\xaa', '\xbb', '\xcc', '\xdd',
'\xee', '\xff', '\x8a', '\x8b',
'\x8c', '\x8d', '\x8e', '\x8f',
'\xd0', '\xab'
};

const uint8_t EthernetIITest::s_addr[] = {
'\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f'
};

const uint8_t EthernetIITest::d_addr[] = {
'\xaa', '\xbb', '\xcc', '\xdd', '\xee', '\xff'
};

const uint8_t EthernetIITest::empty_addr[6] = {0};

const uint16_t EthernetIITest::p_type = 0xd0ab;

const uint32_t EthernetIITest::iface = 0x12345678;

void EthernetIITest::test_equals(const EthernetII &eth1, const EthernetII &eth2) {
    EXPECT_TRUE(std::equal(eth1.dst_addr(), eth1.dst_addr() + EthernetII::ADDR_SIZE, eth2.dst_addr()));
    EXPECT_TRUE(std::equal(eth1.src_addr(), eth1.src_addr() + EthernetII::ADDR_SIZE, eth2.src_addr()));
    EXPECT_EQ(eth1.payload_type(), eth2.payload_type());
    EXPECT_EQ(eth1.iface(), eth2.iface());
    EXPECT_EQ((bool)eth1.inner_pdu(), (bool)eth2.inner_pdu());
}

TEST_F(EthernetIITest, DefaultConstructor) {
    EthernetII eth(0);
    EXPECT_EQ(eth.iface(), 0);
    EXPECT_TRUE(memcmp(eth.dst_addr(), empty_addr, 6) == 0);
    EXPECT_TRUE(memcmp(eth.src_addr(), empty_addr, 6) == 0);
    EXPECT_EQ(eth.payload_type(), 0);
    EXPECT_FALSE(eth.inner_pdu());
    EXPECT_EQ(eth.pdu_type(), PDU::ETHERNET_II);
}

TEST_F(EthernetIITest, CopyConstructor) {
    EthernetII eth1(expected_packet, sizeof(expected_packet));
    eth1.iface(0);
    EthernetII eth2(eth1);
    test_equals(eth1, eth2);
}

TEST_F(EthernetIITest, CopyAssignmentOperator) {
    EthernetII eth1(expected_packet, sizeof(expected_packet));
    eth1.iface(0);
    EthernetII eth2(0);
    eth2 = eth1;
    test_equals(eth1, eth2);
}

TEST_F(EthernetIITest, NestedCopy) {
    EthernetII *nested = new EthernetII(expected_packet, sizeof(expected_packet));
    nested->iface(0);
    EthernetII eth1(expected_packet, sizeof(expected_packet));
    eth1.iface(0);
    eth1.inner_pdu(nested);
    EthernetII eth2(eth1);
    test_equals(eth1, eth2);
}

TEST_F(EthernetIITest, SourceAddress) {
    EthernetII eth(0);
    eth.src_addr(s_addr);
    ASSERT_TRUE(memcmp(eth.src_addr(), s_addr, 6) == 0);
}

TEST_F(EthernetIITest, DestinationAddress) {
    EthernetII eth(0);
    eth.dst_addr(d_addr);
    ASSERT_TRUE(memcmp(eth.dst_addr(), d_addr, 6) == 0);
}

TEST_F(EthernetIITest, PayloadType) {

    EthernetII eth(0);
    eth.payload_type(p_type);
    ASSERT_EQ(eth.payload_type(), p_type);
}

TEST_F(EthernetIITest, Interface) {

    EthernetII eth(0);
    eth.iface(iface);
    ASSERT_EQ(eth.iface(), iface);
}

TEST_F(EthernetIITest, CompleteConstructor) {
    EthernetII* eth2 = new EthernetII(0);
    EthernetII eth(iface, d_addr, s_addr, eth2);
    EXPECT_TRUE(memcmp(eth.dst_addr(), d_addr, 6) == 0);
    EXPECT_TRUE(memcmp(eth.src_addr(), s_addr, 6) == 0);
    EXPECT_TRUE(eth.inner_pdu() == eth2);
    EXPECT_EQ(eth.payload_type(), 0);
    EXPECT_EQ(eth.iface(), iface);
}

TEST_F(EthernetIITest, Serialize) {
    EthernetII eth(0, d_addr, s_addr);
    eth.payload_type(p_type);
    uint32_t sz;
    uint8_t *serialized = eth.serialize(sz);
    EXPECT_EQ(eth.size(), sz);
    EXPECT_TRUE(serialized);
    EXPECT_TRUE(memcmp(serialized, expected_packet, sz) == 0);
    delete[] serialized;
}

TEST_F(EthernetIITest, ConstructorFromBuffer) {
    EthernetII eth(expected_packet, sizeof(expected_packet));
    EXPECT_TRUE(memcmp(eth.src_addr(), s_addr, sizeof(s_addr)) == 0);
    EXPECT_TRUE(memcmp(eth.dst_addr(), d_addr, sizeof(d_addr)) == 0);
    EXPECT_EQ(eth.payload_type(), p_type);
}


