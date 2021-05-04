#ifndef _AGENTSIGVERIFY_H_
#define _AGENTSIGVERIFY_H_

#include <cstddef>

class CAgentSigVerify
{
public:
    static int CheckIntegrity(const char * const binary);
private:
    static void GetMD5(char *m, size_t l, unsigned char *md5);
    static int CalculateMD5(const char *const fname, unsigned char *md5);
    static int GetSignature(const char *const fname, unsigned char *encryptedMD5);
};

#endif