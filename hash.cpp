#include "include/lib.h"

std::string hash(const std::string &input)
{
    std::array<unsigned int, 16> hash{0};
    std::stringstream finalHash;

    if (input.empty())
    {
        for (int i = 0; i < 16; i++)
        {
            finalHash << std::hex << std::setfill('0') << std::setw(4) << (hash[i] & 0xFFFF);
        }
        return finalHash.str();
    }

    for (int i = 0; i < input.length(); i++)
    {
        unsigned int utfVal = static_cast<unsigned char>(input[i]);

        // randomly adjusting the values in the hex using seeds derived from the UTF value that go through several rounds of randomization while accounting for position in hash and overall input
        unsigned int seed1 = utfVal * 2654435761u + static_cast<unsigned int>(i);
        unsigned int seed2 = (seed1 ^ (seed1 >> 16)) * 2246822507u;
        unsigned int seed3 = (seed2 ^ (seed2 >> 13)) * 3266489917u;

        // linear congruential generator for random number generation
        auto lcg = [](unsigned int &state) -> unsigned int
        {
            state = state * 1664525u + 1013904223u;
            return state;
        };

        // lcg and bit operations to ensure better avalanche effect
        for (int j = 0; j < 16; j++)
        {
            unsigned int change = (lcg(seed3) + j * lcg(seed2) + i * lcg(seed1)) & 0xFFFF;
            hash[j] ^= change;
            hash[j] = ((hash[j] << 7) | (hash[j] >> 9)) & 0xFFFF;
            hash[j] ^= (hash[j] >> 4) & 0xFFFF;
        }

        // rotating the characters in the hash before inputing a new one to ensure avalanche
        std::rotate(hash.begin(), hash.begin() + ((i % 5) + 1), hash.end());

        // Additional random rotation to be less predictable
        if (i % 4 == 3)
        {
            for (int k = 0; k < 16; k++)
            {
                hash[k] ^= hash[(k + 8) % 16];
            }
        }
    }
    // Additional round of changes
    for (int i = 0; i < 16; i++)
    {
        hash[i] ^= hash[(i + 8) % 16];
        hash[i] = ((hash[i] << 5) | (hash[i] >> 11)) & 0xFFFF;
    }

    // casting everything to hex
    for (int i = 0; i < 16; i++)
    {
        unsigned int val = hash[i] & 0xFFFF;
        char hexChars[] = "0123456789abcdef";
        char hexStr[5];
        hexStr[4] = '\0';
        hexStr[3] = hexChars[val & 0xF];
        hexStr[2] = hexChars[(val >> 4) & 0xF];
        hexStr[1] = hexChars[(val >> 8) & 0xF];
        hexStr[0] = hexChars[(val >> 12) & 0xF];

        finalHash << hexStr;
    }
    return finalHash.str();
}
