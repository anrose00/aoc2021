/* https://adventofcode.com/2021/day/16 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

#define CHAR_SPACE 32
#define MASK_LSBYTE 0x00FFFFFF
#define MASK_MSBIT 0x80000000
#define PTYPE_LIT  4

using std::vector;
using vNumbers = vector<uint64_t>;


// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

bool isOnlyWhiteSpace(std::string _str)
{
    for (const auto& c : _str)
    {
        if (c > CHAR_SPACE )
            return false;
    }
    return true;
}

vector<uint8_t> hexStringToByteArray(const std::string &input)
{
    auto result = vector<uint8_t>();
    int next=0;
    uint8_t nibble,num=0;
    int inputSize = input.size();
    while (next<inputSize)
    {
        if (isdigit(input[next]))
            nibble = input[next] - '0';
        if (isalpha(input[next]))
            nibble = (input[next] - 'A') + 10;
        num <<= 4;
        num |= nibble;
        next++;
        if ((next & 1) == 0)
        {
            result.push_back(num);
            num = 0;
        }
    }
    return result;
}

class Packet {
    public: 
    uint8_t version;
    uint8_t type;

    Packet() {
        version = 0;
        type = 0;
    }
    virtual uint32_t versionSum() = 0;
    virtual void addValue(uint8_t i) = 0;
    virtual void addValue(Packet *p) = 0;
    virtual bool hasData() = 0;
    virtual void renderData(std::ostream &os) const = 0;
    friend std::ostream & operator<<(std::ostream &os, const Packet& p) { 
        std::ios_base::fmtflags f( os.flags() );
        os << std::hex << std::uppercase;
        os << "V:" << (int)p.version << "T:" << (int)p.type << '[';
        p.renderData(os);
        os << ']';
        os.flags( f );
        return os;
    }        

};

class PacketLiteral : public Packet {
    public: 
    vector<uint8_t> values;

    PacketLiteral(uint8_t _v, uint8_t _t) {
        version = _v;
        type = _t;
        values = vector<uint8_t>();
    }
    uint32_t versionSum() override {
        return version;
    }
    void addValue(uint8_t i) override {
        values.push_back(i);
    }
    void addValue(Packet *p) override { }
    bool hasData() override { return values.size() > 0;}
    void renderData(std::ostream &os) const override {
        for (auto &v : values)
            os << (int)v << ' ';
    };
};

class PacketOperator : public Packet {
    public: 
    vector<Packet*> subPackets;

    PacketOperator(uint8_t _v, uint8_t _t) {
        version = _v;
        type = _t;
        subPackets = vector<Packet*>();
    }
    ~PacketOperator() {
        for (auto &sp : subPackets)
            delete sp;
    }
    uint32_t versionSum() override {
        uint32_t result = version;
        for (auto &sp : subPackets)
            result += sp->versionSum();
        return result;
    }
    void addValue(uint8_t i) override {
    }
    void addValue(Packet *p) override { 
        subPackets.push_back(p);
    }
    bool hasData() override { return subPackets.size() > 0;}
    void renderData(std::ostream &os) const override {
        for (auto &sp : subPackets)
            os << *sp << ' ';
    };
};

class BitStreamReader {
    private:
        vector<uint8_t> bytes;
        uint32_t sbuffer;  // buffer for shifting
        uint16_t valid;    // how many bits in the sbuffer are valid
        uint16_t position; // the position in the bytes array

    uint32_t getMask(uint8_t _num) {
        uint32_t result = 0;
        int i=0;
        while (i < _num) {
            result >>= 1;
            result |= MASK_MSBIT;
            i++;
        }
        return result;
    }

    public:
        BitStreamReader() : BitStreamReader(vector<uint8_t>()) {}

        BitStreamReader(vector<uint8_t> _input) {
            bytes = _input;
            sbuffer = 0;
            valid = 0;
            position = 0;
        }
        void setBytes(vector<uint8_t> _input)
        {
            bytes = _input;
            sbuffer = 0;
            valid = 0;
            position = 0;
        }
        int32_t getBits(uint8_t _num) {
            uint32_t result = 0;
            uint32_t mask = getMask(_num);

            while (valid < _num) {
                sbuffer |= bytes[position]<<(24-valid);
                if (position < bytes.size())
                    position ++;
                else
                    return -1;  // EOF
                valid += sizeof(uint8_t)<<3;
            }
            result = sbuffer & mask;
            result >>= 32-_num;
            valid -= _num;
            sbuffer <<= _num;
            return result;
        }
        std::pair<uint8_t,Packet*> parsePackets() {
            Packet *result=nullptr;
            int bitCount=0;
            int32_t ver, type, lit;
            ver = getBits(3);
            if (ver < 0)  // no valid packet
                return std::make_pair(bitCount,result);    
            bitCount += 3;
            type = getBits(3);
            if (type < 0) // no valid packet
                return std::make_pair(0,result);    
            bitCount += 3;
            if (type == PTYPE_LIT)  // literal packet
            {
                result = new PacketLiteral(ver,type);
                do {
                    lit = getBits(5);
                    if (lit >= 0)
                    {
                        result->addValue(lit&0xF);
                        bitCount += 5;
                    }
                    else
                        break;
                }
                while ((lit & 0x10) > 0);
            }
            else { // operator packet
                int lengthType = getBits(1);
                bitCount += 1;
                int numBits,numSubPackets;
                result = new PacketOperator(ver,type);
                if (lengthType == 0)
                {
                    numBits = getBits(15);
                    bitCount += 15;
                    do
                    {
                        std::pair<uint8_t,Packet*> p = parsePackets();
                        if (p.second)
                        {
                            result->addValue(p.second);
                            numBits -= p.first;
                            bitCount += p.first;
                        }
                        else
                            break;
                    }
                    while (numBits > 0);
                }
                else
                {
                    numSubPackets = getBits(11);
                    bitCount += 11;
                    do
                    {
                        std::pair<uint8_t,Packet*> p = parsePackets();
                        if (p.second)
                        {
                            result->addValue(p.second);
                            bitCount += p.first;
                            numSubPackets--;
                        }
                        else
                            break;
                    }
                    while (numSubPackets > 0);
                }
            }
            if (!result->hasData())
            {
                delete result;
                result = nullptr;
            }
            return std::make_pair(bitCount,result);
        }
};

int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;
    auto bitStream = BitStreamReader();

    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,numberString);
            if (!isOnlyWhiteSpace(numberString))
            {
                bitStream.setBytes(hexStringToByteArray(numberString));
            }
        }
        stream.close();

        int sum = 0;
        std::pair<uint8_t,Packet*> p=std::make_pair(0,nullptr);
        do {
            p = bitStream.parsePackets();
            if (p.second)
            {
                std::cout << *p.second << '\n';
                sum += p.second->versionSum();
                delete p.second;
            }
        } 
        while (p.second);

        std::cout << "Result: "<< sum << '\n';
    }
}