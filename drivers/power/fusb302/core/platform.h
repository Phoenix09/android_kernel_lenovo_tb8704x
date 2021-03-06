/* platform.h
 *
 * THIS FILE DEFINES EVERY PLATFORM-DEPENDENT ELEMENT THAT THE CORE REQUIRES.
 * 
 * INSTRUCTIONS FOR THIS FILE:
 * 1. Modify this file with a definition of Generic Type Definitions
 * (FSC_S8, FSC_U32, etc) Either by include or putting directly in this file.
 * 2. Include this as a header file for your platform.c and implement the 
 * function headers as defined below.
 * 
 * It is the driver-writer's responsibility to implement each function
 * stub and to allocate/initialize/reserve sufficient system resources.
 * 
 */
#ifndef _FSC_PLATFORM_H_
#define _FSC_PLATFORM_H_

/* PLATFORM_NONE
 * 
 * This is a set of stubs for no platform in particular.
 */
#ifdef PLATFORM_NONE
#include "../Platform_None/FSCTypes.h"
#endif // PLATFORM_NONE

/* PLATFORM_PIC32
 * 
 * This platform is for the Microchip PIC32 microcontroller.
 */
#ifdef PLATFORM_PIC32
#include "../Platform_PIC32/GenericTypeDefs.h"
#endif // PLATFORM_PIC32

/* PLATFORM_ARM
 *
 * This platform is for the ARM M0.
 */
#ifdef PLATFORM_ARM
#include "../Platform_ARM/app/FSCTypes.h"
#endif // PLATFORM_ARM

/* PLATFORM_ARM_M7
 *
 * This platform is for the ARM M7.
 */
#ifdef PLATFORM_ARM_M7
#include "../Platform_ARM_M7/app/FSCTypes.h"
#endif // PLATFORM_ARM_M7

/* FSC_PLATFORM_LINUX
 * 
 * This platform is for the Linux kernel driver.
 */
#ifdef FSC_PLATFORM_LINUX
#include "../Platform_Linux/FSCTypes.h"
#endif // FSC_PLATFORM_LINUX

#include <linux/gpio.h>
//#include <linux/interrupt.h>
//#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/power_supply.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>
#include <linux/slab.h>
#include <linux/spmi.h>
#include <linux/usb/class-dual-role.h>


typedef struct {
    FSC_U16 start_time;
    FSC_U16 timeout;
} TIMER;

typedef enum {
    VBUS_LVL_5V,
  /*VBUS_LVL_9V, */
   // VBUS_LVL_12V, //add by longcheer_liml
  /*VBUS_LVL_15V,*/
  /*VBUS_LVL_20V,*/
    VBUS_LVL_COUNT,
    VBUS_LVL_ALL = 99
} VBUS_LVL;

typedef enum {
    CC1,
    CC2,
    NONE,
} CC_ORIENTATION;

typedef enum {
    SINK = 0,
    SOURCE
} SourceOrSink;

struct typec_wakeup_source {
	struct wakeup_source	source;
	unsigned long		enabled;
};


struct qpnp_typec_chip {
	struct device		*dev;
	struct spmi_device	*spmi;
	struct power_supply	*batt_psy;
	struct power_supply	type_c_psy;
	struct regulator	*ss_mux_vreg;
	struct mutex		typec_lock;
	spinlock_t		rw_lock;

	u16			base;

	/* IRQs */
	int			vrd_changed;
	int			ufp_detach;
	int			ufp_detect;
	int			dfp_detach;
	int			dfp_detect;
	int			vbus_err;
	int			vconn_oc;

	/* Configurations */
	int			cc_line_state;
	int			current_ma;
	int			ssmux_gpio;
	enum of_gpio_flags	gpio_flag;
	int			typec_state;

	/* Dual role support */
	bool				role_reversal_supported;
	bool				in_force_mode;
	int				force_mode;
	struct dual_role_phy_instance	*dr_inst;
	struct dual_role_phy_desc	dr_desc;
	struct delayed_work		role_reversal_check;
	struct typec_wakeup_source	role_reversal_wakeup_source;
};


/*******************************************************************************
* Function:        platform_set/get_vbus_lvl_enable
* Input:           level - specify the vbus level
*                  blnEnable - enable or disable the specified vbus level
*                  blnDisableOthers - if true, ensure all other levels are off
* Return:          Boolean - State of vbus
* Description:     Provide access to the VBUS control pin(s).
******************************************************************************/
void platform_set_vbus_lvl_enable(VBUS_LVL level, FSC_BOOL blnEnable, FSC_BOOL blnDisableOthers);
FSC_BOOL platform_get_vbus_lvl_enable(VBUS_LVL level);

/*******************************************************************************
 * Function:        platform_set_vbus_discharge
 * Input:           Boolean
 * Return:          None
 * Description:     Enable/Disable Vbus Discharge Path
 ******************************************************************************/
void platform_set_vbus_discharge(FSC_BOOL blnEnable);

/*******************************************************************************
 * Function:        platform_get_device_irq_state
 * Input:           None
 * Return:          Boolean.  TRUE = Interrupt Active
 * Description:     Get the state of the INT_N pin.  INT_N is active low.  This
 *                  function handles that by returning TRUE if the pin is
 *                  pulled low indicating an active interrupt signal.
 ******************************************************************************/
