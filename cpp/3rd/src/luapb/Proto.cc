#include "Proto.h"
#include <fstream>

class ErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
    virtual void AddError(const std::string & filename, int line, int column, const std::string & message)
    {
        fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
    }
};

static ErrorCollector s_collector;
static google::protobuf::compiler::DiskSourceTree s_tree;

Proto::Proto() : importer_(&s_tree, &s_collector)
{
    s_tree.MapPath("", "");
}

bool Proto::Import(const std::string& proto)
{
    auto descriptor = importer_.Import(proto);
    if (!descriptor)
    {
        fprintf(stderr, "import (%s) file descriptor error\n", proto.c_str());
        return false;
    }
    return true;
}

google::protobuf::Message* Proto::CreateMessage(const std::string& typeName)
{
    google::protobuf::Message* message = NULL;
    const google::protobuf::Descriptor* descriptor = importer_.pool()->FindMessageTypeByName(typeName);
    if (descriptor)
    {
        const google::protobuf::Message* prototype = factory_.GetPrototype(descriptor);
        if (prototype)
        {
            message = prototype->New();
        }
    }
    return message;
}

Proto& GetProto()
{
    static Proto proto;
    return proto;
}
