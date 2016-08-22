//
//  main.c
//  mt
//
//  Created by Wassim Lababidi on 7/6/16.
//  Copyright (c) 2016 lababidi. All rights reserved.
//

#include <stdio.h>
#include "mttlv.h"
#include "Time.h"

/*
 
 Binary representation
 
 This representation is used for:
 * sending data in packets
 * writing data to a file
 
 Each group of Objects consists of the following fields:
 
 
 | SOL header | Objects list |
 
 SOL HEADER:
 
 The header always starts with a 2-bit `V` field (version).
 Only value `b00` is defined in this document. Other values for the 2 first bits are reserved and may be defined in later revisions of this document.
 
 
                                                 0 1 2 3 4 5 6 7
                                                +-+-+-+-+-+-+-+-+
                                                | V |T|M|S|Y| L |
                                                +-+-+-+-+-+-+-+-+

 
 * T: Type of MTtlv Object:
        0: single-MTtlv Object
        1: multi-MTtlv Object (MTNtlv) this implies the 1st byte next to timestamp is N: number of Objects
 * M: MAC address encoding:
        0: no MAC address present
        1: 8-byte MAC address present
 * S: timestamp encoding
        0: timestamp is a 4-byte Linux epoch in UTC (1-second granularity)
        1: timestamp is elided, and recovered from the timestamp field present in the SmartMesh IP header
 * Y: type encoding
        0: 1-byte type field
        1: 2-byte type field
 * L: length encoding
        b00: use well-known value. No length field present
        b01: 1-byte length field present
        b10: 2-byte length field present
        b11: elided. The length is recovered from the length of the packet or HDLC frame.
 
 OBJECT LIST: IN MTtlv FORMAT DEPENDS IF SINGLE OBJECT OR MULTIPLE OBJECTS
 
 */



   void createHEADER(uint16_t* version,uint8_t vtype ,uint8_t macPresent,
                         uint8_t tstmpVer, uint8_t typeVer,
                         uint8_t lenPresent,TNtlv_t* MTNtlv,...
                         )


/* 
                                This function returns the header
 
        if providing both timestamp and mac-adress please input mac-adress first.
 */
    
    {
        va_list ap;
        
        va_start (ap,MTNtlv);
        
        if (*version != VERSION_0)
        {
            printf("Unknown release version: %u.\n\r", *version);
            
        }
        
        uint8_t header = 0;
        uint8_t finalSize;
        
        HEADER_SET_VERSION(header, VERSION_0);
        HEADER_SET_TYPE(header, vtype);
        
        //default init
        uint8_t tstmp_size = 4;
        uint8_t mac_size = 0;
        uint8_t type_size = 1;
        uint8_t len_size = 0;
        uint8_t N_size = 1;
        
        if (macPresent) {
            mac_size = 8;
            HEADER_SET_MAC(header, 1);
            MTNtlv->mac = va_arg(ap,UINT64_MAX);
            
        }
        else {
            HEADER_SET_MAC(header, 0);
        }
        if (tstmpVer == TSTMP_ELIDED) {
            tstmp_size = 0;
            HEADER_SET_TSTMP(header, TSTMP_ELIDED);
            MTNtlv->mac = va_arg(ap,UINT64_MAX);
        }
        else {
            HEADER_SET_TSTMP(header, TSTMP_EPOCH);
            MTNtlv->timestamp=(int)time(NULL);
        }
        if (typeVer == DTYPE_2BYTES) {
            type_size = 2;
            HEADER_SET_DTYPE(header, DTYPE_2BYTES);
        }
        else {
            HEADER_SET_DTYPE(header, DTYPE_1BYTE);
        }
        if (lenPresent) {
            HEADER_SET_DLEN(header, 1);
        }
        else {
            HEADER_SET_DLEN(header, 0);
        }
                
       /* finalSize = HEADER_SIZE + tstmp_size + mac_size + N_size + numObjects *(type_size + len_size) + sizeofObjects;
        if (finalSize > MTTLV_MAX_SIZE)
            return 0;
        //printf("finaleSize: %u\n\r", finalSize);*/
        
        
        MTNtlv->header = header;
        
    }


    
    void destroyMTNtlv(TNtlv_t* MTNtlv) {
        free(MTNtlv);
    }
    
    void insert_t1v(uint8_t* offset, uint8_t* tv, uint8_t t, uint8_t* v, uint8_t v_size) {
        tv[*offset] = t;
        *offset = *offset + 1;
        memcpy(tv + *offset, v, v_size);
        *offset += v_size;
    }
    
    void create_objectList(uint8_t* values, uint8_t* t_value, uint8_t object_size,TNtlv_t* MTNtlv,int *f(TNtlv_t*,int,int,uint8_t*,uint8_t*,uint8_t))
{
    int N =0;
    
  if (HEADER_GET_TYPE(MTNtlv->header)==SINGLE_TLV)
    {
        
        if (object_size < (MTTLV_MAX_SIZE-5))
        {
            MTNtlv->tlvs[N]=values[0];
            
        }
        else
        {
            printf("unrecognized object size");
        }
 
    }
    else if (HEADER_GET_TYPE(MTNtlv->header)==MULTI_TLV)
    {
        int size=object_size;
        int ar=sizeof(values);
        int MN=ar/size;
        int j=0;
        MTNtlv->tlvs[0]=MN;
        if (object_size < (MTTLV_MAX_SIZE-6))
        {
            for (int i=1;size+object_size<MTTLV_MAX_SIZE-5 && i<=MN;i+=1)
            {
                size+=object_size;
                MTNtlv->tlvs[i]=values[i-1];
                j=i;
                
            }
            f(MTNtlv,MN,j,values,t_value,object_size);
        }
        else
        {
            printf("unrecognized object size");
        }
    
    }
    else
    {
        printf("unrecognized MTLV TYPE");
    }

}

int SaveThePacket(TNtlv_t* MNTtlv, int MN,int j,uint8_t*values,uint8_t *t_value,uint8_t object_size)
{
 
   

    FILE *f = fopen("output.txt", "a");

    
   
  
    fprintf(f, "Header: %hhu Timestamp: %d, Mac: %llu Values: ",MNTtlv->header, MNTtlv->timestamp,MNTtlv->mac);
    for (int i=0; i<sizeof(MNTtlv->tlvs);i+=1)
    {
        fprintf(f,"%d,",MNTtlv->tlvs[i] );
        
    }
    
    if (j<MN)
    {
        create_objectList(values, t_value, object_size, MNTtlv,SendThePacket(MNTtlv, MN, j, values, t_value
                                                                              , object_size));
    }
    else{
        destroyMTNtlv(MNTtlv);
    }
    fprintf(f,"\n");
    
    return 0;
    
}

