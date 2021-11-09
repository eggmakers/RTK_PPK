#include <string.h>
#include "stm32f4xx_hal.h"
#include "canard.h"
#include "info.h"
#include "time.h"
#include "can_bus.h"
#include "uavcan_node_status.h"
#include "uavcan_get_node_info.h"
#include "uavcan_allocatee_id.h"
#include "uavcan_file_read.h"
#include "uavcan_reboot.h"
#include "uavcan.h"
#include "GetSet.h"
#include "ExecuteOpcode.h"
#include "param.h"

/** CAN 消息缓冲区大小. */
#define CAN_BUFFER_LEN 100

/**
 * @note CAN1
 */
CanardInstance g_canard1;          /** CAN1 UAVCAN 实例 */
can_result_e   gloal_stat1;        /** Mark global status for CAN1. */
static uint8_t memory_pool1[4096]; /** CAN1 内存池 */

/**
 * @note CAN2
 */
CanardInstance g_canard2;          /** CAN2 UAVCAN 实例 */
can_result_e   gloal_stat2;        /** Mark global status for CAN2. */
static uint8_t memory_pool2[4096]; /** CAN2 内存池 */

/**
 * @note CAN 消息缓冲区.
 */
static struct
{
    /** CAN 消息. */
    struct
    {
        can_port_e port;
        CanardCANFrame frame;
    } can_frame[CAN_BUFFER_LEN];

    uint16_t len;
    uint16_t head;
    uint16_t tail;
} can_frame_ins;

/**
 * @brief 接收完成回调函数, 过滤不需要的消息.
 * @arg ins, 传入 UAVCAN 的实例.
 * @arg out_data_type_signature, 得到消息类型的签名.
 * @arg data_type_id, 消息类型 ID.
 * @arg transfer_type, 传输类型.
 *      CanardTransferTypeResponse, 应答消息.
 *      CanardTransferTypeReques, 请求消息.
 *      CanardTransferTypeBroadcast, 广播消息.
 * @arg source_node_id, 源节点 ID
 * @ret 是否过滤消息
 *      false, 过滤.
 *      true, 不过滤.
 */
static bool shouldAcceptTransfer(const CanardInstance* ins,
                                 uint64_t* out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id)
{
    if (allocation_is_running())
    {
        /** 动态分配 ID. */
        if ((transfer_type == CanardTransferTypeBroadcast) && (data_type_id == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID))
        {
            *out_data_type_signature = UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_SIGNATURE;
            return true;
        }
    }
    else
    {
        if (ins == &g_canard1)
        {
            /** @note  Firmware update only in CAN1. */

            /** 开始固件更新. */
            if ((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID))
            {
                *out_data_type_signature = UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE;
                return true;
            }

            /** 固件读写. */
            if ((transfer_type == CanardTransferTypeResponse) && (data_type_id == UAVCAN_PROTOCOL_FILE_READ_ID))
            {
                *out_data_type_signature = UAVCAN_PROTOCOL_FILE_READ_SIGNATURE;
                return true;
            }
        }
        else if (ins == &g_canard2)
        {

        }
        else
        {

        }

        /** @note  通用消息. */

        /** 节点信息(请求) */
        if ((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
        {
            *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE;
            return true;
        }

        /** 重启. */
        if((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
        {
            *out_data_type_signature = UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE;
            return true;
        }
		if((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID))
        {
			*out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
			return true;
		}
		
		if((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID))
        {
			*out_data_type_signature = UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_SIGNATURE;
			return true;
		}

    }

    return false;
}

/**
 * @brief 接收完成回调函数, 处理接收到的数据.
 * @arg ins, 传入 UAVCAN 的实例.
 * @arg transfer, 接收到的消息. @see CanardRxTransfer
 * @ret None
 */
static void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer)
{
    if (ins == &g_canard1)
    {

    }
    else if (ins == &g_canard2)
    {

    }

    /**
     * @note 通用消息处理.
     */

    /** 节点信息请求响应 */
    if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
    {
        uavcan_get_node_info_respond(ins, transfer);
    }
    /** 动态分配 ID. */
    else if ((transfer->transfer_type == CanardTransferTypeBroadcast) && (transfer->data_type_id == UAVCAN_PROTOCOL_DYNAMIC_NODE_ID_ALLOCATION_ID))
    {
        handle_allocation_data_broadcast(ins, transfer);
    }

    /** 开始固件更新. */
    else if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID))
    {
        handle_begin_firmware_update(ins, transfer);
    }
	
    /** 固件读写. */
    else if ((transfer->transfer_type == CanardTransferTypeResponse) && (transfer->data_type_id == UAVCAN_PROTOCOL_FILE_READ_ID))
    {
        handle_file_read_response(ins, transfer);
    }
	
	else if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID))
	{
		g_handle_param_get_set(ins, transfer);
	}
	
	else if ((transfer->transfer_type == CanardTransferTypeRequest) &&(transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID))
	{
		g_handle_param_execute(ins, transfer);
	}
	

    /** 外部节点发起重启Reboot处理. */
    else if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
    {
        uavcan_handle_node_restart(ins, transfer);
    }
}

