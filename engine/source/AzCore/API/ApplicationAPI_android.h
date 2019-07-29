/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#ifndef AZCORE_APPLICATIONAPI_ANDROID_H
#define AZCORE_APPLICATIONAPI_ANDROID_H

#pragma once

#include <AzCore/EBus/EBus.h>

struct android_app;

namespace AZ
{
    class AndroidLifecycleEvents
        : public AZ::EBusTraits
    {
    public:
        // Bus Configuration
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~AndroidLifecycleEvents() {}

        using Bus = AZ::EBus<AndroidLifecycleEvents>;

        // Android can also generate onStop/onRestart events which
        // (at first glance) would appear to match better with our
        // suspend/resume events. However there is no guarantee of
        // either these methods being called, and the behavior of
        // onPause/onResume more closely matches that of suspend/
        // resume on our other platforms. 

        virtual void OnPause() {}       // Constrain -> Suspend
        virtual void OnResume() {}      // Resume -> Unconstrain

        virtual void OnDestroy() {}     // Terminate
        virtual void OnLowMemory() {}   // Low memory

        virtual void OnWindowInit() {}     // Application window was created
        virtual void OnWindowDestroy() {}   // Application window is going to be destoryed
    };


    class AndroidAppRequests
        : public AZ::EBusTraits
    {
    public:
        using Bus = AZ::EBus<AndroidAppRequests>;

        // Bus Configuration
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~AndroidAppRequests() {}

        //! Sets the Android application state required to pumping the event loop
        virtual void SetAppState(android_app* appState) = 0;
    };
} // namespace AZ

#endif // AZCORE_APPLICATIONAPI_ANDROID_H