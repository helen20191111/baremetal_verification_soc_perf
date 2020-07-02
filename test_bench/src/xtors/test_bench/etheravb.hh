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

#ifndef _ETHERAVB_H_
#define _ETHERAVB_H_

#include <stdint.h>

#include <svdpi.h>
#include <svdpi_src.h>
#include "MPXtorBase.hh"
#include "etheravbCommon.hh"
#include "ethPtpFrame.hh"
#include "etheravbFrame.hh"
#include "Xtor.hh"

using namespace ZEBU_IP;
using namespace ZEBU_MP;
using namespace ETHERAVBFRAME;

namespace ZEBU_IP {
namespace ZEBU_ETHERAVB {

    /*! \User interface to the etheravb XTOR.
     */
    //class etheravb : public etheravbCommon {    
    class etheravb : public Xtor {    
    // ==============================================================================================
    // ================= Constructor / Destructor (Transactor Common API) ===========================   
    // ==============================================================================================
    public :    

    /*! \fn static etheravb* get( const char* path, svt_c_runtime_cfg* runtime = 0 );
     * 
     * Get the singleton etheravb API instance for the etheravb_dpi_top module instance
     * located at the specified path in the hardware platform.
     * If the singleton instance does not yet exists,
     * it is created using the specified runtime configuration.
     * Otherwise, the runtime configuration is ignored.
     *
     * \param path Full HDL path to the RTL driver
     * \param runtime Runtime environment descriptor.
     */
     static etheravb* get(const char* path, svt_c_runtime_cfg* runtime = 0);
    
    private:
    /*! \brief Constructor of the etheravb transactor
     */
    etheravb (const char* path, svt_c_runtime_cfg* runtime = 0);
    etheravb (const char * xtorTypeName, const char * driverName, svt_c_runtime_cfg * runtime, XtorScheduler * sched, bool isDPI = true) ;
    
    /*! \brief Destructor of the etheravb transactor
     */
    ~etheravb ( void );
    
    public:
     // ==============================================================================================
     // ================= Initialization, Save&Restore (Transactor Common API) =======================
     // ==============================================================================================
    
    static void Register(const char * xtorTypeName) ;
    static Xtor * getInstance(const char * xtorTypeName, const char * driverName, XtorScheduler * sched, svt_c_runtime_cfg * runtime = NULL) ;

    /*! \fn const char * getHdlPath( );
     *  \brief Return the HDL path corresponding to this BFM instance
     *  \return const char*
     *  \retval string containing the HDL path 
     */
     const char  * getHdlPath();
    
    /*! \fn const char * getVersion ( void );
     *  \brief Return the current etheravb transactor version
     *  \return const char*
     *  \retval string containing the current etheravb Xactor version
     */
     const char  * getVersion ( void );
    
    /*! \fn bool init ()
     *  \brief Initialize the XTOR
     */
     bool  init ( );
      
    /*! \fn bool runUntilReset ()
     *  \brief Check if the XTOR is done with RESET
     */
     bool  runUntilReset ( );

    /*! \fn  void runClk(uint32_t numClock)
     *  \brief Advance the clock by the specified number of cycles.
     */ 
     void  runClk(uint32_t numClock);

    /*! \fn bool restartAutoNeg ()
     *  \brief restarts auto-negotiation process
     *  \It is non-blocking call, user must call waitForAutoNegComplete() after this to ensure autonegotiation is complete 
     *  \valid only in SGMII mode
     *  \returns bool
     *  \retval returns true if successful
     *  \retval returns false if any error encountered 
     */
     bool  restartAutoNeg ( );

    /*! \fn bool waitForAutoNegComplete ()
     *  \brief waits till auto-negotiation is completed for transactor
     *  \valid only in SGMII mode
     *  \returns bool
     *  \retval returns true if successful
     *  \retval returns false if any error encountered 
     */
     bool   waitForAutoNegComplete ( );

    // ==============================================================================================
    // ============================= Configuration related APIs =====================================
    // ==============================================================================================

