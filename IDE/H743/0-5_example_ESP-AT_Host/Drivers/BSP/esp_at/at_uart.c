#include <esp_at/at_uart.h>


AT_UART_HandleTypeDef* AT_UART = NULL;


static size_t ATuart_get_readable_bytes(void);


void ATuart_driver_init(UART_HandleTypeDef* atuart) {
    if(atuart  == NULL){
    	return;
    }
    AT_UART->uart_port = atuart;
    // Circular模式: DMA将在这个缓冲区上以环形模式运行
    HAL_UARTEx_ReceiveToIdle_DMA(AT_UART->uart_port, AT_UART->loopbuff, loopbuffer_Size);
}


/** @brief			从环形缓冲读取数据
 *	@param buffer	读取到哪里
 *	@param len		读取缓冲的最大长度
 *	@return			实际读取长度
 *
 */
size_t ATuart_read(uint8_t *buffer, size_t len) {
	size_t writeIndex = loopbuffer_Size - __HAL_DMA_GET_COUNTER(AT_UART->uart_port->hdmarx);
    size_t bytes_to_read = 0;
    // 如果没有数据可读，直接返回
    if (len == 0 || writeIndex == AT_UART->readIndex) {
        return 0;
    }
    // 计算实际可以读取的字节数
    size_t readable = ATuart_get_readable_bytes();
    bytes_to_read = (len > readable) ? readable : len;//实可读长度(防止非法访问)
    // 开始拷贝数据
    if (writeIndex > AT_UART->readIndex) {
        // 情况1: 数据是连续的，直接拷贝
        memcpy(buffer, AT_UART->loopbuff + AT_UART->readIndex, bytes_to_read);
    } else {
        // 情况2: 数据被环形缓冲区分成两段
        size_t first_chunk_len = loopbuffer_Size - AT_UART->readIndex;//前半数据块长度
        if (bytes_to_read <= first_chunk_len) {
            // 需要读取的数据还在第一段内
            memcpy(buffer, AT_UART->loopbuff + AT_UART->readIndex, bytes_to_read);
        } else {
            // 需要读取的数据跨越了两段
            // 拷贝第一段
            memcpy(buffer, AT_UART->loopbuff + AT_UART->readIndex, first_chunk_len);
            // 拷贝第二段
            memcpy(buffer + first_chunk_len, AT_UART->loopbuff, bytes_to_read - first_chunk_len);
        }
    }

    // 更新读指针
    AT_UART->readIndex = (AT_UART->readIndex + bytes_to_read) % loopbuffer_Size;

    return bytes_to_read;
}


// --- 中断回调处理 --- (放到HAL_UARTEx_RxEventCallback中)
static void ATuart_RxCpltHandle(UART_HandleTypeDef *ituart) {
    // 停止DMA，以确保能安全地读取CNDTR寄存器，并且防止在处理过程中发生新的接收
	if(ituart == AT_UART->uart_port){
		//HAL_UART_DMAStop(ituart);

		HAL_UARTEx_ReceiveToIdle_DMA(ituart, AT_UART->loopbuff, loopbuffer_Size);
	}
}


/** 获取已接收但还未读的字节数
 *
 **/
static size_t ATuart_get_readable_bytes(void) {
    // CNDTR寄存器递减计数，表示还剩下多少空间可以写入
    // 总大小 - 剩余空间(已接收 - buf大小) = 当前写入位置
	size_t writeIndex = loopbuffer_Size - __HAL_DMA_GET_COUNTER(AT_UART->uart_port->hdmarx);
    if (writeIndex >= AT_UART->readIndex) {
        // 写指针在读指针后面，未发生回绕
        return writeIndex - AT_UART->readIndex;
    } else {
        // 写指针在读指针前面，发生了回绕
        return loopbuffer_Size - AT_UART->readIndex + writeIndex;
    }
}
