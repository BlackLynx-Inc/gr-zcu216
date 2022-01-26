/*!
\file T4.h
\brief T4 turbo header
*<pre>
* Author: Bill Douskalis, 2018
* DFLX.COM, INC.
*</pre>
*/

//#ifndef T4
//#define T4

#include <sys/time.h>

#include <iostream>

#define PACKET_SIZE 96
#define UNC_BITS 762
#define ENC_BITS 1536
#define ENC_BYTES 192
#define PDU 1600
#define WF58_FRAME_SIZE 1088
#define WF58_AMBLE_SIZE 512
#define MOD_FRAME 3200
#define TRANSITIONS 16  // 3GPP2 and UMTS

#define LITTLE
#define NUM_CHIPS 25
#define VWF_CYCLE 960000
#define WF58_CYCLE 595200

#define PACKET_SIZE_T 762
#define WF58_PAYLOAD_SIZE 72
#define MAX_BITS_T 1536

#define LEN_LLRS_T 762  // 3GPP2
#define HALF_PACKET 381
#define NTURBO_LIMIT 1024  // this may not be right for VWF

//#endif
