#include "purc.h"
#include "private/avl.h"
#include "private/hashtable.h"
#include "private/variant.h"
#include "private/errors.h"
#include "private/debug.h"
#include "private/utils.h"
#include "private/dvobjs.h"

#include "../helpers.h"

#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#ifndef __USE_GNU
#define __USE_GNU                       /* for M_PIl when using glibc */
#endif
#ifndef __MATH_LONG_DOUBLE_CONSTANTS
#define __MATH_LONG_DOUBLE_CONSTANTS    /* for M_PIl when using MacOSX SDK */
#endif
#include <math.h>
#include <sstream>
#include <gtest/gtest.h>

extern void get_variant_total_info (size_t *mem, size_t *value, size_t *resv);
#define MAX_PARAM_NR    20

struct dvobjs_math_method_d
{
    const char * func;
    double param;
    double d;
};

struct dvobjs_math_method_ld
{
    const char * func;
    long double param;
    long double ld;
};

TEST(dvobjs, dvobjs_math_pi_e)
{
    static struct dvobjs_math_method_d math_d[] =
    {
        {
            "pi",
            0.0,
            M_PI,
        },
        {
            "e",
            0.0,
            M_E,
        }
    };

    static struct dvobjs_math_method_ld math_ld[] =
    {
        {
            "pi_l",
            0.0L,
            M_PIl
        },
        {
            "e_l",
            0.0L,
            M_El
        }
    };

    size_t i = 0;
    size_t size = PCA_TABLESIZE(math_d);
    purc_variant_t param[MAX_PARAM_NR];
    purc_variant_t ret_var = NULL;
    double number;
    long double numberl;
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;

    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    for (i = 0; i < size; i++) {
        // test double function
        purc_variant_t dynamic = purc_variant_object_get_by_ckey (math,
                math_d[i].func);
        ASSERT_NE(dynamic, nullptr);
        ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

        purc_dvariant_method func = NULL;
        func = purc_variant_dynamic_get_getter (dynamic);
        ASSERT_NE(func, nullptr);

        ret_var = func (NULL, 0, param);
        ASSERT_NE(ret_var, nullptr);

        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER),
                true);

        purc_variant_cast_to_number (ret_var, &number, false);
        ASSERT_EQ(number, math_d[i].d);
        purc_variant_unref(ret_var);

        // test long double function
        dynamic = purc_variant_object_get_by_ckey (math, math_ld[i].func);
        ASSERT_NE(dynamic, nullptr);
        ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

        func = purc_variant_dynamic_get_getter (dynamic);
        ASSERT_NE(func, nullptr);

        ret_var = func (NULL, 0, param);
        ASSERT_NE(ret_var, nullptr);

        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
                true);

        purc_variant_cast_to_long_double (ret_var, &numberl, false);
        ASSERT_EQ(numberl, math_ld[i].ld);
        purc_variant_unref(ret_var);
    }

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}


