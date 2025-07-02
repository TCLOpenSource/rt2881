#ifndef __RTK_MCP_TEST_H_
#define __RTK_MCP_TEST_H_
#include <rbus/timer_reg.h>

#define TIMER_REG_VAL_ADDR  TIMER_SCPU_CLK90K_LO_reg
#define TIMER_REG_CTRL_ADDR  TIMER_CLK90K_CTRL_reg
extern int PrintCheckReuslt(unsigned int enc, unsigned char* pRefData, unsigned char* pOutData, 
                        unsigned int OutDataLen, const char *msg, const char *fun);
extern int MCP_AES_H_DataHashTest(void);
extern int do_mcp_burst_test(unsigned int round);
extern int do_mcp_error_test(unsigned int round);
extern void do_mcp_function_test(void);
extern int do_mcp_throughput_test(unsigned int size);
extern int do_mcp_help(void);
#endif
