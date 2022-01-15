/* https://adventofcode.com/2021/day/16 - Part 2 
   I had a silly bug in there, that cause all smaller test files
   to work, but only the puzzle input was wrong. Finding this bug took some time.
   It happened at the input byte #226 (a packet with a bit length > 256)
   I encoded the input data again and then compared where the first difference occurred.
*/

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <signal.h>
#include "BitStream.h"

#define CHAR_SPACE 32
#define MASK_LSBYTE 0x00FFFFFF
#define MASK_MSBIT 0x80000000
#define PTYPE_LIT  4

using std::vector;
using std::string;
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
    virtual int64_t value() const = 0;
    virtual string name() const = 0;
    virtual int packetType() const = 0;
    virtual void renderComputation(std::ostream &os) {};
    virtual void writeTo(BitStream & bs) const = 0;
    virtual int bitLength() const = 0;
    virtual int lengthType() const = 0;
    friend std::ostream & operator<<(std::ostream &os, const Packet& p) { 
        os << "V:" << (int)p.version << "T:" << (int)p.type << "L:" << p.lengthType() << '(';

        // os << p.name() << '(';
        p.renderData(os);
        os << ')';
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
        os << value();
    };
    string name() const {return string("");}
    int packetType() const {return 0;}
    int64_t value() const override {
        int64_t result=0;
        for (auto &v : values)
        {
            result <<=4;
            result |= v;
        }
        return result;
    }
    void writeTo(BitStream &bs)const override {
        bs.putBits(version,3);
        bs.putBits(type,3);
        auto last = values.size() - 1;
        for (auto i=0;i<=last;i++)
            if (! (i==last))
                bs.putBits(values[i]|0x10,5);
            else
                bs.putBits(values[i],5);
    }
    int bitLength() const override {
        return 6 + (values.size()*5);
    }
    int lengthType() const override {return -1;}

};

class PacketOperator : public Packet {
    public: 
    uint8_t ltype;
    vector<Packet*> subPackets;

