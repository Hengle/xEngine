#ifndef __DRAGDROPMANAGER_H__
#define __DRAGDROPMANAGER_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GLoader.h"

NS_FGUI_BEGIN

class DragDropManager
{
public:
    DragDropManager();
    ~DragDropManager();

    static DragDropManager* getInstance();

    GLoader* getAgent() const { return _agent; }
    bool isDragging() const { return _agent->getParent() != nullptr; }
    void startDrag(const AZStd::string& icon, const Value& sourceData = Value::Null, int touchPointID = -1);
    void cancel();

private:
    void onDragEnd(EventContext* context);

    static DragDropManager* _inst;

    GLoader* _agent;
    Value _sourceData;
};

NS_FGUI_END

#endif
