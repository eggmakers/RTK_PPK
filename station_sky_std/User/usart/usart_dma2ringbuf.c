#include <string.h>
//#include "time.h"
#include "usart_dma2ringbuf.h"
#include "usart.h"

/**
  * @brief  接收字符串.
  * @param  uart_port: uart 端口处理结构的地址.
  * @param  buff: 接收缓冲区.
  * @param  read_len: 读取长度.
  * @retval 取出的数据长度.
  */
uint16_t uart_read_data_unit(UART_HandleTypeDef * huart, uint8_t * buff, uint16_t read_len)
{
    if (huart == NULL)
    {
        return 0;
    }

    usart_dma2ringbuf_t * usart_dma2ringbuf = get_usart_dma2ringbuf(huart);
    usart_dma2ringbuf->tail = usart_dma2ringbuf->uart_dma_rx_buff_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    uint16_t head = usart_dma2ringbuf->head;
    uint16_t tail = usart_dma2ringbuf->tail;
    uint16_t size = usart_dma2ringbuf->uart_dma_rx_buff_size;
    uint16_t len = 0;

    uint8_t* rx_buff = get_uart_rx_buffer(huart);

    if (head < tail)
    {
        /* 长度 */
        len = tail - head;

        if (len >= read_len)
        {
            /* Oversupply. */
            memcpy(buff, rx_buff + head, read_len);
            head += read_len;
            len = read_len;
        }
        else
        {
            /* Less than demand */
            memcpy(buff, rx_buff + head, len);
            head = tail;
        }
    }
    else if (head > tail)
    {
        /* 溢出前的长度 */
        len = size - head;

        if (len >= read_len)
        {
            /* Oversupply. */
            memcpy(buff, rx_buff + head, read_len);
            head += read_len;
            len = read_len;
        }
        else
        {
            /* Less than demand */

            /* 首先将数据处理到缓冲区的末尾 */
            memcpy(buff, rx_buff + head, len);

            if (tail + len >= read_len)
            {
                /* Oversupply. */

                /* 继续从缓冲区开始 */
                memcpy(buff + len, rx_buff, (read_len - len));
                head = (read_len - len);
                len = read_len;
            }
            else
            {
                /* Less than demand */
                /* 继续从缓冲区开始 */
                memcpy(buff + len, rx_buff, tail);
                /* 累加溢出后的长度 */
                len += tail;
                head = tail;
            }
        }
    }

    /* 更新结构体指针 */
    usart_dma2ringbuf->head = head;
    usart_dma2ringbuf->tail = tail;
    usart_dma2ringbuf->available -= len;
    return len;
}

/**
  * @brief  接收字符串.
  * @param  uart_port: uart 端口处理结构的地址.
  * @param  buff: 接收缓冲区.
  * @retval 取出的数据长度.
  */
uint16_t uart_read_data(UART_HandleTypeDef * huart, uint8_t * buff)
{
    if (huart == NULL)
    {
        return 0;
    }

    usart_dma2ringbuf_t * usart_dma2ringbuf = get_usart_dma2ringbuf(huart);
    //usart_dma2ringbuf->tail = usart_dma2ringbuf->uart_dma_rx_buff_size - __HAL_DMA_GET_COUNTER(huart->hdmarx);

    uint16_t head = usart_dma2ringbuf->head;
    uint16_t tail = usart_dma2ringbuf->tail;
    uint16_t size = usart_dma2ringbuf->uart_dma_rx_buff_size;
    uint16_t len = 0;

    uint8_t* rx_buff = get_uart_rx_buffer(huart);

    if (head < tail)
    {
        /* 长度 */
        len = tail - head;
        memcpy(buff, rx_buff + head, len);
        head = tail;
    }
    else if (head > tail)
    {
        /* 溢出前的长度 */
        len = size - head;
        /* 首先将数据处理到缓冲区的末尾 */
        memcpy(buff, rx_buff + head, len);
        /* 继续从缓冲区开始 */
        memcpy(buff + len, rx_buff, tail);
        /* 累加溢出后的长度 */
        len += tail;
        head = tail;
    }

    /* 更新结构体指针 */
    usart_dma2ringbuf->head = head;
    usart_dma2ringbuf->tail = tail;
    usart_dma2ringbuf->available = 0;
    return len;
}

/**
  * @brief  将DMA的数据拷贝到对应的环形队列中
  * @param  None
  * @retval None
  */
void usart_update_ringbuf(UART_HandleTypeDef *huart)
{
    uint16_t head, tail, size, available, empty, now_sapce, counter, last_space;
    usart_dma2ringbuf_t * usart_dma2ringbuf = get_usart_dma2ringbuf(huart);
    uint8_t* rx_buff = get_uart_rx_buffer(huart);

    head = usart_dma2ringbuf->head;
    tail = usart_dma2ringbuf->tail;
    size = usart_dma2ringbuf->uart_dma_rx_buff_size;
    available = usart_dma2ringbuf->available;
    empty = size - available;
    last_space = usart_dma2ringbuf->last_space;
    now_sapce = __HAL_DMA_GET_COUNTER(huart->hdmarx); /* 新接收到字节数 */

    if(last_space >= now_sapce)
    {
        counter = last_space - now_sapce;
    }
    else
    {
        counter = last_space - now_sapce + size;
    }

    last_space = now_sapce;

    /* 更新当前的尾指针 */
    tail = (tail+ counter) % size;

    /* 当前缓冲区没有位置放了，必然要移动头指针 */
    if(empty < counter)
    {
        head = (tail + 1) % size;
    }

    /* 更新已有数据占用数 _available */
    available += counter;

    if(available > size)
    {
        available = size;
    }

    /* 更新结构体指针 */
    usart_dma2ringbuf->head = head;
    usart_dma2ringbuf->tail = tail;
    usart_dma2ringbuf->available = available;
    usart_dma2ringbuf->last_space = last_space;

    /* 防止接收数据错乱 */
    //HAL_UART_Receive_DMA (huart, rx_buff, usart_dma2ringbuf->uart_dma_rx_buff_size);
}

/**
  * @brief  串口空闲中断
  * @param  None
  * @retval None
  */
void USART_IrqHandler (UART_HandleTypeDef *huart)
{
	HAL_UART_IRQHandler(huart);
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) == SET)
    {
        /* clear idle line interrupt flag */
        __HAL_UART_CLEAR_IDLEFLAG(huart);

        /* disable DMA */
        //__HAL_DMA_DISABLE(huart->hdmarx);

        /* disable DMA irq */
        //HAL_DMA_IRQHandler(huart->hdmarx);

        /* update ringbuffer */
        usart_update_ringbuf(huart);

        /* enable DMA */
        //__HAL_DMA_ENABLE(huart->hdmarx);
    }

}

