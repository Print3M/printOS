#pragma once
#include <libc/stdarg.hpp>
#include <libc/stdint.hpp>

void kprint_err(const char *msg);
void kprint_succ(const char *msg);
void kprint_info(const char *msg);

void kprint_pmem_info();
void kprint_cpu_info();
void kprint_heap_info();

void kpanic(const char *msg);
void kpanic(const char *msg, char const *file, size line, char const *func);

void kprintf(const char *fmt, ...);
