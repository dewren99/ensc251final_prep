################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mainTokenClasses.cpp \
../src/tokenClassifier.cpp 

CPP_DEPS += \
./src/mainTokenClasses.d \
./src/tokenClassifier.d 

OBJS += \
./src/mainTokenClasses.o \
./src/tokenClassifier.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/usr/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/mainTokenClasses.d ./src/mainTokenClasses.o ./src/tokenClassifier.d ./src/tokenClassifier.o

.PHONY: clean-src

