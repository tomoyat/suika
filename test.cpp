#include <iostream>
#include <vector>
#include <cstdint>
#include <format>

using namespace std;

vector<uint16_t> bigEndian(const vector<uint8_t> &arr) {
    vector<uint16_t> ret{};
    for (int i = 0; i + 1 < arr.size(); i+=2) {
        uint16_t v = static_cast<uint16_t>(arr[i]) << 8 | static_cast<uint16_t>(arr[i + 1]);
        ret.push_back(v);
    }
    return ret;
}

vector<uint16_t> littleEndian(const vector<uint8_t> &arr) {
    vector<uint16_t> ret{};
    for (int i = 0; i + 1 < arr.size(); i+=2) {
        uint16_t v = static_cast<uint16_t>(arr[i + 1]) << 8 | static_cast<uint16_t>(arr[i]);
        ret.push_back(v);
    }
    return ret;
}

uint16_t two_complement_sum(const vector<uint16_t> &arr) {
    uint32_t ret = 0;
    for (auto v : arr) {
        ret = ret + v;
    }
    return static_cast<uint16_t>(0x0000FFFF & ret);
}

uint16_t one_complement_sum(const vector<uint16_t> &arr) {
    uint32_t ret = 0;
    for (auto v : arr) {
        ret = ret + v;
    }
    return static_cast<uint16_t>((0x0000FFFF & ret) + ((ret & 0xFFFF0000) >> 16));
}

int main() {
    vector<uint8_t> arr{0x1f, 0xe3, 0xd4, 0xa8, 0xf6, 0xf3};

    auto big = bigEndian(arr);
    auto little = littleEndian(arr);

    auto bigEndianOneComplementSum = one_complement_sum(big);
    auto littleEndianOneComplementSum = one_complement_sum(little);
    auto bigEndianTwoComplementSum = two_complement_sum(big);
    auto littleEndianTwoComplementSum = two_complement_sum(little);

    uint16_t checksumBigEndianOwoComplementSum = ~bigEndianOneComplementSum;
    uint16_t checksumLittleEndianOneComplementSum = ~littleEndianOneComplementSum;

    uint16_t checksumBigEndianTwoComplementSum = ~bigEndianTwoComplementSum;
    uint16_t checksumLittleEndianTwoComplementSum = ~littleEndianTwoComplementSum;

    cout << format("bigEndian one_complement : {:04x}, checksum = {:04x}", bigEndianOneComplementSum, checksumBigEndianOwoComplementSum) << endl;
    cout << format("littleEndian one_complement : {:04x}, checksum = {:04x}", littleEndianOneComplementSum, checksumLittleEndianOneComplementSum) << endl;

    cout << format("bigEndian two_complement : {:04x}", bigEndianTwoComplementSum) << endl;
    cout << format("littleEndian two_complement : {:04x}", littleEndianTwoComplementSum) << endl;

    vector<uint8_t> bigEndianOneComplementPayload = arr;
    bigEndianOneComplementPayload.push_back(static_cast<uint8_t>(checksumBigEndianOwoComplementSum >> 8));
    bigEndianOneComplementPayload.push_back(static_cast<uint8_t>(checksumBigEndianOwoComplementSum & 0x00FF));

    cout << format("bigEndian one_complement verify bigEndian: {:04x}", one_complement_sum(bigEndian(bigEndianOneComplementPayload))) << endl;
    cout << format("bigEndian one_complement verify littleEndian: {:04x}", one_complement_sum(littleEndian(bigEndianOneComplementPayload))) << endl;

    vector<uint8_t> littleEndianOneComplementPayload = arr;
    littleEndianOneComplementPayload.push_back(static_cast<uint8_t>(checksumLittleEndianOneComplementSum & 0x00FF));
    littleEndianOneComplementPayload.push_back(static_cast<uint8_t>(checksumLittleEndianOneComplementSum >> 8));

    cout << format("bigEndian one_complement verify bigEndian: {:04x}", one_complement_sum(bigEndian(littleEndianOneComplementPayload))) << endl;
    cout << format("bigEndian one_complement verify littleEndian: {:04x}", one_complement_sum(littleEndian(littleEndianOneComplementPayload))) << endl;


    return 0;
}