    /*! \fn EtheravbReturn_t config ( zetheravbCommonConfig *config);
     *  \brief Configure the etheravb transactor hardware
     *  \param  config The zetheravbCommonConfig object configuration of the transactor
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  config(zetheravbCommonConfig *config);


    /*! \fn EtheravbReturn_t checkConfigValidity( zetheravbCommonConfig *config);
     *  \brief to validate the configuration object for any error
     *  \this can be called before calling config object.
     *  \param  config The zetheravbCommonConfig object configuration of the transactor
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  checkConfigValidity(zetheravbCommonConfig *config);

    /*! \fn EtheravbReturn_t getConfig (zetheravbCommonConfig* config);
     *  \brief Get the Configuration done in the etheravb transactor hardware
     *  \param  config The zetheravbCommonConfig object to get copy of transactor configuration
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  getConfig( zetheravbCommonConfig* config);

   /*! \fn EtheravbReturn_t printConfig (zetheravbCommonConfig* config );
     *  \brief print the configuration provided in the argument
     *  \param  config The zetheravbCommonConfig object to be printed 
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  printConfig(zetheravbCommonConfig* config);

    /*! \fn  void setDebugLevel(uint32_t lvl)
     *  \brief  Sets the verbosity of the display messages to be displayed while run
     *         Input: Verbosity of the messages
     *         4 - FULL
     *         3 - High
     *         2 - Medium
     *         1 - Low
     *         0 - NONE
     */ 
     void  setDebugLevel(uint32_t lvl);

    /*! \fn  void setDebugFile(const char* fname, int control)
     *  \brief  Controls the log files while run
     *  \param fname char* file_name
     *  \param control 0:=create a new file, 1:=append a file, 2: close the file
     */ 
     void  setDebugFile(const char* fname, int control);

    /*! \fn  void setDebugFile(FILE* fname, int control)
     *  \brief  Controls the log files while run
     *  \param fname FILE* file_name
     *  \param control 0/1:Uses the FILE* to redirect log to file
     */ 
     void  setDebugFile(FILE* fname, int control);

    /*! \fn EtheravbReturn_t configWrite (uint32_t addr, uint32_t data);
     *  \brief Write the data at given addr
     *  \param addr where data to be written
     *  \param data data to be written
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  configWrite(uint32_t addr, uint32_t data);

    /*! \fn uint32_t config_read(uint32_t addr);
     *  \brief Read the data from given addr
     *  \param addr address from where data to be read
     *  \return uint32_t
     *  \retval read data
     */
     uint32_t  configRead(uint32_t addr);

    /*! \fn bool setPbl(uint32_t byte_count);
     *  \brief Set the preamble value 
     *  \param byte_count control the preamble byte that are added to the TX packet
     *  \valid values are 7, 5 and 3
     *  \return bool
     *  \retval true if operation happens successfully
     */
      bool  setPbl(uint32_t byte_count);

    /*! \fn bool setIFGValue(uint32_t value);
     *  \brief control the minimum inter frame gap during Tx.
     *  \param value is the IFG value to be set
     *  \value should be between 40 to 96 and multiple of 8
     *  \return bool
     *  \retval true if IFG value is correctly set already
     *  \retval false if IFG value is just set
     */
      bool  setIFGValue(uint32_t value);

    /*! \fn bool setMacAddr(uint32_t mac_addr_high, uint32_t mac_addr_low);
     *  \brief set the MAC address of the station
     *  \param mac_addr_high is the upper 16 bits of the MAC address of the station 
     *  \param mac_addr_low is lower 32 bits of the MAC address of the station
     *  \return bool
     *  \retval true if operation happens successfully
     */
      bool  setMacAddr(uint32_t mac_addr_high, uint32_t mac_addr_low);
    
    
    /*! \fn EtheravbReturn_t sendPauseFrame(uint32_t pause_time);
     *  \brief send the pause frame
     *  \param pause_time specify the time for which tx should pause 
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  sendPauseFrame(uint32_t pause_time);

    /*! \fn EtheravbReturn_t setRxPacketFilter(MacRxPacketCtrlUserStruct mac_packet_filter);
     *  \brief Set the Receive packet filter by the values given by user using MacRxPacketUserStruct structure defined in etheravbCommon.hh file
     *  \param mac_packet_filter is the object of MacRxPacketUserStruct structure contains all register field values
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  setRxPacketFilter(MacRxPacketCtrlUserStruct mac_packet_filter);

     // ==============================================================================================
     // ========================== PTP Configuration related APIs ====================================
     // ==============================================================================================

    /*! \fn bool enableTimestampInterrupt(bool enable);
     *  \brief Enable or disable timestamp interrupt
     *  \param enable is the enable bit, when set, it enables the assertion of the interrupt signal
     *  \return bool
     *  \retval true if operation happens successfully
     */
      bool  enableTimestampInterrupt(bool enable);

