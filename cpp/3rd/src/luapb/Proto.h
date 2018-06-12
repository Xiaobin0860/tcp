#pragma once

#include <string>

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>


class Proto
{
public:
    Proto();
public:
    bool Import(const std::string& proto);
    google::protobuf::Message* CreateMessage(const std::string& typeName);
private:
    google::protobuf::compiler::Importer importer_;
    google::protobuf::DynamicMessageFactory factory_;
};

Proto& GetProto();
