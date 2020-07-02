//---------------------------------------------------------------------
// COPYRIGHT (C) 2019 SYNOPSYS INC.
// This software and the associated documentation are confidential and
// proprietary to Synopsys, Inc. Your use or disclosure of this software
// is subject to the terms and conditions of a written license agreement
// between you, or your company, and Synopsys, Inc. In the event of
// publications, the following notice is applicable:
//
// ALL RIGHTS RESERVED
//
// The entire notice above must be reproduced on all authorized copies.
//----------------------------------------------------------------------



#ifndef VS_SO_OBSOLETE
#define VS_SO_OBSOLETE __attribute__((deprecated))
#endif

#ifndef _ETHPTPFRAME_H_
#define _ETHPTPFRAME_H_

#include <stdint.h>

#include "MPXtorBase.hh"
#include "etheravbFrame.hh"

namespace ZEBU_IP {

using namespace ZEBU_MP;
using namespace ETHERAVBFRAME;
    

namespace ZEBU_ETHERAVB {

//================================================================================
//========================== Base class for PTP Packets ==========================
//================================================================================
/*! \Currently this class supports only untagged PTP frame
 *  \This is a helper class to set/get different fields of PTP frame
 *  \xtor APIs will still be using only etheravbframe.
 */

class ethPtpFrame {
    private:
    public:
        /*!\brief ethPtpFrame Constructor
         */
          ethPtpFrame ( ptpMessage_t frame_type = PTP_SYNC, uint32_t frame_length=0 );
   
        /*!\brief ethPtpFrame Destructor
         */
          ~ethPtpFrame ( );
        
       /*! \fn bool generateFrame();
        *  \brief generates etheravbFrame
        *  \Should be called only after all the parameters have been set
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          generateFrame();

       /*! \fn bool getFrameSize();
        *  \brief get the size of frame, which includes Destination Address, Source Address, length_field, PTP header and PTP payload
        *  \return uint32_t
        *  \retval frame size 
        */
         uint32_t      getFrameSize();

       /*! \fn bool getEthFrame();
        *  \brief get etheravbFrame which is generated using generateFrame() API
        *  \return etheravbFrame*        
        */
         etheravbFrame  *  getEthFrame();

       /*! \fn bool fillFromEthFrame (etheravbFrame& eth_frame);
        *  \brief fills the ethPtpFrame from etheravbFrame class, this API can be used when we receive a packet from xtor
        *  \param eth_frame is the reference to the frame
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          fillFromEthFrame (etheravbFrame& eth_frame);

       /*! \fn bool getTimestamp(uint16_t& timestamp_9_8, uint32_t & timestamp_7_4, uint32_t & timestamp_3_0);
        *  \brief get timestamp of supported ptp frame types
        *  \param timestamp_9_8, timestamp_7_8, timestamp_3_0 are the reference to the timestamp values  
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          getTimestamp(uint16_t& timestamp_9_8, uint32_t & timestamp_7_4, uint32_t & timestamp_3_0);

       /*! \fn bool getCorrectionField( uint32_t & correction_field_high, uint32_t & correction_field_low);
        *  \brief get correction field of supported ptp frame types
        *  \param correction_field_high and correction_field_low are the reference to the correction field values  
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          getCorrectionField( uint32_t & correction_field_high, uint32_t & correction_field_low);

       /*! \fn bool setTimestamp(uint16_t& timestamp_9_8, uint32_t & timestamp_7_4, uint32_t & timestamp_3_0);
        *  \brief set timestamp of supported ptp frame types
        *  \param timestamp_9_8, timestamp_7_8, timestamp_3_0 are the timestamp values  
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          setTimestamp(uint16_t timestamp_9_8, uint32_t  timestamp_7_4, uint32_t  timestamp_3_0);
       
       /*! \fn bool isOneStepTimestampingPresent ();
        *  \brief checks if One step Timestamping is being used
        *  \return bool
        *  \retval true if one step timestamping is enabled 
        */
         bool          isOneStepTimestampingPresent ();
 
       /*! \fn bool setRequestingPortId(uint16_t  port_id_9_8, uint32_t  port_id_7_4, uint32_t port_id_3_0);
        *  \brief set reqesting_port_id for applicable frame types
        *  \param port_id_9_8, port_id_7_4 port_id_3_0 are the port ids
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool          setRequestingPortId(uint16_t  port_id_9_8, uint32_t  port_id_7_4, uint32_t port_id_3_0);
       
       /*! \fn bool getRequestingPortId(uint16_t & port_id_9_8, uint32_t & port_id_7_4, uint32_t &port_id_3_0);
        *  \brief get reqesting_port_id for applicable frame types
        *  \param  port_id_9_8, port_id_7_4, port_id_3_0 are the reference to the port ids
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool        getRequestingPortId(uint16_t & port_id_9_8, uint32_t & port_id_7_4, uint32_t &port_id_3_0);
      
       /*! \fn bool getMessageType ();
        *  \brief get PTP message type
        *  \return PTP message type
        */
         ptpMessage_t   getMessageType ();
      
