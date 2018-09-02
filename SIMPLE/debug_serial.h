// An SPI-derived debug channel output for our transmitter.

// Includes port setup so we don't interfere with other users of our three wires.

#include <inttypes.h>
#include <avr/pgmspace.h>

typedef enum {
	Debug_MSG_CSTRING = 0xFD,
	Debug_MSG_REQUEST_TIMING = 0xFE,
	Debug_MSG_END_SESSION = 0xFC,
	Debug_MSG_PAUSE_SESSION = 0xFB,
	Debug_MSG_TARGET_FAIL = 0xFA,
	Debug_MSG_TARGET_GOOD = 0xF9,
} Debug_MessageKind;

void Debug_startMessage(Debug_MessageKind kind);
void Debug_sendString(char *s);
void Debug_sendString_P(PGM_P s);
void Debug_sendUint8(uint8_t v);
void Debug_sendNumber(int32_t n, int base);
void Debug_endMessage();

#define SEND_ERROR(X) do { static const char S[] PROGMEM = (X); Debug_sendError(S); } while (0)

#define SEND_STRING(X) do { static const char S[] PROGMEM = (X); Debug_sendString_P(S); } while (0)

#define SEND_VARIABLE(X, V, B) do{ static const char S[] PROGMEM = (X); \
		Debug_startMessage(Debug_MSG_CSTRING);\
		Debug_sendString_P(S); \
		Debug_sendNumber(V, B);\
		Debug_endMessage();\
		} while (0)
void Debug_sendError(PGM_P s);
