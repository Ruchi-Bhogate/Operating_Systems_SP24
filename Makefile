GPORT=9988
IMG=bareOS.img

ARCH=riscv64-unknown-linux-gnu
CC=$(ARCH)-gcc
LD=$(ARCH)-ld
GDB=$(ARCH)-gdb 
OBJCOPY=$(ARCH)-objcopy
QEMU=qemu-system-riscv64

IDIR=kernel/include
TDIR=kernel/testing
BDIR=.build
IODIR=io
MAP=$(BDIR)/kernel.map
ENV=.env

SRC=$(wildcard kernel/*/*.c)
ASM=$(wildcard kernel/*/*.s) $(wildcard kernel/*/*.S)
OBJ=$(patsubst %.c,$(BDIR)/%.o,$(notdir $(SRC))) $(patsubst %.s,$(BDIR)/%_asm.o,$(patsubst %.S,$(BDIR)/%_asm.o,$(notdir $(ASM))))
OBJ_TEST=$(patsubst %.c,$(BDIR)/%.o,$(notdir $(wildcard $(TDIR)/*.c)))
OBJ_LINK=$(filter-out $(OBJ_TEST),$(OBJ))
VPATH=$(dir $(ASM)) $(dir $(SRC))

CFLAGS=-Wall -Werror -fno-builtin -nostdlib -march=rv64imac -mabi=lp64 -mcmodel=medany -I $(IDIR) -O0 -g -imacros $(ENV)
SFLAGS= -I $(IDIR) -march=rv64imac -mabi=lp64 -g
DFLAGS= -ex "file $(IMG)" -ex "target remote :$(GPORT)"
EFLAGS= -E -march=rv64imac -mabi=lp64
LDFLAGS=-nostdlib -Map $(MAP)
QFLAGS=-M virt -kernel $(IMG) -bios none -chardev stdio,id=uart0 -serial chardev:uart0 -display none


STAGE=.setup

.PHONY: all clean qemu qemu-debug gdb dirs pack

all: dirs $(OBJ) $(BDIR)/kernel.elf $(IMG)

$(IMG): $(BDIR)/kernel.elf
	$(OBJCOPY) $(BDIR)/kernel.elf -I binary $(IMG)

$(BDIR)/kernel.elf: $(OBJ_LINK) kernel/kernel.ld
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o $(BDIR)/kernel.elf $(OBJ_LINK)

%.s: %.S
	$(CC) $(EFLAGS) $< > $@

$(BDIR)/%_asm.o: %.s
	$(CC) $(SFLAGS) -c $< -o $@

$(BDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dirs: 
	@mkdir -p kernel/lib
	@mkdir -p kernel/app
	@mkdir -p kernel/testing
	@mkdir -p $(BDIR)
	@mkdir -p $(IODIR)
	@touch $(IODIR)/p8.in
	@touch $(IODIR)/p8.out

clean:
	rm -f bareOS.tar.gz
	rm -f $(IODIR)/p8.in $(IODIR)/p8.out
	rm -rf $(BDIR) bareOS.img

qemu: dirs
ifneq ("$(wildcard $(BDIR)/.force)","")
	$(MAKE) clean
endif
	$(MAKE) .qemu

.qemu: all
	$(QEMU) $(QFLAGS)

qemu-debug: QFLAGS += -S -gdb tcp::${GPORT}
qemu-debug: .qemu

gdb:
	$(GDB) $(DFLAGS)

milestone-%: dirs
	@echo -e Building $@...
	@echo
	@cd $(STAGE) && \
		for dir in lib app system include device testing; do \
			for file in *; do if [ "$$file" != "$${file#$@-$$dir-}" ]; then echo "  Creating kernel/$$dir/$${file#$@-$$dir-}"; cp -i "$$file" ../kernel/$$dir/$${file#$@-$$dir-}; fi done \
		done
	@echo "#define MILESTONE $(patsubst milestone-%,%,$@)" > $(ENV)
	@echo
	@echo Build complete

test: LDFLAGS += --wrap=shell --wrap=handle_clk --wrap=initialize --wrap=resched
test: OBJ_LINK += $(OBJ_TEST)
test: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-1: EFLAGS += -D BS_ENTRY_FUNC=__ms1
test-milestone-1: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-2: EFLAGS += -D BS_ENTRY_FUNC=__ms2
test-milestone-2: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-3: EFLAGS += -D BS_ENTRY_FUNC=__ms3
test-milestone-3: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-4: test

test-milestone-5: EFLAGS += -D BS_ENTRY_FUNC=__ms5
test-milestone-5: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-6: EFLAGS += -D BS_ENTRY_FUNC=__ms6
test-milestone-6: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-7: EFLAGS += -D BS_ENTRY_FUNC=__ms7
test-milestone-7: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-8: EFLAGS += -D BS_ENTRY_FUNC=__ms8
test-milestone-8: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-9: EFLAGS += -D BS_ENTRY_FUNC=__ms9
test-milestone-9: clean dirs all
	touch $(BDIR)/.force
	$(MAKE) .qemu

test-milestone-10: EFLAGS += -D BS_ENTRY_FUNC=__ms10
test-milestone-10: clean .qemu
	touch $(BDIR)/.force
	$(MAKE) .qemu

pack:
	rm -f bareOS.tar.gz
	tar -czvf bareOS.tar.gz kernel

FORCE:
