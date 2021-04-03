#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=src/keyboard.asm src/main.asm lib/i2c_slave.asm lib/misc.asm lib/txrx.asm lib/txrx_buffer.asm lib/usart.asm

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/src/keyboard.o ${OBJECTDIR}/src/main.o ${OBJECTDIR}/lib/i2c_slave.o ${OBJECTDIR}/lib/misc.o ${OBJECTDIR}/lib/txrx.o ${OBJECTDIR}/lib/txrx_buffer.o ${OBJECTDIR}/lib/usart.o
POSSIBLE_DEPFILES=${OBJECTDIR}/src/keyboard.o.d ${OBJECTDIR}/src/main.o.d ${OBJECTDIR}/lib/i2c_slave.o.d ${OBJECTDIR}/lib/misc.o.d ${OBJECTDIR}/lib/txrx.o.d ${OBJECTDIR}/lib/txrx_buffer.o.d ${OBJECTDIR}/lib/usart.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/src/keyboard.o ${OBJECTDIR}/src/main.o ${OBJECTDIR}/lib/i2c_slave.o ${OBJECTDIR}/lib/misc.o ${OBJECTDIR}/lib/txrx.o ${OBJECTDIR}/lib/txrx_buffer.o ${OBJECTDIR}/lib/usart.o

# Source Files
SOURCEFILES=src/keyboard.asm src/main.asm lib/i2c_slave.asm lib/misc.asm lib/txrx.asm lib/txrx_buffer.asm lib/usart.asm


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=16f886
MP_LINKER_DEBUG_OPTION=-r=ROM@0x1F00:0x1FFE -r=RAM@SHARE:0x70:0x70 -r=RAM@SHARE:0xF0:0xF0 -r=RAM@SHARE:0x170:0x170 -r=RAM@GPR:0x1E5:0x1EF -r=RAM@SHARE:0x1F0:0x1F0
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/keyboard.o: src/keyboard.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/keyboard.o.d 
	@${RM} ${OBJECTDIR}/src/keyboard.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/src/keyboard.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/src/keyboard.lst\\\" -e\\\"${OBJECTDIR}/src/keyboard.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/src/keyboard.o\\\" \\\"src/keyboard.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/src/keyboard.o"
	@${FIXDEPS} "${OBJECTDIR}/src/keyboard.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/src/main.o: src/main.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/src/main.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/src/main.lst\\\" -e\\\"${OBJECTDIR}/src/main.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/src/main.o\\\" \\\"src/main.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/src/main.o"
	@${FIXDEPS} "${OBJECTDIR}/src/main.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/i2c_slave.o: lib/i2c_slave.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/i2c_slave.o.d 
	@${RM} ${OBJECTDIR}/lib/i2c_slave.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/i2c_slave.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/i2c_slave.lst\\\" -e\\\"${OBJECTDIR}/lib/i2c_slave.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/i2c_slave.o\\\" \\\"lib/i2c_slave.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/i2c_slave.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/i2c_slave.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/misc.o: lib/misc.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/misc.o.d 
	@${RM} ${OBJECTDIR}/lib/misc.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/misc.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/misc.lst\\\" -e\\\"${OBJECTDIR}/lib/misc.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/misc.o\\\" \\\"lib/misc.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/misc.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/misc.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/txrx.o: lib/txrx.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/txrx.o.d 
	@${RM} ${OBJECTDIR}/lib/txrx.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/txrx.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/txrx.lst\\\" -e\\\"${OBJECTDIR}/lib/txrx.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/txrx.o\\\" \\\"lib/txrx.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/txrx.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/txrx.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/txrx_buffer.o: lib/txrx_buffer.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/txrx_buffer.o.d 
	@${RM} ${OBJECTDIR}/lib/txrx_buffer.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/txrx_buffer.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/txrx_buffer.lst\\\" -e\\\"${OBJECTDIR}/lib/txrx_buffer.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/txrx_buffer.o\\\" \\\"lib/txrx_buffer.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/txrx_buffer.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/txrx_buffer.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/usart.o: lib/usart.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/usart.o.d 
	@${RM} ${OBJECTDIR}/lib/usart.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/usart.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -d__DEBUG -d__MPLAB_DEBUGGER_PICKIT2=1 -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/usart.lst\\\" -e\\\"${OBJECTDIR}/lib/usart.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/usart.o\\\" \\\"lib/usart.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/usart.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/usart.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
