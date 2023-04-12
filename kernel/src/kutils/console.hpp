#pragma once
#include <libc/stdarg.hpp>

void kprint_err(const char *msg);
void kprint_succ(const char *msg);
void kprint_info(const char *msg);

void kprint_pmem_info(void);
void kprint_cpu_info(void);
void kpanic(const char *msg);

void kprintf(const char *fmt, ...);