    /*! \fn EtheravbReturn_t enableTimestamp(bool enable);
     *  \brief Enable or disable timestamping 
     *  \param enable is the enable bit
     *  \when this bit is set, the timestamp is added for TX and RX packets
     *  \use all the steps mentioned in Doc //todo
     *  \when disabled, timestamp is not added for TX and RX packets  
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  enableTimestamp(bool enable);

   /*! \fn EtheravbReturn_t setTimestampMode(bool enable_external_timestamp);
     *  \brief Enable or disable external timestamping mode. In default, internal timestamping is enabled.
     *  \param enable_external_timestamp true/false indicates if external/internal timestamp is to be enabled. 
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  setTimestampMode(bool enable_external_timestamp);

    /*! \fn bool setPtpClockPeriodInPs(uint32_t period);
     *  \brief set the period of PTP reference clock
     *  \at every clock cycle of PTP reference clock the value in sub second and nanosecond register is added to system time
     *  \param period is the period for clock in pico-seconds
     *  \return bool
     *  \retval true if operation happens successfully
     */
      bool  setPtpClockPeriodInPs (uint32_t period);
    
    /*! \fn bool setTxClockPeriodInPs(uint32_t period);
     *  \brief set the period of Tx clock
     *  \param period is the period for clock in pico-seconds
     *  \return bool
     *  \retval true if operation happens successfully
     */
      bool  setTxClockPeriodInPs (uint32_t period);
    
    /*! \fn EtheravbReturn_t setSystemTime(uint32_t seconds, uint32_t nano_seconds);
     *  \brief initializes or updates the system time maintained by the MAC
     *  \param seconds and nano_seconds represents seconds and nano-seconds value of time
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  setSystemTime (uint32_t seconds, uint32_t nano_seconds);    

    /*! \fn EtheravbReturn_t updateSystemTime(bool add_sub, uint32_t seconds, uint32_t nano_seconds)
     *  \brief Update system time
     *  \param add_sub decides whether to add or subtract given seconds and nanoseconds values from system time
     *  \param seconds Seconds to add or subtract
     *  \param nano_seconds Nano_seconds to add or subtract
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  updateSystemTime (bool add_sub, uint32_t seconds, uint32_t nano_seconds);

    /*! \fn EtheravbReturn_t setPtpMode(bool master_slave);
     *  \brief set the xtor PTP mode, needed for sync-delay path delay mechanism
     *  \param master_slave if set it wil act as master and if reset it will act as slave
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  setPtpMode (bool master_slave);

    /*! \fn EtheravbReturn_t configDirectFilterAccess(uint32_t offset, uint32_t address_high, uint32_t address_low , bool sa_da_bar,uint32_t mask_byte_control=0);
     *  \brief to set the source/destination addresses for packet filtering
     *  \param offset its value should lie between 1-31. there are total 31 filter spaces.
     *  \param address_high: contains 47:32 bits of address.
     *  \param address_low:  contains 31:0 bits of address.
     *  \param sa_da_bar: if true: address is treated as source address else as destination address.
     *  \param mask_byte_control: 6 bit field, each bit corresponds to one byte of address. if x bit is 1, that byte[x] of address is not compared for address filtering
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t       configDirectFilterAccess(uint32_t offset, uint32_t address_high, uint32_t address_low , bool sa_da_bar,uint32_t mask_byte_control=0);

    /*! \fn EtheravbReturn_t configIndirectFilterAccess(uint32_t hash_table_reg1, uint32_t hash_table_reg0);
     *  \brief to configure hash table registers for indirect packet filtering, refer to etherAVB userguide for more details
     *  \param hash_table_reg1: 32 bit (relate to table index 63 to 32), each bit value 1/0 indicates filter pass/fail for given index 
     *  \param hash_table_reg0: 32 bit (relate to table index 31 to  0), each bit value 1/0 indicates filter pass/fail for given index
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t       configIndirectFilterAccess(uint32_t hash_table_reg1, uint32_t hash_table_reg0);

    /*! \fn int getExpectedIndirectFilterIndex( etheravbFrame & frame);
     *  \brief this is a helper function to calculate index of indirect filtering table that will be used for filtering of given frame
     *  \param frame: reference to frame for whcih calculation has to be done
     *  \return int : calculated index for given frame
     */
      int      getExpectedIndirectFilterIndex( etheravbFrame & frame);

