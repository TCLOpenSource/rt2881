include ../../../../../../audio/src/Integrated/src/MakeConfig
LIBNAME= ../../../lib/libaudiorpc.a

#DEFS=

INCLUDES +=  -I. -I../../include/audio \
	-I../../../include -I ../../../../../../audio/src/Integrated/include/rpc  \
	-I ../../../../../../audio/src/Integrated/include/

INCLUDES += -I../../../../../inc
DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= $(DEFINES) $(COMPILE_OPT)

SRCS = AudioRPC_Agent_clnt.c AudioRPC_System_svc.c AudioRPCBaseDS_xdr.c AudioRPC_Agent_xdr.c AudioRPC_System_xdr.c RPCBaseDS_xdr.c
OBJS = AudioRPC_Agent_clnt.o AudioRPC_System_svc.o AudioRPCBaseDS_xdr.o AudioRPC_Agent_xdr.o AudioRPC_System_xdr.o RPCBaseDS_xdr.o

HEADERS= defs.h

%.o : %.c
	@echo "    CC $<"
	@$(rm) -f $@
	@$(CC) $(CFLAGS) $(WARNING) $(DEBUGFLAG) -c $*.c

all: $(LIBNAME)

$(LIBNAME) : $(OBJS)
	$(rm) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@
clean:
	$(rm) $(OBJS) $(LIBNAME) core *~