/**
 * @brief  UAVCAN 原始消息入队.
 * @param  canx, 指定是从哪个 can 收到的消息.
 * @param  frame, 待入队的数据的指针.
 * @retval None
 */
static void uavcan_frame_push(can_port_e canx, CanardCANFrame *frame)
{
    /** CANx 收到的数据. */
    can_frame_ins.can_frame[can_frame_ins.tail].port = canx;

    /** 拷贝数据. */
    memcpy(&can_frame_ins.can_frame[can_frame_ins.tail].frame, frame, sizeof(CanardCANFrame));

    /** 计算新的 tail 下标. */
    uint16_t new_tail = (can_frame_ins.tail + 1) % CAN_BUFFER_LEN;

    can_frame_ins.tail = new_tail;

    /** 超出范围了. */
    if (++can_frame_ins.len > CAN_BUFFER_LEN)
    {
        can_frame_ins.len = CAN_BUFFER_LEN;

        /** 减少数据的损失. */
        can_frame_ins.head = (can_frame_ins.tail + 1) % CAN_BUFFER_LEN;
    }
}

/**
 * @brief  UAVCAN 原始消息出队.
 * @param  out_frame, 数据取出存放的地址.
 * @retval can 编号.
 */
static can_port_e uavcan_framne_pop(CanardCANFrame *out_frame)
{
    if (can_frame_ins.len == 0)
    {
        return CAN_PORT_RESERVED;
    }

    /** CANx */
    can_port_e canx = can_frame_ins.can_frame[can_frame_ins.head].port;

    /** 取数据. */
    memcpy(out_frame, &can_frame_ins.can_frame[can_frame_ins.head].frame, sizeof(CanardCANFrame));

    /** 计算新指针. */
    uint16_t new_head = (can_frame_ins.head + 1) % CAN_BUFFER_LEN;
    can_frame_ins.head = new_head;

    /** 更新长度. */
    can_frame_ins.len--;

    return canx;
}

/**
 * @brief 发送数据包.
 * @arg port, CAN1 or CAN2
 * @ret None
 */
static void canard_handle_tx(can_port_e port)
{
    CAN_TxHeaderTypeDef tx_header;
    CAN_HandleTypeDef* hcan;
    CanardInstance* ins;
    can_result_e* g_stat;

    /** Get UAVCAN instance. */
    if (port == CAN_P1)
    {
        ins = &g_canard1;
        hcan = get_can1_handle();
        g_stat = &gloal_stat1;
    }
    else if (port == CAN_P2)
    {
        ins = &g_canard2;
        hcan = get_can2_handle();
        g_stat = &gloal_stat2;
    }

    /** 获取队列中的最前面的元素 */
    const CanardCANFrame* txf = canardPeekTxQueue(ins);

    while(txf)
    {
        /** 发送 */
        uint32_t p_tx_mailbox;

        static struct
        {
            uint8_t bus_err;
            uint8_t mailbox_full;
        } err_count;

        tx_header.ExtId = txf->id;
        tx_header.DLC   = txf->data_len;
        tx_header.IDE   = CAN_ID_EXT;
        tx_header.RTR   = CAN_RTR_DATA;

        if (HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)txf->data, &p_tx_mailbox) != HAL_OK)
        {

#define CAN_ERROR_MAX_AMOUNT 10

            if (hcan->ErrorCode & HAL_CAN_ERROR_INTERNAL)
            {
                /** CAN 内部错误. */
                *g_stat = CAN_BUS_ERR;

                err_count.mailbox_full = 0;

                if (++err_count.bus_err >= CAN_ERROR_MAX_AMOUNT)
                {
                    err_count.bus_err = 0;
                    break;
                }
            }
            else if (hcan->ErrorCode & HAL_CAN_ERROR_PARAM)
            {
                /** 无可用邮箱. */
                *g_stat = CAN_MAILBOX_FULL;
                break;

            }
            else if (hcan->ErrorCode & HAL_CAN_ERROR_NOT_INITIALIZED)
            {
                /** CAN 没初始化. */
                *g_stat = CAN_CONFIG_ERR;

                /** break. */
                break;
            }
        }
        else
        {
            /** 弹出已发送的内容 */
            canardPopTxQueue(ins);

            /** 获取下一个元素 */
            txf = canardPeekTxQueue(ins);
        }

        /** Very important. */
        HAL_Delay(1);
    }
}