    /*! \fn EtheravbReturn_t configVlanFilterAccess(uint32_t offset, VlanTagFilterStruct vlan_filter);
     *  \brief to set the vlan tag filters for packet filtering
     *  \param offset: its value should lie between 1-31. there are total 31 vlan tag filter spaces.
     *  \param vlan_filter: provides filter values to be configured on given offset 
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t       configVlanFilterAccess(uint32_t offset, VlanTagFilterStruct vlan_filter);

     // ==============================================================================================
     // ==============================TSN related configurations======================================
     // ==============================================================================================

    /*! \fn EtheravbReturn_t configGcl( uint32_t *gcl_ptr, EstUserConfigStruct est_config, bool is_blocking = false);
     *  \brief to configure the EST(enhanced scheduling for transmission) registers.
     *  \param gcl_ptr: this is a uint32_t pointer which contains Gate control list. each location contains two fields: { queue_gate_control[3:0], time_interval_in_nanosec[23:0]}
     *  \queue_gate_control[i] when 1: queue is open for transmission, when 0: queue is not allowed for transmission
     *  \param est_config: this is a struct which contains different parameters to control EST functionality. Please go through etheravbCommon.hh for the description of this struct.
     *  \param is_blocking: true indicates the API call should block for configuration to complete, if set false, user can call readEstStatus() API to check status of Gcl configuration
     *  \return EtheravbReturn_t
     */
      EtheravbReturn_t  configGcl( uint32_t *gcl_ptr, EstUserConfigStruct est_config, bool is_blocking =false);

    /*! \fn EtheravbReturn_t disableGcl();
     *  \brief to disable the gate control List (Gcl) and frame preemption (FPE) configured through configGcl() API
     *  \If Gcl needs to be enabled after disabling it through this API, user must call configGcl() again
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  disableGcl();

     /*! \fn bool readEstStatus(EstUserStatusStruct&  status_enum);
       * \brief this api is to get the different status variables related to EST functionality.
       * \param status_enum: For description, please check etheravbCommon.hh
       * \return bool
       * \retval  true if operation happens successfully
       */
      bool  readEstStatus(EstUserStatusStruct&  status_enum);

     /*! \fn bool sendFPECapabilityFrame( bool send_verify_frame, bool send_respond_frame);
       * \brief this api is to send frames used for frame preemption (FPE) capability discovery: Verify and Response
       * \param send_verify_frame: set to true if a Verify frame is to be sent by transactor
       * \param send_response_frame: set to true if a Respond frame should be sent by transactor
       * \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
       */
      EtheravbReturn_t  sendFPECapabilityFrame( bool send_verify_frame = false, bool send_respond_frame = false);

     // ==============================================================================================
     // ================================== User callbacks ============================================
     // ==============================================================================================

    /*! \fn void registerCallbackRxdPacket(void (*read_rxd_frame_cb)(void* xtor, etheravbFrame* frame_ptr), etheravb* xtor);
     *  \brief callback for received frames, arguments are xtor_instance pointer, frame pointer and queue number on which frame is received
     */
     void  registerCallbackRxdPacket(void (*read_rxd_frame_cb)(void* xtor, etheravbFrame* frame_ptr, int queue_num), etheravb* xtor);

    /*! \fn void registerCallbackTxStatus(void (*call_tx_status_cb)(void* xtor, uint32_t* status_ptr, uint32_t bit_size), etheravb* xtor);
     *  \brief callback for tx frame status, from every transmitted frame, user can get tx_status using this API
     *  \param array_size: size of status ptr
     *  \param status_ptr is the poiter to TX status word
     *          status_ptr[0] = transmit_frame_status[17:0]
     *          status_ptr[1] = transmit_frame_status[49:18] ==timestamp[31:0]
     *          status_ptr[2] = transmit_frame_status[81:50] ==timestamp[63:32]
     *  \Please refer to xtor documentation for more detail
     */
     void  registerCallbackTxStatus(void (*call_tx_status_cb)(void* xtor, uint32_t* status_ptr, uint32_t array_size, int queue_num), etheravb* xtor);
      
