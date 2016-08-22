//
//  Lib.h
//  Lib
//
//  Created by Wassim Lababidi on 7/25/16.
//  Copyright (c) 2016 lababidi. All rights reserved.
//

#ifndef Lib_Lib_h
#define Lib_Lib_h


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"


//--------------------------------------START: DEFAULT VALUES FOR HEADER
#define MTTLV_MIN_SIZE				3
#define MTTLV_MAX_SIZE				69
#define VERSION_0					00
#define SINGLE_TLV					0
#define MULTI_TLV					1
#define TSTMP_EPOCH					0	//timestamp is a 4-byte Linux epoch in UTC (1-second granularity)
#define TSTMP_ELIDED				1
#define DTYPE_1BYTE					0
#define DTYPE_2BYTES				1
//--------------------------------------END: DEFAULT VALUES FOR HEADER

//--------------------------------------START: HEADER RELATED
#define HEADER_SIZE					1

#define HEADER_VERSION_MASK			0xc0
#define HEADER_TYPE_MASK			0x20
#define HEADER_MAC_MASK				0x10
#define HEADER_TSTMP_MASK			0x08
#define HEADER_DTYPE_MASK			0x04
#define HEADER_DLEN_MASK			0x03

#define HEADER_GET_VERSION(X)		((X & HEADER_VERSION_MASK)>>6)
#define HEADER_GET_TYPE(X)			((X & HEADER_TYPE_MASK)>>5)
#define HEADER_GET_MAC(X)			((X & HEADER_MAC_MASK)>>4)
#define HEADER_GET_TSTMP(X)			((X & HEADER_TSTMP_MASK)>>3)
#define HEADER_GET_DTYPE(X)			((X & HEADER_DTYPE_MASK)>>2)
#define HEADER_GET_DLEN(X)			((X & HEADER_DLEN_MASK))

#define HEADER_SET_VERSION(X,Y)		(X=((X & ~(HEADER_VERSION_MASK))|(Y<<6)))
#define HEADER_SET_TYPE(X,Y)		(X=((X & ~(HEADER_TYPE_MASK))|(Y<<5)))
#define HEADER_SET_MAC(X,Y)			(X=((X & ~(HEADER_MAC_MASK))|(Y<<4)))
#define HEADER_SET_TSTMP(X,Y)		(X=((X & ~(HEADER_TSTMP_MASK))|(Y<<3)))
#define HEADER_SET_DTYPE(X,Y)		(X=((X & ~(HEADER_DTYPE_MASK))|(Y<<2)))
#define HEADER_SET_DLEN(X,Y)		(X=((X & ~(HEADER_DLEN_MASK))|(Y)))

#define HEADER_CLEAR(X)				(X = 0)
//--------------------------------------END: HEADER RELATED





typedef struct  __attribute__((__packed__))
    {
    uint8_t		header;
    uint32_t	timestamp;
    uint64_t    mac;
    uint8_t		tlvs[MTTLV_MAX_SIZE-5];		// to make it easier to build the mttlv
    }TNtlv_t;

uint8_t buffer[MTTLV_MAX_SIZE]; // to store
uint8_t offset;
void test(uint8_t* buffer,uint8_t* offset);


int createHEADER(uint16_t* version ,uint8_t macPresent,
                  uint8_t tstmpVer, uint8_t typeVer,
                  uint8_t lenPresent,TNtlv_t* MTNtlv,...
                  );   // returns size

void insertObject(TNtlv_t* MTNtlv,uint8_t* buffer,uint8_t* offset,uint8_t* sense_values,uint8_t type,uint8_t value_size); // inserting mttlvs into the buffer

int SaveThePacket(TNtlv_t* MTNtlv, int MN,int j,uint8_t*values,uint8_t *t_value,uint8_t object_size);
// saves the packet from the buffer then resets all parameters

#endif
