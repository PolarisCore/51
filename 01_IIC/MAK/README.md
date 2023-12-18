# Makefile
  基于51芯片构建的工程编译makefile文件
# 介绍
 makefile文件主要包括3部分。

1、扫描指定文件夹，将源文件、头文件、.o文件或.hex文件拷贝到指定编译文件夹中<br>
2、编译生成目标文件.o并放到指定文件夹中<br>
3、链接目标文件，并生成hex文件, 并放到指定文件夹中，且生成文件放到指定文件夹<br>

4、hex文件转bin文件<br>

注意事项

1、*********************

2、*************************


makefile示例：
#指定sdcc编译器路径（注：请改为自己的路径）
sdcc  := C:\SDCC\bin\sdcc.exe
packihx := C:\SDCC\bin\packihx.exe

#指定stcflash烧录器路径（注：请改为自己的路径）
stcflash := E:\Study\01_c\project\51sdcc\tool\stcflash\stcflash.py

#指定根目录
pes_parent_dir:=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
pes_parent_dir:=$(shell dirname $(pes_parent_dir))
pes_parent_dir:=$(shell dirname $(pes_parent_dir))
ROOT:= $(pes_parent_dir)
print :
    echo $(ROOT)
#指定.c文件（注：请将工程中所有的.c文件添加进来）
SRCS = 

#指定.h文件（注：请将工程中所有.h文件所在的文件夹添加进来）
INCS = \
-IUser \
-ILibraries \
-IHardware \

#指定输出hex文件的路径与文件名
outdir = Build
outname = output

all: $(outdir)/$(outname).hex download

#将所有的.c->.rel，存入OBJECT
OBJECTS = $(addprefix $(outdir)/,$(notdir $(SRCS:.c=.rel)))
vpath %.c $(sort $(dir $(SRCS)))

#（注：请对照自己的芯片调整以下几条语句的--iram-size、--xram-size参数）
$(outdir)/%.rel: %.c Makefile | $(outdir)
	$(sdcc) --code-size 64000 --iram-size 256 --xram-size 8192 --stack-auto -c $(INCS) $< -o $@

$(outdir)/$(outname).ihx: $(OBJECTS)
	$(sdcc) --code-size 64000 --iram-size 256 --xram-size 8192 --stack-auto $^ -o $(outdir)/$(outname).ihx

$(outdir)/%.hex: $(outdir)/%.ihx | $(outdir)
	$(packihx) $< $@ > $(outdir)/$(outname).hex

#调用stcflash进行烧录（注：请对照自己的设备修改COM口）
download:
	python $(stcflash) Build/output.hex --port COM7 --lowbaud 2400 --highbaud 460800

#定义清除操作
.PHONY : clean
clean :
  del $(outdir)\*.* /q

