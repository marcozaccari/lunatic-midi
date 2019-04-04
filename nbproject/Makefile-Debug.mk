#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/lib/exceptions.o \
	${OBJECTDIR}/lib/i2c.o \
	${OBJECTDIR}/lib/ipc.o \
	${OBJECTDIR}/lib/log.o \
	${OBJECTDIR}/lib/utils.o \
	${OBJECTDIR}/lib/zini.o \
	${OBJECTDIR}/src/adc.o \
	${OBJECTDIR}/src/buttons.o \
	${OBJECTDIR}/src/console.o \
	${OBJECTDIR}/src/keyboard.o \
	${OBJECTDIR}/src/led_monitor.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/main.settings.o \
	${OBJECTDIR}/src/threads.o


# C Compiler Flags
CFLAGS=-mpush-args -mno-accumulate-outgoing-args -mno-stack-arg-probe -pthread -Wno-unused-function -Wextra -Wall

# CC Compiler Flags
CCFLAGS=-mpush-args -mno-accumulate-outgoing-args -mno-stack-arg-probe -pthread -Wno-unused-function
CXXFLAGS=-mpush-args -mno-accumulate-outgoing-args -mno-stack-arg-probe -pthread -Wno-unused-function

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk midi-controller

midi-controller: ${OBJECTFILES}
	${LINK.cc} -o midi-controller ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/lib/exceptions.o: lib/exceptions.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/exceptions.o lib/exceptions.c

${OBJECTDIR}/lib/i2c.o: lib/i2c.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/i2c.o lib/i2c.c

${OBJECTDIR}/lib/ipc.o: lib/ipc.cpp
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/ipc.o lib/ipc.cpp

${OBJECTDIR}/lib/log.o: lib/log.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/log.o lib/log.c

${OBJECTDIR}/lib/utils.o: lib/utils.c
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/utils.o lib/utils.c

${OBJECTDIR}/lib/zini.o: lib/zini.cpp
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/zini.o lib/zini.cpp

${OBJECTDIR}/src/adc.o: src/adc.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/adc.o src/adc.c

${OBJECTDIR}/src/buttons.o: src/buttons.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/buttons.o src/buttons.c

${OBJECTDIR}/src/console.o: src/console.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/console.o src/console.c

${OBJECTDIR}/src/keyboard.o: src/keyboard.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/keyboard.o src/keyboard.c

${OBJECTDIR}/src/led_monitor.o: src/led_monitor.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/led_monitor.o src/led_monitor.c

${OBJECTDIR}/src/main.o: src/main.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/main.settings.o: src/main.settings.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.settings.o src/main.settings.c

${OBJECTDIR}/src/threads.o: src/threads.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads.o src/threads.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
