import gc, os, ctypes, platform, argparse

# parse arguments
parser = argparse.ArgumentParser(description="Set BigInt confiugration")
parser.add_argument("--WORD_BITS", type=int, choices=[8, 16, 32, 64], required=True,
                    help="Specify the size of WORD_BITS (8, 16, 32, or 64).")
parser.add_argument("--RANDOM", type=int, choices=[0, 1], required=True,
                    help="Specify whether to use random values (0 = not random or 1 = random).")
parser.add_argument("--ITERATE", type=int, required=True,
                    help="Specify the number of iterations.")
args = parser.parse_args()

WORD_BITS = args.WORD_BITS
if WORD_BITS == 64:
    word = ctypes.c_uint64
    MAX_VALUE = (1 << 64) - 1  # 64비트 최대값
elif WORD_BITS == 32:
    word = ctypes.c_uint32
    MAX_VALUE = (1 << 32) - 1  # 32비트 최대값
elif WORD_BITS == 16:
    word = ctypes.c_uint16
    MAX_VALUE = (1 << 16) - 1  # 16비트 최대값
elif WORD_BITS == 8:
    word = ctypes.c_uint8
    MAX_VALUE = (1 << 8) - 1   # 8비트 최대값
else:
    raise ValueError("Unsupported WORD_BITS value")

class bigint(ctypes.Structure):
    _fields_ = [
        ("sign", ctypes.c_uint8),       # uint8_t (byte)
        ("word_len", ctypes.c_int),    # int
        ("a", ctypes.POINTER(word)),   # word* pointer
    ]

class msg(ctypes.Structure):
    _fields_ = [("msg", ctypes.c_int)]

BITS_SIZE = 1024
WORD_BITS = args.WORD_BITS
test_word_size = BITS_SIZE // WORD_BITS if WORD_BITS != 0 else 0
test_word_size_limit = BITS_SIZE // WORD_BITS if WORD_BITS != 0 else 0
test_size = args.ITERATE
barret_word_size = test_word_size
RANDOM_TEST = args.RANDOM

def preset():
    gc.disable()
    current_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(current_path)
    os.chdir("../") # change dir to upper level

def load_lib(OS):
    lib_path = "./lib/lib_HEPHAISTOS"
    lib = None
    try:
        if OS == "windows":
            lib = ctypes.CDLL(f"{lib_path}.dll")
        elif OS == "linux":
            lib = ctypes.CDLL(f"{lib_path}.so")
        elif OS == "Darwin":
            os.environ['DYLD_INSERT_LIBRARIES'] = '/Library/Developer/CommandLineTools/usr/lib/clang/16/lib/darwin/libclang_rt.asan_osx_dynamic.dylib'
            lib = ctypes.CDLL(f"{lib_path}.dylib")
        else:
            raise ValueError("Unsupported OS type")
        print("Library loaded successfully.")
    except OSError as e:
        print(f"Failed to load library: {e}")
    except ValueError as ve:
        print(f"Invalid input: {ve}")

    return lib

def load_func(lib):
    function = {}
    # get util funciton
    function['bi_new'] = lib.bi_new
    function['bi_delete'] = lib.bi_delete
    function['bi_refine'] = lib.bi_refine
    function['bi_assign'] = lib.bi_assign
    function['bi_resize'] = lib.bi_resize
    function['bi_expand'] = lib.bi_expand
    function['bi_set_from_array'] = lib.bi_set_from_array
    function['bi_set_from_string'] = lib.bi_set_from_string
    function['bi_compare'] = lib.bi_compare
    function['bi_compare_abs'] = lib.bi_compare_abs
    function['bi_shift_left'] = lib.bi_shift_left
    function['bi_shift_right'] = lib.bi_shift_right
    function['bi_get_lower'] = lib.bi_get_lower
    function['bi_cat'] = lib.bi_cat
    function['bi_is_zero'] = lib.bi_is_zero
    function['get_power_decomposition'] = lib.get_power_decomposition

    # get random function
    function['bi_get_random'] = lib.bi_get_random
    function['array_random'] = lib.array_random
    function['get_random_bigint'] = lib.get_random_bigint
    function['randombytes'] = lib.randombytes
    function['get_random_string'] = lib.get_random_string

    # get operate function
    function['bi_add'] = lib.bi_add
    function['bi_sub'] = lib.bi_sub
    function['bi_mul'] = lib.bi_mul
    function['bi_mul_word'] = lib.bi_mul_word
    function['bi_mul_karachuba'] = lib.bi_mul_karachuba
    function['init_karachuba_pool'] = lib.init_karachuba_pool
    function['clear_karachuba_pool'] = lib.clear_karachuba_pool
    function['bi_div'] = lib.bi_div
    function['divc'] = lib.divc
    function['divc_gener'] = lib.divc_gener
    function['divcc'] = lib.divcc
    function['two_word_long_div'] = lib.two_word_long_div
    function['bi_squ'] = lib.bi_squ
    function['bi_squ_karachuba'] = lib.bi_squ_karachuba
    function['bi_exp_MS'] = lib.bi_exp_MS
    function['bi_exp_R_TO_L'] = lib.bi_exp_R_TO_L
    function['bi_exp_L_TO_R'] = lib.bi_exp_L_TO_R
    function['barret_reduction'] = lib.barret_reduction
    function['init_barret_N'] = lib.init_barret_N

    # get crypto function
    function['RSA'] = lib.RSA
    function['RSA_CRT'] = lib.RSA_CRT
    function['RSA_KeyGEN'] = lib.RSA_KeyGEN
    function['RSA_ENC'] = lib.RSA_ENC
    function['RSA_DEC'] = lib.RSA_DEC
    function['RSA_CRT_KeyGEN'] = lib.RSA_CRT_KeyGEN
    function['RSA_CRT_DEC'] = lib.RSA_CRT_DEC
    function['miller_rabin_primality'] = lib.miller_rabin_primality
    function['bi_gcd'] = lib.bi_gcd
    function['bi_EEA'] = lib.bi_EEA

    # get util function
    function['bi_print'] = lib.bi_print
    function['int_to_string'] = lib.int_to_string
    function['bigint_to_hex'] = lib.bigint_to_hex

    for key in function.keys():
        function[key].restype = msg

    return function

def print_test_result(result, msg = ''):
    if result == 1:
        print('\n--------------- WORD INFO ---------------')
        print(f'Bigint bit size : {BITS_SIZE}')
        print(f'Word Bit Size : {WORD_BITS}, Word size : {test_word_size}')
        print(f'itersize : {test_size}')
        print('-----------------------------------------\n')
    else:
        print(f"Test failed: {msg}")

if __name__ == '__main__':
    OS = platform.system()

    preset()
    lib = load_lib(OS)

    function = load_func(lib)
