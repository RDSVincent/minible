/*!  \file     platform_io.c
*    \brief    Platform IO related functions
*    Created:  10/11/2017
*    Author:   Mathieu Stephan
*/
#include <asf.h>
#include "platform_defines.h"
#include "driver_sercom.h"
#include "driver_clocks.h"
#include "driver_timer.h"
#include "platform_io.h"


/*! \fn     platform_io_release_aux_reset(void)
*   \brief  Release aux MCU reset
*/
void platform_io_release_aux_reset(void)
{
     PORT->Group[MCU_AUX_RST_EN_GROUP].OUTCLR.reg = MCU_AUX_RST_EN_MASK;
}

/*! \fn     platform_io_enable_ble(void)
*   \brief  Enable BLE module
*/
void platform_io_enable_ble(void)
{
    PORT->Group[BLE_EN_GROUP].OUTSET.reg = BLE_EN_MASK;
}

/*! \fn     platform_io_init_smc_ports(void)
*   \brief  Basic initialization of SMC ports at boot
*/
void platform_io_init_smc_ports(void)
{
    PORT->Group[SMC_DET_GROUP].DIRCLR.reg = SMC_DET_MASK;               // Setup card detection input with pull-up
    PORT->Group[SMC_DET_GROUP].OUTSET.reg = SMC_DET_MASK;               // Setup card detection input with pull-up    
    PORT->Group[SMC_DET_GROUP].PINCFG[SMC_DET_PINID].bit.PULLEN = 1;    // Setup card detection input with pull-up
    PORT->Group[SMC_DET_GROUP].PINCFG[SMC_DET_PINID].bit.INEN = 1;      // Setup card detection input with pull-up
    PORT->Group[SMC_POW_NEN_GROUP].DIRSET.reg = SMC_POW_NEN_MASK;       // Setup power enable, disabled by default
    PORT->Group[SMC_POW_NEN_GROUP].OUTSET.reg = SMC_POW_NEN_MASK;       // Setup power enable, disabled by default    
}

/*! \fn     platform_io_smc_remove_function(void)
*   \brief  Function called when smartcard is removed
*/
void platform_io_smc_remove_function(void)
{
    PORT->Group[SMC_POW_NEN_GROUP].OUTSET.reg = SMC_POW_NEN_MASK;       // Deactivate power to the smart card
    PORT->Group[SMC_PGM_GROUP].DIRCLR.reg = SMC_PGM_MASK;               // Setup all output pins as tri-state
    PORT->Group[SMC_RST_GROUP].DIRCLR.reg = SMC_RST_MASK;               // Setup all output pins as tri-state
    PORT->Group[SMC_SCK_GROUP].PINCFG[SMC_SCK_PINID].bit.PMUXEN = 0;    // Disable SPI functionality
    PORT->Group[SMC_MOSI_GROUP].PINCFG[SMC_MOSI_PINID].bit.PMUXEN = 0;  // Disable SPI functionality
    PORT->Group[SMC_MISO_GROUP].PINCFG[SMC_MISO_PINID].bit.PMUXEN = 0;  // Disable SPI functionality    
    //card_powered = FALSE;
}

