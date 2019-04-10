################################################################################
# Version: ODE5.0 (Build 20041004.0550, 04-Oct-2004)
#
# This file defines the PROGRAMS target.
#  Some variables used:
#   PROGRAMS
#       The program names with the .exe extension if needed.
#   OFILES, _OFILES_
#       The object files that make up a given program.
#   _LINKING_LIBS_
#       Defined in std.mk, a combination of LIBS and EXTRA_LIBS.
#   LDFLAGS
#       Flags that are used by LD.
#   LD 
#       The linker/loader used to generate a program.
#   USE_RESPFILE
#       If this variable is defined the a temp file will be created to use as
#       a response file for the linker.
#   USE_MSGBIND
#       If this variable is defined then messages are bound to the executable.
#   MSGBIND
#       The message binding program.
#
################################################################################
.if !defined(__PROG_MK_)
__PROG_MK_=

_PROGRAMS_ = ${PROGRAMS} ${_NLV_PROGRAMS_}

.if !empty(_PROGRAMS_)

# Temporary variable for OFILES for programs
#
_ALL_PROG_OFILES_TMP_   = ${.ALLSRC:N*${IMPLIB_SUFF}:N*${STATLIB_SUFF}} \
                          ${_EXTRA_OFILES_:p}
_ALL_PROG_RESFILES_TMP_ = ${.ALLSRC:l:M*${RES_SUFF}}

.if ${BACKSLASH_PATHS}
_ALL_PROG_OFILES_   = ${_ALL_PROG_OFILES_TMP_:s|/|\\|g}
_ALL_PROG_RESFILES_ = ${_ALL_PROG_RESFILES_TMP_:s|/|\\|g}
.else
_ALL_PROG_OFILES_   = ${_ALL_PROG_OFILES_TMP_}
_ALL_PROG_RESFILES_ = ${_ALL_PROG_RESFILES_TMP_}
.endif

.if ${CCFAMILY} == "vage" || ${CCFAMILY} == "visual"
################################################################################
#
# IBM Visual Age C++ and Microsoft Visual C++ rules.
#

.ifdef USE_MSGBIND
BINDING_TEMPFILE = ${.TARGET}.brc
BINDING_FILE_NAMES=${BIND_FILES:@.F.@${<${.F.}:L:a${BINDING_TEMPFILE}}:b${BINDING_MESG_NUMBERS}@}
BINDING_MESG_NUMBERS=${${.F.}_BIND_MESSAGES:U${BIND_MESSAGES}:@.M.@${.M.:a${BINDING_TEMPFILE}}@}
.endif # USE_MSGBIND

${_PROGRAMS_}:  $${_OFILES_} $${_LIBS_}
	${.TARGET:H:C}
.if defined(USE_RESPFILE)
	${_RESPFILE_:rm-}
	${_LDFLAGS_:N\-L*:A${_RESPFILE_}}
.if !empty(MACHINE:Mx86_os2_*)
	${_ALL_PROG_OFILES_:l:N*${RES_SUFF}:@.WORD.@${.WORD.:a${_RESPFILE_}}@}
.else
	${_ALL_PROG_OFILES_:@.WORD.@${.WORD.:a${_RESPFILE_}}@}
.endif
	${_LINKING_LIBS_:@.WORD.@${.WORD.:b${.WORD.:a${_RESPFILE_}}}@}
	${LD_TRAILING_FLAGS:a${_RESPFILE_}}
	${_LD_} @${_RESPFILE_}
.else # USE_RESPFILE
.if !empty(MACHINE:Mx86_os2_*)
	${_LD_} ${_LDFLAGS_:N\-L*} ${_ALL_PROG_OFILES_:l:N*${RES_SUFF}} \
	  ${_LINKING_LIBS_} ${LD_TRAILING_FLAGS}
.else
	${_LD_} ${_LDFLAGS_:N\-L*} ${_ALL_PROG_OFILES_} \
	  ${_LINKING_LIBS_} ${LD_TRAILING_FLAGS}
.endif
.endif # USE_RESPFILE
.if !empty(MACHINE:Mx86_os2_*)
	-${_ALL_PROG_RESFILES_:b${_RC_} ${_ALL_PROG_RESFILES_} ${.TARGET}}
.endif
.ifdef USE_MSGBIND
	${>${.TARGET:T}:L:A${BINDING_TEMPFILE}}
	${${BINDING_FILE_NAMES}:L:A${NULL_DEVICE}}
	${MSGBIND} ${BINDING_TEMPFILE}
	${BINDING_TEMPFILE:rm-}
.endif # USE_MSGBIND
.if defined(STRIP_EXE)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif

.else # CCFAMILY
################################################################################
#
# Rules for all other compilers.
#

${_PROGRAMS_}: $${_OFILES_} $${_LIBS_} 
	${.TARGET:H:C}
.if !empty(MACHINE:M*_openvms_*)
	${_RESPFILE_:rm-}
	${_LDOPTS_:@.WORD.@${.WORD.:a${_RESPFILE_}}@}
	${SHORT_OBJ_PATHS:D${_ALL_PROG_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_PROG_OFILES_}:@.PATH.@${VMS_PATH_CONV}@:s|$| ${_LINKING_LIBS_}|:@.PATH.@${.PATH.:a${_RESPFILE_}}@}
	${_LD_} ${_LDFLAGS_:N-L*} ${_RESPFILE_}/OPT
.else
.if !empty(MACHINE:Mas400_os400_*)
	${_LD_} ${_LDFLAGS_} -o ${.TARGET} \
          ${SHORT_OBJ_PATHS:D${_ALL_PROG_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_PROG_OFILES_}} \
	        ${_LINKING_LIBS_} ${LD_TRAILING_FLAGS}
.else
	${_LD_} ${_LDFLAGS_} -o ${.TARGET}.X \
          ${SHORT_OBJ_PATHS:D${_ALL_PROG_OFILES_:S|^${_CURDIR_}${_CURDIRSEP_}||g}:U${_ALL_PROG_OFILES_}} \
	        ${_LINKING_LIBS_} ${LD_TRAILING_FLAGS}
.endif
.ifndef UNIX
	${.TARGET:rm-}
.endif
.if !empty(MACHINE:Mx86_os2_*)
	${_EMXBIND_} ${_EMXBINDFLAGS_} -o ${.TARGET} ${.TARGET}.X
.elif empty(MACHINE:Mas400_os400_*)
	${MV} ${_MVFLAGS_} ${.TARGET}.X ${.TARGET}
.endif
.if defined(STRIP_EXE)
	${_STRIP_} ${_STRIPFLAGS_} ${.TARGET}
.endif
.endif

.endif # CCFAMILY

.endif # _PROGRAMS_

.endif # __PROG_MK_
