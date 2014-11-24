/*************************************************************************
 * Copyright © 2012-2014, Targoman.com
 *
 * Published under the terms of TCRL(Targoman Community Research License)
 * You can find a copy of the license file with distributed source or
 * download it from http://targoman.com/License.txt
 *
 *************************************************************************/
 /**
 @author S. Mohammad M. Ziabary <smm@ziabary.com>
 */


#ifndef TARGOMAN_DEFINITIONS_HASHFUNCTIONS_H
#define TARGOMAN_DEFINITIONS_HASHFUNCTIONS_H

#include <QtCore>

namespace Targoman {
namespace Common {

static const int HashKeys[]={29,71,113,173,229,31,73,127,179,37,79,131,181,41,83,137};
static const int HashKeysCount = sizeof(HashKeys) / sizeof(int);
/*http://planetmath.org/goodhashtableprimes*/
static const quint32 GoodHashPrimeNumbers[]={
    53,
    97,
    193,
    389,
    769,
    1543,
    3079,
    6151,
    12289,
    24593,
    49157,
    98317,
    196613,
    393241,
    786433,
    1572869,
    3145739,
    6291469,
    12582917,
    25165843,
    50331653,
    100663319,
    201326611,
    402653189
};

class HashFunctions
{
public:
    static quint64 murmurHash64(const void* _buff, size_t _len, int _level){
        const quint64 Constant1 = 0xc6a4a7935bd1e995LLU;
        const int Remain1 = 47;

        quint64 Hash = HashKeys[_level % HashKeysCount] ^ (_len * Constant1);

        const quint64 * Data = (const quint64 *)_buff;
        const quint64 * End = (_len >> 3) + Data;

        while(Data != End)
        {
          quint64 K = *Data++;

          K *= Constant1;
          K ^= K >> Remain1;
          K *= Constant1;

          Hash ^= K;
          Hash *= Constant1;
        }

        const unsigned char * Data2 = (const unsigned char *)Data;

        switch(_len & 7)
        {
        case 7: Hash ^= (quint64)(Data2[6]) << 48;
        case 6: Hash ^= (quint64)(Data2[5]) << 40;
        case 5: Hash ^= (quint64)(Data2[4]) << 32;
        case 4: Hash ^= (quint64)(Data2[3]) << 24;
        case 3: Hash ^= (quint64)(Data2[2]) << 16;
        case 2: Hash ^= (quint64)(Data2[1]) << 8;
        case 1: Hash ^= (quint64)(Data2[0]);
                Hash *= Constant1;
        };

        Hash ^= Hash >> Remain1;
        Hash *= Constant1;
        Hash ^= Hash >> Remain1;

        return Hash;
    }

    static quint32 murmurHash32(const void* _buff, size_t _len, int _level)
    {
        static const quint32 Const1 = 0xcc9e2d51;
        static const quint32 Const2 = 0x1b873593;
        static const quint32 Round1 = 15;
        static const quint32 Round2 = 13;
        static const quint32 M = 5;
        static const quint32 Const3 = 0xe6546b64;

        quint32 Hash = HashKeys[_level % HashKeysCount];

        const int BlockCount = _len / 4;
        const quint32 *Blocks = (const quint32 *) _buff;
        int i;
        for (i = 0; i < BlockCount; i++) {
            quint32 Buffer = Blocks[i];
            Buffer *= Const1;
            Buffer = (Buffer << Round1) | (Buffer >> (32 - Round1));
            Buffer *= Const2;

            Hash ^= Buffer;
            Hash = ((Hash << Round2) | (Hash >> (32 - Round2))) * M + Const3;
        }

        const quint8 *Tail = (const quint8 *) (_buff + BlockCount * 4);
        quint32 Buffer = 0;

        switch (_len & 3) {
        case 3:
            Buffer ^= Tail[2] << 16;
        case 2:
            Buffer ^= Tail[1] << 8;
        case 1:
            Buffer ^= Tail[0];

            Buffer *= Const1;
            Buffer = (Buffer << Round1) | (Buffer >> (32 - Round1));
            Buffer *= Const2;
            Hash ^= Buffer;
        }

        Hash ^= _len;
        Hash ^= (Hash >> 16);
        Hash *= 0x85ebca6b;
        Hash ^= (Hash >> 13);
        Hash *= 0xc2b2ae35;
        Hash ^= (Hash >> 16);

        return Hash;
    }
};

}
}

#endif // TARGOMAN_DEFINITIONS_HASHFUNCTIONS_H