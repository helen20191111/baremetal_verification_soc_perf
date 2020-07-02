//---------------------------------------------------------------------
// COPYRIGHT (C) 2019  SYNOPSYS INC.
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

#ifndef VS_SO_EXPORT
#define VS_SO_EXPORT 
#endif

#ifndef VS_SO_OBSOLETE
#define VS_SO_OBSOLETE __attribute__((deprecated))
#endif

#ifndef _ETHERAVBFRAME_HH_
#define _ETHERAVBFRAME_HH_

#include "etheravbFrame/etheravbFrame_defines.hh"

#define ETHERAVBFRAME_VER 2

namespace ZEBU_IP {

  namespace ETHERAVBFRAME {

    class etheravbFrameImp;

    /*! This is the etheravbFrame object of the ETHERAVB Transactor whose methods are used to store and acces content of an etheravbFrame
     */
    class etheravbFrame{

    public: 
      // ==============================================================================================
      // ================================= Constructors & Destructors =================================
      // ==============================================================================================

      /*! 
       *  \brief Constructor of the etheravbFrame initializing its content
       */
      VS_SO_EXPORT etheravbFrame ( svt_byte * data, uint32_t size, TrafficClass_t tc = TC0 );
  
      /*! 
       *  \brief Constructor of the a blank etheravbFrame
       */
      VS_SO_EXPORT etheravbFrame ( void );
        
      /*! 
       *  \brief Assignment operator
       */
      etheravbFrame VS_SO_EXPORT & operator = ( etheravbFrame & frame );
        
      /*! 
       *  \brief Destructor of the etheravbFrame
       */
      VS_SO_EXPORT virtual ~etheravbFrame ( void );

      // ==============================================================================================
      // ================================= Frame Content & manipulation ===============================
      // ==============================================================================================
       
      /*! \fn bool fill ( svt_byte* data, uint32_t size );
       *  \brief Fill the Ethernet Frame with the given amount of specified byte. Overwrite any existing content. 
       *  \param data pointer to a byte buffer containing the data to read
       *  \param size the number of bytes to copy into the Ethernet Frame
       *  \param crc_mode Speccify the CRC mode for the Frame (Auto / Manual)
       *  \param ipg to apply for the frame (-1 for auto management by the core)
       *  \return bool
       *  \retval true after succesfull filling process
       *  \retval false otherwise
       */
      bool VS_SO_EXPORT fill ( svt_byte* data, uint32_t size, CrcMode_t crc_mode = CrcAuto, int ipg = -1 );

      /*! \fn bool getContent ( svt_byte * data, uint32_t size = 1, uint32_t offset = 0 );
       *  \brief Copy part of the ontent of the Ethernet Frame.
       *  \param data A pointer to a buffer where the content of the frame size will be copied
       *  \param size The amount of Frame content to copy
       *  \param offset The offset in the frame content where to start the copy
       *  \return bool
       *  \retval true after succesful frame content copy
       *  \retval false otherwise
       */
      bool VS_SO_EXPORT getContent ( svt_byte * data, uint32_t size = 1, uint32_t offset = 0 );

      /*! \fn const svt_byte * getContent ( uint32_t offset = 0 );
       *  \brief Get a handler to the Ethernet Frame data.
       *  \param offset The offset in the frame content
       *  \return const svt_byte *
       *  \retval The pointer to the Ethernet Frame size content 
       */
      const svt_byte VS_SO_EXPORT * getContent ( uint32_t offset = 0 );

      // ==============================================================================================
      // ============================= Frame Info & Protocol Settings =================================
      // ==============================================================================================

      /*! \fn bool getFrameSize ( uint32_t & size );
       *  \brief Get size of the Ethernet Frame.
       *  \param size reference where frame size will be stored
       *  \return bool
       *  \retval true if Ethernet Frame size is not equel to 0
       *  \retval false if Ethernet Frame size is 0
       */
      bool VS_SO_EXPORT getFrameSize ( uint32_t & size );

      /*! \fn void getSrcAddr (uint64_t& src_addr);
       *  \brief Gets the Frame Source address
       *  \param src_addr reference where Source address will be stored
       *  \retval void
       */
      void VS_SO_EXPORT getSrcAddr (uint64_t & src_addr);

      /*! \fn uint64_t getSrcAddr ( );
       *  \brief Gets the Frame Source address
       *  \return uint64_t
       *  \retval the Frame Source Address
       */
      uint64_t VS_SO_EXPORT getSrcAddr ( );

      /*! \fn void getDestAddr (uint64_t& dest_addr);
       *  \brief Gets the Frame destination address
       *  \param dest_addr reference where Frame destination address will be stored
       *  \retval void
       */
      void VS_SO_EXPORT getDestAddr (uint64_t & dest_addr);

      /*! \fn uint64_t getDestAddr ( );
       *  \brief Gets the Frame destination address
       *  \return uint64_t
       *  \retval the Frame Destination Address
       */
      uint64_t VS_SO_EXPORT getDestAddr ( );

      /*! \fn void setSrcAddr (uint64_t src_addr);
       *  \brief Sets the Frame source address
       *  \param src_addr The Src Address to set
       *  \retval void
       */
      void VS_SO_EXPORT setSrcAddr (uint64_t src_addr);

      /*! \fn void setDestAddr (uint64_t& dest_addr);
       *  \brief Sets the Frame destination address
       *  \param dest_addr The Dest Address to set
       *  \retval void
       */
      void VS_SO_EXPORT setDestAddr (uint64_t dest_addr);

      /*! \fn bool containsCRC ( );
       *  \brief Returns the CRC status for the Frame 
       *  \return bool
       *  \retval true  if the Frame CRC is present in the Frame object (Manual CRC management)
       *  \retval false if the Frame CRC is not present in the Frame object (Auo CRC management)
       */
      bool  VS_SO_EXPORT containsCRC ( );

      /*! \fn void setCRC (uint32_t crc);
       *  \brief Sets the Frame CRC
       *  \param crc The crc to set
       */
      void VS_SO_EXPORT setCRC (uint32_t crc);

      /*! \fn uint32_t getCRC ( );
       *  \brief return the CRC set by setCRC API
       *  \return uint32_t
       *  \retval CRC value 
       */
      uint32_t VS_SO_EXPORT getCRC ( );

      /*! \fn uint32_t calculateCRC ( );
       *  \brief calculates CRC for the frame cuntents set
       *  \return uint32_t
       *  \retval the calculated CRC 
       */
      uint32_t VS_SO_EXPORT calculateCRC ( );
      
      /*! \fn void printFrame ();
       *  \brief Formatted print
       *  \retval void
       */
      void  VS_SO_EXPORT  printFrame ( );

    private :
      etheravbFrameImp* _etheravbFrame;

    };
  }
}

#endif
