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

#include <AzCore/Input/Devices/InputDevice.h>

#include <AzCore/Input/Buses/Notifications/InputChannelNotificationBus.h>
#include <AzCore/Input/Buses/Notifications/InputDeviceNotificationBus.h>
#include <AzCore/Input/Buses/Notifications/InputTextNotificationBus.h>
#include <AzCore/Input/Buses/Requests/InputChannelRequestBus.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDevice::InputDevice(const InputDeviceId& inputDeviceId)
        : m_inputDeviceId(inputDeviceId)
    {
        InputDeviceRequestBus::Handler::BusConnect(m_inputDeviceId);
        ApplicationLifecycleEvents::Bus::Handler::BusConnect();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDevice::~InputDevice()
    {
        ApplicationLifecycleEvents::Bus::Handler::BusDisconnect();
        InputDeviceRequestBus::Handler::BusDisconnect(m_inputDeviceId);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const InputDevice* InputDevice::GetInputDevice() const
    {
        return this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const InputDeviceId& InputDevice::GetInputDeviceId() const
    {
        return m_inputDeviceId;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const GridMate::PlayerId* InputDevice::GetAssignedLocalPlayerId() const
    {
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::BroadcastInputChannelEvent(const InputChannel& inputChannel) const
    {
        bool hasBeenConsumed = false;
        InputChannelNotificationBus::Broadcast(
            &InputChannelNotifications::OnInputChannelEvent, inputChannel, hasBeenConsumed);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::BroadcastInputTextEvent(const AZStd::string& textUTF8) const
    {
        bool hasBeenConsumed = false;
        InputTextNotificationBus::Broadcast(
            &InputTextNotifications::OnInputTextEvent, textUTF8, hasBeenConsumed);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::BroadcastInputDeviceConnectedEvent() const
    {
        InputDeviceNotificationBus::Broadcast(
            &InputDeviceNotifications::OnInputDeviceConnectedEvent, *this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::BroadcastInputDeviceDisconnectedEvent() const
    {
        InputDeviceNotificationBus::Broadcast(
            &InputDeviceNotifications::OnInputDeviceDisonnectedEvent, *this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::ResetInputChannelStates()
    {
        const InputChannelByIdMap& inputChannelsById = GetInputChannelsById();
        for (const auto& inputChannelById : inputChannelsById)
        {
            // We could do this more efficiently using a const_cast:
            //const_cast<InputChannel*>(inputChannelById.second)->ResetState();
            const InputChannelRequests::BusIdType requestId(inputChannelById.first,
                                                            m_inputDeviceId.GetIndex());
            InputChannelRequestBus::Event(requestId, &InputChannelRequests::ResetState);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::GetInputDeviceIds(InputDeviceIdSet& o_deviceIds) const
    {
        o_deviceIds.insert(m_inputDeviceId);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::GetInputDevicesById(InputDeviceByIdMap& o_devicesById) const
    {
        o_devicesById[m_inputDeviceId] = this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::GetInputChannelIds(InputChannelIdSet& o_channelIds) const
    {
        const InputChannelByIdMap& inputChannelsById = GetInputChannelsById();
        for (const auto& inputChannelById : inputChannelsById)
        {
            o_channelIds.insert(inputChannelById.first);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::GetInputChannelsById(InputChannelByIdMap& o_channelsById) const
    {
        const InputChannelByIdMap& inputChannelsById = GetInputChannelsById();
        for (const auto& inputChannelById : inputChannelsById)
        {
            o_channelsById[inputChannelById.first] = inputChannelById.second;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDevice::OnApplicationConstrained(Event /*lastEvent*/)
    {
        ResetInputChannelStates();
    }
} // namespace AZ
