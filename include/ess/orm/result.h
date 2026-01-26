#pragma once
#include <sqlite3.h>

struct InsertResult {
  sqlite3_int64 last_insert_id;
  int affected_rows;
};

struct ModifyResult {
  int affected_rows;
};
