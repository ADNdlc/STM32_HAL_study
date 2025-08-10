/*
 * esp_parser.c
 *
 *  Created on: Aug 10, 2025
 *      Author: 12114
 */

#include <esp_at/at_parser.h>
#include <string.h>
#include <stdio.h> // for printf, for debug

// --- 私有变量 ---
static uint8_t line_buffer[AT_PARSER_LINE_BUFFER_SIZE];
static size_t current_len = 0;			//目前解析出的长度

// --- 外部回调函数 ---
// 当解析器找到一个完整的行时，会调用这个函数
// 你需要在别处实现这个函数，比如在 command_controller.c 中
extern void process_at_line(const char* line);

/** 初始化清零整个缓冲区
 *
 **/
void at_parser_init(void) {
    current_len = 0;
    memset(line_buffer, 0, sizeof(line_buffer));
}


void at_parser_input(const uint8_t* data, size_t len) {
    if (len == 0) {
        return;
    }
    // 检查是否会溢出
    if (current_len + len > AT_PARSER_LINE_BUFFER_SIZE) {
        // 错误处理：缓冲区溢出。可以清空缓冲区或者只追加部分数据。
        // 这里我们选择清空，防止错误数据累积。
        current_len = 0;
        // 实际项目中这里应该记录一个错误日志
        return;
    }

    // 将新数据追加到行缓冲区的末尾
    memcpy(line_buffer + current_len, data, len);
    current_len += len;

    // --- 开始处理行缓冲 ---
    size_t search_offset = 0;
    while (search_offset < current_len) {
        // 查找 \n (通常 \r\n 一起出现，我们以 \n 为准)
        uint8_t* newline_pos = (uint8_t*)memchr(line_buffer + search_offset, '\n', current_len - search_offset);

        if (newline_pos) {
            // 找到了一个换行符
            size_t line_end_index = newline_pos - line_buffer;

            // 临时将换行符替换为 \0，以形成一个标准的C字符串
            // 我们不关心 \r, 它会被当做普通字符处理然后丢弃
            line_buffer[line_end_index] = '\0';
            if (line_end_index > 0 && line_buffer[line_end_index - 1] == '\r') {
                 line_buffer[line_end_index - 1] = '\0';
            }

            // 提取出这一行 (从search_offset开始)
            char* line_to_process = (char*)(line_buffer + search_offset);

            // 过滤掉空行
            if (strlen(line_to_process) > 0) {
                // 将完整的行交给上层处理
                process_at_line(line_to_process);
            }

            // 计算下一轮搜索的起始位置
            search_offset = line_end_index + 1;

        } else {
            // 在剩余的数据中没有找到换行符，退出循环
            break;
        }
    }

    // --- 清理已处理的数据 ---
    if (search_offset > 0) {
        size_t remaining_len = current_len - search_offset;
        // 使用 memmove 而不是 memcpy，因为源和目标区域可能重叠
        memmove(line_buffer, line_buffer + search_offset, remaining_len);
        current_len = remaining_len;
    }
}

