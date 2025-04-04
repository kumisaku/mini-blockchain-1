#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define SHA256_BLOCK_SIZE 32  // 256 bits

// SHA-256 Constants
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// SHA-256 Macros
#define ROTR(x, n) ((x >> n) | (x << (32 - n)))
#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SIG0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIG1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define EP0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3))
#define EP1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10))

// SHA-256 Hashing Function
void sha256(const uint8_t *message, uint64_t len, uint8_t *digest) {
    uint32_t h[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

    uint64_t new_len = len + 1;
    while (new_len % 64 != 56) new_len++;
    uint8_t *msg = calloc(new_len + 8, 1);
    if (!msg) return;
    
    memcpy(msg, message, len);
    msg[len] = 0x80;
    uint64_t bit_len = len * 8;
    for (int i = 0; i < 8; i++) msg[new_len + i] = (bit_len >> ((7 - i) * 8)) & 0xFF;

    for (uint64_t i = 0; i < new_len; i += 64) {
        uint32_t w[64];
        for (int j = 0; j < 16; j++)
            w[j] = (msg[i + (j * 4)] << 24) | (msg[i + (j * 4) + 1] << 16) | (msg[i + (j * 4) + 2] << 8) | (msg[i + (j * 4) + 3]);
        for (int j = 16; j < 64; j++)
            w[j] = EP1(w[j - 2]) + w[j - 7] + EP0(w[j - 15]) + w[j - 16];

        uint32_t a, b, c, d, e, f, g, h_;
        a = h[0]; b = h[1]; c = h[2]; d = h[3];
        e = h[4]; f = h[5]; g = h[6]; h_ = h[7];

        for (int j = 0; j < 64; j++) {
            uint32_t temp1 = h_ + SIG1(e) + CH(e, f, g) + k[j] + w[j];
            uint32_t temp2 = SIG0(a) + MAJ(a, b, c);
            h_ = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_;
    }

    for (int i = 0; i < 8; i++) {
        digest[i * 4] = (h[i] >> 24) & 0xFF;
        digest[i * 4 + 1] = (h[i] >> 16) & 0xFF;
        digest[i * 4 + 2] = (h[i] >> 8) & 0xFF;
        digest[i * 4 + 3] = h[i] & 0xFF;
    }
    free(msg);
}

// Blockchain Structure
typedef struct Block {
    time_t Timestamp;
    unsigned char PreviousBlockHash[SHA256_BLOCK_SIZE];
    char *Data;
    struct Block *next;
    unsigned char blockHash[SHA256_BLOCK_SIZE];
} Block;

Block *Chains = NULL;

// Hash to hexa string
void hashToHex(unsigned char *hash, char *hexStr) {
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(hexStr + (i * 2), "%02x", hash[i]);
    }
    hexStr[SHA256_BLOCK_SIZE * 2] = '\0';
}

// Add a new block to the blockchain
void addBlock(const char* data) {
    Block *newBlock = malloc(sizeof(Block));

    newBlock->Timestamp = time(NULL);
    newBlock->Data = strdup(data);

    if (Chains){
        memcpy(newBlock->PreviousBlockHash, Chains->blockHash, SHA256_BLOCK_SIZE);
    }
    else {
        memset(newBlock->PreviousBlockHash, 0, SHA256_BLOCK_SIZE);
    }
    sha256((uint8_t*)data, strlen(data), newBlock->blockHash);
    newBlock->next = Chains;
    Chains = newBlock;
}

// Display all blocks
void displayBlocks() {
    Block *current = Chains;
    while (current) {
        char prevHashHex[SHA256_BLOCK_SIZE * 2 + 1];
        char blockHashHex[SHA256_BLOCK_SIZE * 2 + 1];
        hashToHex(current->PreviousBlockHash, prevHashHex);
        hashToHex(current->blockHash, blockHashHex);

        printf("\n--- Block ---\n");
        printf("Timestamp: %s", ctime(&current->Timestamp));
        printf("Previous Hash: %s\n", prevHashHex);
        printf("Data: %s\n", current->Data);
        printf("Current Hash: %s\n", blockHashHex);
        
        current = current->next;
    }
}

void menu(){
    int answer;

   while(1){
        printf("\n===Mini BlockChain===\n");
        printf("\n1. Add Block\n");
        printf("2. Display Block\n");
        printf("3. Exit\n");

        printf("Select Menu (1-3): ");
        scanf("%d", &answer);

        switch(answer){
            case 1:
                char data[1001];
                printf("Add Data to the Block: ");
                scanf(" %[^\n]", data);
                addBlock(data);
                break;
            case 2:
                displayBlocks();
                break;
            case 3:
                exit(1);
                break;
            default:
                printf("Invalid Input!\n");
        }
    }
}

int main() {
    menu();
    return 0;
}
