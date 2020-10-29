# This makefile is written for the padauk pfs 173 rompler project.
# Make sure to keep this makefile in the root directory of the project

# ---------------------------------------------------------------------
# Device Parameter.

# Set the device to PFS173 which makes sure the right libraries are included
DEVICE = PFS173
F_CPU = 8000000			# Set the CPU Clock to 8Mhz
TARGET_VDD_MV = 4000	# Set the reference voltage for the internet bandgap reference
TARGET_VDD = 4.0

# Exact port for easypdkprog
# See: https://github.com/free-pdk/easy-pdk-programmer-software/issues/23
# You can remove the path if you dont need it
# Test with: easypdkprog --port=$(PORT)
PORT = /dev/tty.usbmodem1234567855AA

# ---------------------------------------------------------------------
# Project Parameters:
PROJECT_NAME = PFS_Rompler
MAIN_FILE = rompler.c
ROOT_DIR = .
BUILD_DIR = .build
OUTPUT_DIR = .output
SOURCE_DIR = src

OUTPUT_NAME = $(PROJECT_NAME)_$(DEVICE)
SOURCES = $(SOURCE_DIR)/$(MAIN_FILE)

# ---------------------------------------------------------------------
# Makefile
include arch-from-device.mk

OUTPUT = $(OUTPUT_DIR)/$(OUTPUT_NAME)

SOURCE_DIR = src
SOURCES = $(SOURCE_DIR)/rompler.c
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.rel,$(SOURCES))

# http://sdcc.sourceforge.net/doc/sdccman.pdf
COMPILE = sdcc -m$(ARCH) -c --std-sdcc11 --opt-code-size -D$(DEVICE) -DF_CPU=$(F_CPU) -DTARGET_VDD_MV=$(TARGET_VDD_MV) -I. -I$(ROOT_DIR)/include
LINK = sdcc -m$(ARCH)

ifdef PORT
	EASYPDKPROG = easypdkprog --port=$(PORT)
else
	EASYPDKPROG = easypdkprog
endif
# ---------------------------------------------------------------------
# Symbolic Targets:
all: upload

print-%: ; @echo $* = $($*)

$(BUILD_DIR)/%.rel: %.c
	@mkdir -p $(dir $@)
	$(COMPILE) -o $@ $<

$(OUTPUT).ihx: $(OBJECTS)
	@mkdir -p $(dir $(OUTPUT))
	$(LINK) --out-fmt-ihx -o $(OUTPUT).ihx $(OBJECTS)

$(OUTPUT).bin: $(OUTPUT).ihx
	makebin -p $(OUTPUT).ihx $(OUTPUT).bin

build: $(OUTPUT).bin

size: build
	@echo '---------- Segments ----------'
	@egrep '(ABS,CON)|(REL,CON)' $(OUTPUT).map | gawk --non-decimal-data '{dec = sprintf("%d","0x" $$2); print dec " " $$0}' | /usr/bin/sort -n -k1 | cut -f2- -d' '
	@echo '------------------------------'
	@stat -f "Size of $(OUTPUT_NAME).bin: %z bytes" $(OUTPUT_DIR)/$(OUTPUT_NAME).bin

run:
    # You can use the run command to run the MCU on the Programmer for testing serial printf
	$(EASYPDKPROG) -r $(TARGET_VDD) start


upload: size
	@echo 'Upload $(OUTPUT_NAME) to $(DEVICE): '
	$(EASYPDKPROG) -n $(DEVICE) write $(OUTPUT).ihx
	@echo 'Upload Successful'

clean:
	rm -r -f $(BUILD_DIR) $(OUTPUT_DIR)
	@echo 'All build data cleaned'

help:
	@echo 'Build options:'
	@echo 'size:		Build the program and displays the size'
	@echo 'run:		Run the MCU on the programmer (You can test serial printf with this)'
	@echo 'upload:		Upload the program to the device'
	@echo 'clean:		Removes all build files'