FSC_BOOL platform_get_device_irq_state( void );
        
/*******************************************************************************
 * Function:        platform_i2c_write
 * Input:           SlaveAddress - Slave device bus address
 *                  RegAddrLength - Register Address Byte Length
 *                  DataLength - Length of data to transmit
 *                  PacketSize - Maximum size of each transmitted packet
 *                  IncSize - Number of bytes to send before incrementing addr
 *                  RegisterAddress - Internal register address
 *                  Data - Buffer of char data to transmit
 * Return:          Error state
 * Description:     Write a char buffer to the I2C peripheral.
 ******************************************************************************/
FSC_BOOL platform_i2c_write(FSC_U8 SlaveAddress,
                            FSC_U8 RegAddrLength,
                            FSC_U8 DataLength,
                            FSC_U8 PacketSize,
                            FSC_U8 IncSize,
                            FSC_U32 RegisterAddress,
                            FSC_U8* Data);

/*******************************************************************************
 * Function:        platform_i2c_read
 * Input:           SlaveAddress - Slave device bus address
 *                  RegAddrLength - Register Address Byte Length
 *                  DataLength - Length of data to attempt to read
 *                  PacketSize - Maximum size of each received packet
 *                  IncSize - Number of bytes to recv before incrementing addr
 *                  RegisterAddress - Internal register address
 *                  Data - Buffer for received char data
 * Return:          Error state.
 * Description:     Read char data from the I2C peripheral.
 ******************************************************************************/
FSC_BOOL platform_i2c_read( FSC_U8 SlaveAddress,
                            FSC_U8 RegAddrLength,
                            FSC_U8 DataLength,
                            FSC_U8 PacketSize,
                            FSC_U8 IncSize,
                            FSC_U32 RegisterAddress,
                            FSC_U8* Data);

/*****************************************************************************
* Function:        platform_enable_timer
* Input:           enable - TRUE to enable platform timer, FALSE to disable
* Return:          None
* Description:     Enables or disables platform timer
******************************************************************************/
void platform_enable_timer(FSC_BOOL enable);

/******************************************************************************
 * Function:        platform_delay_10us
 * Input:           delayCount - Number of 10us delays to wait
 * Return:          None
 * Description:     Perform a blocking software delay in intervals of 10us
 ******************************************************************************/
void platform_delay_10us( FSC_U32 delayCount );

/******************************************************************************
 * Function:        platform_get_system_time
 * Input:           None
 * Return:          Returns a running global unsigned 16-bit time in ms.
 *                  The global timer may sleep when platform_enable_timer(FALSE)
 *                  is called.
 * Description:     To get the system time
 ******************************************************************************/
FSC_U16 platform_get_system_time(void);

/******************************************************************************
 * Function:        platform_set_timer
 * Input:           timer - TIMER struct for a partcular timer
 *                  timeout - timeout value in ms; 0xFFFF is used to disable a timer
 * Return:          None
 * Description:     Starts a particular timer
 ******************************************************************************/
void platform_set_timer(TIMER *timer, FSC_U16 timeout);

/******************************************************************************
 * Function:        platform_check_timer
 * Input:           timer - TIMER struct for a partcular timer
 * Return:          Boolean: TRUE if the timer has expired
 *                           FALSE if the timer has not expired or is disabled
 * Description:     Determine if a partcular timer has expired
 ******************************************************************************/
FSC_BOOL platform_check_timer(TIMER *timer);

/*******************************************************************************
* Function:        platform_notify_cc_orientation
* Input:           orientation - Orientation of CC (NONE, CC1, CC2)
* Return:          None
* Description:     A callback used by the core to report to the platform the 
*                  current CC orientation. Called in SetStateAttached... and
*                  SetStateUnattached functions.                 
******************************************************************************/
void platform_notify_cc_orientation(CC_ORIENTATION orientation);

/*******************************************************************************
* Function:        platform_notify_pd_contract
* Input:           contract - TRUE: Contract, FALSE: No Contract
* Return:          None
* Description:     A callback used by the core to report to the platform the 
*                  current PD contract status. Called in PDPolicy.
*******************************************************************************/
void platform_notify_pd_contract(FSC_BOOL contract);

/*******************************************************************************
* Function:        platform_notify_unsupported_accessory
* Input:           None
* Return:          None
* Description:     A callback used by the core to report entry to the
*                  Unsupported Accessory state. The platform may implement
*                  USB Billboard.
*******************************************************************************/
void platform_notify_unsupported_accessory(void);

/*******************************************************************************
* Function:        platform_set_data_role
* Input:           PolicyIsDFP - Current data role
* Return:          None
* Description:     A callback used by the core to report the new data role after
*                  a data role swap.
*******************************************************************************/
void platform_set_data_role(FSC_BOOL PolicyIsDFP);

/*******************************************************************************
* Function:        platform_notify_bist
* Input:           bistEnabled - TRUE when BIST enabled, FALSE when disabled
* Return:          None
* Description:     A callback that may be used to limit current sinking during
*                  BIST 
*******************************************************************************/
void platform_notify_bist(FSC_BOOL bistEnabled);
extern struct qpnp_typec_chip *tusb_typec_chip;
#endif  // _FSC_PLATFORM_H_