/*! \fn     platform_io_init_flash_ports(void)
*   \brief  Initialize the platform flash IO ports
*/
void platform_io_init_flash_ports(void)
{    
    /* DATAFLASH */
    PORT->Group[DATAFLASH_nCS_GROUP].DIRSET.reg = DATAFLASH_nCS_MASK;                                                       // DATAFLASH nCS, OUTPUT high by default
    PORT->Group[DATAFLASH_nCS_GROUP].OUTSET.reg = DATAFLASH_nCS_MASK;                                                       // DATAFLASH nCS, OUTPUT high by default
    PORT->Group[DATAFLASH_SCK_GROUP].DIRSET.reg = DATAFLASH_SCK_MASK;                                                       // DATAFLASH SCK, OUTPUT
    PORT->Group[DATAFLASH_SCK_GROUP].PINCFG[DATAFLASH_SCK_PINID].bit.PMUXEN = 1;                                            // Enable peripheral multiplexer
    PORT->Group[DATAFLASH_SCK_GROUP].PMUX[DATAFLASH_SCK_PINID/2].bit.DATAFLASH_SCK_PMUXREGID = DATAFLASH_SCK_PMUX_ID;       // DATAFLASH SCK, OUTPUT
    PORT->Group[DATAFLASH_MOSI_GROUP].DIRSET.reg = DATAFLASH_MOSI_MASK;                                                     // DATAFLASH MOSI, OUTPUT
    PORT->Group[DATAFLASH_MOSI_GROUP].PINCFG[DATAFLASH_MOSI_PINID].bit.PMUXEN = 1;                                          // Enable peripheral multiplexer
    PORT->Group[DATAFLASH_MOSI_GROUP].PMUX[DATAFLASH_MOSI_PINID/2].bit.DATAFLASH_MOSI_PMUXREGID = DATAFLASH_MOSI_PMUX_ID;   // DATAFLASH MOSI, OUTPUT
    PORT->Group[DATAFLASH_MISO_GROUP].DIRCLR.reg = DATAFLASH_MISO_MASK;                                                     // DATAFLASH MISO, INPUT
    PORT->Group[DATAFLASH_MISO_GROUP].PINCFG[DATAFLASH_MISO_PINID].bit.PMUXEN = 1;                                          // Enable peripheral multiplexer
    PORT->Group[DATAFLASH_MISO_GROUP].PMUX[DATAFLASH_MISO_PINID/2].bit.DATAFLASH_MISO_PMUXREGID = DATAFLASH_MISO_PMUX_ID;   // DATAFLASH MOSI, OUTPUT 
    PM->APBCMASK.bit.DATAFLASH_APB_SERCOM_BIT = 1;                                                                          // APB Clock Enable
    clocks_map_gclk_to_peripheral_clock(GCLK_ID_48M, DATAFLASH_GCLK_SERCOM_ID);                                             // Map 48MHz to SERCOM unit
    sercom_spi_init(DATAFLASH_SERCOM, DATAFLASH_BAUD_DIVIDER, SPI_MODE0, SPI_HSS_DISABLE, DATAFLASH_MISO_PAD, DATAFLASH_MOSI_SCK_PADS, TRUE);    

    /* DBFLASH */
    PORT->Group[DBFLASH_nCS_GROUP].DIRSET.reg = DBFLASH_nCS_MASK;                                                           // DBFLASH nCS, OUTPUT high by default
    PORT->Group[DBFLASH_nCS_GROUP].OUTSET.reg = DBFLASH_nCS_MASK;                                                           // DBFLASH nCS, OUTPUT high by default
    PORT->Group[DBFLASH_SCK_GROUP].DIRSET.reg = DBFLASH_SCK_MASK;                                                           // DBFLASH SCK, OUTPUT
    PORT->Group[DBFLASH_SCK_GROUP].PINCFG[DBFLASH_SCK_PINID].bit.PMUXEN = 1;                                                // Enable peripheral multiplexer
    PORT->Group[DBFLASH_SCK_GROUP].PMUX[DBFLASH_SCK_PINID/2].bit.DBFLASH_SCK_PMUXREGID = DBFLASH_SCK_PMUX_ID;               // DBFLASH SCK, OUTPUT
    PORT->Group[DBFLASH_MOSI_GROUP].DIRSET.reg = DBFLASH_MOSI_MASK;                                                         // DBFLASH MOSI, OUTPUT
    PORT->Group[DBFLASH_MOSI_GROUP].PINCFG[DBFLASH_MOSI_PINID].bit.PMUXEN = 1;                                              // Enable peripheral multiplexer
    PORT->Group[DBFLASH_MOSI_GROUP].PMUX[DBFLASH_MOSI_PINID/2].bit.DBFLASH_MOSI_PMUXREGID = DBFLASH_MOSI_PMUX_ID;           // DBFLASH MOSI, OUTPUT
    PORT->Group[DBFLASH_MISO_GROUP].DIRCLR.reg = DBFLASH_MISO_MASK;                                                         // DBFLASH MISO, INPUT
    PORT->Group[DBFLASH_MISO_GROUP].PINCFG[DBFLASH_MISO_PINID].bit.PMUXEN = 1;                                              // Enable peripheral multiplexer
    PORT->Group[DBFLASH_MISO_GROUP].PMUX[DBFLASH_MISO_PINID/2].bit.DBFLASH_MISO_PMUXREGID = DBFLASH_MISO_PMUX_ID;           // DBFLASH MOSI, OUTPUT
    PM->APBCMASK.bit.DBFLASH_APB_SERCOM_BIT = 1;                                                                            // APB Clock Enable
    clocks_map_gclk_to_peripheral_clock(GCLK_ID_48M, DBFLASH_GCLK_SERCOM_ID);                                               // Map 48MHz to SERCOM unit
    sercom_spi_init(DBFLASH_SERCOM, DBFLASH_BAUD_DIVIDER, SPI_MODE0, SPI_HSS_DISABLE, DBFLASH_MISO_PAD, DBFLASH_MOSI_SCK_PADS, TRUE);
}

