#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <limits.h>
#else
#include <linux/limits.h>
#endif
#include <ftdi.h>
#include <time.h>

// The FT240X has a 512 byte buffer.  Must be multiple of 64
// We also write this in one go to the Keccak sponge, which is at most 1600 bits
#define BUFLEN 512u

struct infnoise_context {
    struct ftdi_context ftdic;
    uint32_t entropyThisTime;
    char *message;
    bool errorFlag;
    //uint8_t keccakState[KeccakPermutationSizeInBytes];

    // used in multiplier mode to keep track of bytes to be put out
    uint32_t numBits;
    uint32_t bytesWritten;
};

struct infnoise_devlist_node {
    uint8_t id;
    char manufacturer[128];
    char description[129];
    char serial[128];
    struct infnoise_devlist_node *next;
};

typedef struct infnoise_devlist_node *devlist_node;

/*
 * returns a struct of infnoise_devlist_node listing all connected FTDI FT240 devices by its USB descriptors,
 *
 * parameters:
 *  - message: pointer for error message
 *
 *  returns: NULL when none found or infnoise_devlist_node
 */
devlist_node listUSBDevices(char **message);


/*
 * initialize the Infinite Noise TRNG - must be called once before readData() works.
 *
 * parameters:
 *  - context: pointer to infnoise_context struct
 *  - serial: optional serial number of the device (NULL)
 *  - keccak: initialize Keccak sponge (required to use readData with raw=false)
 *  - debug: debug flag
 * returns: boolean success indicator (0=success)
*/
bool initInfnoise(struct infnoise_context *context, char *serial, bool keccak, bool debug);

/*
 * Reads some bytes from the TRNG and stores them in the "result" byte array.
 * The array has to be of sufficient size. Please refer to the example programs.
 *
 * After each read operation, the infnoise_context's errorFlag must be checked,
 * and the data from this call has to be discarded!
 * Detailed error messages can be found in context->message.
 *
 * parameters:
 *  - context: infnoise_context struct with device pointer and state variables
 *  - result: pointer to byte array to store the result
 *  - raw: boolean flag for raw or whitened output
 *  - outputMultiplier: only used for whitened output
 *
 * returns: number of bytes written to the array
*/
uint32_t readData(struct infnoise_context *context, uint8_t *result, bool raw, uint32_t outputMultiplier);
