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
ifeq "$(wildcard nbproject/Makefile-local-Debug.mk)" "nbproject/Makefile-local-Debug.mk"
include nbproject/Makefile-local-Debug.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Debug
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../source/main.c ../source/apiDelay.c ../source/apiTick.c ../source/pUART.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/812168374/main.o ${OBJECTDIR}/_ext/812168374/apiDelay.o ${OBJECTDIR}/_ext/812168374/apiTick.o ${OBJECTDIR}/_ext/812168374/pUART.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/812168374/main.o.d ${OBJECTDIR}/_ext/812168374/apiDelay.o.d ${OBJECTDIR}/_ext/812168374/apiTick.o.d ${OBJECTDIR}/_ext/812168374/pUART.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/812168374/main.o ${OBJECTDIR}/_ext/812168374/apiDelay.o ${OBJECTDIR}/_ext/812168374/apiTick.o ${OBJECTDIR}/_ext/812168374/pUART.o

# Source Files
SOURCEFILES=../source/main.c ../source/apiDelay.c ../source/apiTick.c ../source/pUART.c


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-Debug.mk dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/812168374/main.o: ../source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/main.o.d" -o ${OBJECTDIR}/_ext/812168374/main.o ../source/main.c   
	
${OBJECTDIR}/_ext/812168374/apiDelay.o: ../source/apiDelay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiDelay.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiDelay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/apiDelay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/apiDelay.o.d" -o ${OBJECTDIR}/_ext/812168374/apiDelay.o ../source/apiDelay.c   
	
${OBJECTDIR}/_ext/812168374/apiTick.o: ../source/apiTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiTick.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiTick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/apiTick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/apiTick.o.d" -o ${OBJECTDIR}/_ext/812168374/apiTick.o ../source/apiTick.c   
	
${OBJECTDIR}/_ext/812168374/pUART.o: ../source/pUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/pUART.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/pUART.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/pUART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/pUART.o.d" -o ${OBJECTDIR}/_ext/812168374/pUART.o ../source/pUART.c   
	
else
${OBJECTDIR}/_ext/812168374/main.o: ../source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/main.o.d" -o ${OBJECTDIR}/_ext/812168374/main.o ../source/main.c   
	
${OBJECTDIR}/_ext/812168374/apiDelay.o: ../source/apiDelay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiDelay.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiDelay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/apiDelay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/apiDelay.o.d" -o ${OBJECTDIR}/_ext/812168374/apiDelay.o ../source/apiDelay.c   
	
${OBJECTDIR}/_ext/812168374/apiTick.o: ../source/apiTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiTick.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/apiTick.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/apiTick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/apiTick.o.d" -o ${OBJECTDIR}/_ext/812168374/apiTick.o ../source/apiTick.c   
	
${OBJECTDIR}/_ext/812168374/pUART.o: ../source/pUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/812168374 
	@${RM} ${OBJECTDIR}/_ext/812168374/pUART.o.d 
	@${RM} ${OBJECTDIR}/_ext/812168374/pUART.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/812168374/pUART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -D__DEBUG -I"../include" -MMD -MF "${OBJECTDIR}/_ext/812168374/pUART.o.d" -o ${OBJECTDIR}/_ext/812168374/pUART.o ../source/pUART.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)    -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=_min_heap_size=1000,--defsym=_min_stack_size=2000,-L"C:/Program Files/Microchip/mplabc32/v2.01/pic32mx/lib",-Map="${DISTDIR}/YKGW.X.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=1000,--defsym=_min_stack_size=2000,-L"C:/Program Files/Microchip/mplabc32/v2.01/pic32mx/lib",-Map="${DISTDIR}/YKGW.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/UART_TXIF.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} -r dist/Debug

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
