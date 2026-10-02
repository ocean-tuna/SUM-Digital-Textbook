
dyn_pth = "30_Generation/RTA-CAR_SPM_Project_Dev/src/sum"
sta_pth = "20_Implementation/src"

tmpl = """
<file target="C" name="{file_name}" folder="C:/Users/pig1tn/Documents/RTA-SUM/01_dev/sum_spm/{final_pth}">
   <defines>
    <define name="WIN32=1"/>
    <define name="WINNT=1"/>
    <define name="_INTEGRAL_MAX_BITS=64"/>
    <define name="_WIN32=1"/>
    <define name="_X86_=1"/>
    <define name="__ATOMIC_ACQUIRE=2"/>
    <define name="__ATOMIC_ACQ_REL=4"/>
    <define name="__ATOMIC_CONSUME=1"/>
    <define name="__ATOMIC_HLE_ACQUIRE=65536"/>
    <define name="__ATOMIC_HLE_RELEASE=131072"/>
    <define name="__ATOMIC_RELAXED=0"/>
    <define name="__ATOMIC_RELEASE=3"/>
    <define name="__ATOMIC_SEQ_CST=5"/>
    <define name="__BIGGEST_ALIGNMENT__=16"/>
    <define name="__BYTE_ORDER__=__ORDER_LITTLE_ENDIAN__"/>
    <define name="__CHAR16_TYPE__=short unsigned int"/>
    <define name="__CHAR32_TYPE__=unsigned int"/>
    <define name="__CHAR_BIT__=8"/>
    <define name="__DBL_DECIMAL_DIG__=17"/>
    <define name="__DBL_DENORM_MIN__=((double)4.94065645841246544176568792868221372e-324L)"/>
    <define name="__DBL_DIG__=15"/>
    <define name="__DBL_EPSILON__=((double)2.22044604925031308084726333618164062e-16L)"/>
    <define name="__DBL_HAS_DENORM__=1"/>
    <define name="__DBL_HAS_INFINITY__=1"/>
    <define name="__DBL_HAS_QUIET_NAN__=1"/>
    <define name="__DBL_MANT_DIG__=53"/>
    <define name="__DBL_MAX_10_EXP__=308"/>
    <define name="__DBL_MAX_EXP__=1024"/>
    <define name="__DBL_MAX__=((double)1.79769313486231570814527423731704357e+308L)"/>
    <define name="__DBL_MIN_10_EXP__=(-307)"/>
    <define name="__DBL_MIN_EXP__=(-1021)"/>
    <define name="__DBL_MIN__=((double)2.22507385850720138309023271733240406e-308L)"/>
    <define name="__DEC128_EPSILON__=1E-33DL"/>
    <define name="__DEC128_MANT_DIG__=34"/>
    <define name="__DEC128_MAX_EXP__=6145"/>
    <define name="__DEC128_MAX__=9.999999999999999999999999999999999E6144DL"/>
    <define name="__DEC128_MIN_EXP__=(-6142)"/>
    <define name="__DEC128_MIN__=1E-6143DL"/>
    <define name="__DEC128_SUBNORMAL_MIN__=0.000000000000000000000000000000001E-6143DL"/>
    <define name="__DEC32_EPSILON__=1E-6DF"/>
    <define name="__DEC32_MANT_DIG__=7"/>
    <define name="__DEC32_MAX_EXP__=97"/>
    <define name="__DEC32_MAX__=9.999999E96DF"/>
    <define name="__DEC32_MIN_EXP__=(-94)"/>
    <define name="__DEC32_MIN__=1E-95DF"/>
    <define name="__DEC32_SUBNORMAL_MIN__=0.000001E-95DF"/>
    <define name="__DEC64_EPSILON__=1E-15DD"/>
    <define name="__DEC64_MANT_DIG__=16"/>
    <define name="__DEC64_MAX_EXP__=385"/>
    <define name="__DEC64_MAX__=9.999999999999999E384DD"/>
    <define name="__DEC64_MIN_EXP__=(-382)"/>
    <define name="__DEC64_MIN__=1E-383DD"/>
    <define name="__DEC64_SUBNORMAL_MIN__=0.000000000000001E-383DD"/>
    <define name="__DECIMAL_BID_FORMAT__=1"/>
    <define name="__DECIMAL_DIG__=21"/>
    <define name="__DEC_EVAL_METHOD__=2"/>
    <define name="__FINITE_MATH_ONLY__=0"/>
    <define name="__FLOAT_WORD_ORDER__=__ORDER_LITTLE_ENDIAN__"/>
    <define name="__FLT128_DECIMAL_DIG__=36"/>
    <define name="__FLT128_DENORM_MIN__=6.47517511943802511092443895822764655e-4966F128"/>
    <define name="__FLT128_DIG__=33"/>
    <define name="__FLT128_EPSILON__=1.92592994438723585305597794258492732e-34F128"/>
    <define name="__FLT128_HAS_DENORM__=1"/>
    <define name="__FLT128_HAS_INFINITY__=1"/>
    <define name="__FLT128_HAS_QUIET_NAN__=1"/>
    <define name="__FLT128_MANT_DIG__=113"/>
    <define name="__FLT128_MAX_10_EXP__=4932"/>
    <define name="__FLT128_MAX_EXP__=16384"/>
    <define name="__FLT128_MAX__=1.18973149535723176508575932662800702e+4932F128"/>
    <define name="__FLT128_MIN_10_EXP__=(-4931)"/>
    <define name="__FLT128_MIN_EXP__=(-16381)"/>
    <define name="__FLT128_MIN__=3.36210314311209350626267781732175260e-4932F128"/>
    <define name="__FLT32X_DECIMAL_DIG__=17"/>
    <define name="__FLT32X_DENORM_MIN__=4.94065645841246544176568792868221372e-324F32x"/>
    <define name="__FLT32X_DIG__=15"/>
    <define name="__FLT32X_EPSILON__=2.22044604925031308084726333618164062e-16F32x"/>
    <define name="__FLT32X_HAS_DENORM__=1"/>
    <define name="__FLT32X_HAS_INFINITY__=1"/>
    <define name="__FLT32X_HAS_QUIET_NAN__=1"/>
    <define name="__FLT32X_MANT_DIG__=53"/>
    <define name="__FLT32X_MAX_10_EXP__=308"/>
    <define name="__FLT32X_MAX_EXP__=1024"/>
    <define name="__FLT32X_MAX__=1.79769313486231570814527423731704357e+308F32x"/>
    <define name="__FLT32X_MIN_10_EXP__=(-307)"/>
    <define name="__FLT32X_MIN_EXP__=(-1021)"/>
    <define name="__FLT32X_MIN__=2.22507385850720138309023271733240406e-308F32x"/>
    <define name="__FLT32_DECIMAL_DIG__=9"/>
    <define name="__FLT32_DENORM_MIN__=1.40129846432481707092372958328991613e-45F32"/>
    <define name="__FLT32_DIG__=6"/>
    <define name="__FLT32_EPSILON__=1.19209289550781250000000000000000000e-7F32"/>
    <define name="__FLT32_HAS_DENORM__=1"/>
    <define name="__FLT32_HAS_INFINITY__=1"/>
    <define name="__FLT32_HAS_QUIET_NAN__=1"/>
    <define name="__FLT32_MANT_DIG__=24"/>
    <define name="__FLT32_MAX_10_EXP__=38"/>
    <define name="__FLT32_MAX_EXP__=128"/>
    <define name="__FLT32_MAX__=3.40282346638528859811704183484516925e+38F32"/>
    <define name="__FLT32_MIN_10_EXP__=(-37)"/>
    <define name="__FLT32_MIN_EXP__=(-125)"/>
    <define name="__FLT32_MIN__=1.17549435082228750796873653722224568e-38F32"/>
    <define name="__FLT64X_DECIMAL_DIG__=21"/>
    <define name="__FLT64X_DENORM_MIN__=3.64519953188247460252840593361941982e-4951F64x"/>
    <define name="__FLT64X_DIG__=18"/>
    <define name="__FLT64X_EPSILON__=1.08420217248550443400745280086994171e-19F64x"/>
    <define name="__FLT64X_HAS_DENORM__=1"/>
    <define name="__FLT64X_HAS_INFINITY__=1"/>
    <define name="__FLT64X_HAS_QUIET_NAN__=1"/>
    <define name="__FLT64X_MANT_DIG__=64"/>
    <define name="__FLT64X_MAX_10_EXP__=4932"/>
    <define name="__FLT64X_MAX_EXP__=16384"/>
    <define name="__FLT64X_MAX__=1.18973149535723176502126385303097021e+4932F64x"/>
    <define name="__FLT64X_MIN_10_EXP__=(-4931)"/>
    <define name="__FLT64X_MIN_EXP__=(-16381)"/>
    <define name="__FLT64X_MIN__=3.36210314311209350626267781732175260e-4932F64x"/>
    <define name="__FLT64_DECIMAL_DIG__=17"/>
    <define name="__FLT64_DENORM_MIN__=4.94065645841246544176568792868221372e-324F64"/>
    <define name="__FLT64_DIG__=15"/>
    <define name="__FLT64_EPSILON__=2.22044604925031308084726333618164062e-16F64"/>
    <define name="__FLT64_HAS_DENORM__=1"/>
    <define name="__FLT64_HAS_INFINITY__=1"/>
    <define name="__FLT64_HAS_QUIET_NAN__=1"/>
    <define name="__FLT64_MANT_DIG__=53"/>
    <define name="__FLT64_MAX_10_EXP__=308"/>
    <define name="__FLT64_MAX_EXP__=1024"/>
    <define name="__FLT64_MAX__=1.79769313486231570814527423731704357e+308F64"/>
    <define name="__FLT64_MIN_10_EXP__=(-307)"/>
    <define name="__FLT64_MIN_EXP__=(-1021)"/>
    <define name="__FLT64_MIN__=2.22507385850720138309023271733240406e-308F64"/>
    <define name="__FLT_DECIMAL_DIG__=9"/>
    <define name="__FLT_DENORM_MIN__=1.40129846432481707092372958328991613e-45F"/>
    <define name="__FLT_DIG__=6"/>
    <define name="__FLT_EPSILON__=1.19209289550781250000000000000000000e-7F"/>
    <define name="__FLT_EVAL_METHOD_TS_18661_3__=2"/>
    <define name="__FLT_EVAL_METHOD__=2"/>
    <define name="__FLT_HAS_DENORM__=1"/>
    <define name="__FLT_HAS_INFINITY__=1"/>
    <define name="__FLT_HAS_QUIET_NAN__=1"/>
    <define name="__FLT_MANT_DIG__=24"/>
    <define name="__FLT_MAX_10_EXP__=38"/>
    <define name="__FLT_MAX_EXP__=128"/>
    <define name="__FLT_MAX__=3.40282346638528859811704183484516925e+38F"/>
    <define name="__FLT_MIN_10_EXP__=(-37)"/>
    <define name="__FLT_MIN_EXP__=(-125)"/>
    <define name="__FLT_MIN__=1.17549435082228750796873653722224568e-38F"/>
    <define name="__FLT_RADIX__=2"/>
    <define name="__GCC_ASM_FLAG_OUTPUTS__=1"/>
    <define name="__GCC_ATOMIC_BOOL_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_CHAR16_T_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_CHAR32_T_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_CHAR_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_INT_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_LLONG_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_LONG_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_POINTER_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_SHORT_LOCK_FREE=2"/>
    <define name="__GCC_ATOMIC_TEST_AND_SET_TRUEVAL=1"/>
    <define name="__GCC_ATOMIC_WCHAR_T_LOCK_FREE=2"/>
    <define name="__GCC_HAVE_DWARF2_CFI_ASM=1"/>
    <define name="__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1=1"/>
    <define name="__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2=1"/>
    <define name="__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4=1"/>
    <define name="__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8=1"/>
    <define name="__GCC_IEC_559=2"/>
    <define name="__GCC_IEC_559_COMPLEX=2"/>
    <define name="__GNUC_MINOR__=1"/>
    <define name="__GNUC_PATCHLEVEL__=0"/>
    <define name="__GNUC_STDC_INLINE__=1"/>
    <define name="__GNUC__=8"/>
    <define name="__GXX_ABI_VERSION=1012"/>
    <define name="__GXX_MERGED_TYPEINFO_NAMES=0"/>
    <define name="__GXX_TYPEINFO_EQUALITY_INLINE=0"/>
    <define name="__INT16_C(c)=c"/>
    <define name="__INT16_MAX__=0x7fff"/>
    <define name="__INT16_TYPE__=short int"/>
    <define name="__INT32_C(c)=c"/>
    <define name="__INT32_MAX__=0x7fffffff"/>
    <define name="__INT32_TYPE__=int"/>
    <define name="__INT64_C(c)=c ## LL"/>
    <define name="__INT64_MAX__=0x7fffffffffffffffLL"/>
    <define name="__INT64_TYPE__=long long int"/>
    <define name="__INT8_C(c)=c"/>
    <define name="__INT8_MAX__=0x7f"/>
    <define name="__INT8_TYPE__=signed char"/>
    <define name="__INTMAX_C(c)=c ## LL"/>
    <define name="__INTMAX_MAX__=0x7fffffffffffffffLL"/>
    <define name="__INTMAX_TYPE__=long long int"/>
    <define name="__INTMAX_WIDTH__=64"/>
    <define name="__INTPTR_MAX__=0x7fffffff"/>
    <define name="__INTPTR_TYPE__=int"/>
    <define name="__INTPTR_WIDTH__=32"/>
    <define name="__INT_FAST16_MAX__=0x7fff"/>
    <define name="__INT_FAST16_TYPE__=short int"/>
    <define name="__INT_FAST16_WIDTH__=16"/>
    <define name="__INT_FAST32_MAX__=0x7fffffff"/>
    <define name="__INT_FAST32_TYPE__=int"/>
    <define name="__INT_FAST32_WIDTH__=32"/>
    <define name="__INT_FAST64_MAX__=0x7fffffffffffffffLL"/>
    <define name="__INT_FAST64_TYPE__=long long int"/>
    <define name="__INT_FAST64_WIDTH__=64"/>
    <define name="__INT_FAST8_MAX__=0x7f"/>
    <define name="__INT_FAST8_TYPE__=signed char"/>
    <define name="__INT_FAST8_WIDTH__=8"/>
    <define name="__INT_LEAST16_MAX__=0x7fff"/>
    <define name="__INT_LEAST16_TYPE__=short int"/>
    <define name="__INT_LEAST16_WIDTH__=16"/>
    <define name="__INT_LEAST32_MAX__=0x7fffffff"/>
    <define name="__INT_LEAST32_TYPE__=int"/>
    <define name="__INT_LEAST32_WIDTH__=32"/>
    <define name="__INT_LEAST64_MAX__=0x7fffffffffffffffLL"/>
    <define name="__INT_LEAST64_TYPE__=long long int"/>
    <define name="__INT_LEAST64_WIDTH__=64"/>
    <define name="__INT_LEAST8_MAX__=0x7f"/>
    <define name="__INT_LEAST8_TYPE__=signed char"/>
    <define name="__INT_LEAST8_WIDTH__=8"/>
    <define name="__INT_MAX__=0x7fffffff"/>
    <define name="__INT_WIDTH__=32"/>
    <define name="__LDBL_DECIMAL_DIG__=21"/>
    <define name="__LDBL_DENORM_MIN__=3.64519953188247460252840593361941982e-4951L"/>
    <define name="__LDBL_DIG__=18"/>
    <define name="__LDBL_EPSILON__=1.08420217248550443400745280086994171e-19L"/>
    <define name="__LDBL_HAS_DENORM__=1"/>
    <define name="__LDBL_HAS_INFINITY__=1"/>
    <define name="__LDBL_HAS_QUIET_NAN__=1"/>
    <define name="__LDBL_MANT_DIG__=64"/>
    <define name="__LDBL_MAX_10_EXP__=4932"/>
    <define name="__LDBL_MAX_EXP__=16384"/>
    <define name="__LDBL_MAX__=1.18973149535723176502126385303097021e+4932L"/>
    <define name="__LDBL_MIN_10_EXP__=(-4931)"/>
    <define name="__LDBL_MIN_EXP__=(-16381)"/>
    <define name="__LDBL_MIN__=3.36210314311209350626267781732175260e-4932L"/>
    <define name="__LONG_LONG_MAX__=0x7fffffffffffffffLL"/>
    <define name="__LONG_LONG_WIDTH__=64"/>
    <define name="__LONG_MAX__=0x7fffffffL"/>
    <define name="__LONG_WIDTH__=32"/>
    <define name="__MINGW32__=1"/>
    <define name="__MSVCRT__=1"/>
    <define name="__NO_INLINE__=1"/>
    <define name="__ORDER_BIG_ENDIAN__=4321"/>
    <define name="__ORDER_LITTLE_ENDIAN__=1234"/>
    <define name="__ORDER_PDP_ENDIAN__=3412"/>
    <define name="__PRAGMA_REDEFINE_EXTNAME=1"/>
    <define name="__PTRDIFF_MAX__=0x7fffffff"/>
    <define name="__PTRDIFF_TYPE__=int"/>
    <define name="__PTRDIFF_WIDTH__=32"/>
    <define name="__REGISTER_PREFIX__="/>
    <define name="__SCHAR_MAX__=0x7f"/>
    <define name="__SCHAR_WIDTH__=8"/>
    <define name="__SEG_FS=1"/>
    <define name="__SEG_GS=1"/>
    <define name="__SHRT_MAX__=0x7fff"/>
    <define name="__SHRT_WIDTH__=16"/>
    <define name="__SIG_ATOMIC_MAX__=0x7fffffff"/>
    <define name="__SIG_ATOMIC_MIN__=(-__SIG_ATOMIC_MAX__ - 1)"/>
    <define name="__SIG_ATOMIC_TYPE__=int"/>
    <define name="__SIG_ATOMIC_WIDTH__=32"/>
    <define name="__SIZEOF_DOUBLE__=8"/>
    <define name="__SIZEOF_FLOAT128__=16"/>
    <define name="__SIZEOF_FLOAT80__=12"/>
    <define name="__SIZEOF_FLOAT__=4"/>
    <define name="__SIZEOF_INT__=4"/>
    <define name="__SIZEOF_LONG_DOUBLE__=12"/>
    <define name="__SIZEOF_LONG_LONG__=8"/>
    <define name="__SIZEOF_LONG__=4"/>
    <define name="__SIZEOF_POINTER__=4"/>
    <define name="__SIZEOF_PTRDIFF_T__=4"/>
    <define name="__SIZEOF_SHORT__=2"/>
    <define name="__SIZEOF_SIZE_T__=4"/>
    <define name="__SIZEOF_WCHAR_T__=2"/>
    <define name="__SIZEOF_WINT_T__=2"/>
    <define name="__SIZE_MAX__=0xffffffffU"/>
    <define name="__SIZE_TYPE__=unsigned int"/>
    <define name="__SIZE_WIDTH__=32"/>
    <define name="__STDC_HOSTED__=1"/>
    <define name="__STDC_UTF_16__=1"/>
    <define name="__STDC_UTF_32__=1"/>
    <define name="__STDC_VERSION__=201710L"/>
    <define name="__STDC__=1"/>
    <define name="__UINT16_C(c)=c"/>
    <define name="__UINT16_MAX__=0xffff"/>
    <define name="__UINT16_TYPE__=short unsigned int"/>
    <define name="__UINT32_C(c)=c ## U"/>
    <define name="__UINT32_MAX__=0xffffffffU"/>
    <define name="__UINT32_TYPE__=unsigned int"/>
    <define name="__UINT64_C(c)=c ## ULL"/>
    <define name="__UINT64_MAX__=0xffffffffffffffffULL"/>
    <define name="__UINT64_TYPE__=long long unsigned int"/>
    <define name="__UINT8_C(c)=c"/>
    <define name="__UINT8_MAX__=0xff"/>
    <define name="__UINT8_TYPE__=unsigned char"/>
    <define name="__UINTMAX_C(c)=c ## ULL"/>
    <define name="__UINTMAX_MAX__=0xffffffffffffffffULL"/>
    <define name="__UINTMAX_TYPE__=long long unsigned int"/>
    <define name="__UINTPTR_MAX__=0xffffffffU"/>
    <define name="__UINTPTR_TYPE__=unsigned int"/>
    <define name="__UINT_FAST16_MAX__=0xffff"/>
    <define name="__UINT_FAST16_TYPE__=short unsigned int"/>
    <define name="__UINT_FAST32_MAX__=0xffffffffU"/>
    <define name="__UINT_FAST32_TYPE__=unsigned int"/>
    <define name="__UINT_FAST64_MAX__=0xffffffffffffffffULL"/>
    <define name="__UINT_FAST64_TYPE__=long long unsigned int"/>
    <define name="__UINT_FAST8_MAX__=0xff"/>
    <define name="__UINT_FAST8_TYPE__=unsigned char"/>
    <define name="__UINT_LEAST16_MAX__=0xffff"/>
    <define name="__UINT_LEAST16_TYPE__=short unsigned int"/>
    <define name="__UINT_LEAST32_MAX__=0xffffffffU"/>
    <define name="__UINT_LEAST32_TYPE__=unsigned int"/>
    <define name="__UINT_LEAST64_MAX__=0xffffffffffffffffULL"/>
    <define name="__UINT_LEAST64_TYPE__=long long unsigned int"/>
    <define name="__UINT_LEAST8_MAX__=0xff"/>
    <define name="__UINT_LEAST8_TYPE__=unsigned char"/>
    <define name="__USER_LABEL_PREFIX__=_"/>
    <define name="__VERSION__=&quot;8.1.0&quot;"/>
    <define name="__WCHAR_MAX__=0xffff"/>
    <define name="__WCHAR_MIN__=0"/>
    <define name="__WCHAR_TYPE__=short unsigned int"/>
    <define name="__WCHAR_WIDTH__=16"/>
    <define name="__WIN32=1"/>
    <define name="__WIN32__=1"/>
    <define name="__WINNT=1"/>
    <define name="__WINNT__=1"/>
    <define name="__WINT_MAX__=0xffff"/>
    <define name="__WINT_MIN__=0"/>
    <define name="__WINT_TYPE__=short unsigned int"/>
    <define name="__WINT_WIDTH__=16"/>
    <define name="__cdecl=__attribute__((__cdecl__))"/>
    <define name="__code_model_32__=1"/>
    <define name="__declspec(x)=__attribute__((x))"/>
    <define name="__fastcall=__attribute__((__fastcall__))"/>
    <define name="__has_include(STR)=__has_include__(STR)"/>
    <define name="__has_include_next(STR)=__has_include_next__(STR)"/>
    <define name="__i386=1"/>
    <define name="__i386__=1"/>
    <define name="__i686=1"/>
    <define name="__i686__=1"/>
    <define name="__pentiumpro=1"/>
    <define name="__pentiumpro__=1"/>
    <define name="__stdcall=__attribute__((__stdcall__))"/>
    <define name="__thiscall=__attribute__((__thiscall__))"/>
    <define name="_cdecl=__attribute__((__cdecl__))"/>
    <define name="_fastcall=__attribute__((__fastcall__))"/>
    <define name="_stdcall=__attribute__((__stdcall__))"/>
    <define name="_thiscall=__attribute__((__thiscall__))"/>
    <define name="i386=1"/>
   </defines>
   <includes>
    <include path="C:/Users/pig1tn/Documents/installation_stuff/Cantata_9.5_Win_64/cantata/inc"/>
    <include path="${{SOURCE_ROOT}}/../../30_Generation/RTA-CAR_SPM_Project_Dev/src/rte"/>
    <include path="${{SOURCE_ROOT}}/../../30_Generation/RTA-CAR_SPM_Project_Dev/src/sum"/>
    <include path="${{SOURCE_ROOT}}/Stubs/api"/>
    <include path="${{SOURCE_ROOT}}/../../20_Implementation/api"/>
   </includes>
  </file>
"""

lst_of_files = [
    {
        "File_type": "Dyn",
        "File_name": "Sum_Spm_CanIf_Cbk.c"
    },
    {
        "File_type": "Dyn",
        "File_name": "Sum_Spm_CanTransmit.c"
    },
    {
        "File_type": "Dyn",
        "File_name": "Sum_Spm_DevicePowerState.c"
    },
    {
        "File_type": "Dyn",
        "File_name": "Sum_Spm_PowerOnDevice.c"
    },
    {
        "File_type": "Dyn",
        "File_name": "Sum_Spm_WakeUpOnCan.c"
    },
    {
        "File_type": "Stat",
        "File_name": "Sum_Spm_CanIfDispatchUL.c"
    },
    {
        "File_type": "Stat",
        "File_name": "Sum_Spm_CommErrHndlr.c"
    }
]

esit = []

for f in lst_of_files:
    esit.append(tmpl.format(
        file_name=f["File_name"],
        final_pth=sta_pth if f["File_type"] == "Stat" else dyn_pth
    ))

with open("dummy.txt", "w", encoding='utf-8') as fw:
    fw.write("\n".join(esit))