TEST(dvobjs, dvobjs_math_const)
{
    static struct dvobjs_math_method_d math_d[] =
    {
        {
            "e",
            0.0,
            M_E,
        },
        {
            "log2e",
            0.0,
            M_LOG2E,
        },
        {
            "log10e",
            0.0,
            M_LOG10E,
        },
        {
            "ln2",
            0.0,
            M_LN2,
        },
        {
            "ln10",
            0.0,
            M_LN10,
        },
        {
            "pi",
            0.0,
            M_PI,
        },
        {
            "pi/2",
            0.0,
            M_PI_2,
        },
        {
            "pi/4",
            0.0,
            M_PI_4,
        },
        {
            "1/pi",
            0.0,
            M_1_PI,
        },
        {
            "1/sqrt(2)",
            0.0,
            M_SQRT1_2,
        },
        {
            "2/pi",
            0.0,
            M_2_PI,
        },
        {
            "sqrt(2)",
            0.0,
            M_SQRT2,
        }
    };

    static struct dvobjs_math_method_ld math_ld[] =
    {
        {
            "e",
            0.0L,
            M_El
        },
        {
            "log2e",
            0.0L,
            M_LOG2El
        },
        {
            "log10e",
            0.0L,
            M_LOG10El
        },
        {
            "ln2",
            0.0L,
            M_LN2l
        },
        {
            "ln10",
            0.0L,
            M_LN10l
        },
        {
            "pi",
            0.0L,
            M_PIl
        },
        {
            "pi/2",
            0.0L,
            M_PI_2l
        },
        {
            "pi/4",
            0.0L,
            M_PI_4l
        },
        {
            "1/pi",
            0.0L,
            M_1_PIl
        },
        {
            "1/sqrt(2)",
            0.0L,
            M_SQRT1_2l
        },
        {
            "2/pi",
            0.0L,
            M_2_PIl
        },
        {
            "sqrt(2)",
            0.0L,
            M_SQRT2l
        }
    };

    size_t i = 0;
    size_t size = PCA_TABLESIZE(math_d);
    purc_variant_t param[MAX_PARAM_NR];
    purc_variant_t ret_var = NULL;
    double number;
    long double numberl;
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;


    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "const");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method getter = NULL;
    getter = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(getter, nullptr);

    purc_dvariant_method setter = NULL;
    setter = purc_variant_dynamic_get_setter (dynamic);
    ASSERT_NE(setter, nullptr);

    for (i = 0; i < size; i++) {
        param[0] = purc_variant_make_string (math_d[i].func, true);
        ret_var = getter (NULL, 1, param);
        ASSERT_NE(ret_var, nullptr);
        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER),
                true);
        purc_variant_cast_to_number (ret_var, &number, false);
        ASSERT_EQ(number, math_d[i].d);
        purc_variant_unref(ret_var);
        purc_variant_unref(param[0]);
    }

    // test setter to replace
    param[0] = purc_variant_make_string ("e", true);
    param[1] = purc_variant_make_number(123);
    ret_var = setter (NULL, 2, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_BOOLEAN),
                true);
    purc_variant_unref (ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);

    param[0] = purc_variant_make_string ("e", true);
    ret_var = getter (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER),
                true);
    purc_variant_cast_to_number (ret_var, &number, false);
    ASSERT_EQ(number, 123);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    // restore e for test const_l
    param[0] = purc_variant_make_string ("e", true);
    param[1] = purc_variant_make_number(M_E);
    param[2] = purc_variant_make_longdouble(M_El);
    ret_var = setter (NULL, 3, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_BOOLEAN),
                true);
    purc_variant_unref (ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);
    purc_variant_unref(param[2]);
    // test setter to create
    param[0] = purc_variant_make_string ("newone", true);
    param[1] = purc_variant_make_number(123);
    ret_var = setter (NULL, 2, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_BOOLEAN),
                true);
    purc_variant_unref (ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);

    param[0] = purc_variant_make_string ("newone", true);
    ret_var = getter (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER),
                true);
    purc_variant_cast_to_number (ret_var, &number, false);
    ASSERT_EQ(number, 123);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    // test const_l
    dynamic = purc_variant_object_get_by_ckey (math, "const_l");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    getter = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(getter, nullptr);
    setter = purc_variant_dynamic_get_setter (dynamic);
    ASSERT_NE(getter, nullptr);

    for (i = 0; i < size; i++) {
        param[0] = purc_variant_make_string (math_ld[i].func, true);
        ret_var = getter (NULL, 1, param);
        ASSERT_NE(ret_var, nullptr);
        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
                true);
        purc_variant_cast_to_long_double (ret_var, &numberl, false);
        ASSERT_EQ(numberl, math_ld[i].ld);
        purc_variant_unref(ret_var);
        purc_variant_unref(param[0]);
    }

    param[0] = purc_variant_make_string ("abcd", true);
    ret_var = getter (NULL, 1, param);
    ASSERT_EQ(ret_var, nullptr);
    purc_variant_unref(param[0]);

    // test setter to create
    param[0] = purc_variant_make_string ("newone", true);
    param[1] = purc_variant_make_longdouble(123);
    ret_var = setter (NULL, 2, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_BOOLEAN),
                true);
    purc_variant_unref (ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);

    param[0] = purc_variant_make_string ("newone", true);
    ret_var = getter (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
                true);
    purc_variant_cast_to_long_double (ret_var, &numberl, false);
    ASSERT_EQ(numberl, 123);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}