    /*! \fn void registerCallbackRxStatus(void (*call_rx_status_cb)(void* xtor, uint32_t* status_ptr, uint32_t bit_size), etheravb* xtor);
     *  \brief callback for rx frame status
     *  \param array size: size of status ptr
     *  \param status_ptr is the poiter to RX status word
     *         status_ptr[0]: normal_status
     *         status_ptr[1]: vlan status
     *         status_ptr[2]: extended status
     *         status_ptr[3]: mac_filter status
     *         status_ptr[4]: timestamp_low
     *         status_ptr[5]: timestamp_high
     *  \Please refer to xtor documentation for more detail
     */
     void  registerCallbackRxStatus(void (*call_rx_status_cb)(void* xtor, uint32_t* status_ptr, uint32_t bit_size, int queue_num), etheravb* xtor);

     /*! \fn void registerCallbackFPEPacket(void (*rxd_fpe_frame_cb)(void* xtor,bool verify_frame_received, bool respond_frame_received  ), etheravb* xtor);
     * \brief callback for received FPE frames, arguments are xtor_instance pointer and bolean for type of frame received
     */
     void  registerCallbackFPEPacket(void (*rxd_fpe_frame_cb)(void* xtor, bool verify_frame_received, bool respond_frame_received ), etheravb* xtor);

     // ==============================================================================================
     // =============================== Traffic related APIs =========================================
     // ==============================================================================================

    /*! \fn FrameReturn_t send (etheravbFrame & frame);
     *  \brief to send the frame  
     *  \param frame Reference to a frame object
     *  \return FrameReturn_t
     *  \retval Is an enum with following fields:
     *  \FRAME_SENT, FRAME_RETRY and FRAME_ERROR, Please go through enum definition for more detail
     */
      FrameReturn_t  send ( etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL);

    /*! \fn FrameReturn_t send (etheravbFrame & frame);
     *  \brief Send the frame packet on the queue specified by queue_num 
     *  \param queue_num: queue through which frame should be transmitted
     *  \param wait_for_status: API is blocking(if wait_for_status is high), till receives status from hardware
     *  \param status ptr: user provided uint* for tx_status, should be at least of 3 locations
     *  \return FrameReturn_t
     *  \retval Is an enum with following fields:
     *  \FRAME_SENT, FRAME_RETRY and FRAME_ERROR, Please go through enum definition for more detail
     */
      FrameReturn_t  send ( int queue_num, etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL);

    /*! \fn FrameReturn_t sendPtpFrame (int queue_num, etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL, bool OST_Correction_en = false, bool OST_Avail = false, uint32_t timestamp_7_4 = 0, uint32_t timestamp_3_0 = 0);
     *  \brief Send the frame packet on the queue specified by queue_num 
     *  \param queue_num: queue through which frame should be transmitted
     *  \param wait_for_status: API is blocking, till receives status from hardware
     *  \param status ptr: user provided uint* for tx_status, should be at least of 3 locations
     *  \param OST_Correction_en: Enable for One Step Timestamping Correction feature for the given frame
     *  \param OST_Avail: Indicator that the values provided in timestamp argument are one step timestamp values (applicable for pdelay_resp only), the correction field of frame will be updated with difference of this timestamp and transmit timestamp of this frame 
     *  \param timestamp_7_4: MSB four bytes of the timestamp
     *  \param timestamp_3_0: LSB four bytes of the timestamp
     *  \return FrameReturn_t
     *  \retval Is an enum with following fields:
     *  \FRAME_SENT, FRAME_RETRY and FRAME_ERROR, Please go through enum definition for more detail
     */
      FrameReturn_t  sendPtpFrame ( int queue_num, etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL, bool OST_Correction_en = false, bool OST_Avail = false, uint32_t timestamp_7_4 = 0, uint32_t timestamp_3_0 = 0);

    /*! \fn FrameReturn_t sendPtpFrame (etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL, bool OST_Correction_en = false, bool OST_Avail = false, uint32_t timestamp_7_4 = 0, uint32_t timestamp_3_0 = 0);
     *  \brief Send the frame packet on the queue specified by queue_num 
     *  \param wait_for_status: API is blocking, till receives status from hardware
     *  \param status ptr: user provided uint* for tx_status, should be at least of 3 locations
     *  \param OST_Correction_en: Enable for One Step Timestamping feature for the given frame
     *  \param OST_Avail: Indicator that the values provided in timestamp argument are one step timestamp values (applicable for pdelay_resp only), the correction field of frame will be updated with difference of this timestamp and transmit timestamp of this frame 
     *  \param timestamp_7_4: MSB four bytes of the timestamp
     *  \param timestamp_3_0: LSB four bytes of the timestamp
     *  \return FrameReturn_t
     *  \retval Is an enum with following fields:
     *  \FRAME_SENT, FRAME_RETRY and FRAME_ERROR, Please go through enum definition for more detail
     */
      FrameReturn_t  sendPtpFrame ( etheravbFrame & frame, bool wait_for_status=false, uint32_t* status_ptr=NULL, bool OST_Correction_en = false, bool OST_Avail = false, uint32_t timestamp_7_4 = 0, uint32_t timestamp_3_0 = 0);

