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

#include <AzCore/Input/System/InputSystemComponent.h>

#include <AzCore/Input/Buses/Notifications/InputSystemNotificationBus.h>

#include <AzCore/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzCore/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzCore/Input/Devices/Motion/InputDeviceMotion.h>
#include <AzCore/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzCore/Input/Devices/Touch/InputDeviceTouch.h>
#include <AzCore/Input/Devices/VirtualKeyboard/InputDeviceVirtualKeyboard.h>

#include <AzCore/Serialization/SerializeContext.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    AZStd::vector<AZStd::string> GetAllMotionChannelNames()
    {
        AZStd::vector<AZStd::string> allMotionChannelNames;
        for (AZ::u32 i = 0; i < InputDeviceMotion::Acceleration::All.size(); ++i)
        {
            const InputChannelId& channelId = InputDeviceMotion::Acceleration::All[i];
            allMotionChannelNames.push_back(channelId.GetName());
        }
        for (AZ::u32 i = 0; i < InputDeviceMotion::RotationRate::All.size(); ++i)
        {
            const InputChannelId& channelId = InputDeviceMotion::RotationRate::All[i];
            allMotionChannelNames.push_back(channelId.GetName());
        }
        for (AZ::u32 i = 0; i < InputDeviceMotion::MagneticField::All.size(); ++i)
        {
            const InputChannelId& channelId = InputDeviceMotion::MagneticField::All[i];
            allMotionChannelNames.push_back(channelId.GetName());
        }
        for (AZ::u32 i = 0; i < InputDeviceMotion::Orientation::All.size(); ++i)
        {
            const InputChannelId& channelId = InputDeviceMotion::Orientation::All[i];
            allMotionChannelNames.push_back(channelId.GetName());
        }
        return allMotionChannelNames;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InputSystemComponent, AZ::Component>()
                ->Field("GamepadsEnabled", &InputSystemComponent::m_gamepadsEnabled)
                ->Field("KeyboardEnabled", &InputSystemComponent::m_keyboardEnabled)
                ->Field("MotionEnabled", &InputSystemComponent::m_motionEnabled)
                ->Field("MouseEnabled", &InputSystemComponent::m_mouseEnabled)
                ->Field("TouchEnabled", &InputSystemComponent::m_touchEnabled)
                ->Field("VirtualKeyboardEnabled", &InputSystemComponent::m_virtualKeyboardEnabled)
                ;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("InputSystemService", 0x5438d51a));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("InputSystemService", 0x5438d51a));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputSystemComponent::InputSystemComponent()
        : m_gamepads()
        , m_keyboard()
        , m_motion()
        , m_mouse()
        , m_touch()
        , m_virtualKeyboard()
        , m_gamepadsEnabled(4)
        , m_keyboardEnabled(true)
        , m_motionEnabled(true)
        , m_mouseEnabled(true)
        , m_touchEnabled(true)
        , m_virtualKeyboardEnabled(true)
        , m_currentlyUpdatingInputDevices(false)
        , m_recreateInputDevicesAfterUpdate(false)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputSystemComponent::~InputSystemComponent()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::Activate()
    {
        // Create all enabled input devices
        CreateEnabledInputDevices();

        InputSystemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        InputSystemRequestBus::Handler::BusDisconnect();

        // Destroy all enabled input devices
        DestroyEnabledInputDevices();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    int InputSystemComponent::GetTickOrder()
    {
        return AZ::ComponentTickBus::TICK_INPUT;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::OnTick(float /*deltaTime*/, AZ::ScriptTimePoint /*scriptTimePoint*/)
    {
        TickInput();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::TickInput()
    {
        InputSystemNotificationBus::Broadcast(&InputSystemNotifications::OnPreInputUpdate);
        m_currentlyUpdatingInputDevices = true;
        InputDeviceRequestBus::Broadcast(&InputDeviceRequests::TickInputDevice);
        m_currentlyUpdatingInputDevices = false;
        InputSystemNotificationBus::Broadcast(&InputSystemNotifications::OnPostInputUpdate);

        if (m_recreateInputDevicesAfterUpdate)
        {
            CreateEnabledInputDevices();
            m_recreateInputDevicesAfterUpdate = false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::RecreateEnabledInputDevices()
    {
        if (m_currentlyUpdatingInputDevices)
        {
            // Delay the request until we've finished updating to protect against getting called in
            // response to an input event, in which case calling CreateEnabledInputDevices here will
            // cause a crash (when the stack unwinds back up to the device which dispatced the event
            // but was then destroyed). An unlikely (but possible) scenario we must protect against.
            m_recreateInputDevicesAfterUpdate = true;
        }
        else
        {
            CreateEnabledInputDevices();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::CreateEnabledInputDevices()
    {
        DestroyEnabledInputDevices();

        m_gamepads.resize(m_gamepadsEnabled);
        for (AZ::u32 i = 0; i < m_gamepadsEnabled; ++i)
        {
            m_gamepads[i].reset(aznew InputDeviceGamepad(i));
        }

        m_keyboard.reset(m_keyboardEnabled ? aznew InputDeviceKeyboard() : nullptr);
        m_motion.reset(m_motionEnabled ? aznew InputDeviceMotion() : nullptr);
        m_mouse.reset(m_mouseEnabled ? aznew InputDeviceMouse() : nullptr);
        m_touch.reset(m_touchEnabled ? aznew InputDeviceTouch() : nullptr);
        m_virtualKeyboard.reset(m_virtualKeyboardEnabled ? aznew InputDeviceVirtualKeyboard() : nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputSystemComponent::DestroyEnabledInputDevices()
    {
        m_virtualKeyboard.reset(nullptr);
        m_touch.reset(nullptr);
        m_mouse.reset(nullptr);
        m_motion.reset(nullptr);
        m_keyboard.reset(nullptr);
        m_gamepads.clear();
    }
} // namespace AZ
