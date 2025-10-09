#####################################
#         MLE Tech		    #
#   ESP32 Project build Makefile.   #
#####################################

############################################
# User supplied and default parameters
############################################
# default platform
S3  := 1

# Base package name
PROJECT := GameTimer.ino

# header files
INCLUDES := GameTimer.h Knobby.h Player.h
	   
# source files
SOURCES := $(PROJECT) Knobby.cpp FontVerdanaz142.c Verdanaz72.c Player.cpp

# library paths
LIBS := --libraries ./libraries

# serial port for uploading
DEFAULT_PORT := COM7

# IP address for uploading
UPLOAD_OTA := 192.168.2.21

############################################
# Configuration for Seeed Studio ESP32c3
############################################
ifdef C3
  ESP32  := --fqbn esp32:esp32:XIAO_ESP32C3
  OUTPUT := output_esp32c3
endif

############################################
# Configuration for Wrooom ESP32-S3-WROOM-1
############################################
ifdef S3
  ESP32 := --fqbn esp32:esp32:esp32s3usbotg
#  ESP32 := --fqbn esp32:esp32:esp32s3
  OUTPUT := output_esp32s3
endif

############################################
# Configuration for Wrooom ESP32-WROOM
############################################
ifdef DOIT
  ESP32  := --fqbn esp32:esp32:esp32doit-devkit-v1
  OUTPUT := output_doit
endif

############################################
# Fixed definitions
############################################
ARDUNIO := arduino-cli
BUILD   := --build-path $(OUTPUT)/build --output-dir $(OUTPUT) 
COMPILE := compile $(BUILD) --log-level warn --log-file $(OUTPUT)/build.log $(LIBS)
TARGET  := $(OUTPUT)/$(PROJECT).bin
MD5      = md5sums -u $(TARGET)

############################################
# Use default or user supplied COM port
############################################
ifndef SERIAL_PORT
  SERIAL_PORT := $(DEFAULT_PORT)
endif

############################################
# Use Serial port or OTA to upload
############################################
ifdef OTA
  UPLOAD := espota.exe -d -r -i $(OTA) -p 3232 --auth=admin -f $(TARGET)
else
  UPLOAD := $(ARDUNIO) upload --input-dir $(OUTPUT) -p $(PT) $(ESP32) $(TARGET)
endif

############################################
############################################
# User input targets
############################################
PHONY: Build_OTA Build_Serial monitor clean

Build_OTA :
	@$(MAKE) target OTA=$(UPLOAD_OTA)

Build_Serial :
	@$(MAKE) target PT=$(SERIAL_PORT)

monitor :
	$(ARDUNIO) monitor -p $(PORT) -c baudrate=115200

clean :
	@rmdir /Q /S $(OUTPUT)

############################################
############################################
# Build instructions
############################################
target : $(TARGET)
#	@python ../Tools/Beep.py UPLOAD
	$(UPLOAD)
#	@python ../Tools/Beep.py OK

$(TARGET) : $(SOURCES) $(INCLUDES)
	if not exist $(OUTPUT) (mkdir $(OUTPUT))
	$(ARDUNIO) $(COMPILE) $(ESP32)

