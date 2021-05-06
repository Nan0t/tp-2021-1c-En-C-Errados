#ifndef UTILS_H
#define UTILS_H

/**
 * @File: utils.h
 * @Desc: Header que contiene la mayoria de los includes relevantes de la libreria
 */

#include "utils/diagnostics/log.h"
#include "utils/diagnostics/assert.h"
#include "utils/config/config.h"
#include "utils/memory/allocator.h"
#include "utils/network/socket.h"
#include "utils/protocol/opcodes.h"
#include "utils/protocol/package.h"
#include "utils/protocol/messages/messages.h"

const char* u_get_version(void);

#endif