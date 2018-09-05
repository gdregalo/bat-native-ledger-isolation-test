/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_REWARDS_MEDIA_PUBLISHER_INFO_BACKEND_
#define BRAVE_REWARDS_MEDIA_PUBLISHER_INFO_BACKEND_

/*
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/sequence_checker.h"
*/

#include<string>

namespace leveldb {
class DB;
}  // namespace leveldb

namespace brave_rewards {

class MediaPublisherInfoBackend {
 public:
  MediaPublisherInfoBackend(const std::string & path);
  ~MediaPublisherInfoBackend();

  bool Put(const std::string& key, const std::string& value);
  bool Get(const std::string& lookup, std::string* value);

 private:
  bool EnsureInitialized();
  const std::string path_;
  std::unique_ptr<leveldb::DB> db_;

  //SEQUENCE_CHECKER(sequence_checker_);
  //DISALLOW_COPY_AND_ASSIGN(MediaPublisherInfoBackend);
};

}  // namespace brave_rewards

#endif  // BRAVE_REWARDS_PUBLISHER_INFO_BACKEND_
