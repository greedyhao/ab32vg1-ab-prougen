/*
 * Copyright (c) 2020-2021, Bluetrum Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-01-07     greedyhao         first version
 */

#include <board.h>
#include "drv_soft_i2c.h"
// #include "drv_config.h"

#ifdef RT_USING_I2C

static const struct ab32_soft_i2c_config soft_i2c_config[] =
{
#ifdef BSP_USING_I2C1
    I2C1_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C2
    I2C2_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C3
    I2C3_BUS_CONFIG,
#endif
#ifdef BSP_USING_I2C4
    I2C4_BUS_CONFIG,
#endif
};

static struct ab32_i2c i2c_obj[sizeof(soft_i2c_config) / sizeof(soft_i2c_config[0])];

#endif
