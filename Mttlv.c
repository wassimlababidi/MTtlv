//
//  main.c
//  Lib
//
//  Created by Wassim Lababidi on 7/25/16.
//  Copyright (c) 2016 lababidi. All rights reserved.

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



#include <stdio.h>
#include "Lib.h"
#include "Time.h"
#include <stdarg.h>
#include <netinet/in.h>

#define SENS_X_ID	0x15
//PACK(
typedef struct __attribute__((__packed__)) {
    uint8_t  val1;
    uint8_t	 val2;
    uint8_t  val3;
}sensXdata_t;   //packet data




void main()
{
    sensXdata_t sensXdata;
    sensXdata.val1 = 0x76;
    sensXdata.val2 = 0x54;
    sensXdata.val3 = 0x43;
    uint8_t offset = 0;
    uint16_t version=0;
    uint8_t macPresent=1;
    uint8_t tstmpVer=TSTMP_EPOCH;
    uint8_t typeVer=DTYPE_2BYTES;
    uint8_t lenPresent=0;
    TNtlv_t MTNtlv;
    int s;
    uint64_t mac=0x0f0e0d0c0b0a0908;
    s=createHEADER( &version , macPresent,
                  tstmpVer,  typeVer,
                 lenPresent, &MTNtlv);
        if (s==0)
        {
            printf("something wrong homie");
        }
        else
        {
            printf("the size is %d \n",s);
        }
    
    for (int i=0;i<=MTTLV_MAX_SIZE+8;i+=8)
    {
    insertObject( &MTNtlv, buffer, &offset, &sensXdata.val1,1,0x0f0e0d0c0b0a0908);
    }
    //printf("%d",offset);
 
   // test(buffer,&offset);
    

    }

uint32_t swap_bytes32(uint32_t* num){
    uint32_t swapped = ((*num>>24)&0xff) | // move byte 3 to byte 0
    ((*num<<8)&0xff0000) | // move byte 1 to byte 2
    ((*num>>8)&0xff00) | // move byte 2 to byte 1
    ((*num<<24)&0xff000000); // byte 0 to byte 3
    return swapped;
}

uint16_t swap_bytes16(uint16_t* num){
    uint16_t swapped = (*num>>8) | (*num<<8);
    return swapped;
}


int createHEADER(uint16_t* version ,uint8_t macPresent,uint8_t tlvtype,
                  uint8_t tstmpVer, uint8_t typeVer,
                  uint8_t lenPresent,TNtlv_t* MTNtlv
                  )                         // creates header and  returns size
{
    
  
    
  
    
    if (*version != VERSION_0)
    {
        printf("Unknown release version: %u.\n\r", *version);
        
    }
    
    uint8_t header = 0;
    uint8_t finalSize=0;
    
    HEADER_SET_VERSION(header, VERSION_0);
   
    
    //default init
    uint8_t tstmp_size = 4;
    uint8_t mac_size = 0;
    uint8_t type_size = 1;
    if (tlvtype==1)
    {
     HEADER_SET_TYPE(header,1)}
    
    if (macPresent==1) {
        mac_size = 8;
        HEADER_SET_MAC(header, 1);
       // printf("OK!mac\n");
        
    }
    else {
        HEADER_SET_MAC(header, 0);
        //printf("ok no mac\n");
    }
    if (tstmpVer == TSTMP_ELIDED) {         // elided here either means passed as arguement or not needed
        tstmp_size = 0;
        HEADER_SET_TSTMP(header, TSTMP_ELIDED);
     //   printf("elided time\n");
        
    }
    else {                                  // means epoch timestamp
        HEADER_SET_TSTMP(header, TSTMP_EPOCH);
        MTNtlv->timestamp=(int)time(NULL);
      //  printf("epoch time\n");
    }
    if (typeVer == DTYPE_2BYTES) {
        type_size = 2;
        HEADER_SET_DTYPE(header, DTYPE_2BYTES);
      //  printf("2 bytes nigha\n");
    }
    else {
        HEADER_SET_DTYPE(header, DTYPE_1BYTE);
      //  printf("1 byte mf\n");
    }
    if (lenPresent==1) {
        HEADER_SET_DLEN(header, 1);
       // printf("len 1 \n");
    }
    else if ( lenPresent==2){
        HEADER_SET_DLEN(header, 2);
       // printf("len 2 \n");
    }
    else if (lenPresent==3)
    {
        HEADER_SET_DLEN(header, 3);
       // printf("len 3 \n");
    }
    
    else {
        HEADER_SET_DLEN(header, 0);
    }
    
    
    MTNtlv->header = header;
   // printf("the header is  %02x\n",header);
    
    finalSize=HEADER_SIZE+tstmp_size+mac_size+type_size;
    

    return finalSize;                       // returns final size to check if an object is passed or not
}