TEST(dvobjs, dvobjs_math_func)
{
    static struct dvobjs_math_method_d math_d[] =
    {
        {
            "sin",
            M_PI / 2,
            1.0,
        },
        {
            "cos",
            M_PI,
            -1.0,
        },
        {
            "sqrt",
            9.0,
            3.0,
        }
    };

    static struct dvobjs_math_method_ld math_ld[] =
    {
        {
            "sin_l",
            M_PIl / 2,
            1.0L
        },
        {
            "cos_l",
            M_PIl,
            -1.0L
        },
        {
            "sqrt_l",
            9.0L,
            3.0L
        }
    };

    size_t i = 0;
    size_t size = PCA_TABLESIZE(math_d);
    purc_variant_t param[MAX_PARAM_NR];
    purc_variant_t ret_var = NULL;
    double number;
    long double numberl;
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;

    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);


    for (i = 0; i < size; i++) {
        purc_variant_t dynamic = purc_variant_object_get_by_ckey (math,
                math_d[i].func);
        ASSERT_NE(dynamic, nullptr);
        ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

        purc_dvariant_method func = NULL;
        func = purc_variant_dynamic_get_getter (dynamic);
        ASSERT_NE(func, nullptr);

        param[0] = purc_variant_make_number (math_d[i].param);
        ret_var = func (NULL, 1, param);
        ASSERT_NE(ret_var, nullptr);
        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER),
                true);
        purc_variant_cast_to_number (ret_var, &number, false);
        ASSERT_EQ(number, math_d[i].d);

        purc_variant_unref(ret_var);
        purc_variant_unref(param[0]);


        dynamic = purc_variant_object_get_by_ckey (math, math_ld[i].func);
        ASSERT_NE(dynamic, nullptr);
        ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

        func = purc_variant_dynamic_get_getter (dynamic);
        ASSERT_NE(func, nullptr);

        param[0] = purc_variant_make_longdouble (math_ld[i].param);
        ret_var = func (NULL, 1, param);
        ASSERT_NE(ret_var, nullptr);
        ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
                true);
        purc_variant_cast_to_long_double (ret_var, &numberl, false);
        ASSERT_EQ(numberl, math_ld[i].ld);

        purc_variant_unref(ret_var);
        purc_variant_unref(param[0]);
    }

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}

