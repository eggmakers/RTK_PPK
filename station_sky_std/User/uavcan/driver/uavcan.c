#include <string.h>
#include <stdio.h>
#include <math.h>
#include "can_bus.h"
#include "uavcan.h"
#include "canard.h"
#include "gnss.h"
#include "time.h"
 
#include "GnssEcef.h"
#include "Timestamp.h"
#include "RTCMStream.h"
#include "uavcan_reboot.h"
#include "uavcan_update_firmware.h"
#include "uavcan_get_node_info.h"
#include "stm32f4xx_hal.h"
#include "fix2.h"
#include "Auxiliary.h"
#include "MagneticFieldStrength.h"
#include "param.h"

#define CAN1_APP_NODE_ID 19

/** CAN 消息缓冲区大小. */
#define CAN_BUFFER_LEN (1024)

/**
 * @note CAN1
 */
CanardInstance g_canard1;          /** CAN1 UAVCAN 实例 */
static uint8_t memory_pool1[1024*4]; /** CAN1 内存池 */
static can_result_e gloal_stat1;   /** Mark global status for CAN1. */
extern uint8_t SK3_STANDARD;

/**
 * @note CAN 消息缓冲区.
 */
//static struct
struct
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

    /** 节点心跳包 */
    if ((transfer_type == CanardTransferTypeBroadcast) && (data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE;
        return true;
    }
	
    /** 节点信息(请求) */
    if ((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE;
        return true;
    }

    /** 节点信息(回复) */
    if ((transfer_type == CanardTransferTypeResponse) && (data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE;
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
	
	
#if 1
    /** 固件更新. */
    if ((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID))
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_SIGNATURE;
        return true;
    }

    /** 重启. */
    if((transfer_type == CanardTransferTypeRequest) && (data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
    {
        *out_data_type_signature = UAVCAN_PROTOCOL_RESTARTNODE_SIGNATURE;
        return true;
    }
#endif	
	
    /** rtcm数据包 */
    if((transfer_type == CanardTransferTypeBroadcast) && (data_type_id == UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_ID))
    {
        *out_data_type_signature = UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_SIGNATURE;
		//printf("transfer_type: %d\n  data_type_id: %d\n",transfer_type,data_type_id);
        return true;
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
	
	//printf("onTransferReceived\n");
    if (ins == &g_canard1)
    {  
		/** 开始固件更新 */
		if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_FILE_BEGINFIRMWAREUPDATE_ID))
		{
			handle_file_beginfirmwareupdate_request(ins, transfer);
		}
				
		/** 节点信息(回复) */		
		if((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_GETNODEINFO_ID))
		{
			uavcan_get_node_info_respond(ins, transfer);
		}

		if((transfer->transfer_type == CanardTransferTypeBroadcast) && (transfer->data_type_id == UAVCAN_EQUIPMENT_GNSS_RTCMSTREAM_ID))
		{
			printf("Receive the RTCM message\r\n");
			// 接收到RTCM，需要转发到板卡
			handle_rtcm(transfer);
		}
		
//		//重启节点
//		if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_RESTARTNODE_ID))
//		{
//			//printf("Are you onTransferReceived uavcan_handle_node_restart\n");
//			uavcan_handle_node_restart(ins, transfer);
//		}
//		
//		//读、设置参数
//		if ((transfer->transfer_type == CanardTransferTypeRequest) && (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID))
//		{
//			g_handle_param_get_set(ins, transfer);
//		}
//		
//		//执行参数
//		if ((transfer->transfer_type == CanardTransferTypeRequest) &&(transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_EXECUTEOPCODE_ID))
//		{
//			g_handle_param_execute(ins, transfer);
//		}

	}
}

/**
 * @brief  UAVCAN 原始消息入队.
 * @param  canx, 指定是从哪个 can 收到的消息.
 * @param  frame, 待入队的数据的指针.
 * @retval None
 */
void uavcan_framne_push(can_port_e canx, CanardCANFrame *frame)
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
can_port_e uavcan_framne_pop(CanardCANFrame *out_frame)
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
//static void canard_handle_tx(can_port_e port)
//{
//    CAN_TxHeaderTypeDef tx_header;
//    CAN_HandleTypeDef* hcan;
//    CanardInstance* ins;
//    can_result_e* g_stat;
//	
//    /** Get UAVCAN instance. */
//    if (port == CAN_P1)
//    {
//        ins = &g_canard1;
//        hcan = get_can1_handle();
//        g_stat = &gloal_stat1;
//    }
//    /** 获取队列中的最前面的元素 */
//    const CanardCANFrame* txf = canardPeekTxQueue(ins);

//    while(txf)
//    {
//		if(HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0)
//		{
//			/** 发送 */
//			uint32_t p_tx_mailbox;

//			tx_header.ExtId = txf->id;
//			tx_header.DLC   = txf->data_len;
//			tx_header.IDE   = CAN_ID_EXT;
//			tx_header.RTR   = CAN_RTR_DATA;
//			tx_header.TransmitGlobalTime = DISABLE;
//			if (HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)txf->data, &p_tx_mailbox) != HAL_OK)
//			{
//				break;
//			}
//			else
//			{
//				/** 弹出已发送的内容 */
//				canardPopTxQueue(ins);
//				/** 获取下一个元素 */
//				txf = canardPeekTxQueue(ins);
//			}
//			/** Very important. */
//			HAL_Delay(1);			
//		}
//		else
//		{
//			break;
//		}
//    }
//}


static void canard_handle_tx(can_port_e port)
{
    CAN_TxHeaderTypeDef tx_header;
    CAN_HandleTypeDef* hcan;
    CanardInstance* ins;
//    can_result_e* g_stat;
	
    /** Get UAVCAN instance. */
    if (port == CAN_P1)
    {
        ins = &g_canard1;
        hcan = get_can1_handle();
//        g_stat = &gloal_stat1;
    }
    /** 获取队列中的最前面的元素 */
    //const CanardCANFrame* txf = canardPeekTxQueue(ins);

    //while(txf)
	for(const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(ins)) != NULL;)
    {
			if(HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0)
			{
				/** 发送 */
				uint32_t p_tx_mailbox;

				tx_header.ExtId = txf->id;
				tx_header.DLC   = txf->data_len;
				tx_header.IDE   = CAN_ID_EXT;
				tx_header.RTR   = CAN_RTR_DATA;
				tx_header.TransmitGlobalTime = DISABLE;
				if (HAL_CAN_AddTxMessage(hcan, &tx_header, (uint8_t *)txf->data, &p_tx_mailbox) == HAL_OK)
				{
					//if((__HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TXOK0)==true)||(__HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TXOK1)==true)||(__HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TXOK1)==true))
					//canardPopTxQueue(ins);
					//{
					//canardPopTxQueue(ins);
					//__HAL_CAN_CLEAR_FLAG(hcan,CAN_FLAG_TXOK0);
					//__HAL_CAN_CLEAR_FLAG(hcan,CAN_FLAG_TXOK1);	
					//__HAL_CAN_CLEAR_FLAG(hcan,CAN_FLAG_TXOK2);	
					//}
					canardPopTxQueue(ins);
					
				}
				else
				{
					/** 弹出已发送的内容 */
					
					/** 获取下一个元素 */
					//txf = canardPeekTxQueue(ins);
				}
				/** Very important(wait for txmaibox sned finish). */
				//	delay_us(125);//时间太短，CAN上数据太多时丢包	
				HAL_Delay(1);
			}
			else
			{
				break;
			}
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
        /** There are no other nodes in the bus or the CAN chip is broken. */
        *g_stat = CAN_BUS_ERR;
    }
}
#endif

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */ 
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#define UAVCAN_FRAME_EFF  (1UL << 31U) /** 扩展帧格式 */
#define UAVCAN_FRAME_RTR  (1UL << 30U) /** 遥控远程帧 */

    CAN_RxHeaderTypeDef rx_header;
    CanardCANFrame rx_frame;

    /** 接收数据. */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rx_header, rx_frame.data) == HAL_OK)
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
				uavcan_framne_push(CAN_P1, &rx_frame);    	
        }
        else if (hcan->Instance == CAN2)
        {
            /** 加到 CAN2 消息队列. */
			//uavcan_framne_push(CAN_P2, &rx_frame);
        }
    }
}