    PacketOperator(uint8_t _v, uint8_t _t, uint8_t _lt) {
        version = _v;
        type = _t;
        ltype = _lt;
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
    int packetType() const {return 1;}
    void renderData(std::ostream &os) const override {
        for (auto &sp : subPackets)
            os << *sp << ' ';
    }
    void renderComputation(std::ostream &os) {
        os << name() << '(';
        for (auto &sp : subPackets)
        {
            os << *sp << ' ';
            sp->renderComputation(os);
        }
        os << ')';
        os << "[=" << value() << "]\n";
    }
    string name() const {
        switch (type) {
            case 0:
                return string("+"); break;
            case 1:
                return string("*"); break;
            case 2:
                return string("<<"); break;
            case 3:
                return string(">>"); break;
            case 5:
                return string(">"); break;
            case 6:
                return string("<"); break;
            case 7:
                return string("=="); break;
        }
    }

    int64_t value() const {
        int64_t result=0;
        switch (type) {
            case 0: // sum
                {
                    result = subPackets[0]->value();
                    for (auto i=1;i<subPackets.size();i++)
                        result += subPackets[i]->value();
                }
                break;
            case 1: // product
                {
                    result = subPackets[0]->value();
                    for (auto i=1;i<subPackets.size();i++)
                        result *= subPackets[i]->value();
                }
                break;
            case 2: // minimum
                {
                    result = subPackets[0]->value();
                    for (auto i=1;i<subPackets.size();i++)
                        result = std::min(result,subPackets[i]->value());
                }
                break;
            case 3: // maximum
                {
                    result = subPackets[0]->value();
                    for (auto i=1;i<subPackets.size();i++)
                        result = std::max(result,subPackets[i]->value());
                }
                break;
            case 5: // greater
                {
                    result = (subPackets[0]->value() > subPackets[1]->value())?1:0;
                }
                break;
            case 6: // less
                {
                    result = (subPackets[0]->value() < subPackets[1]->value())?1:0;
                }
                break;
            case 7: // equal
                {
                    result = (subPackets[0]->value() == subPackets[1]->value())?1:0;
                }
        }
        return result;
    }
    void writeTo(BitStream &bs) const override {
        bs.putBits(version,3);
        bs.putBits(type,3);
        bs.putBits(ltype,1);
        int length = 0;
        if (ltype == 0) {
            for (auto &sp : subPackets)
                length += sp->bitLength();
            bs.putBits(length,15);
        }
        else {
            bs.putBits(subPackets.size(),11);
        }
        for (auto &sp : subPackets)
            sp->writeTo(bs);
    }
    int bitLength() const override {
        int result = 7;
        result += (ltype == 0)?15:11;
        for (auto &sp : subPackets)
            result += sp->bitLength();
        return result;
    }
    int lengthType() const override {return ltype;}
};

uint32_t BitStream::getMask(uint8_t _num) {
    uint32_t result = 0;
    int i=0;
    while (i < _num) {
        result >>= 1;
        result |= MASK_MSBIT;
        i++;
    }
    return result;
}

BitStream::BitStream(vector<uint8_t> _input) {
    bytes = _input;
    sbuffer = valid = position = 0;
}
void BitStream::setBytes(vector<uint8_t> _input) {
    bytes = _input;
    sbuffer = valid = position = 0;
}
void BitStream::reset() {
    bytes.clear();
    sbuffer = valid = position = 0;
}
int32_t BitStream::getBits(uint8_t _num) {
    uint32_t result = 0;
    uint32_t mask = getMask(_num);


    while (valid < _num) {
        sbuffer |= bytes[position]<<(24-valid);
        if (position < bytes.size())
            position ++;
        else
            return -1;  // EOF
        valid += 8;
    }
    result = sbuffer & mask;
    result >>= 32-_num;
    valid -= _num;
    sbuffer <<= _num;
    return result;
}
void BitStream::putBits(uint16_t _bits, uint8_t _len) {
    uint32_t t;

    while (valid + _len > 32)
    {
        t = sbuffer;
        t >>= (valid - 8);
        t &= 0xFF;
        bytes.push_back(t);
        valid -= 8;
    }
    sbuffer <<= _len;
    sbuffer |= _bits;
    valid += _len;
}
std::pair<uint32_t,Packet*> BitStream::parsePackets() {
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
        result = new PacketOperator(ver,type,lengthType);
        if (lengthType == 0)
        {
            numBits = getBits(15);
            bitCount += 15;
            do
            {
                std::pair<uint32_t,Packet*> p = parsePackets();
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
                std::pair<uint32_t,Packet*> p = parsePackets();
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
void BitStream::hexOutOn(std::ostream & os) {
    uint32_t t;
    // move remaining bits from sbuffer to bytes
    while (valid > 7)
    {
        t = sbuffer;
        t >>= (valid - 8);
        t &= 0xFF;
        bytes.push_back(t);
        valid -= 8;
    }
    if (valid != 0)
    {
        t = sbuffer;
        t <<= (8 - valid);
        t &= 0xFF;
        bytes.push_back(t);
        valid = 0;
    }
    std::ios_base::fmtflags f( os.flags() );
    os << std::hex << std::uppercase;
    for (auto &i : bytes)
        os << std::setw(2) << std::setfill('0') << (int) i;
    os.flags( f );
}


int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;
    auto bitStream = BitStream();

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

        int64_t value = 0;
        std::pair<uint32_t,Packet*> p;

        p = bitStream.parsePackets();
        if (p.second)
        {
            // std::cout << *p.second << '\n';
            // p.second->renderComputation(std::cout);
            value = p.second->value();
            bitStream.reset();
            p.second->writeTo(bitStream);
            bitStream.hexOutOn(std::cout);
            delete p.second;
        }

        std::cout << "Result: "<< value << '\n';
    }
}