TEST(dvobjs, dvobjs_math_eval)
{
    purc_variant_t param[MAX_PARAM_NR];
    purc_variant_t ret_var = NULL;
    double number;
    long double numberl;
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;

    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "eval");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method func = NULL;
    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    const char *exp = "(3 + 7) * (2 + 3 * 4)";
    param[0] = purc_variant_make_string (exp, false);
    ret_var = func (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER), true);
    purc_variant_cast_to_number (ret_var, &number, false);
    printf("TEST eval: param is \"%s\" = %lf\n", exp, number);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    exp = "(3 + 7) / (2 - 2)";
    param[0] = purc_variant_make_string (exp, false);
    ret_var = func (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER), true);
    purc_variant_cast_to_number (ret_var, &number, false);
    printf("TEST eval: param is \"%s\" = %f\n", exp, number);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    param[0] = purc_variant_make_string ("pi * r * r", false);
    param[1] = purc_variant_make_object (0, PURC_VARIANT_INVALID,
                PURC_VARIANT_INVALID);
    purc_variant_t pi = purc_variant_make_number(M_PI);
    purc_variant_t one = purc_variant_make_number(1.0);
    purc_variant_object_set_by_static_ckey (param[1], "pi", pi);
    purc_variant_object_set_by_static_ckey (param[1], "r", one);
    purc_variant_unref(one);
    purc_variant_unref(pi);
    ret_var = func (NULL, 2, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER), true);
    purc_variant_cast_to_number (ret_var, &number, false);
    printf("TEST eval: param is \"pi * r * r\", r = 1.0, value = %lf\n",
            number);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);

    dynamic = purc_variant_object_get_by_ckey (math, "eval_l");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    exp = "(3 + 7) * (2 + 3)";
    param[0] = purc_variant_make_string (exp, false);
    ret_var = func (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
            true);
    purc_variant_cast_to_long_double (ret_var, &numberl, false);
    printf("TEST eval_l: param is \"%s\" = %Lf\n", exp, numberl);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    exp = "(3 + 7) / (2 - 2)";
    param[0] = purc_variant_make_string (exp, false);
    ret_var = func (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
            true);
    purc_variant_cast_to_long_double (ret_var, &numberl, false);
    printf("TEST eval_l: param is \"%s\" = %Lf\n", exp, numberl);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);

    param[0] = purc_variant_make_string ("pi * r * r", false);
    param[1] = purc_variant_make_object (0, PURC_VARIANT_INVALID,
                PURC_VARIANT_INVALID);
    pi = purc_variant_make_longdouble(M_PI);
    one = purc_variant_make_longdouble (1.0);
    purc_variant_object_set_by_static_ckey (param[1], "pi", pi);
    purc_variant_object_set_by_static_ckey (param[1], "r", one);
    purc_variant_unref(one);
    purc_variant_unref(pi);
    ret_var = func (NULL, 2, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_LONGDOUBLE),
            true);
    purc_variant_cast_to_long_double (ret_var, &numberl, false);
    printf("TEST eval_l: param is \"pi * r * r\", r = 1.0, value = %Lf\n",
            numberl);
    purc_variant_unref(ret_var);
    purc_variant_unref(param[0]);
    purc_variant_unref(param[1]);

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}

TEST(dvobjs, dvobjs_math_assignment)
{
    purc_variant_t param[MAX_PARAM_NR];
    purc_variant_t ret_var = NULL;
    double number;
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;

    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "eval");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method func = NULL;
    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    const char *exp = "x = (3 + 7) * (2 + 3 * 4)\nx*3";
    param[0] = purc_variant_make_string (exp, false);
    ret_var = func (NULL, 1, param);
    ASSERT_NE(ret_var, nullptr);
    ASSERT_EQ(purc_variant_is_type (ret_var, PURC_VARIANT_TYPE_NUMBER), true);
    purc_variant_cast_to_number (ret_var, &number, false);
    printf("TEST eval: param is \"%s\" = %lf\n", exp, number);

    purc_variant_unref(param[0]);

    purc_variant_unref(ret_var);

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}

struct test_sample {
    const char      *expr;
    const char      *result;
};

