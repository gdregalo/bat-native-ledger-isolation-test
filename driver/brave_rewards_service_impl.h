 /* This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this file,
  * You can obtain one at http://mozilla.org/MPL/2.0/. */

 #ifndef BRAVE_REWARDS_SERVICE_IMPL_
 #define BRAVE_REWARDS_SERVICE_IMPL_

 #include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <locale>
#include <codecvt>
#include <map>

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

#include <boost/asio.hpp>


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

class PublisherInfoDatabase;

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
  void FetchWalletProperties() override;
  void FetchGrant(const std::string& lang, const std::string& paymentId) override;
  void GetGrantCaptcha() override;
  void SolveGrantCaptcha(const std::string& solution) const override;
  std::string GetWalletPassphrase() const override;
  unsigned int GetNumExcludedSites() const override;
  void RecoverWallet(const std::string passPhrase) const override;
  void GetContentSiteList(uint32_t start,
                          uint32_t limit,
     const GetContentSiteListCallback& callback) override;
  void OnLoad(uint32_t tab_id, const std::string & url) override;
  void OnUnload(uint32_t tab_id) override;
  void OnShow(uint32_t tab_id) override;
  void OnHide(uint32_t tab_id) override;
  void OnForeground(uint32_t tab_id) override;
  void OnBackground(uint32_t tab_id) override;
  void OnMediaStart(uint32_t tab_id) override;
  void OnMediaStop(uint32_t tab_id) override;
  void OnXHRLoad(uint32_t tab_id,
                 const std::string & url,
                 const std::string & first_party_url,
                 const std::string & referrer) override;
 void OnPostData(uint32_t tab_id,
                  const std::string & url,
                  const std::string & first_party_url,
                  const std::string & referrer,
                  const std::string& post_data) override;
  std::string URIEncode(const std::string& value) override;
  uint64_t GetReconcileStamp() const override;
  std::map<std::string, std::string> GetAddresses() const override;
  void LoadMediaPublisherInfo(
      const std::string& media_key,
      ledger::PublisherInfoCallback callback) override;
  void SaveMediaPublisherInfo(const std::string& media_key, const std::string& publisher_id) override;
  void ExcludePublisher(const std::string publisherKey) const override;
  void RestorePublishers() override;
  std::map<std::string, brave_rewards::BalanceReport> GetAllBalanceReports() override;
  void GetCurrentBalanceReport() override;

  bool IsWalletCreated() override;
  void GetPublisherActivityFromUrl(uint64_t windowId, const std::string& url, const std::string& favicon_url) override;

	double GetContributionAmount() override;
  void GetPublisherBanner(const std::string& publisher_id) override;
  void OnPublisherBanner(std::unique_ptr<ledger::PublisherBanner> banner);
  void RemoveRecurring(const std::string& publisher_key) override;
  void UpdateRecurringDonationsList() override;
  void UpdateTipsList() override;

  void SetContributionAutoInclude(
    std::string publisher_key, bool excluded, uint64_t windowId) override;


  //testing
  void TestingJoinAllRunningTasks() override;

  //timers: allow only a number of timers to run
  //0 - unlimited
  void AllowTimersRun(uint32_t timers) override;