/*
 * @brief  uavcan 接收处理函数.
 * @param  None
 * @retval None
 */
void uavcan_handle_rx(void)
{
    CanardCANFrame frame;

    /** 获取数据 */
    can_port_e canx = uavcan_framne_pop(&frame);

	while (canx != CAN_PORT_RESERVED)
	{
		if (canx == CAN_P1)
		{
			canardHandleRxFrame(&g_canard1, &frame, micros());
		}
		
		canx = uavcan_framne_pop(&frame);
	}

}
/*
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
    if (port == CAN_P1)
    {
        return gloal_stat1;
    }
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
/*
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

    /** 设置本地节点 ID */
    canardSetLocalNodeID(&g_canard1, CAN1_APP_NODE_ID);
			   
	g_load_flash_param();

			   
    return CAN_BUS_OK;
}



/**
 * @brief 采用中断的方式接收数据,然后在while(1)中进行数据处理
 * @arg None
 * @ret None
 */
void uavcan_task(void)
{

    /** 发送数据包 */
    canard_handle_tx(CAN_P1);
   // canard_handle_tx(CAN_P2);

#if USE_CAN_POLLING
    /** 接收数据包 */
    canard_handle_rx(CAN_P1);
    canard_handle_rx(CAN_P2);
#else
    uavcan_handle_rx();
#endif

    /** 清除队列数据 */
    canard_handle_clean(CAN_P1);
    //canard_handle_clean(CAN_P2);

	//if (SK3_STANDARD == 1)
	//{
		//sk3, ecef
	   send_gnss();
	//}
	//else if (SK3_STANDARD == 2)
	//{
	
		//标准化
//		send_fix2();
//		
//		send_MagneticFieldStrength();
//		
//		send_auxiliary();
	//}


    /** 发送心跳广播 */
    uavcan_send_heartbeat(CAN_P1);

    /** 获取节点信息请求 */
    //uavcan_get_node_info();
}