TEST(dvobjs, dvobjs_math_samples)
{
    struct test_sample samples[] = {
        {"1+2", "3"},
        {"-1", "-1"},
        {"1+-2", "-1"},
        {"1 + - 2", "-1"},
        {"x = (3 + 7) * (2 + 3 * 4)\nx*3", "420"},
        {"-(3+4)", "-7"},
        {"1+2\n", "3"},
        {"1+2\n\n", "3"},
        {"\n\n1+2\n\n", "3"},
        {"\n\n1+2", "3"},
        {"\n1+2", "3"},
    };
    purc_variant_t param[MAX_PARAM_NR];
    size_t sz_total_mem_before = 0;
    size_t sz_total_values_before = 0;
    size_t nr_reserved_before = 0;
    size_t sz_total_mem_after = 0;
    size_t sz_total_values_after = 0;
    size_t nr_reserved_after = 0;

    purc_instance_extra_info info = {};
    int ret = purc_init ("cn.fmsoft.hybridos.test", "test_init", &info);
    ASSERT_EQ (ret, PURC_ERROR_OK);

    get_variant_total_info (&sz_total_mem_before, &sz_total_values_before,
            &nr_reserved_before);

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "eval");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method func = NULL;
    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    char buf[4096];
    purc_rwstream_t ws = purc_rwstream_new_from_mem(buf, sizeof(buf)-1);
    for (size_t i=0; i<sizeof(samples)/sizeof(samples[0]); ++i) {
        purc_rwstream_seek(ws, 0, SEEK_SET);
        buf[0] = '\0';
        buf[sizeof(buf)-1] = '\0';

        const char *expr= samples[i].expr;
        param[0] = purc_variant_make_string(expr, false);
        purc_variant_t ret_var = func(NULL, 1, param);
        purc_variant_unref(param[0]);

        if (!ret_var) {
            EXPECT_NE(ret_var, nullptr) << "eval failed: ["
                << expr << "]" << std::endl;
            continue;
        }
        ssize_t nr = purc_variant_serialize(ret_var, ws,
                        0, 0, NULL);
        EXPECT_GE(nr, 0) << "cast failed: ["
            << expr << "]" << std::endl;
        if (nr>=0)
            buf[nr] = '\0';
        EXPECT_STREQ(buf, samples[i].result) << "eval failed: ["
                << expr << "]" << std::endl;
        purc_variant_unref(ret_var);
    }

    purc_rwstream_destroy(ws);

    purc_variant_unload_dvobj (math);

    get_variant_total_info (&sz_total_mem_after,
            &sz_total_values_after, &nr_reserved_after);
    ASSERT_EQ(sz_total_values_before, sz_total_values_after);
    ASSERT_EQ(sz_total_mem_after, sz_total_mem_before + (nr_reserved_after -
                nr_reserved_before) * sizeof(purc_variant));

    purc_cleanup ();
}

static void
_trim_tail_spaces(char *dest, size_t n)
{
    while (n>1) {
        if (!isspace(dest[n-1]))
            break;
        dest[--n] = '\0';
    }
}

static void
_eval(purc_dvariant_method func, const char *expr, long double *v,
    std::stringstream &ss)
{
    purc_variant_t param[3];
    param[0] = purc_variant_make_string(expr, false);

    purc_variant_t ret_var = func(NULL, 1, param);
    purc_variant_unref(param[0]);

    if (!ret_var) {
        EXPECT_NE(ret_var, nullptr) << "eval failed: ["
            << expr << "]" << std::endl;
        return;
    }

    if (purc_variant_is_number(ret_var)) {
        *v = ret_var->d;
        ss << *v;
    } else if (purc_variant_is_longdouble(ret_var)) {
        *v = ret_var->ld;
        ss << *v;
    }

    purc_variant_unref(ret_var);
}

static void
_eval_bc(const char *fn, long double *v,
    std::stringstream &ss)
{
    FILE *fin = NULL;
    char cmd[8192], dest[8192];
    size_t n = 0;
    char *endptr = NULL;

    snprintf(cmd, sizeof(cmd), "(echo 'scale=20'; cat '%s';) | bc", fn);
    fin = popen(cmd, "r");
    EXPECT_NE(fin, nullptr) << "failed to execute: [" << cmd << "]"
        << std::endl;
    if (!fin)
        return;

    n = fread(dest, 1, sizeof(dest)-1, fin);
    dest[n] = '\0';
    _trim_tail_spaces(dest, n);

    *v = strtold(dest, &endptr);
    if (endptr && *endptr) {
        EXPECT_TRUE(false) << "failed to execute: [" << cmd << "]"
            << std::endl;
    } else {
        ss << dest;
    }

    if (fin)
        pclose(fin);
}

