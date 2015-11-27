// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <queue>
#include "consts.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit



#include <atlbase.h>
#include <atlstr.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

#include <locale>
#include <codecvt>


// TODO: reference additional headers your program requires here
