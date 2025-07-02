#ifndef __RTICE_AT_INPUT_H__
#define __RTICE_AT_INPUT_H__

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
typedef enum mc_state_e {
        MC_RESET = 0,
        MC_RTICE_ACTIVE      = (1<<1), 	/* starting to receive rtice TPDU */
        MC_RTICE_PENDING     = (1<<2), 	/* rtice TPDU waiting for processing */
        MC_RTICE_TX_LOCKED   = (1<<3), 	/* rtice lock TX for ack transmittion */
        //MC_CONSOLE_UART_EN   = (1<<4),	/* UART as console interfaces */
        //MC_CONSOLE_CDC_EN    = (1<<5),  /* USB CDC as console interfaces */
        //MC_CONSOLE_CDC_READY = (1<<7),  /* USB CDC ready status (Read-only) */
} MC_STATE;


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern UINT8 mc_uart_state;

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define RTICE_COMMAND_BUFFER_SIZE 150

#define rtice_in_debug_mode()  	mc_state_test(MC_RTICE_TX_LOCKED)

#define mc_state_test(state) 	(mc_uart_state & (UINT8)(state))
#define mc_rtice_active()   	do { mc_uart_state |= MC_RTICE_ACTIVE ; } while(0)
#define mc_rtice_inactive()  	do { mc_uart_state &= ~MC_RTICE_ACTIVE; } while(0)
#define mc_rtice_reset()    	do { mc_uart_state &= ~(MC_RTICE_ACTIVE | MC_RTICE_PENDING); } while(0)
#define mc_rtice_process()  	do { mc_uart_state |= (MC_RTICE_PENDING); } while(0)
#define mc_rtice_tx_lock()  	do { mc_uart_state |= (MC_RTICE_TX_LOCKED); } while(0)
#define mc_rtice_tx_unlock() 	do {mc_uart_state &= ~(MC_RTICE_TX_LOCKED); } while(0)
	
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
void mc_rtice_cmd_reset(unsigned char *pm);
void rtice_main(unsigned char *pm);
void rtice_at_command_handler(unsigned char *pm);
void rtice_flow_control_handler(unsigned char *pm);

#endif
