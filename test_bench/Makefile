
ifeq ($(CONFIG_TEST_ZEMI3),y)
HOST_EXTRACXXFLAGS := -Dtest_zemi3_EXPORTS
tb := test_zemi3
else
HOST_EXTRACXXFLAGS := -Dtest_zcei_share_EXPORTS
tb := test_zcei_share
endif

hostprogs-y := $(tb)

tb_dir := test_bench
xtor_dir := src/xtors

common-cxxobjs := src/main.o

ifeq ($(CONFIG_TEST_ZEMI3),y)
common-cxxobjs += src/zemi3/board.o
else
common-cxxobjs += src/zcei/board.o
endif

xotr-y :=
include $(tb_dir)/$(xtor_dir)/Makefile

$(tb)-cxxobjs := $(common-cxxobjs) $(xtor-y)

test_bench:$(obj)/$(tb)
	@:
	
HOST_EXTRACXXFLAGS += \
	$(if $(ZEBU_ROOT), -I$(ZEBU_ROOT)/include) \
	-I/home/soc_shares/ZeBu_tools/O-2018.09-SP1/zebu/O-2018.09-SP1/include	\
	-I/home/shares/zebu_ip_2017.12/include \
	-I./$(tb_dir)/include $(if $(ZEBU_IP_ROOT),-I$(ZEBU_IP_ROOT) -I$(ZEBU_IP_ROOT)/XTOR) \
	-include $(objtree)/include/generated/autoconf.h


HOST_EXTRACXXFLAGS += -m64 -Wall -Wno-deprecated-declarations

HOSTLDLIBS_$(tb) := $(if $(ZEBU_ROOT), -L$(ZEBU_ROOT)/lib)
HOSTLDLIBS_$(tb) += $(if $(ZEBU_IP_ROOT), -L$(ZEBU_IP_ROOT)/lib)
HOSTLDLIBS_$(tb) += -lZLPDDR4Xtor -lZebu -lUart -lI2c -lI2s -lZebuXtor -lpthread -lZebuThreadSafe -lZebuXtor  -lcurses -lUsbUrb -lZebuZEMI3

ifneq ($(ZEBU_ROOT),)
HOSTLDLIBS_$(tb) += -Wl,-rpath,$(ZEBU_ROOT)/lib
endif
ifneq ($(ZEBU_IP_ROOT),)
HOSTLDLIBS_$(tb) += -Wl,-rpath,$(ZEBU_IP_ROOT)/lib
endif

clean-files += $(tb)

help:
	@echo  '  test_zcei_share - If select compile test_bench'
	@echo  '  test_zemi3      - If select compile test_bench and select gmac, usb*'