#if USE_CAN_POLLING
/**
 * @brief 接收并处理数据包.
 * @arg port, CAN1 or CAN2
 * @ret None
 */
static void canard_handle_rx(can_port_e port)
{
    CanardCANFrame rx_frame;
    CanardInstance* ins;
    can_result_e* g_stat;

    /** Get UAVCAN instance. */
    if (port == CAN_P1)
    {
        ins = &g_canard1;
        g_stat = &gloal_stat1;
    }
    else if (port == CAN_P2)
    {
        ins = &g_canard2;
        g_stat = &gloal_stat2;
    }

    /** 接收帧 */
    const int16_t rx_res = canardSTM32Receive(&rx_frame, port);

    if (rx_res < 0)
    {
        /** CAN_INVALID_ARG */
        *g_stat = CAN_INVALID_ARG;
    }
    else if (rx_res > 0)
    {
        /** Everything work great. */
        *g_stat = CAN_BUS_OK;

        /** 处理接收到的帧 */
        canardHandleRxFrame(ins, &rx_frame, micros());
    }
    else
    {
    }
}
#endif

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#define UAVCAN_FRAME_EFF  (1UL << 31U) /** 扩展帧格式 */
#define UAVCAN_FRAME_RTR  (1UL << 30U) /** 遥控远程帧 */

    CAN_RxHeaderTypeDef rx_header;
    CanardCANFrame rx_frame;

    /** 接收数据. */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_frame.data) == HAL_OK)
    {
        rx_frame.data_len = rx_header.DLC; /** Payload length. */

        if (rx_header.IDE == CAN_ID_STD)
        {
            /** 标准帧 */
            rx_frame.id = rx_header.StdId;

            /** 不处理. */
            return;
        }
        else
        {
            /** 扩展帧 */
            rx_frame.id = rx_header.ExtId;
            rx_frame.id |= UAVCAN_FRAME_EFF;
        }

        /** 不是数据帧，而是远程帧（不用于 UAVCAN） */
        if (rx_header.RTR != CAN_RTR_DATA)
        {
            rx_frame.id |= UAVCAN_FRAME_RTR;

            /** 不处理. */
            return;
        }

        if (hcan->Instance == CAN1)
        {
            /** 加入 CAN1 消息队列. */
            uavcan_frame_push(CAN_P1, &rx_frame);
        }
        else if (hcan->Instance == CAN2)
        {
            /** 加到 CAN2 消息队列. */
            uavcan_frame_push(CAN_P2, &rx_frame);
        }
    }
}

/**
 * @brief  uavcan 接收处理函数.
 * @param  None
 * @retval None
 */
void uavcan_handle_rx(void)
{
    CanardCANFrame frame;

    /** 获取数据 */
    can_port_e canx = uavcan_framne_pop(&frame);

    if (canx == CAN_P1)
    {
        canardHandleRxFrame(&g_canard1, &frame, micros());
    }
    else if (canx == CAN_P2)
    {
        canardHandleRxFrame(&g_canard2, &frame, micros());
    }
}

/**
 * @brief 定时清除队列数据.
 * @arg port, CAN1 or CAN2
 * @ret None
 */
