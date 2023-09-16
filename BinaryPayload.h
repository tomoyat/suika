#ifndef SUIKA_BINARYPAYLOAD_H
#define SUIKA_BINARYPAYLOAD_H

#include <vector>
#include <cstdint>

struct BinaryPayload {
    // 数値はビッグエンディアンで格納されている
    std::vector<std::uint8_t> data;
protected:
    std::uint8_t readUint8(int offset) {
        return data[offset];
    }

    std::uint16_t readUint16(int offset) {
        return static_cast<uint16_t>(data[offset]) << 8
               | static_cast<uint16_t>(data[offset + 1]);
    }

    std::uint32_t readUint32(int offset) {
        return
                static_cast<uint32_t>(data[offset]) << 24
                | static_cast<uint32_t>(data[offset + 1]) << 16
                | static_cast<uint16_t>(data[offset + 2]) << 8
                | static_cast<uint16_t>(data[offset + 3]);
    }

    std::vector<std::uint8_t> readVector(int offset, int len) {
        std::vector<std::uint8_t> result(len);
        std::copy(data.begin() + offset, data.begin() + offset + len, result.begin());
        return result;
    }

    void writeUint8(std::uint8_t value, int offset) {
        write(static_cast<std::uint8_t>(value), offset);
    }

    void writeUint16(std::uint16_t value, int offset) {
        write(static_cast<std::uint8_t>(value >> 8), offset);
        write(static_cast<std::uint8_t>(value), offset + 1);
    }

    void writeUint32(std::uint32_t value, int offset) {
        write(static_cast<std::uint8_t>(value >> 24), offset);
        write(static_cast<std::uint8_t>(value >> 16), offset);
        write(static_cast<std::uint8_t>(value >> 8), offset);
        write(static_cast<std::uint8_t>(value), offset);
    }

    void writeVector(const std::vector<std::uint8_t> &vec, int offset) {
        for (int idx = 0; const auto &v: vec) {
            data[offset + idx] = v;
            idx++;
        }
    }

    void write(std::uint8_t b, int offset) {
        data[offset] = b;
    }
};

#endif //SUIKA_BINARYPAYLOAD_H
