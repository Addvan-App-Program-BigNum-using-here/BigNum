import test_util as util
import ctypes
import platform

def test_bi_new_delete(function):
    a = ctypes.POINTER(util.bigint)()
    test_word_size = util.test_word_size

    print('\n============ Testing bi_new_delete ============')

    for i in range(util.test_size):
        if util.RANDOM_TEST == 1:
            while True:
                temp = ctypes.c_uint8(0)
                function['randombytes'](ctypes.byref(temp), 1)
                test_word_size = temp.value % util.test_word_size_limit
                if test_word_size > 0:
                    break
        function['bi_new'](ctypes.byref(a), ctypes.c_int(test_word_size))

        if a.contents.word_len != test_word_size:
            function['bi_delete'](ctypes.byref(a))
            util.print_test_result(0, "word_len is not equal to rand_test_word_size")

        function['bi_new'](ctypes.byref(a))
        if a.contents.word_len != 0:
            function['bi_delete'](ctypes.byref(a))
            util.print_test_result(0, "word_len is not equal to 0")

    util.print_test_result(1, "")

def test_bi_random(function):
    dst = ctypes.POINTER(util.bigint)()
    test_word_size = util.test_word_size

    print('\n============ Testing bi_random ============')

    for i in range(util.test_size):
        if util.RANDOM_TEST == 1:
            while True:
                temp = ctypes.c_uint8(0)
                function['randombytes'](ctypes.byref(temp), 1)
                test_word_size = temp.value % util.test_word_size_limit
                if test_word_size > 0:
                    break
        function['bi_get_random'](ctypes.byref(dst), ctypes.c_int(test_word_size))

        function['bi_delete'](ctypes.byref(dst))

    util.print_test_result(1, "")

def test_bi_set_from(function):
    a = ctypes.POINTER(util.bigint)()
    test_array = ctypes.POINTER(util.word)()
    test_word_size = util.test_word_size

    print('\n============ Testing bi_set_from ============')

    for i in range(util.test_size):
        if util.RANDOM_TEST == 1:
            while True:
                temp = ctypes.c_uint8(0)
                function['randombytes'](ctypes.byref(temp), 1)
                test_word_size = temp.value % (util.test_word_size_limit * 10)
                if test_word_size > 0:
                    break

        test_array = (util.word * test_word_size)()
        function['array_random'](test_array, test_word_size)

        endian = 1
        function['bi_set_from_array'](ctypes.byref(a), ctypes.c_int(test_word_size), test_array, ctypes.c_int(endian))

        for i in range(test_word_size):
            if a.contents.a[i] != test_array[i]:
                util.print_test_result(0, "bi_set_from_array failed")
                break

        endian = 0
        function['bi_set_from_array'](ctypes.byref(a), ctypes.c_int(test_word_size), test_array, ctypes.c_int(endian))

        for i in range(test_word_size):
            if a.contents.a[(test_word_size - 1) - i] != test_array[i]:
                util.print_test_result(0, "bi_set_from_array failed")
                break


    test_bi_set_from_base(function, 2)
    # test_bi_set_from_base(function, 10)
    test_bi_set_from_base(function, 16)

    function['bi_delete'](ctypes.byref(a))
    del test_array

def test_bi_set_from_base(function, base):
    a = ctypes.POINTER(util.bigint)()
    str_base = ctypes.c_char_p()
    check_str = ctypes.c_char_p()
    array_size = util.test_word_size

    for i in range(util.test_size):
        if util.RANDOM_TEST == 1:
            while True:
                temp = ctypes.c_uint8(0)  # 랜덤 값을 받아오기 위한
                function['randombytes'](ctypes.byref(temp), 1)
                array_size = temp.value % (util.test_word_size_limit * 10)
                if array_size > 0:
                    break

        check_str = (array_size * util.WORD_BITS * ctypes.c_char)()
        function['get_random_string'](ctypes.byref(str_base), ctypes.c_int(array_size), ctypes.c_int(base))
        function['bi_set_from_string'](ctypes.byref(a), str_base, ctypes.c_int(base))
        function['bigint_to_hex'](check_str, ctypes.byref(a))

        if int(str_base.value.decode('utf-8'), base) != int(check_str.value.decode('utf-8'), 16):
            util.print_test_result(0, "bi_set_from_string failed")
            print(f"str_base: {hex(int(str_base.value.decode('utf-8'), base))}")
            print(f"check_str: {check_str.value.decode('utf-8')}")
            break

    function['bi_delete'](ctypes.byref(a))
    util.print_test_result(1, "")
    del str_base

def test_bi_shift(function):
    test_word_size = util.test_word_size
    a = ctypes.POINTER(util.bigint)()
    c = ctypes.POINTER(util.bigint)()
    shift_size = 0
    temp = ctypes.c_uint8(0)

    print('\n============ Testing bi_shift ============')

    for i in range(util.test_size):
        if util.RANDOM_TEST == 1:
            while True:
                function['randombytes'](ctypes.byref(temp), 1)
                test_word_size = temp.value % util.test_word_size_limit
                if test_word_size > 0:
                    break
        function['randombytes'](ctypes.byref(temp), 1)
        shift_size = temp.value % (test_word_size * util.WORD_BITS)

        function['bi_get_random'](ctypes.byref(a), ctypes.c_int(test_word_size))
        if a.contents.word_len != test_word_size:
            util.print_test_result(0, "word_len is not equal to rand_test_word_size")

        a.contents.sign = 0

        function['bi_shift_left'](ctypes.byref(c), ctypes.byref(a), ctypes.c_int(shift_size))

        # 결과 값 확인하는 부분 구현

        function['bi_shift_right'](ctypes.byref(c), ctypes.byref(a), ctypes.c_int(shift_size))

        # 결과 값 확인하는 부분 구현

    util.print_test_result(1, "")

    function['bi_delete'](ctypes.byref(a))
    function['bi_delete'](ctypes.byref(c))


if __name__ == '__main__':
    OS = platform.system()
    util.preset()
    lib = util.load_lib(OS)
    function = util.load_func(lib)

    # test_bi_new_delete(function)
    # test_bi_random(function)
    test_bi_set_from(function)
    # test_bi_shift(function)