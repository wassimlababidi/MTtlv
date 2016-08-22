//
//  Mttlv.h
//  MTTLV
//
//  Created by Wassim Lababidi on 7/6/16.
//  Copyright (c) 2016 lababidi. All rights reserved.
//

#ifndef MTTLV_Mttlv_h
#define MTTLV_Mttlv_h

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


//PACK(


typedef struct  __attribute__((__packed__)) {
    uint8_t		header;
    uint32_t	timestamp;
    uint64_t    mac;
    uint8_t		tlvs[MTTLV_MAX_SIZE-5];		//1st byte is N when multi-tlv
}TNtlv_t;

void createHEADER(uint16_t* version,uint8_t vtype ,uint8_t macPresent,
                  uint8_t tstmpVer, uint8_t typeVer,
                  uint8_t lenPresent,TNtlv_t* MTNtlv,...
                  );//returns pointer to MTtlv or Ttlv


void destroyMTNtlv(TNtlv_t* MTNtlv);

void insert_t1v(uint8_t* offset, uint8_t* tv,
                uint8_t t, uint8_t* v, uint8_t v_size);
void create_objectList(uint8_t* values, uint8_t* t_value, uint8_t object_size,TNtlv_t* MTNtlv,int *f(TNtlv_t*,int,int,uint8_t*,uint8_t*,uint8_t));
int SaveThePacket(TNtlv_t* MNTtlv, int MN,int j,uint8_t*values,uint8_t *t_value,uint8_t object_size)
;



#endif