void insertObject(TNtlv_t* MTNtlv,uint8_t* buffer,uint8_t* offset,uint8_t* sense_values,uint16_t type,...)
/*
    inserts object in buffer till maximum size is reached then prints , can take as optional arguements mac, timestamp and length.
    mac always preceeds the other 2 , and timestamp always should preceed the length
 */

{
    
    

    if (*offset+sizeof(sense_values)>=MTTLV_MAX_SIZE)
    {
        if(sizeof(sense_values)>MTTLV_MAX_SIZE-5)
        {
            printf("wrong value type, size too big");
        }
        test(buffer,offset); //TODO: test function is to print the packet , but please replace by the send function when integrating into code
        *offset=0;
    }
    
    else
    {
        va_list ap;
        
        va_start (ap,type);
        
        buffer[*offset]=MTNtlv->header;
        *offset+=1;
        if (HEADER_GET_MAC(MTNtlv->header)==1)
            
        {           MTNtlv->mac = va_arg(ap,uint64_t);
            
            ((uint64_t*)(buffer + *offset))[0] = htonll(MTNtlv->mac);
            
                    *offset+=8;
        }
        
        if (HEADER_GET_TSTMP(MTNtlv->header)==TSTMP_EPOCH)
        {
                    ((uint32_t*)(buffer + *offset))[0]=htonl(MTNtlv->timestamp);
                    *offset+=4;
                    printf("timestamp %x\n",MTNtlv->timestamp);
        }
        else
        {
                    MTNtlv->timestamp= va_arg(ap,uint32_t);
                    ((uint32_t*)(buffer + *offset))[0]=htonl(MTNtlv->timestamp);
                    *offset+=4;
                    printf("timestamp %x\n",MTNtlv->timestamp);
        }
        
        
        
        
        
        if (HEADER_GET_DTYPE(MTNtlv->header)==DTYPE_2BYTES)
        {
                    ((uint16_t*)(buffer + *offset))[0] = htons(type);
                    *offset+=2;
        }
        else
        {
                    buffer[*offset]=type;
                    *offset+=1;
                   // printf("1 homie\n");
        }
        
        if (HEADER_GET_DLEN(MTNtlv->header)==1)
        {
            buffer[*offset]=va_arg(ap,uint64_t);;
            *offset+=1;
        }
        else if (HEADER_GET_DLEN(MTNtlv->header)==2)
        {
            buffer[*offset]=va_arg(ap,uint64_t);;
            *offset+=2;
            
        }
        buffer[*offset]=*sense_values;
        *offset+=sizeof(sense_values)/8;
      
       /* int i=0;
        for(i=0;i<=*offset;i+=1)
        {
            printf("%d",buffer[i]);
        }
        printf("\n %d %d",i,*offset);

        */
           va_end(ap);     }
            }

        
void test(uint8_t* buffer,uint8_t* offset)
{
    for(int i=0;i<=*offset;i+=1)
    {
        printf("%02x ",buffer[i]);
    }
}