       /*! \fn bool fill (uint32_t offset, uint32_t size, uint8_t* payload);
        *  \brief fill the packet with user defined uint8_t array
        *  \param offset is the offset from which you want to start filling
        *  \param size is the frame size which is to be filled
        *  \param payload is the pointer to payload which contains all data
        *  \return bool
        *  \retval true if operation is successful 
        */
         bool       fill (uint32_t offset, uint32_t size, uint8_t* payload);

       /*! \fn bool setAnnounceTlv (uint16_t length, uint8_t* tlv);
        *  \brief fill the pathSequence field of announce message tlv with given uint8_t* tlv
        *  \param length is the number of octets be filled
        *  \param tlv is the pointer to payload which contains all data
        *  \return bool
        *  \retval true if operation is successful 
        */

         bool       setAnnounceTlv (uint16_t length, uint8_t* tlv);
 
       /*! \fn  bool      dumpFrame();
        *  \brief prints the complete PTP frame contents
        *  \return bool
        *  \retval true if operation is successful 
        */

         bool      dumpFrame();

       /*! \fn bool setPayload(bool en_payload, uint16_t payload_length, uint8_t* payload);  
        *  \brief fill the message body with this payload when generateframe() API is called
        *  \param payload_en indicate if this payload should be used for generating Frame, if false, generateFrame() will use individual fields to create frame
        *  \param payload_length is the number of octets to be filled
        *  \param payload is the pointer to payload which contains all data
        *  \return bool
        *  \retval true if operation is successful 
        */

         bool      setPayload(bool payload_en, uint16_t payload_length, uint8_t* payload);

    public:

	//======== Ethernet Frame Header ===========
  /*!\brief is the destination address 
   */
  uint64_t        dest_addr;             	//48 bit
  /*!\brief is the source address 
   */
  uint64_t        src_addr;               //48 bit

  /*!\brief defines the packet type field, default value  0x88F7
   */
  uint16_t ether_type; 		           	    //16 bit  

	//========= PTP frame Header ===========
  /*!\brief defines the ptp_message type
   */
  uint8_t  message_type;              		//4 bit

  /*!\brief is the transportspecific field of PTP header
   */
  uint8_t  transport_specific;        		//4 bit // 0: for PTP type // 1: for 802.1 

  /*!\brief is the version of PTP
   */
  uint8_t   version_ptp;  	             	//4 bit // always 2

  /*!\brief is the PTP messeage length
   */
  uint16_t message_length;            		//16 bit

  /*!\brief is the domain number
   */
  uint8_t  domain_num;                 		//8 bit // have to add set/get apis

  /*!\brief is the PTP flag field
   */
  uint16_t flag_field;               

  /*!\brief is the low correction value 
   */
  uint32_t correction_field_low;            

  /*!\brief is the high correction value
   */
  uint32_t correction_field_high;      

  /*!\brief is the source port id of PTP frame
   */
  uint32_t source_port_id_3_0;          		// 31:0 of 80 bits
  uint32_t source_port_id_7_4;          		// 63:32 of 80 bits
  uint16_t source_port_id_9_8;          		// 79:64 of 80 bits

  /*!\brief is the sequence id of PTP frame
   */
  uint16_t sequence_id;               		// 16 bit

  /*!\brief control field
   */
  uint8_t  control_field;             		//8 bit 

  /*!\brief is the log message interval field of PTP frame
   */
  uint8_t  log_message_interval;      		//8 bit

	//========= PTP frame Body ===========
  /*!\brief is the payload related variables
   */
  uint32_t timestamp_3_0;				// Act as originTimestamp/receiveTimestamp/responseOriginTimestamp/preciseOriginTimestamp or receiveReceiptTimestamp depending on MessageType
  uint32_t timestamp_7_4;
  uint16_t timestamp_9_8;

  uint32_t requesting_port_id_3_0;          
  uint32_t requesting_port_id_7_4;          
  uint16_t requesting_port_id_9_8;          
	uint16_t tlv_type;
  uint16_t tlv_length_field;

	//======== Specific to Announce message body ====
	uint16_t current_utc_offset;			
	uint8_t  grandmaster_priority_1;
	uint32_t grandmaster_clock_quality;
	uint8_t  grandmaster_priority_2;
	uint32_t grandmaster_identity_3_0;
	uint32_t grandmaster_identity_7_4;
	uint16_t steps_removed;
	uint8_t  time_source;
  uint8_t* path_sequence;

	//======== Specific to Signaling message body ====
  uint32_t target_port_id_3_0;          
  uint32_t target_port_id_7_4;          
  uint16_t target_port_id_9_8;         
  uint32_t organizational_id;			    // 24 bits 
  uint32_t organizational_subtype;		// 24 bits 
	char     link_delay_interval;
  char     time_sync_interval;
  char     announce_interval;
  uint8_t  flags;
  
  /*!\brief is the generated ethernet frame from PTP message
   */
  etheravbFrame* eth_frame;

  /*!\brief is the frame length of PTP message including eth header
   */
  uint32_t        frame_length;             

  private:
      bool		        is_payload_valid;
      uint16_t        payload_length;
      uint8_t*	      payload_data;
      uint32_t        frame_size;
      uint8_t*        frame_ptr;
      ptpMessage_t    frame_type;
      uint32_t        ptp_hstart_offset;    		

};
} // etheravbCommon

} // ZEBU_IP
#endif
