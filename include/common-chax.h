#pragma once

#define CHAX 1

/* Add a magic for FEB PATCH to auto find the pointer */
#define FEB_IDENTIFIER(PTR) const char FEB_##PTR##_Identifier[] = #PTR

#define LYN_REPLACE_CHECK(name) \
    static void const * const __attribute__((unused)) lyn_exists_check_ ## name = &name
