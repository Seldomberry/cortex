/* alias.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef CORTO_LANG_ALIAS_H
#define CORTO_LANG_ALIAS_H

#include <corto/corto.h>
#include <corto/_interface.h>
#include <corto/lang/_type.h>
#include <corto/lang/_api.h>
#include <corto/lang/_meta.h>

#ifdef __cplusplus
extern "C" {
#endif


CORTO_EXPORT corto_int16 _corto_alias_construct(
    corto_alias this);
#define corto_alias_construct(_this) _corto_alias_construct(corto_alias(_this))

CORTO_EXPORT corto_int16 _corto_alias_init(
    corto_alias this);
#define corto_alias_init(_this) _corto_alias_init(corto_alias(_this))

#ifdef __cplusplus
}
#endif
#endif

