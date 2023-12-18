#指定sdcc编译器路径（注：请改为自己的路径)
sdcc := C:\SDCC\bin\sdcc.exe
packihx := C:/SDCC/bin/packihx.exe
hex2bin := E:\Study\01_c\project\51sdcc\tool\hex2bin\Hex2bin-2.5\bin\Release\hex2bin.exe
srecord := D:/Program\Files/srecord/bin/srec_cat.exe

#指定stcflash烧录器路径（注：请改为自己的路径)
stcflash := E:\Study\01_c\project\51sdcc\tool\stcflash\stcflash.py

#指定根目彄1�7
pes_parent_dir:=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
#pes_parent_dir:=$(shell dirname $(pes_parent_dir))
#pes_parent_dir:=$(shell dirname $(pes_parent_dir))
ROOT:= E:/Study/01_c/project/51sdcc# $(pes_parent_dir)

#指定.c文件(注：请将工程中包各1�7.c文件添加进来)
SRCS = $(ROOT)/appl/src/main.c $(ROOT)/appl/src/time51.c

#指定.h文件（注：请将工程中的头文件的文件夹添加进来＄1�7
INCS = -I$(ROOT)/appl/inc -I$(ROOT)/lib/inc

#指定输出hex文件的路径与文件
outdir = $(ROOT)/_build
outname = main51
srcdir = _src
incdir = _inc

all: $(outdir)/$(outname).hex #download
	@echo $(ROOT)

#将所有的.c->.rel，存入OBJECT
OBJECTS = $(addprefix $(outdir)/,$(notdir $(SRCS:.c=.rel)))  #将所有的.c源文件后名更新为.rel后，输出到输出文件夹_build文件下，作为目标文件
vpath %.c $(sort $(dir $(SRCS)))	#如果搜索不到依赖文件，在指定文件中搜約1�7.c文件

#（注：请对照自己的芯片调整以下几条语句的--iram-size --xram-size
$(outdir)/%.rel: %.c Makefile | $(outdir)
	$(sdcc) --code-size 64000 --iram-size 256 --xram-size 8192 --stack-auto -c $(INCS) $< -o $@

$(outdir)/$(outname).ihx: $(OBJECTS)
	$(sdcc) --code-size 64000 --iram-size 256 --xram-size 8192 --stack-auto $^ -o $(outdir)/$(outname).ihx

$(outdir)/%.hex: $(outdir)/%.ihx | $(outdir)
	$(packihx) $< > $@

bin1:
	$(hex2bin) $(outdir)/$(outname).hex

#srec_cat的bin文件夹加载到系统变量中，可以直接识别
bin2:
	srec_cat -Disable_Sequence_Warning $(outdir)/$(outname).hex -intel -o $(outdir)/$(outname).bin -binary

bin3:
	$(srecord) -Disable_Sequence_Warning $(outdir)/$(outname).hex -intel -o $(outdir)/$(outname).bin -binary


#调用stcflash进行烧录（注：请对照自己的设备修改COM口）
downloadhex:
	python $(stcflash) $(outdir)/$(outname).hex --port COM4 --lowbaud 2400 --highbaud 460800
downloadbin:
	python $(stcflash) $(outdir)/$(outname).bin --port COM4 --lowbaud 2400 --highbaud 460800
downloadinfo:
	python $(stcflash)
#定义伪命令
.PHONY : bin1 bin2 bin3 all run clean debug test buildfile info
dir_file := $(CURDIR)/$(lastword $(MAKEFILE_LIST))
test:
#@echo $(OBJECTS)
#@echo $(shell cygpath -w $(shell pwd))
#@echo $(abspath $(lastword $(MAKEFILE_LIST)))
	@echo $(realpath $(dir_file))
#@echo $(shell dirname $(dir_file))
#@echo $(CURDIR)
#创建_build、_build/_src、_build/_inc文件
buildfile:
	mkdir $(outdir)
	mkdir $(outdir)/$(srcdir)
	mkdir $(outdir)/$(incdir)
clean:
	rm -rf $(outdir)/*.hex
	rm -rf $(outdir)/*.rel
	rm -rf $(outdir)/*.rst
	rm -rf $(outdir)/*.sym
	rm -rf $(outdir)/*.mem
	rm -rf $(outdir)/*.map
	rm -rf $(outdir)/*.lst
	rm -rf $(outdir)/*.lk
	rm -rf $(outdir)/*.ihx
	rm -rf $(outdir)/*.asm