static void canard_handle_clean(can_port_e port)
{
    CanardInstance* ins;

    /** Get UAVCAN instance. */
    if (port == CAN_P1)
    {
        ins = &g_canard1;
    }
    else if (port == CAN_P2)
    {
        ins = &g_canard2;
    }

    static uint64_t update = 0;

    if (millis() > update)
    {
        canardCleanupStaleTransfers(ins, millis() * 1000);
        update = millis() + 1000;
    }
}

/**
 * @brief Get CAN status.
 * @arg port, CAN1 or CAN2
 * @ret @see can_result_e
 *     CAN_BUS_OK,         Everything is ok.
 *     CAN_UNSUPPORTED_BR, Unsupported bit rate or supplied clock was incorrect.
 *     CAN_CONFIG_ERR,     The CAN clock and pins was configured incorrectly.
 *     CAN_BUS_ERR,        There are no other nodes in the bus or the CAN chip is broken.
 *     CAN_INVALID_ARG     Invalid arguments.
 */
can_result_e get_can_stat(can_port_e port)
{
    if (port == CAN_P2)
    {
        return gloal_stat2;
    }

    return gloal_stat1;
}

/**
 * @brief  初始化 CAN 消息缓冲区.
 * @param  None
 * @retval None
 */
static void can_buffer_init(void)
{
    can_frame_ins.len = 0;
    can_frame_ins.head = 0;
    can_frame_ins.tail = 0;
}

/**
 * @brief  本地节点 ID 初始化.
 * @param  None
 * @retval None
 */
static void node_id_init(void)
{
    /** Get node ID. */
    uint32_t app_node_id = get_can_node_id();

    /** 检查是否是固件更新, 检查 app_node_id 范围是否合法. */
    if ((app_node_id > 0) && (app_node_id < 127))
    {
        canardSetLocalNodeID(&g_canard1, app_node_id);
        canardSetLocalNodeID(&g_canard2, app_node_id);
    }
    else
    {
        /** 使用匿名 ID. */
        canardSetLocalNodeID(&g_canard1, 0);
        canardSetLocalNodeID(&g_canard2, 0);
    }
}

/**
 * @brief 初始化 UAVCAN
 * @arg None
 * @ret None
 */
can_result_e uavcan_init(void)
{
    can_buffer_init();

    /** 配置 CAN GPIO 及时钟 */
    can_gpio_init();

    /** 配置 CAN 外设. */
    can_config();

    /** 初始化节点状态 */
    uavcan_node_status_init();

    /** 初始化节点信息 */
    uavcan_node_info_init();

    /** 初始化 CAN1 节点 */
    canardInit(&g_canard1,            /* Uninitialized library instance */
               memory_pool1,          /* Raw memory chunk used for dynamic allocation */
               sizeof(memory_pool1),  /* Size of the above, in bytes */
               onTransferReceived,    /* Callback, see CanardOnTransferReception */
               shouldAcceptTransfer,  /* Callback, see CanardShouldAcceptTransfer */
               NULL);

    /** 初始化 CAN2 节点 */
    canardInit(&g_canard2,            /* Uninitialized library instance */
               memory_pool2,          /* Raw memory chunk used for dynamic allocation */
               sizeof(memory_pool2),  /* Size of the above, in bytes */
               onTransferReceived,    /* Callback, see CanardOnTransferReception */
               shouldAcceptTransfer,  /* Callback, see CanardShouldAcceptTransfer */
               NULL);

    /** Setup uavcan node ID. */
    node_id_init();

    /** Setup allocatee configuration. */
    allocation_init();
			   
	g_load_flash_param();

    return CAN_BUS_OK;
}

/**
 * @brief 采用轮询的方式定时调用, 进行数据的收发
 * @arg None
 * @ret None
 */
void uavcan_task(void)
{
    /** 发送数据包 */
    canard_handle_tx(CAN_P1);
    canard_handle_tx(CAN_P2);

#if USE_CAN_POLLING
    /** 接收数据包 */
    canard_handle_rx(CAN_P1);
    canard_handle_rx(CAN_P2);
#else
    uavcan_handle_rx();
#endif

    /** 清除队列数据 */
    canard_handle_clean(CAN_P1);
    canard_handle_clean(CAN_P2);

    /** 发送心跳广播 */
    uavcan_send_heartbeat(CAN_P1);
    uavcan_send_heartbeat(CAN_P2);

    /** Dynamic node ID request. */
    allocation_update();

    /** Read file. */
    upgrade_read_request();
}
