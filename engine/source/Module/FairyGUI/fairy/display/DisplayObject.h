#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/display/NGraphics.h"
#include "FairyGUI/fairy/render/RenderTypes.h"

#include <AzCore/Component/Entity.h>
#include <AzCore/std/string/string.h>
NS_FGUI_BEGIN

class GObject;
class Container;


class DisplayObject
{
public:
    DisplayObject();
    virtual ~DisplayObject();

    void CreateEntity(const AZStd::string& entityName);
    void SetEntity(AZ::EntityId& entityId);
    void DestroyEntity();
    void RemoveFromParent();
    void SetParent(Container* container);
    void RefreshNgraphicsData(DisplayObjRenderContext& context);
    void CreateGraphic();
    void ChangeVisibleState(bool value);

    void retain() {}
    void release() {}

    // ����
    Container* m_parent = nullptr;
    // Entity����ID
    AZ::EntityId m_entityId;
    // Owner ����ָ��
    GObject* m_owner = nullptr;
    // ��Ⱦ�ڵ�
    NGraphics* m_graphics = nullptr;
    // ��Ⱦ����
    DisplayObjRenderContext m_context;
};

NS_FGUI_END