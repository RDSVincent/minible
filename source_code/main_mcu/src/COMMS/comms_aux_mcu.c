/*!  \file     comms_aux_mcu.c
*    \brief    Communications with aux MCU
*    Created:  03/03/2018
*    Author:   Mathieu Stephan
*/
#include <asf.h>
#include "comms_hid_msgs_debug.h"
#include "platform_defines.h"
#include "comms_hid_msgs.h"
#include "comms_aux_mcu.h"
#include "defines.h"
#include "dma.h"
aux_mcu_message_t aux_mcu_message_buffer1;
aux_mcu_message_t aux_mcu_message_buffer2;
BOOL aux_mcu_msg_rcv_on_buffer1 = TRUE;


/*! \fn     comms_aux_init(void)
*   \brief  Init communications with aux MCU
*/
void comms_aux_init(void)
{
    dma_aux_mcu_init_rx_transfer((void*)&AUXMCU_SERCOM->USART.DATA.reg, (void*)&aux_mcu_message_buffer1, sizeof(aux_mcu_message_buffer1));
    
    /* For test */
    //aux_mcu_message_buffer2.message_type = 0x0000;
    //dma_aux_mcu_init_tx_transfer((void*)&AUXMCU_SERCOM->USART.DATA.reg, (void*)&aux_mcu_message_buffer2, sizeof(aux_mcu_message_buffer1));
}

/*! \fn     comms_aux_mcu_routine(void)
*   \brief  Routine dealing with aux mcu comms
*/
void comms_aux_mcu_routine(void)
{
    /* Pointer to the currently received message and possible packet to fill as an answer */
    aux_mcu_message_t* rcv_msg_pointer = &aux_mcu_message_buffer2;
    aux_mcu_message_t* tosend_msg_pointer = &aux_mcu_message_buffer1;
    if (aux_mcu_msg_rcv_on_buffer1 != FALSE)
    {
        rcv_msg_pointer = &aux_mcu_message_buffer1;
        tosend_msg_pointer = &aux_mcu_message_buffer2;
    }
	
    /* Did we receive a message? */
    if (dma_aux_mcu_check_and_clear_dma_transfer_flag() != FALSE)
    {
        /* USB / BLE Messages */
        if ((rcv_msg_pointer->message_type == AUX_MCU_MSG_TYPE_USB) || (rcv_msg_pointer->message_type == AUX_MCU_MSG_TYPE_BLE))
        {
            /* Cast payloads into correct type */
            int16_t hid_reply_payload_length = -1;
            hid_message_t* hid_message_pt = (hid_message_t*)rcv_msg_pointer->payload_as_uint32;
            hid_message_t* hid_reply_message_pt = (hid_message_t*)tosend_msg_pointer->payload_as_uint32;
            
            /* Parse message */
            #ifndef DEBUG_USB_COMMANDS_ENABLED
                hid_reply_payload_length = comms_hid_msgs_parse(hid_message_pt, rcv_msg_pointer->payload_length - sizeof(hid_message_pt->message_type) - sizeof(hid_message_pt->payload_length), hid_reply_message_pt);
            #else
                if (hid_message_pt->message_type >= HID_MESSAGE_START_CMD_ID_DBG)
                {
                    hid_reply_payload_length = comms_hid_msgs_parse_debug(hid_message_pt, rcv_msg_pointer->payload_length - sizeof(hid_message_pt->message_type) - sizeof(hid_message_pt->payload_length), hid_reply_message_pt);
                } 
                else
                {
                    hid_reply_payload_length = comms_hid_msgs_parse(hid_message_pt, rcv_msg_pointer->payload_length - sizeof(hid_message_pt->message_type) - sizeof(hid_message_pt->payload_length), hid_reply_message_pt);
                }
            #endif
            
            /* Send reply if needed */
            if (hid_reply_payload_length >= 0)
            {
                /* Reply with same message type */
                tosend_msg_pointer->message_type = rcv_msg_pointer->message_type;
                
                /* Compute payload size */
                tosend_msg_pointer->payload_length = hid_reply_payload_length + sizeof(hid_message_pt->message_type) + sizeof(hid_message_pt->payload_length);
                
                /* Send message */                
                dma_aux_mcu_init_tx_transfer((void*)&AUXMCU_SERCOM->USART.DATA.reg, (void*)tosend_msg_pointer, sizeof(aux_mcu_message_buffer1));
            }
        }
        
        /* Flip buffers */
        aux_mcu_msg_rcv_on_buffer1 = !aux_mcu_msg_rcv_on_buffer1;
    }
}
