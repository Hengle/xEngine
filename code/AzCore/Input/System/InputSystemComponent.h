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
#pragma once

#include <AzCore/Input/Buses/Requests/InputSystemRequestBus.h>
#include <AzCore/Input/Buses/Requests/InputSystemCursorRequestBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <AzCore/std/containers/vector.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    class InputDeviceGamepad;
    class InputDeviceKeyboard;
    class InputDeviceMotion;
    class InputDeviceMouse;
    class InputDeviceTouch;
    class InputDeviceVirtualKeyboard;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //! This system component manages instances of the default input devices supported by the engine.
    //! Other systems/modules/gems/games are free to create additional input device instances of any
    //! type; this system component manages devices that are supported "out of the box", which other
    //! systems (and most games) will expect to be available for platforms where they are supported.
    class InputSystemComponent : public AZ::Component
                               , public AZ::TickBus::Handler
                               , public InputSystemRequestBus::Handler
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component Setup
        AZ_COMPONENT(InputSystemComponent, "{CAF3A025-FAC9-4537-B99E-0A800A9326DF}")

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::ComponentDescriptor::GetProvidedServices
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::ComponentDescriptor::GetIncompatibleServices
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::ComponentDescriptor::Reflect
        static void Reflect(AZ::ReflectContext* reflection);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Constructor
        InputSystemComponent();

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destructor
        ~InputSystemComponent() override;

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::Component::Activate
        void Activate() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::Component::Deactivate
        void Deactivate() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::TickEvents::GetTickOrder
        int GetTickOrder() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::TickEvents::OnTick
        void OnTick(float deltaTime, AZ::ScriptTimePoint scriptTimePoint) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputSystemRequests::TickInput
        void TickInput() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputSystemRequests::RecreateEnabledInputDevices
        void RecreateEnabledInputDevices() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Create enabled input devices
        void CreateEnabledInputDevices();

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destroy enabled input devices
        void DestroyEnabledInputDevices();

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Disable copy constructor
        InputSystemComponent(const InputSystemComponent&) = delete;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Input Device Variables
        AZStd::vector<AZStd::unique_ptr<InputDeviceGamepad>> m_gamepads;        //!< Game-pad devices
        AZStd::unique_ptr<InputDeviceKeyboard>               m_keyboard;        //!< Keyboard device
        AZStd::unique_ptr<InputDeviceMotion>                 m_motion;          //!< Motion device
        AZStd::unique_ptr<InputDeviceMouse>                  m_mouse;           //!< Mouse device
        AZStd::unique_ptr<InputDeviceTouch>                  m_touch;           //!< Touch device
        AZStd::unique_ptr<InputDeviceVirtualKeyboard>        m_virtualKeyboard; //!< Virtual keyboard device

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Serialized Variables
        AZ::u32 m_gamepadsEnabled;        //!< The number of enabled game-pads
        bool    m_keyboardEnabled;        //!< Is the keyboard enabled?
        bool    m_motionEnabled;          //!< Is motion enabled?
        bool    m_mouseEnabled;           //!< Is the mouse enabled?
        bool    m_touchEnabled;           //!< Is touch enabled?
        bool    m_virtualKeyboardEnabled; //!< Is the virtual keyboard enabled?

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Other Variables
        bool m_currentlyUpdatingInputDevices;   //!< Are we currently updating input devices?
        bool m_recreateInputDevicesAfterUpdate; //!< Should we recreate devices after update?
    };
} // namespace AZ