static void
_process_file(purc_dvariant_method func, const char *fn, long double *v,
    std::stringstream &ss)
{
    FILE *fin = NULL;
    size_t sz = 0;
    char buf[8192];
    buf[0] = '\0';

    fin = fopen(fn, "r");
    if (!fin) {
        int err = errno;
        EXPECT_NE(fin, nullptr) << "Failed to open ["
            << fn << "]: [" << err << "]" << strerror(err) << std::endl;
        goto end;
    }

    sz = fread(buf, 1, sizeof(buf)-1, fin);
    buf[sz] = '\0';

    _eval(func, buf, v, ss);

end:
    if (fin)
        fclose(fin);
}

static inline bool
long_double_eq(long double l, long double r)
{
    long double lp = fabsl(l);
    long double rp = fabsl(r);
    long double maxp = lp > rp ? lp : rp;
    return fabsl(lp - rp) <= maxp * FLT_EPSILON;
}

TEST(dvobjs, dvobjs_math_bc)
{
    int r = 0;
    DIR *d = NULL;
    struct dirent *dir = NULL;
    char path[1024] = {0};

    purc_instance_extra_info info = {};
    r = purc_init("cn.fmsoft.hybridos.test",
        "dvobjs_math_bc", &info);
    EXPECT_EQ(r, PURC_ERROR_OK);
    if (r)
        return;

    char so_path[PATH_MAX+1];
    const char *env;
    env = "DVOBJS_SO_PATH";
    test_getpath_from_env_or_rel(so_path, sizeof(so_path),
        env, "../../../build/Source/ExtDVObjs");
    std::cerr << "env: " << env << "=" << so_path << std::endl;

#ifndef __APPLE__
    strncat(so_path, "/math/libpurc-dvobj-MATH.so", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#else
    strncat(so_path, "/math/purc-dvobj-MATH.framework/purc-dvobj-MATH", sizeof(so_path)-1);
    so_path[sizeof(so_path)-1] = '\0';
#endif
    purc_variant_t math = purc_variant_load_dvobj_from_so (
        so_path, "MATH");
    ASSERT_EQ(purc_variant_is_object (math), true);
    ASSERT_NE(math, nullptr) << "Failed to load_from_so: ["
            << so_path << "]" << std::endl;
    ASSERT_EQ(purc_variant_is_object (math), true);

    purc_variant_t dynamic = purc_variant_object_get_by_ckey (math, "eval");
    ASSERT_NE(dynamic, nullptr);
    ASSERT_EQ(purc_variant_is_dynamic (dynamic), true);

    purc_dvariant_method func = NULL;
    func = purc_variant_dynamic_get_getter (dynamic);
    ASSERT_NE(func, nullptr);

    char math_path[PATH_MAX+1];
    env = "DVOBJS_TEST_PATH";
    test_getpath_from_env_or_rel(math_path, sizeof(math_path),
        env, "test_files");
    std::cerr << "env: " << env << "=" << math_path << std::endl;

    strcpy (path, math_path);
    strcat (path, "/math_bc");

    d = opendir(path);
    EXPECT_NE(d, nullptr) << "Failed to open dir @["
            << path << "]: [" << errno << "]" << strerror(errno)
            << std::endl;

    if (d) {
        if (chdir(path) != 0) {
            purc_variant_unref(math);
            return;
        }
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type & DT_REG) {
                std::stringstream ss;
                ss << "bc file:[" << dir->d_name << "][";
                long double l, r;
                _process_file(func, dir->d_name, &l, ss);
                ss << "]-[";
                _eval_bc(dir->d_name, &r, ss);
                ss << "]";
                ss << "==?==[" << fabsl(l-r) << "]";
                std::cout << ss.str() << std::endl;
                EXPECT_TRUE(long_double_eq(l, r))
                    << "Failed to parse bc file: ["
                    << dir->d_name << "]"
                    << std::endl;
            }
        }
        closedir(d);
    }

    if (math)
        purc_variant_unload_dvobj (math);

    purc_cleanup ();
}
