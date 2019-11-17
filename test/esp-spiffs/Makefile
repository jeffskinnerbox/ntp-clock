# programs name
PROG = esp-spiffs
#
# type of ESP chip in use
CHIP = esp8266

# upload the spiffs filesystem to esp
SPIFFS = spiffs.bin

# spiffs files directory
DIR = data

# fully qualified board name (FQBN)
FQBN = $(CHIP):$(CHIP):nodemcuv2

# serial port used by the board
PORT = /dev/ttyUSB0


#ESPTOOL = /home/jeff/.arduino15/packages/esp32/tools/esptool_py/2.6.1/esptool.py
ESPTOOL = /home/jeff/.arduino15/packages/esp8266/hardware/esp8266/2.5.2/tools/esptool/esptool

# string within names give to .bin and .elf files
VAR = $(shell echo $(FQBN) | tr ':' '.')

# compile and firmware flashing commands
CC = arduino-cli compile --fqbn $(FQBN)
UPLOAD = arduino-cli upload --fqbn $(FQBN) --port $(PORT)


.PHONY: build upload clean erase size

all: upspiffs upload

$(PROG).ino: $(PROG).$(VAR).bin
	$(CC) $(CURDIR)                             # build the binary executable

upload:
	$(UPLOAD) $(CURDIR)                         # up load the binary executable

$(SPIFFS): $(DIR)/index.html $(DIR)/style.css   # create the spiffs filesystem for uploading
	mkspiffs -c data -b 4096 -p 256 -s 0x100000 $(SPIFFS)

upspiffs: $(SPIFFS)                             # upload the spiffs filesystem to esp
	$(ESPTOOL) --chip $(CHIP) --port $(PORT) --baud 115200 write_flash -z 0x110000 $(SPIFFS)


build:
	$(CC) $(CURDIR)                             # build the binary executable

erase:                                          # erase the entire flash
	$(ESPTOOL) --port $(PORT) erase_flash

size:                                           # determine the flash size
	$(ESPTOOL) --port $(PORT) flash_id

clean:                                          # delete all binaries an object files
	rm --force $(PROG).$(VAR).bin $(PROG).$(VAR).elf $(SPIFFS)


