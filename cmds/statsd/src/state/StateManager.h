/*
 * Copyright (C) 2019, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <gtest/gtest_prod.h>
#include <inttypes.h>
#include <utils/RefBase.h>

#include "HashableDimensionKey.h"
#include "state/StateListener.h"
#include "state/StateTracker.h"

namespace android {
namespace os {
namespace statsd {

class StateManager : public virtual RefBase {
public:
    StateManager(){};

    ~StateManager(){};

    // Returns a pointer to the single, shared StateManager object.
    static StateManager& getInstance();

    // Notifies the correct StateTracker of an event.
    void onLogEvent(const LogEvent& event);

    // Returns true if atomId is being tracked and is associated with a state
    // atom. StateManager notifies the correct StateTracker to register listener.
    // If the correct StateTracker does not exist, a new StateTracker is created.
    bool registerListener(int32_t atomId, wp<StateListener> listener);

    // Notifies the correct StateTracker to unregister a listener
    // and removes the tracker if it no longer has any listeners.
    void unregisterListener(int32_t atomId, wp<StateListener> listener);

    // Returns true if the StateTracker exists and queries for the
    // original state value mapped to the given query key. The state value is
    // stored and output in a FieldValue class.
    // Returns false if the StateTracker doesn't exist.
    bool getStateValue(int32_t atomId, const HashableDimensionKey& queryKey,
                       FieldValue* output) const;

    inline int getStateTrackersCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mStateTrackers.size();
    }

    inline int getListenersCount(int32_t atomId) const {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mStateTrackers.find(atomId);
        if (it != mStateTrackers.end()) {
            return it->second->getListenersCount();
        }
        return -1;
    }

private:
  mutable std::mutex mMutex;

  // Maps state atom ids to StateTrackers
  std::unordered_map<int32_t, sp<StateTracker>> mStateTrackers;
};

}  // namespace statsd
}  // namespace os
}  // namespace android
