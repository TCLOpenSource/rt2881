struct optee_ta {
        struct tee_context *ctx;
        __u32 session;
};


/* 
call the following funtions
optee_se_init() 
optee_se_excute_cmd()
optee_se_deinit()
*/
int call_se_ta_excute_cmd(unsigned char* pbyCommandBuffer, int lCommandLength);
int optee_se_excute_cmd(unsigned char* pbyCommandBuffer, int lCommandLength);
int optee_se_init(void);
void optee_se_deinit(void);
int optee_se_check_cmd(int* se_ready);
int optee_se_unlockmem_cmd(unsigned int src_addr,unsigned int src_BufferSize,unsigned int dst_addr,unsigned int dst_BufferSize);
int optee_se_lockmem_cmd(unsigned int src_addr,unsigned int src_BufferSize,unsigned int dst_addr,unsigned int dst_BufferSize);
int call_se_ta_lock_cmd(unsigned int src_addr,unsigned int src_BufferSize,unsigned int dst_addr,unsigned int dst_BufferSize,bool block);

