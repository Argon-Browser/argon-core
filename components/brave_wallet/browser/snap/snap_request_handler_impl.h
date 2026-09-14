/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_SNAP_SNAP_REQUEST_HANDLER_IMPL_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_SNAP_SNAP_REQUEST_HANDLER_IMPL_H_

#include <optional>
#include <string>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"

namespace brave_wallet {

// Implements mojom::SnapRequestHandler — handles snap.request() calls
// dispatched by the page-side TS executor via the Mojo pipe.
// This extraction returns a deterministic mocked answer for every method.
class SnapRequestHandlerImpl : public mojom::SnapRequestHandler {
 public:
  using RequestObserverForTesting =
      base::RepeatingCallback<void(const std::string& snap_id,
                                   const std::string& method,
                                   const base::Value& params)>;

  SnapRequestHandlerImpl();
  ~SnapRequestHandlerImpl() override;

  SnapRequestHandlerImpl(const SnapRequestHandlerImpl&) = delete;
  SnapRequestHandlerImpl& operator=(const SnapRequestHandlerImpl&) = delete;

  void Bind(mojo::PendingReceiver<mojom::SnapRequestHandler> receiver);

  // mojom::SnapRequestHandler:
  void HandleSnapRequest(const std::string& snap_id,
                         const std::string& method,
                         base::Value params,
                         HandleSnapRequestCallback callback) override;

  void SetRequestObserverForTesting(RequestObserverForTesting observer);

 private:
  mojo::Receiver<mojom::SnapRequestHandler> receiver_{this};

  RequestObserverForTesting request_observer_for_testing_;

  base::WeakPtrFactory<SnapRequestHandlerImpl> weak_ptr_factory_{this};
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_SNAP_SNAP_REQUEST_HANDLER_IMPL_H_
