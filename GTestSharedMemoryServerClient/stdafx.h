// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h>
#include <queue>
#include "consts.h"

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


// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы
