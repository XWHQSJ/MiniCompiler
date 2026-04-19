#include "ast_print.h"
#include <sstream>

namespace minicc {

static std::string xml_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '&':  out += "&amp;"; break;
            case '<':  out += "&lt;"; break;
            case '>':  out += "&gt;"; break;
            case '"':  out += "&quot;"; break;
            default:   out += c; break;
        }
    }
    return out;
}

static std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            default:   out += c; break;
        }
    }
    return out;
}

std::string ast_to_xml(const AstNode* node, int indent) {
    if (!node) return "";
    std::string pad(indent * 2, ' ');
    std::ostringstream ss;

    ss << pad << "<" << node_kind_name(node->kind);
    ss << " text=\"" << xml_escape(node->text) << "\"";
    ss << " line=\"" << node->line << "\"";

    if (node->children.empty()) {
        ss << " />\n";
    } else {
        ss << ">\n";
        for (const auto& ch : node->children) {
            ss << ast_to_xml(ch.get(), indent + 1);
        }
        ss << pad << "</" << node_kind_name(node->kind) << ">\n";
    }
    return ss.str();
}

std::string ast_to_json(const AstNode* node, int indent) {
    if (!node) return "null";
    std::string pad(indent * 2, ' ');
    std::string pad1((indent + 1) * 2, ' ');
    std::ostringstream ss;

    ss << pad << "{\n";
    ss << pad1 << "\"kind\": \"" << node_kind_name(node->kind) << "\",\n";
    ss << pad1 << "\"text\": \"" << json_escape(node->text) << "\",\n";
    ss << pad1 << "\"line\": " << node->line;

    if (!node->children.empty()) {
        ss << ",\n" << pad1 << "\"children\": [\n";
        for (size_t i = 0; i < node->children.size(); ++i) {
            ss << ast_to_json(node->children[i].get(), indent + 2);
            if (i + 1 < node->children.size()) ss << ",";
            ss << "\n";
        }
        ss << pad1 << "]\n";
    } else {
        ss << "\n";
    }
    ss << pad << "}";
    return ss.str();
}

} // namespace minicc
