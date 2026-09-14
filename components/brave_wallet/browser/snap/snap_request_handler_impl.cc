/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/snap/snap_request_handler_impl.h"

#include <utility>

#include "base/values.h"

namespace brave_wallet {

SnapRequestHandlerImpl::SnapRequestHandlerImpl() = default;
SnapRequestHandlerImpl::~SnapRequestHandlerImpl() = default;

void SnapRequestHandlerImpl::Bind(
    mojo::PendingReceiver<mojom::SnapRequestHandler> receiver) {
  receiver_.reset();
  receiver_.Bind(std::move(receiver));
}

void SnapRequestHandlerImpl::HandleSnapRequest(
    const std::string& snap_id,
    const std::string& method,
    base::Value params,
    HandleSnapRequestCallback callback) {
  if (request_observer_for_testing_) {
    request_observer_for_testing_.Run(snap_id, method, params);
  }

  // Mocked answer for every snap.request() until real handlers land.
  base::DictValue mock;
  mock.Set("mocked", true);
  mock.Set("method", method);
  mock.Set("snapId", snap_id);
  std::move(callback).Run(base::Value(std::move(mock)), std::nullopt);
}

void SnapRequestHandlerImpl::SetRequestObserverForTesting(
    RequestObserverForTesting observer) {
  request_observer_for_testing_ = std::move(observer);
}

}  // namespace brave_wallet
