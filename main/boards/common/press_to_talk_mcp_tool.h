#ifndef PRESS_TO_TALK_MCP_TOOL_H
#define PRESS_TO_TALK_MCP_TOOL_H

#include "mcp_server.h"
#include "settings.h"

// Lớp công cụ MCP chế độ nói bằng nút có thể tái sử dụng
class PressToTalkMcpTool {
private:
    bool press_to_talk_enabled_;

public:
    PressToTalkMcpTool();
    
    // Khởi tạo công cụ, đăng ký vào máy chủ MCP
    void Initialize();
    
    // Lấy trạng thái chế độ nói bằng nút hiện tại
    bool IsPressToTalkEnabled() const;

private:
    // Hàm callback của công cụ MCP
    ReturnValue HandleSetPressToTalk(const PropertyList& properties);
    
    // Phương thức nội bộ: Thiết lập trạng thái press to talk và lưu vào cài đặt
    void SetPressToTalkEnabled(bool enabled);
};

#endif // PRESS_TO_TALK_MCP_TOOL_H 
