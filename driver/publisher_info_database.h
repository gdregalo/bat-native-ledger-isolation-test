/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_REWARDS_PUBLISHER_INFO_DATABASE_H_
#define BRAVE_REWARDS_PUBLISHER_INFO_DATABASE_H_

#include <stddef.h>
/*
#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/memory_pressure_listener.h"
#include "base/sequence_checker.h"
#include "build/build_config.h"
#include "sql/connection.h"
#include "sql/init_status.h"
#include "sql/meta_table.h"
*/

#include "bat/ledger/publisher_info.h"
#include "sqlite_modern_cpp.h"

namespace brave_rewards {

class PublisherInfoDatabase {
 public:
  PublisherInfoDatabase(const std::string & db_path);
  ~PublisherInfoDatabase();

  PublisherInfoDatabase(const PublisherInfoDatabase &) = delete;
  PublisherInfoDatabase & operator = (const PublisherInfoDatabase &) = delete;

  bool InsertOrUpdatePublisherInfo(const ledger::PublisherInfo& info);
  bool Find(int start,
            int limit,
            const ledger::PublisherInfoFilter& filter,
            ledger::PublisherInfoList* list);

  // Returns the current version of the publisher info database
  static int GetCurrentVersion();

 private:
  bool Init();

  bool CreateContributionInfoTable();
  bool CreatePublisherInfoTable();
  bool CreateContributionInfoIndex();

  bool EnsureCurrentVersion();

  std::unique_ptr<sqlite::database> db_;
  const std::string db_path_;
  bool initialized_;

  //SEQUENCE_CHECKER(sequence_checker_);

};

}  // namespace brave_rewards

#endif  // BRAVE_REWARDS_PUBLISHER_INFO_DATABASE_H_