else
${OBJECTDIR}/src/keyboard.o: src/keyboard.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/keyboard.o.d 
	@${RM} ${OBJECTDIR}/src/keyboard.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/src/keyboard.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/src/keyboard.lst\\\" -e\\\"${OBJECTDIR}/src/keyboard.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/src/keyboard.o\\\" \\\"src/keyboard.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/src/keyboard.o"
	@${FIXDEPS} "${OBJECTDIR}/src/keyboard.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/src/main.o: src/main.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.o.d 
	@${RM} ${OBJECTDIR}/src/main.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/src/main.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/src/main.lst\\\" -e\\\"${OBJECTDIR}/src/main.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/src/main.o\\\" \\\"src/main.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/src/main.o"
	@${FIXDEPS} "${OBJECTDIR}/src/main.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/i2c_slave.o: lib/i2c_slave.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/i2c_slave.o.d 
	@${RM} ${OBJECTDIR}/lib/i2c_slave.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/i2c_slave.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/i2c_slave.lst\\\" -e\\\"${OBJECTDIR}/lib/i2c_slave.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/i2c_slave.o\\\" \\\"lib/i2c_slave.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/i2c_slave.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/i2c_slave.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/misc.o: lib/misc.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/misc.o.d 
	@${RM} ${OBJECTDIR}/lib/misc.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/misc.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/misc.lst\\\" -e\\\"${OBJECTDIR}/lib/misc.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/misc.o\\\" \\\"lib/misc.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/misc.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/misc.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/txrx.o: lib/txrx.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/txrx.o.d 
	@${RM} ${OBJECTDIR}/lib/txrx.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/txrx.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/txrx.lst\\\" -e\\\"${OBJECTDIR}/lib/txrx.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/txrx.o\\\" \\\"lib/txrx.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/txrx.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/txrx.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/txrx_buffer.o: lib/txrx_buffer.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/txrx_buffer.o.d 
	@${RM} ${OBJECTDIR}/lib/txrx_buffer.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/txrx_buffer.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/txrx_buffer.lst\\\" -e\\\"${OBJECTDIR}/lib/txrx_buffer.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/txrx_buffer.o\\\" \\\"lib/txrx_buffer.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/txrx_buffer.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/txrx_buffer.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
${OBJECTDIR}/lib/usart.o: lib/usart.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/lib" 
	@${RM} ${OBJECTDIR}/lib/usart.o.d 
	@${RM} ${OBJECTDIR}/lib/usart.o 
	@${FIXDEPS} dummy.d -e "${OBJECTDIR}/lib/usart.err" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -q -p$(MP_PROCESSOR_OPTION) -u  -l\\\"${OBJECTDIR}/lib/usart.lst\\\" -e\\\"${OBJECTDIR}/lib/usart.err\\\" $(ASM_OPTIONS)    -o\\\"${OBJECTDIR}/lib/usart.o\\\" \\\"lib/usart.asm\\\" 
	@${DEP_GEN} -d "${OBJECTDIR}/lib/usart.o"
	@${FIXDEPS} "${OBJECTDIR}/lib/usart.o.d" $(SILENT) -rsi ${MP_AS_DIR} -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION)  -w -x -u_DEBUG -z__ICD2RAM=1 -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"   -z__MPLAB_BUILD=1  -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PICKIT2=1 $(MP_LINKER_DEBUG_OPTION) -odist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
else
dist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"   -z__MPLAB_BUILD=1  -odist/${CND_CONF}/${IMAGE_TYPE}/keyboard-controller.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
