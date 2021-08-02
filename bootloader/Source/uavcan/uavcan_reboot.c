#include "stm32f4xx_hal.h"
#include "uavcan_reboot.h"

extern CanardInstance g_canard1;

/**
 * @brief  外部请求复位.
 * @param  transfer, 数据存储缓冲区.
 * @param  dest, 解码后数据存储地址.
 * @retval None
 */
static int32_t uavcan_reboot_request(CanardRxTransfer* transfer, uavcan_protocol_RestartNodeRequest *dest)
{
    int32_t offset = 0;

    int32_t ret = canardDecodeScalar(transfer, offset, 40, false, (void*)&dest->magic_number);
    if (ret != 40)
    {
        goto error_exit;
    }
    offset += 40;

    return offset;

error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}

/**
 * @brief  回应外部节点重启成功.
 * @param  ins, UAVCAN 实例.
 * @param  transfer, 打包后的数据存储缓冲区.
 * @param  source, 需要发送的数据包.
 * @retval None
 */
static int16_t uavcan_reboot_respond(CanardInstance* ins, CanardRxTransfer* transfer, uavcan_protocol_RestartNodeResponse *source)
{
    uint8_t msg_buf[UAVCAN_PROTOCOL_RESTARTNODE_RESPONSE_MAX_SIZE];

    uint32_t offset = 0;

    canardEncodeScalar(msg_buf, offset, 1, (void*)&source->ok);
    offset += 1;

    return canardRequestOrRespond(ins,
                                  transfer->source_node_id,
                                  UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE,
                                  UAVCAN_PROTOCOL_RESTARTNODE_ID,
                                  &transfer->transfer_id,
                                  transfer->priority,
                                  CanardResponse,
                                  &msg_buf,
                                  BIT_LEN_TO_SIZE(offset));
}

/**
 * @brief  处理节点重启请求.
 * @param  ins, UAVCAN 实例
 * @param  transfer, 打包后的数据存储缓冲区
 * @retval None
 */
void uavcan_handle_node_restart(CanardInstance* ins, CanardRxTransfer* transfer)
{
    /** 外部请求复位. */
    uavcan_protocol_RestartNodeRequest restartRequest;
    uavcan_reboot_request(transfer, &restartRequest);

    /** 回应是否成功. */
    uavcan_protocol_RestartNodeResponse restartResponse;
    restartResponse.ok = false;

    /** 判断魔法数字是否正确,正确则回复True并延时重启. */
    if(restartRequest.magic_number == UAVCAN_PROTOCOL_RESTARTNODE_REQ_MAGIC_NUMBER)
    {
        restartResponse.ok = true;
        uavcan_reboot_respond(ins, transfer, &restartResponse);

        /** Reboot system. */
        HAL_NVIC_SystemReset();
    }
    else
    {
        uavcan_reboot_respond(ins, transfer, &restartResponse);
    }
}