private:
  friend void RunIOTaskCallback(BraveRewardsServiceImpl * service,std::function<void(void)>);

  typedef std::function<void(int, const std::string&, const std::map<std::string,std::string> & headers )> FetchCallback;

  void OnLedgerStateSaved(ledger::LedgerCallbackHandler* handler,
                          bool success);
  void OnLedgerStateLoaded(ledger::LedgerCallbackHandler* handler,
                              const std::string& data);
  void LoadNicewareList(ledger::GetNicewareListCallback callback) override;
  void OnPublisherStateSaved(ledger::LedgerCallbackHandler* handler,
                             bool success);
  void OnPublisherStateLoaded(ledger::LedgerCallbackHandler* handler,
                              const std::string& data);
  void TriggerOnWalletInitialized(int error_code);
  void TriggerOnWalletProperties(int error_code,
                                 std::unique_ptr<ledger::WalletInfo> result);
  void TriggerOnGrant(ledger::Result result, const ledger::Grant& grant);
  void TriggerOnGrantCaptcha(const std::string& image, const std::string& hint);
  void TriggerOnRecoverWallet(ledger::Result result,
                              double balance,
                              const std::vector<ledger::Grant>& grants);
  void TriggerOnGrantFinish(ledger::Result result, const ledger::Grant& grant);


  //using std::shared_ptr instead of original unique_ptr: couldn't bind noncopyable unique_ptr to std::function
  void OnPublisherInfoSaved(ledger::PublisherInfoCallback callback,
                            std::shared_ptr<ledger::PublisherInfo> info,
                            bool success);
  void OnPublisherInfoLoaded(ledger::PublisherInfoCallback callback,
                             const ledger::PublisherInfoList list);
  void OnMediaPublisherInfoSaved(bool success);
  void OnMediaPublisherInfoLoaded(ledger::PublisherInfoCallback callback,
                             std::shared_ptr<ledger::PublisherInfo> info);
  void OnPublisherInfoListLoaded(uint32_t start,
                                 uint32_t limit,
                                 ledger::PublisherInfoListCallback callback,
                                 const ledger::PublisherInfoList& list);
  void OnPublishersListSaved(ledger::LedgerCallbackHandler* handler,
                             bool success);
  void OnTimer(uint32_t timer_id);
  void TriggerOnContentSiteUpdated();
  void OnPublisherListLoaded(ledger::LedgerCallbackHandler* handler,
                             const std::string& data);

  void OnDonate(const std::string& publisher_key, int amount, bool recurring) override;
  void OnContributionInfoSaved(const ledger::PUBLISHER_CATEGORY category, bool success);
  void OnRecurringDonationSaved(bool success);
  void SaveRecurringDonation(const std::string& publisher_key, const int amount);
  void OnRecurringDonationsData(const ledger::PublisherInfoListCallback callback,
                                const ledger::PublisherInfoList list);
  void OnRecurringDonationUpdated(const ledger::PublisherInfoList& list);
  void OnTipsUpdatedData(const ledger::PublisherInfoList list);
  void TipsUpdated();
  void OnRemovedRecurring(ledger::RecurringRemoveCallback callback, bool success);
  void OnRemoveRecurring(const std::string& publisher_key, ledger::RecurringRemoveCallback callback) override;
  void TriggerOnGetCurrentBalanceReport(const ledger::BalanceReportInfo& report);
  void TriggerOnGetPublisherActivityFromUrl(
      ledger::Result result,
      std::unique_ptr<ledger::PublisherInfo> info,
      uint64_t windowId);
  void PostWriteCallback(std::function < void(bool success)> & callback, bool write_success);
  // ledger::LedgerClient/////////////////////////////////////////////////////////////////////////
  std::string GenerateGUID() const override;
  void OnWalletInitialized(ledger::Result result) override;
  void OnWalletProperties(ledger::Result result,
                          std::unique_ptr<ledger::WalletInfo> info) override;
  void OnGrant(ledger::Result result, const ledger::Grant& grant) override;
  void OnGrantCaptcha(const std::string& image, const std::string& hint) override;
  void OnRecoverWallet(ledger::Result result,
                      double balance,
                      const std::vector<ledger::Grant>& grants) override;
  void OnReconcileComplete(ledger::Result result,
                           const std::string& viewing_id,
                           ledger::PUBLISHER_CATEGORY category,
                           const std::string& probi) override;
  void OnGrantFinish(ledger::Result result,
                     const ledger::Grant& grant) override;
  void LoadLedgerState(ledger::LedgerCallbackHandler* handler) override;
  void LoadPublisherState(ledger::LedgerCallbackHandler* handler) override;
  void SaveLedgerState(const std::string& ledger_state,
                       ledger::LedgerCallbackHandler* handler) override;
  void SavePublisherState(const std::string& publisher_state,
                          ledger::LedgerCallbackHandler* handler) override;

  void SavePublisherInfo(std::unique_ptr<ledger::PublisherInfo> publisher_info,
                         ledger::PublisherInfoCallback callback) override;
  void LoadPublisherInfo(ledger::PublisherInfoFilter filter,
                         ledger::PublisherInfoCallback callback) override;
  void LoadPublisherInfoList(
      uint32_t start,
      uint32_t limit,
      ledger::PublisherInfoFilter filter,
      ledger::PublisherInfoListCallback callback) override;
  void LoadCurrentPublisherInfoList(
      uint32_t start,
      uint32_t limit,
      ledger::PublisherInfoFilter filter,
      ledger::PublisherInfoListCallback callback) override;
  void SavePublishersList(const std::string& publishers_list,
                          ledger::LedgerCallbackHandler* handler) override;
  void SetTimer(uint64_t time_offset, uint32_t& timer_id) override;
  void LoadPublisherList(ledger::LedgerCallbackHandler* handler) override;

  std::unique_ptr<ledger::LedgerURLLoader> LoadURL(const std::string& url,
                   const std::vector<std::string>& headers,
                   const std::string& content,
                   const std::string& contentType,
                   const ledger::URL_METHOD& method,
                   ledger::LedgerCallbackHandler* handler) override;

  void RunIOTask(std::unique_ptr<ledger::LedgerTaskRunner> task) override;
  void RunTask(std::unique_ptr<ledger::LedgerTaskRunner> task);
  void SetRewardsMainEnabled(bool enabled) const override;
  void SetPublisherMinVisitTime(uint64_t duration_in_seconds) const override;
  void SetPublisherMinVisits(unsigned int visits) const override;
  void SetPublisherAllowNonVerified(bool allow) const override;
  void SetPublisherAllowVideos(bool allow) const override;
  void SetContributionAmount(double amount) const override;
  void SetUserChangedContribution() const override;
  void SetAutoContribute(bool enabled) const override;
  void OnExcludedSitesChanged() override;
  void OnPublisherActivity(ledger::Result result,
                          std::unique_ptr<ledger::PublisherInfo> info,
                          uint64_t windowId) override;
  void FetchFavIcon(const std::string& url,
                    const std::string& favicon_key,
                    ledger::FetchIconCallback callback) override;

  void OnSetOnDemandFaviconComplete(const std::string& favicon_url,
                                    ledger::FetchIconCallback callback,
                                    bool success);
  void SaveContributionInfo(const std::string& probi,
                            const int month,
                            const int year,
                            const uint32_t date,
                            const std::string& publisher_key,
                            const ledger::PUBLISHER_CATEGORY category) override;
  void GetRecurringDonations(ledger::PublisherInfoListCallback callback) override;
  void Log(ledger::LogLevel level, const std::string& text) override;

  void OnIOTaskComplete(std::function<void(void)> callback);

  // URLFetcherDelegate impl
  void OnURLFetchComplete(const bat_ledger_urlfetcher::URLFetcher* source) /*override*/;

  uint64_t GetCurrentTimeStamp();



  Profile* profile_;  // NOT OWNED
  std::unique_ptr<ledger::Ledger> ledger_;
  //const scoped_refptr<base::SequencedTaskRunner> file_task_runner_;
  std::mutex vec_mx_;
  std::vector <std::thread> tasks_in_progress_;

  const std::string ledger_state_path_;
  const std::string publisher_state_path_;
  const std::string publisher_info_db_path_;
  const std::string publisher_list_path_;
  std::unique_ptr<PublisherInfoDatabase> publisher_info_backend_;

  std::map<const bat_ledger_urlfetcher::URLFetcher*, FetchCallback> fetchers_;


  //timers implementation
  std::vector <boost::asio::deadline_timer> timers_;
  boost::asio::io_service io_;
  std::atomic<uint32_t> timer_id_ = 0u;
  std::mutex timer_mx_;
  std::atomic <uint32_t> max_number_timers_ = 0u;
};

}  // namespace brave_rewards

#endif  // BRAVE_REWARDS_SERVICE_IMPL_
