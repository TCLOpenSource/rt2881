include ../../../audio/src/Integrated/src/MakeConfig

LIBNAME= $(LIBDIR)/librtleventlog.a

DEFINES = $(INCLUDES) $(DEFS)

INCLUDES += -I. -I./public -I./../../../audio/src/Integrated/include -I./../../../audio/src/Integrated/include/rpc -I./../../../audio/src/Integrated/src/rtleventlog
CFLAGS += $(INCLUDES) $(COMPILE_OPTION) $(DEFINES) -O2

SRCS =
ASMS =
SRCS += osal.c

OBJS =
OBJS += $(patsubst %.c, %.o, $(SRCS))
OBJS += $(patsubst %.S, %.o, $(ASMS))

%.o : %.c
	@echo "  CC $<"
	@rm -f $@
	@$(CC) $(CFLAGS) -c $<

%.o : %.S
	@echo "  CC $<"
	@rm -f $@
	@$(CC) $(CFLAGS) -c $<

all: $(LIBNAME)

$(LIBNAME) : $(OBJS)
	@echo "  AR $@"
	@rm -f $@
	@$(AR) $@ $(OBJS)
	@$(RANLIB) $@
	@$(OBJDUMP) -d $(LIBNAME) > $(LIBNAME).text

clean:
	@rm -f $(OBJS) $(LIBNAME) $(LIBNAME).text core *~