/*! \fn     platform_io_init_oled_ports(void)
*   \brief  Initialize the platform oled IO ports
*/
void platform_io_init_oled_ports(void)
{
    PORT->Group[OLED_HV_EN_GROUP].DIRSET.reg = OLED_HV_EN_MASK;                                         // OLED HV enable, OUTPUT low by default
    PORT->Group[OLED_HV_EN_GROUP].OUTCLR.reg = OLED_HV_EN_MASK;                                         // OLED HV enable, OUTPUT low by default
    PORT->Group[OLED_nRESET_GROUP].DIRSET.reg = OLED_nRESET_MASK;                                       // OLED nRESET, OUTPUT
    PORT->Group[OLED_nRESET_GROUP].OUTCLR.reg = OLED_nRESET_MASK;                                       // OLED nRESET, asserted
    PORT->Group[OLED_nCS_GROUP].DIRSET.reg = OLED_nCS_MASK;                                             // OLED nCS, OUTPUT high by default
    PORT->Group[OLED_nCS_GROUP].OUTSET.reg = OLED_nCS_MASK;                                             // OLED nCS, OUTPUT high by default
    PORT->Group[OLED_CD_GROUP].DIRSET.reg = OLED_CD_MASK;                                               // OLED CD, OUTPUT high by default
    PORT->Group[OLED_CD_GROUP].OUTSET.reg = OLED_CD_MASK;                                               // OLED CD, OUTPUT high by default
    PORT->Group[OLED_SCK_GROUP].DIRSET.reg = OLED_SCK_MASK;                                             // OLED SCK, OUTPUT
    PORT->Group[OLED_SCK_GROUP].PINCFG[OLED_SCK_PINID].bit.PMUXEN = 1;                                  // Enable peripheral multiplexer
    PORT->Group[OLED_SCK_GROUP].PMUX[OLED_SCK_PINID/2].bit.OLED_SCK_PMUXREGID = OLED_SCK_PMUX_ID;       // OLED SCK, OUTPUT
    PORT->Group[OLED_MOSI_GROUP].DIRSET.reg = OLED_MOSI_MASK;                                           // OLED MOSI, OUTPUT
    PORT->Group[OLED_MOSI_GROUP].PINCFG[OLED_MOSI_PINID].bit.PMUXEN = 1;                                // Enable peripheral multiplexer
    PORT->Group[OLED_MOSI_GROUP].PMUX[OLED_MOSI_PINID/2].bit.OLED_MOSI_PMUXREGID = OLED_MOSI_PMUX_ID;   // OLED MOSI, OUTPUT
    PM->APBCMASK.bit.OLED_APB_SERCOM_BIT = 1;                                                           // Enable SERCOM APB Clock Enable
    clocks_map_gclk_to_peripheral_clock(GCLK_ID_48M, OLED_GCLK_SERCOM_ID);                              // Map 48MHz to SERCOM unit
    sercom_spi_init(OLED_SERCOM, OLED_BAUD_DIVIDER, SPI_MODE0, SPI_HSS_DISABLE, OLED_MISO_PAD, OLED_MOSI_SCK_PADS, FALSE);
    timer_delay_ms(10);                                                                                 // OLED reset pulse
    PORT->Group[OLED_nRESET_GROUP].OUTSET.reg = OLED_nRESET_MASK;                                       // OLED nRESET, released    
}

/*! \fn     platform_io_init_power_ports(void)
*   \brief  Initialize the platform power ports
*/
void platform_io_init_power_ports(void)
{
    //
}

/*! \fn     platform_io_init_ports(void)
*   \brief  Initialize the platform IO ports
*/
void platform_io_init_ports(void)
{    
    /* Power */
    platform_io_init_power_ports();
    
    /* OLED display */
    platform_io_init_oled_ports();
    
    /* External Flash */
    platform_io_init_flash_ports();

    /* AUX MCU, reset by default */
    PORT->Group[MCU_AUX_RST_EN_GROUP].DIRSET.reg = MCU_AUX_RST_EN_MASK;
    PORT->Group[MCU_AUX_RST_EN_GROUP].OUTSET.reg = MCU_AUX_RST_EN_MASK;
    platform_io_release_aux_reset();

    /* BLE enable, disabled by default */
    PORT->Group[BLE_EN_GROUP].DIRSET.reg = BLE_EN_MASK;
    PORT->Group[BLE_EN_GROUP].OUTCLR.reg = BLE_EN_MASK;
    platform_io_enable_ble();

    /* Smartcards port */
    platform_io_init_smc_ports();
}