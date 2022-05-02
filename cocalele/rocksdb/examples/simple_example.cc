// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/convenience.h"
#include <rocksdb/utilities/object_registry.h>
#include "rocksdb/pfaof.h"

using namespace ROCKSDB_NAMESPACE;

using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::PinnableSlice;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;
using ROCKSDB_NAMESPACE::Env;
using ROCKSDB_NAMESPACE::ConfigOptions;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_simple_example";
#else
std::string kDBPath = "/rocksdb_simple_example";
#endif
static std::shared_ptr<ROCKSDB_NAMESPACE::Env> env_guard;
static ROCKSDB_NAMESPACE::Env* FLAGS_env = ROCKSDB_NAMESPACE::Env::Default();
void __PfAof_init();

int main() {
  DB* db;
  Options options;
  ConfigOptions config_options;
  printf("Hello, I'm example\n");
  __PfAof_init();

  Status s =
      Env::CreateFromUri(config_options, "", "pfaof", &FLAGS_env, &env_guard);
  if (!s.ok()) {
    fprintf(stderr, "Failed creating env: %s\n", s.ToString().c_str());
    exit(1);
  }


  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;
  options.env = FLAGS_env;


  // open DB
  s = DB::Open(options, kDBPath, &db);
  if(!s.ok()){
    fprintf(stderr, "Failed call DB::Open, %s", s.ToString().c_str());
    exit(s.code());
  }

  // Put key-value
  s = db->Put(WriteOptions(), "key1", "value");
  if (!s.ok()) {
    fprintf(stderr, "Failed call DB::Open, %s", s.ToString().c_str());
    exit(s.code());
  }
  std::string value;
  // get value
  s = db->Get(ReadOptions(), "key1", &value);
  if (!s.ok()) {
    fprintf(stderr, "Failed call DB::Open, %s", s.ToString().c_str());
    exit(s.code());
  }
  assert(value == "value");

  // atomically apply a set of updates
  {
    WriteBatch batch;
    batch.Delete("key1");
    batch.Put("key2", value);
    s = db->Write(WriteOptions(), &batch);
  }

  s = db->Get(ReadOptions(), "key1", &value);
  assert(s.IsNotFound());

  db->Get(ReadOptions(), "key2", &value);
  assert(value == "value");

  {
    PinnableSlice pinnable_val;
    db->Get(ReadOptions(), db->DefaultColumnFamily(), "key2", &pinnable_val);
    assert(pinnable_val == "value");
  }

  {
    std::string string_val;
    // If it cannot pin the value, it copies the value to its internal buffer.
    // The intenral buffer could be set during construction.
    PinnableSlice pinnable_val(&string_val);
    db->Get(ReadOptions(), db->DefaultColumnFamily(), "key2", &pinnable_val);
    assert(pinnable_val == "value");
    // If the value is not pinned, the internal buffer must have the value.
    assert(pinnable_val.IsPinned() || string_val == "value");
  }

  PinnableSlice pinnable_val;
  s = db->Get(ReadOptions(), db->DefaultColumnFamily(), "key1", &pinnable_val);
  assert(s.IsNotFound());
  // Reset PinnableSlice after each use and before each reuse
  pinnable_val.Reset();
  db->Get(ReadOptions(), db->DefaultColumnFamily(), "key2", &pinnable_val);
  assert(pinnable_val == "value");
  pinnable_val.Reset();
  // The Slice pointed by pinnable_val is not valid after this point

  delete db;

  return 0;
}
