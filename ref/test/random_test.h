#ifndef RANDOM_TEST_H
#define RANDOM_TEST_H

#include <stdarg.h>
#include "../data_type.h"
#include "../random.h"
#include "../msg_control.h"
#include "test_main.h"

msg get_random_bigint(int count, ...);

#endif // RANDOM_TEST_H