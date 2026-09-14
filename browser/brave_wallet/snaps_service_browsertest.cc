/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/snaps_service.h"

#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "base/json/json_reader.h"
#include "base/memory/raw_ptr.h"
#include "base/test/run_until.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/test_future.h"
#include "base/values.h"
#include "brave/browser/brave_wallet/brave_wallet_service_factory.h"
#include "brave/components/brave_wallet/browser/snap/snap_request_handler_impl.h"
#include "brave/components/brave_wallet/browser/test_utils.h"
#include "brave/components/brave_wallet/common/features.h"
#include "brave/components/brave_wallet/common/web_ui_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "url/gurl.h"

namespace brave_wallet {

namespace {
constexpr char kTestSnapId[] = "npm:test-snap";
}  // namespace

class SnapsServiceBrowserTest : public InProcessBrowserTest {
 public:
  SnapsServiceBrowserTest() {
    feature_list_.InitAndEnableFeature(features::kBraveWalletSnapsFeature);
  }

  void SetUpOnMainThread() override {
    auto* wallet_service = BraveWalletServiceFactory::GetServiceForContext(
        browser()->GetProfile());
    ASSERT_TRUE(wallet_service);
    service_ = wallet_service->snaps_service();
    ASSERT_TRUE(service_);
    service_->SetSnapBundleForTesting(kTestSnapId, ReadTestSnapBundle());
  }

  void OpenWalletPage() {
    ASSERT_TRUE(
        ui_test_utils::NavigateToURL(browser(), GURL(kBraveUIWalletURL)));
    ASSERT_TRUE(base::test::RunUntil(
        [&] { return service()->IsBridgeBoundForTesting(); }));
  }

  std::pair<bool, std::optional<std::string>> LoadSnap(
      const std::string& snap_id) {
    base::test::TestFuture<bool, const std::optional<std::string>&> future;
    service_->LoadSnap(snap_id, future.GetCallback());
    auto [success, error] = future.Take();
    return {success, error};
  }

  std::pair<std::optional<std::string>, std::optional<std::string>> InvokeSnap(
      const std::string& method,
      const std::string& params_json) {
    base::test::TestFuture<const std::optional<std::string>&,
                           const std::optional<std::string>&>
        future;
    service_->InvokeSnap(kTestSnapId, method, params_json,
                         future.GetCallback());
    auto [result_json, error] = future.Take();
    return {result_json, error};
  }

  SnapsService* service() { return service_; }

 private:
  base::test::ScopedFeatureList feature_list_;
  raw_ptr<SnapsService> service_ = nullptr;
};

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest,
                       LoadSnapFailsWhenWalletPageIsNotRunning) {
  auto [success, error] = LoadSnap(kTestSnapId);
  EXPECT_FALSE(success);
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ("Wallet page is not running", *error);
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest,
                       LoadSnapSucceedsWhenBundlePresent) {
  OpenWalletPage();

  auto [success, error] = LoadSnap(kTestSnapId);
  EXPECT_TRUE(success);
  EXPECT_FALSE(error.has_value());
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest,
                       InvokeSnapFailsWhenWalletPageIsNotRunning) {
  auto [result_json, error] = InvokeSnap("echo", R"({"hello":"world"})");
  EXPECT_FALSE(result_json.has_value());
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ("Wallet page is not running", *error);
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest, InvokeSnapEchoReturnsResult) {
  OpenWalletPage();

  auto [result_json, error] = InvokeSnap("echo", R"({"hello":"world"})");
  ASSERT_FALSE(error.has_value()) << *error;
  ASSERT_TRUE(result_json.has_value());

  auto parsed = base::JSONReader::Read(*result_json, base::JSON_PARSE_RFC);
  ASSERT_TRUE(parsed);
  auto expected = base::JSONReader::Read(R"({"echoed":{"hello":"world"}})",
                                         base::JSON_PARSE_RFC);
  ASSERT_TRUE(expected);
  EXPECT_EQ(*parsed, *expected);
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest,
                       SnapRequestReturnsMockedAnswer) {
  OpenWalletPage();

  std::vector<std::tuple<std::string, std::string, base::Value>> observed;
  service()->snap_request_handler_for_testing()->SetRequestObserverForTesting(
      base::BindRepeating(
          [](std::vector<std::tuple<std::string, std::string, base::Value>>*
                 observed,
             const std::string& snap_id, const std::string& method,
             const base::Value& params) {
            observed->emplace_back(snap_id, method, params.Clone());
          },
          &observed));

  auto [result_json, error] = InvokeSnap("roundTrip", R"({"n":7})");
  ASSERT_FALSE(error.has_value()) << *error;
  ASSERT_TRUE(result_json.has_value());

  ASSERT_EQ(1u, observed.size());
  EXPECT_EQ(kTestSnapId, std::get<0>(observed[0]));
  EXPECT_EQ("snap_manageState", std::get<1>(observed[0]));

  auto parsed = base::JSONReader::Read(*result_json, base::JSON_PARSE_RFC);
  ASSERT_TRUE(parsed);
  auto expected = base::JSONReader::Read(
      R"({"fromBrowser":{"mocked":true,"method":"snap_manageState","snapId":"npm:test-snap"}})",
      base::JSON_PARSE_RFC);
  ASSERT_TRUE(expected);
  EXPECT_EQ(*parsed, *expected);
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest, UnknownSnapReturnsError) {
  OpenWalletPage();

  auto [success, error] = LoadSnap("npm:missing");
  EXPECT_FALSE(success);
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ("Bundle not found", *error);
}

IN_PROC_BROWSER_TEST_F(SnapsServiceBrowserTest, BridgeDisconnectOnPageClose) {
  OpenWalletPage();

  ASSERT_TRUE(ui_test_utils::NavigateToURL(browser(), GURL("chrome://newtab")));
  ASSERT_TRUE(base::test::RunUntil(
      [&] { return !service()->IsBridgeBoundForTesting(); }));

  auto [success, error] = LoadSnap(kTestSnapId);
  EXPECT_FALSE(success);
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ("Wallet page is not running", *error);
}

}  // namespace brave_wallet