    /*! \fn EtheravbReturn_t createFrame (packetStruct packet, etheravbFrame *frame_ptr);
     *  \brief Creates the frame to be sent. 
     *  \param packet The packet is a packetStruct object which contains all the frame data.
     *  \param frame_ptr The frame in which all the data will be copied.
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  createFrame (packetStruct packet, etheravbFrame *frame_ptr);

    /*! \fn EtheravbReturn_t sendMDIOTxn (MDIOTxn_t* mdio_txn, MDIOData_t* mdio_data );
     *  \brief send MDIO txn on MDIO interface, this is a blocking API
     *  \param mdio_txn: mdio transaction to be performed
     *  \param mdio_data: mdio data read/to be written in case of Read/Write and Reg_addr in case of Clause45  
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */    
     EtheravbReturn_t  sendMDIOTxn (MDIOTxn_t* mdio_txn, MDIOData_t* mdio_data );

     // ==============================================================================================
     // =============================== Debug related APIs ===========================================
     // ==============================================================================================

    /*! \fn void dumpFrame ( etheravbFrame &frame );
     *  \brief Dump the frame information
     *  \param frame Reference to a frame object
     */
     void  dumpFrame ( etheravbFrame &frame );

    /*! \fn EtheravbReturn_t compareFrame(etheravbFrame* ref_frame_ptr, etheravbFrame* rx_frame_ptr);
     *  \brief Compare the size and content of TX and RX frame
     *  \param ref_frame_ptr pointer to TX frame refrence
     *  \param rx_frame_ptr pointer to RX frame
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  compareFrame(etheravbFrame* ref_frame_ptr, etheravbFrame* rx_frame_ptr);

    /*! \fn EtheravbReturn_t getFrameCount(frameStats_s& frame_stats );
     *  \brief Get the count of transmitted/received frames by the transactor
     *  \param frameStats_s is the struct object to be populated  
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  getFrameCount( frameStats_s& frame_stats); 
    
    /*! \fn int getExpectedRxQueueNum (etheravbFrame& frame_ptr, bool is_preemptable);
     *  \brief returns expected Rx queue based on frame type and configuration done 
     *  \param frame Reference to a frame object
     *  \param is_preemptable indicates if the frame is a preemptable frame
     *  \return int
     *  \retval specifies the queue number
     */
     int	 getExpectedRxQueueNum(etheravbFrame& frame, bool is_preemptable = false);

    /*! \fn const char* etheravbReturnTypeName (EtheravbReturn_t enum_val);
      * \brief returns the string for the corresponding enum value;
      * \ return const char*
     */
      const char  * etheravbReturnTypeName(EtheravbReturn_t enum_val);

    // ==============================================================================================
    // =============================== PCAP Reader related APIs =====================================
    // ==============================================================================================

    /*! \fn EtheravbReturn_t openReader(const char *fileNameBase = "ReaderGmacXtor.PCap.txt", bool loop = false) ;
     *  \brief Open the file for reading frames 
     *  \param fileNameBase defines the text file converted from PCAP dump file using wireshark tool
     *  \param loop Rewind to first frame when end of file reached
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  openReader (const char *fileNameBase = "ReaderGmacXtor.PCap.txt", bool loop = false) ;
    
    /*! \fn EtheravbReturn_t closeReader (  ) ;
     *  \brief Close the file opened through openReader() API
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  closeReader( );

    /*! \fn void setReaderMaxFrame( uint32_t maxFrames );
     *  \brief Defines the maximum total number of frames to be played.
     *  \param maxFrames max frames to be played
     */
     void  setReaderMaxFrame( uint32_t maxFrames ); 

