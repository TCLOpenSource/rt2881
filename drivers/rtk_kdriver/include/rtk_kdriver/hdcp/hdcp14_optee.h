#ifndef _HDCP14_OPTEE_H
#define _HDCP14_OPTEE_H

typedef enum {
    HDMI_HDCP_MODE,
    DP_OPTEE_HDCP_MODE,
}HDMI_DP_HDCP_MODE_T;

#define HDCP14_CMD_LOAD_KEY             0x0

#define HDCP14_CMD_DP_AKSV_KSEL_INDEX   0x40
#define HDCP14_CMD_GET_BKSV             0x41

//HDCP TX
#define HDCP14_CMD_LOAD_TX_KEY          0x42
#define HDCP14_CMD_Delete_TX_key        0x43
#define HDCP14_CMD_Certificate_TX_key   0x44
#define HDCP14_CMD_Burn_TX_KEY          0x45  //an14 don't use this


#define HDCP14_CMD_GENERATE_KM          0x66
#define HDCP14_CMD_Certificate_Keybox   2003

extern void optee_hdcp14_load_key(HDMI_DP_HDCP_MODE_T hdmi_dp, unsigned char load_mode);
extern void optee_hdcp14_dp_aksv_ksel_index(unsigned char key_sel_idx);
extern int optee_hdcp14_get_key(unsigned char *bksv, unsigned int bksv_len, unsigned char *bkey, unsigned int bkey_len);
extern int optee_hdcp14_cert_key(unsigned char *dataBuf,unsigned int dataLen, char *device_id, unsigned int *len);
//tx
extern int optee_hdcp14_load_txkey(unsigned char *key, unsigned int key_len);
extern int optee_hdcp14_cert_tx_key(unsigned char *dataBuf, unsigned int dataLen, char *device_id, unsigned int *len);
extern int optee_hdcp14_burn_tx_key(unsigned char *dataBuf, unsigned int dataLen);
extern int optee_hdcp14_del_tx_key(void);
#endif
