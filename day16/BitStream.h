
class Packet;

class BitStream {
    private:
        std::vector<uint8_t> bytes;
        uint32_t sbuffer;  // buffer for shifting
        uint16_t valid;    // how many bits in the sbuffer are valid
        uint16_t position; // the position in the bytes array

    uint32_t getMask(uint8_t _num);
    public:
        BitStream() : BitStream(std::vector<uint8_t>()) {}

        BitStream(std::vector<uint8_t> _input);
        void setBytes(std::vector<uint8_t> _input);
        void reset();
        int32_t getBits(uint8_t _num);
        void putBits(uint16_t _bits, uint8_t _len);
        std::pair<uint32_t,Packet*> parsePackets();
        void hexOutOn(std::ostream & os);
};        



