################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/squishyXML.cpp 

OBJS += \
./src/squishyXML.o 

CPP_DEPS += \
./src/squishyXML.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++2a -I/usr/include/libxml2 -O3 -Wall -c -fmessage-length=0 -fPIC -fstack-protector-all -Wformat=2 -Wformat-security -Wstrict-overflow -fPIE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


