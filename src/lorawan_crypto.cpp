#include "lorawan_crypto.h"
#include <mbedtls/aes.h>
#include <string.h>

static void xorBlock(uint8_t *a, const uint8_t *b) { for (int i=0;i<16;i++) a[i]^=b[i]; }
static void leftShiftOne(const uint8_t in[16], uint8_t out[16]) {
    uint8_t carry=0;
    for (int i=15;i>=0;i--) { uint8_t x=in[i]; out[i]=(uint8_t)((x<<1)|carry); carry=(x&0x80)?1:0; }
}
static void aesEcb(const uint8_t key[16], const uint8_t in[16], uint8_t out[16]) {
    mbedtls_aes_context ctx; mbedtls_aes_init(&ctx); mbedtls_aes_setkey_enc(&ctx,key,128); mbedtls_aes_crypt_ecb(&ctx,MBEDTLS_AES_ENCRYPT,in,out); mbedtls_aes_free(&ctx);
}
static void cmac(const uint8_t key[16], const uint8_t *msg, size_t len, uint8_t out[16]) {
    uint8_t L[16]={0}, K1[16], K2[16]; aesEcb(key,L,L); leftShiftOne(L,K1); if(L[0]&0x80) K1[15]^=0x87; leftShiftOne(K1,K2); if(K1[0]&0x80) K2[15]^=0x87;
    size_t n=(len+15)/16; if(n==0)n=1; bool complete=(len>0 && (len%16)==0); uint8_t X[16]={0}, Y[16], M[16];
    for(size_t b=0;b<n-1;b++){ memcpy(M,msg+b*16,16); xorBlock(M,X); aesEcb(key,M,X); }
    size_t last=(n-1)*16; memset(M,0,16); size_t rem=len-last; if(rem) memcpy(M,msg+last,rem); if(complete) xorBlock(M,K1); else { M[rem]=0x80; xorBlock(M,K2); }
    xorBlock(M,X); aesEcb(key,M,out);
}

bool lorawanVerifyMic(const uint8_t *phy, size_t len, uint32_t devAddr, uint32_t fCnt, const uint8_t nwkSKey[16]) {
    if(len<7) return false;
    uint8_t b0[16]={0}; b0[0]=0x49; b0[5]=0x00; b0[6]=(uint8_t)(devAddr); b0[7]=(uint8_t)(devAddr>>8); b0[8]=(uint8_t)(devAddr>>16); b0[9]=(uint8_t)(devAddr>>24);
    b0[10]=(uint8_t)fCnt; b0[11]=(uint8_t)(fCnt>>8); b0[12]=(uint8_t)(fCnt>>16); b0[13]=(uint8_t)(fCnt>>24); b0[15]=(uint8_t)len;
    uint8_t tmp[288]; if(len>255) return false; memcpy(tmp,b0,16); memcpy(tmp+16,phy,len); uint8_t tag[16]; cmac(nwkSKey,tmp,16+len,tag);
    return tag[0]==phy[len-4] && tag[1]==phy[len-3] && tag[2]==phy[len-2] && tag[3]==phy[len-1];
}

void lorawanDecryptPayload(uint8_t *payload, size_t len, uint32_t devAddr, uint32_t fCnt, uint8_t direction, const uint8_t key[16]) {
    uint8_t a[16]={0}, s[16]; a[0]=0x01; a[5]=direction; a[6]=(uint8_t)devAddr; a[7]=(uint8_t)(devAddr>>8); a[8]=(uint8_t)(devAddr>>16); a[9]=(uint8_t)(devAddr>>24); a[10]=(uint8_t)fCnt; a[11]=(uint8_t)(fCnt>>8); a[12]=(uint8_t)(fCnt>>16); a[13]=(uint8_t)(fCnt>>24);
    for(size_t pos=0, block=1; pos<len; pos+=16,block++){ a[15]=(uint8_t)block; aesEcb(key,a,s); size_t n=min((size_t)16,len-pos); for(size_t i=0;i<n;i++) payload[pos+i]^=s[i]; }
}