    /*! \fn EtheravbReturn_t readPacket(etheravbFrame & frame);
     *  \brief Read the next frame packet from the PCAP Text file and sent it to the Port 
     *  \param frame Reference to a frame object
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  readPacket(etheravbFrame& frame);
 
    // ==============================================================================================
    // ================================ PCAP Writer related APIs ====================================
    // ==============================================================================================

    /*! \fn EtheravbReturn_t openWriter(const char *fileNameBase = "WriteGmacXtor.PCap.txt", WriteFileMode_t mode);
     *  \brief Open the file for dumping frames in Wireshark readable text format 
     *  \param fileNameBase defines the filename to be used for dumping frames
     *  \param mode indicates if WallClockTime of receiving frame needs to be dumped as well
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
      EtheravbReturn_t  openWriter(const char *fileNameBase = "WriterGmacXtor.PCap.txt", WriteFileMode_t mode = Write_Timed) ;

    /*! \fn EtheravbReturn_t closeWriter (  ) ;
     *  \brief Close the file opened through openWrite() API
     *  \return EtheravbReturn_t: Please go through etheravbCommon.hh for enum type description.
     */
     EtheravbReturn_t  closeWriter( );

    /*! \fn void setWriterMaxFrame( uint32_t maxFrames );
     *  \brief Defines the maximum total number of frames to be dumped.
     *  \param maxFrames max frames to be played
     */
      void  setWriterMaxFrame( uint32_t maxFrames );

    /*! \fn void enablePCAPTxtDump( );
     *  \brief enable/disable dumping of received packet to file opened through openWrite() 
     *  \File for dumping Frames must be opened through openWriter() API before calling enablePCAPTxtDump() API
     *  \param enable indicating if dump to be enabled/disabled 
     */
     void  enablePCAPTxtDump(bool enable);

    // ==============================================================================================
    // ============================= APIs for user related calculations =============================
    // ==============================================================================================

    /*! \fn uint32_t ptpPathDelayCalc(uint32_t t1_high, uint32_t t1_low, uint32_t t2_high, uint32_t t2_low, uint32_t t3_high, uint32_t t3_low, uint32_t t4_high, uint32_t t4_low);
     *  \brief Calculates the mean propagation delay i.e. ((t2-t1)+(t4-t3))/2
     *  \param t(1-4)_high and t(1-4)_low are the time high and low values
     *  \return uint32_t
     *  \retval path delay
     */
       uint32_t   ptpPathDelayCalc (uint32_t t1_high, uint32_t t1_low,
                                                         uint32_t t2_high, uint32_t t2_low,
                                                         uint32_t t3_high, uint32_t t3_low,
                                                         uint32_t t4_high, uint32_t t4_low);
   
    /*! \fn uint32_t ptpPathDelayCalc(uint32_t t1_high, uint32_t t1_low, uint32_t t3_t2_diff_high, uint32_t t3_t2_diff_low, uint32_t t4_high, uint32_t t4_low);
     *  \brief Calculates the mean propagation delay i.e. ((t4-t1)-(t3_t2_diff))/2
     *  \param t3_t2_diff is the time difference of t3-t2 received in one step pdelay packets
     *  \return uint32_t
     *  \retval path delay
     */
       uint32_t   ptpPathDelayCalc(uint32_t t1_high, uint32_t t1_low,
                                                        uint32_t t3_t2_diff_high, uint32_t t3_t2_diff_low,
                                                        uint32_t t4_high, uint32_t t4_low);
 
    /*! \fn bool calcClkOffsetCorrection(uint32_t t1_high, uint32_t t1_low, uint32_t t2_high, uint32_t t2_low, uint32_t path_delay, uint32_t& clk_offset_high, uint32_t& clk_offset_low);
     *  \brief Calculates clock offset i.e. t2-t1-pathdelay
     *  \param t(1-2)_high and t(1-2)_low are the time high and low values
     *  \param path_delay is the mean propagation delay which we get using ptp_delay_calc() API
     *  \param clk_offset_high and clk_offset_low is the corrected clock offset high and low values
     *  \return bool
     *  \retval true: value is to be added
     *  \retval false: value is to be substracted
     */
       bool  calcClkOffsetCorrection (uint32_t t1_high, uint32_t t1_low, 
                                                          uint32_t t2_high, uint32_t t2_low,
                                                          uint32_t path_delay,
                                                          uint32_t& clk_offset_high, uint32_t& clk_offset_low);

    private :
      class etheravbImp  *m_imp;
};
  
} // ZEBU_ETHERAVB
} // ZEBU_IP
#endif
