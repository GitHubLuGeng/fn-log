﻿/*
 *
 * MIT License
 *
 * Copyright (C) 2019 YaweiZhang <yawei.zhang@foxmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ===============================================================================
 *
 * (end of COPYRIGHT)
 */


 /*
  * AUTHORS:  YaweiZhang <yawei.zhang@foxmail.com>
  * VERSION:  1.0.0
  * PURPOSE:  fn-log is a cpp-based logging utility.
  * CREATION: 2019.4.20
  * RELEASED: 2019.6.27
  * QQGROUP:  524700770
  */


#pragma once
#ifndef _FN_LOG_OUT_SCREEN_DEVICE_H_
#define _FN_LOG_OUT_SCREEN_DEVICE_H_

#include "fn_data.h"
#include "fn_fmt.h"

namespace FNLog
{

    inline void EnterProcOutScreenDevice(Logger& logger, int channel_id, int device_id, LogData& log)
    {
        Logger::ScreenLockGuard l(logger.screen_lock_);
        Device& device = logger.shm_->channels_[channel_id].devices_[device_id];
        AtomicAddL(device, DEVICE_LOG_TOTAL_WRITE_LINE);
        AtomicAddLV(device, DEVICE_LOG_TOTAL_WRITE_BYTE, log.content_len_);
        int priority = log.priority_;
        if (log.priority_ < PRIORITY_INFO)
        {
            printf("%s", log.content_);
            return;
        }
        if (priority >= PRIORITY_MAX)
        {
            priority = PRIORITY_ALARM;
        }
#ifndef WIN32
        printf("%s%s\e[0m", PRIORITY_RENDER[priority].scolor_, log.content_);
#else

        HANDLE sc_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if (sc_handle == INVALID_HANDLE_VALUE)
        {
            printf("%s", log.content_);
            return;
        }
        CONSOLE_SCREEN_BUFFER_INFO old_info;
        if (!GetConsoleScreenBufferInfo(sc_handle, &old_info))
        {
            printf("%s", log.content_);
            return;
        }
        else
        {
            SetConsoleTextAttribute(sc_handle, PRIORITY_RENDER[priority].color_);
            printf("%s", log.content_);
            SetConsoleTextAttribute(sc_handle, old_info.wAttributes);
        }
#endif
    }



}


#endif
