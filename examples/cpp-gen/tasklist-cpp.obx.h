// Code generated by ObjectBox; DO NOT EDIT.

#pragma once

#include <cstdbool>
#include <cstdint>

#include "flatbuffers/flatbuffers.h"
#include "objectbox-cpp.h"
#include "objectbox.h"


struct Task_;

struct Task {
    using _OBX_MetaInfo = Task_;
    
    uint64_t id;
    std::string text;
    uint64_t date_created;
    uint64_t date_finished;
};

struct Task_ {
    static const obx::Property<Task, OBXPropertyType_Long> id;
    static const obx::Property<Task, OBXPropertyType_String> text;
    static const obx::Property<Task, OBXPropertyType_Date> date_created;
    static const obx::Property<Task, OBXPropertyType_Date> date_finished;

    static constexpr obx_schema_id entityId() { return 1; }

    static void setObjectId(Task& object, obx_id newId) { object.id = newId; }

    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const Task& object);

    /// Read an object from a valid FlatBuffer
    static Task fromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static std::unique_ptr<Task> newFromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, Task& outObject);
};

