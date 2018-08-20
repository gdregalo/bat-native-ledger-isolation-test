 /* This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this file,
  * You can obtain one at http://mozilla.org/MPL/2.0/. */

 #ifndef BRAVE_REWARDS_SERVICE_IMPL_
 #define BRAVE_REWARDS_SERVICE_IMPL_

 #include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <locale>
#include <codecvt>

#include <shlobj.h>

//#include "base/files/file_path.h"
//#include "base/observer_list.h"
//#include "base/memory/weak_ptr.h"
#include "bat/ledger/ledger_client.h"
#include "brave_rewards_service.h"
#include "url_request_handler.h"
#include "URLFetcher.h"
//#include "content/public/browser/browser_thread.h"
//#include "net/url_request/url_fetcher_delegate.h"


namespace ledger {
class Ledger;
class LedgerCallbackHandler;
}


class Profile
{
public:
  std::string GetPath() {
    PWSTR path = NULL;
    std::string spath;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &path);
    if (SUCCEEDED(hr)) {
      std::wstring_convert <std::codecvt_utf8<wchar_t>, wchar_t> convert;
      spath = convert.to_bytes(path);
    }
    CoTaskMemFree(path);
    return spath;
  }
};

namespace brave_rewards {

class PublisherInfoBackend;

class BraveRewardsServiceImpl : public BraveRewardsService,
                            public ledger::LedgerClient ,public bat_ledger_urlfetcher::URLFetcherDelegate /*,public base::SupportsWeakPtr<BraveRewardsServiceImpl>*/
{
public:
  BraveRewardsServiceImpl(Profile* profile);
  ~BraveRewardsServiceImpl() override;

  BraveRewardsServiceImpl(const BraveRewardsServiceImpl &) = delete;
  BraveRewardsServiceImpl & operator= (const BraveRewardsServiceImpl &) = delete;

  // KeyedService///////////////////////////////////////////////////////////////////////////
  void Shutdown() override;
  void Init();
  //Ledger interface///////////////////////////////////////////////////////////////////////
  void CreateWallet() override;
  void OnLoad(const std::string& _tld,
            const std::string& _domain,
            const std::string& _path,
            uint32_t tab_id) override;
  void OnUnload(uint32_t tab_id) override;
  void OnShow(uint32_t tab_id) override;
  void OnHide(uint32_t tab_id) override;
  void OnForeground(uint32_t tab_id) override;
  void OnBackground(uint32_t tab_id) override;
  void OnMediaStart(uint32_t tab_id) override;
  void OnMediaStop(uint32_t tab_id) override;
  void OnXHRLoad(uint32_t tab_id, const std::string& url) override;
  /*void SaveVisit(const std::string& publisher,
                 uint64_t duration,
                 bool ignoreMinTime) override;*/
  void SavePublisherInfo(std::unique_ptr<ledger::PublisherInfo> publisher_info,
                 ledger::PublisherInfoCallback callback) override;
  void LoadPublisherInfo(const ledger::PublisherInfo::id_type& publisher_id,
                 ledger::PublisherInfoCallback callback) override;
  void LoadPublisherInfoList(
      uint32_t start,
      uint32_t limit,
      ledger::PublisherInfoFilter filter,
      ledger::GetPublisherInfoListCallback callback) override;
  void GetContentSiteList(uint32_t start,
                          uint32_t limit,
     const GetContentSiteListCallback& callback) override;

  void SetPublisherMinVisitTime(uint64_t duration_in_milliseconds) override;
  void SetPublisherMinVisits(unsigned int visits) override;
  void SetPublisherAllowNonVerified(bool allow) override;
  void SetContributionAmount(double amount) override;

  uint64_t GetPublisherMinVisitTime() const override; // In milliseconds
  unsigned int GetPublisherMinVisits() const override;
  bool GetPublisherAllowNonVerified() const override;
  double GetContributionAmount() const override;

  void TestingJoinAllRunningTasks();

private:
  typedef std::function<void(int, const std::string&)> FetchCallback;

  void OnLedgerStateSaved(ledger::LedgerCallbackHandler* handler,bool success);

  void OnLedgerStateLoaded(ledger::LedgerCallbackHandler* handler,const std::string& data);

  void OnPublisherStateSaved(ledger::LedgerCallbackHandler* handler, bool success);

  void OnPublisherStateLoaded(ledger::LedgerCallbackHandler* handler,const std::string& data);

  //using std::shared_ptr instead of original unique_ptr: couldn't bind noncopyable unique_ptr to std::function
  void OnPublisherInfoSaved(ledger::PublisherInfoCallback callback,
                            std::shared_ptr<ledger::PublisherInfo> info,
                            bool success);
  void OnPublisherInfoLoaded(ledger::PublisherInfoCallback callback,
                             std::shared_ptr<ledger::PublisherInfo> info);
  void OnPublisherInfoListLoaded(uint32_t start,
                                 uint32_t limit,
                                 ledger::GetPublisherInfoListCallback callback,
                                 const ledger::PublisherInfoList& list);


  void PostWriteCallback(std::function < void(bool success)> & callback, bool write_success);
  void TriggerOnWalletInitialized(int error_code);

  // ledger::LedgerClient/////////////////////////////////////////////////////////////////////////
  std::string GenerateGUID() const override;
  void OnWalletInitialized(ledger::Result result) override;
  void OnReconcileComplete(ledger::Result result,
                           const std::string& viewing_id) override;
  void LoadLedgerState(ledger::LedgerCallbackHandler* handler) override;
  void LoadPublisherState(ledger::LedgerCallbackHandler* handler) override;
  void SaveLedgerState(const std::string& ledger_state,
                       ledger::LedgerCallbackHandler* handler) override;
  void SavePublisherState(const std::string& publisher_state,
                          ledger::LedgerCallbackHandler* handler) override;

  std::unique_ptr<ledger::LedgerURLLoader> LoadURL(const std::string& url,
                   const std::vector<std::string>& headers,
                   const std::string& content,
                   const std::string& contentType,
                   const ledger::URL_METHOD& method,
                   ledger::LedgerCallbackHandler* handler) override;

  void RunIOTask(std::unique_ptr<ledger::LedgerTaskRunner> task) override;
  void RunTask(std::unique_ptr<ledger::LedgerTaskRunner> task) override;

  // URLFetcherDelegate impl
  void OnURLFetchComplete(const bat_ledger_urlfetcher::URLFetcher* source) /*override*/;

  void OnWalletProperties(ledger::Result result,
                          std::unique_ptr<ledger::WalletInfo> info) override;
  void GetWalletProperties() override;
  void GetPromotion(const std::string& lang, const std::string& paymentId) override;
  void GetPromotionCaptcha() override;
  //void SolvePromotionCaptcha(const std::string& solution) const override;
  //std::string GetWalletPassphrase() const override;
  //void RecoverWallet(const std::string passPhrase) const override;
  void OnPromotion(ledger::Promo result) override;
  void OnPromotionCaptcha(const std::string& image) override;
  void OnRecoverWallet(ledger::Result result, double balance) override;
  void OnPromotionFinish(ledger::Result result, unsigned int statusCode, uint64_t expirationDate) override;

  Profile* profile_;  // NOT OWNED
  std::unique_ptr<ledger::Ledger> ledger_;
  //const scoped_refptr<base::SequencedTaskRunner> file_task_runner_;
  std::mutex vec_mx_;
  std::vector <std::thread> tasks_in_progress_;

  const std::string ledger_state_path_;
  const std::string publisher_state_path_;
  const std::string publisher_info_db_path_;
  std::unique_ptr<PublisherInfoBackend> publisher_info_backend_;

  std::map<const bat_ledger_urlfetcher::URLFetcher*, FetchCallback> fetchers_;

};

}  // namespace brave_rewards

#endif  // BRAVE_REWARDS_SERVICE_IMPL_
