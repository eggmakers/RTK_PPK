#include <string.h>
#include "time.h"
#include "main.h"
#include "uavcan_get_node_info.h"

/** UAVCAN 实例 */
extern CanardInstance g_canard1;
extern CanardInstance g_canard2;

/* 节点信息实例 */
static uavcan_protocol_GetNodeInfoResponse nodeInfo;

/**
  * @brief 软件版本信息编码
  * @arg source: Pointer to source data struct
  * @arg msg_buf: Pointer to msg storage
  * @arg offset: bit offset to msg storage
  * @ret returns message length as bytes
  */
static uint16_t uavcan_protocol_SoftwareVersion_encode(uavcan_protocol_SoftwareVersion *source, void *msg_buf, uint16_t offset)
{
    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->major);
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->minor);
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->optional_field_flags);
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->vcs_commit);
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 64, (void*)&source->image_crc);
    offset += 64;

    return offset;
}

/**
  * @brief 硬件版本信息编码
  * @arg source: Pointer to source data struct
  * @arg msg_buf: Pointer to msg storage
  * @arg offset: bit offset to msg storage
  * @ret returns message length as bytes
  */
static uint16_t uavcan_protocol_HardwareVersion_encode(uavcan_protocol_HardwareVersion *source, void *msg_buf, uint16_t offset)
{
    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->major); /** 255 */
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->minor); /** 255 */
    offset += 8;

    for (uint8_t c = 0; c < 16; c++)
    {
        canardEncodeScalar(msg_buf, offset, 8, (void*)(source->unique_id + c)); /** 255 */
        offset += 8;
    }

    if (true)
    {
        /** 数组长度 */
        canardEncodeScalar(msg_buf, offset, 8, (void*)&source->certificate_of_authenticity.len);
        offset += 8;
    }

    for (uint16_t c = 0; c < source->certificate_of_authenticity.len; c++)
    {
        canardEncodeScalar(msg_buf, offset, 8, (void*)(source->certificate_of_authenticity.data + c)); /** 255 */
        offset += 8;
    }

    return offset;
}

/**
  * @brief 获取节点信息应答编码
  * @arg source: Pointer to source data struct
  * @arg msg_buf: Pointer to msg storage
  * @ret returns message length as bytes
  */
static uint16_t uavcan_protocol_GetNodeInfoResponse_encode(uavcan_protocol_GetNodeInfoResponse *source, void *msg_buf)
{
    uint16_t offset = 0;

    offset = uavcan_protocol_NodeStatus_encode((void*)&source->status, msg_buf, offset);

    offset = uavcan_protocol_SoftwareVersion_encode((void*)&source->software_version, msg_buf, offset);

    offset = uavcan_protocol_HardwareVersion_encode((void*)&source->hardware_version, msg_buf, offset);

    for (uint8_t c = 0; c < source->name.len; c++)
    {
        canardEncodeScalar(msg_buf, offset, 8, (void*)(source->name.data + c));
        offset += 8;
    }

    return offset;
}

#if 1
/**
  * @brief 软件版本信息解码
  * @arg transfer: Pointer to CanardRxTransfer transfer
  * @arg dest: Pointer to destination struct
  * @arg offset: Call with 0, bit offset to msg storage
  * @ret offset or ERROR value if < 0
  */
static int32_t uavcan_protocol_SoftwareVersion_decode(const CanardRxTransfer* transfer, uavcan_protocol_SoftwareVersion* dest, int32_t offset)
{
    int32_t ret = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->major);
    if (ret != 8)
    {
        goto protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->minor);
    if (ret != 8)
    {
        goto protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->optional_field_flags);
    if (ret != 8)
    {
        goto protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->vcs_commit);
    if (ret != 32)
    {
        goto protocol_SoftwareVersion_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 64, false, (void*)&dest->image_crc);
    if (ret != 64)
    {
        goto protocol_SoftwareVersion_error_exit;
    }
    offset += 64;
    return offset;

protocol_SoftwareVersion_error_exit:
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
  * @brief 硬件版本信息解码
  * @arg transfer: Pointer to CanardRxTransfer transfer
  * @arg dest: Pointer to destination struct
  * @arg offset: Call with 0, bit offset to msg storage
  * @ret offset or ERROR value if < 0
  */
static int32_t uavcan_protocol_HardwareVersion_decode(const CanardRxTransfer* transfer, uavcan_protocol_HardwareVersion* dest, int32_t offset)
{
    int32_t ret = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->major);
    if (ret != 8)
    {
        goto protocol_HardwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->minor);
    if (ret != 8)
    {
        goto protocol_HardwareVersion_error_exit;
    }
    offset += 8;

    /** Static array (unique_id) */
    for (uint8_t c = 0; c < 16; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 8, false, (void*)(dest->unique_id + c));
        if (ret != 8)
        {
            goto protocol_HardwareVersion_error_exit;
        }
        offset += 8;
    }

    /** Array length 8 bits */
    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->certificate_of_authenticity.len); /** MAX 255 */
    if (ret != 8)
    {
        goto protocol_HardwareVersion_error_exit;
    }
    offset += 8;

    for (uint8_t c = 0; c < dest->certificate_of_authenticity.len; c++)
    {
        if (dest->certificate_of_authenticity.data)
        {
            ret = canardDecodeScalar(transfer, offset, 8, false, (void*)(dest->certificate_of_authenticity.data + c)); /** MAX 255 */
            if (ret != 8)
            {
                goto protocol_HardwareVersion_error_exit;
            }
        }
        offset += 8;
    }
    return offset;

protocol_HardwareVersion_error_exit:
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
  * @brief 获取节点信息应答解码
  * @arg transfer: Pointer to CanardRxTransfer transfer
  * @arg dest: Pointer to destination struct
  * @ret offset or ERROR value if < 0
  */
static int32_t uavcan_protocol_GetNodeInfoResponse_decode(const CanardRxTransfer* transfer, uavcan_protocol_GetNodeInfoResponse* dest)
{
    int32_t ret = 0;
    int32_t offset = 0;

    /** Node Status. */
    offset = uavcan_protocol_NodeStatus_decode(transfer, (void*)&dest->status, offset);
    if (offset < 0)
    {
        ret = offset;
        goto protocol_GetNodeInfoResponse_error_exit;
    }

    /** Software Version. */
    offset = uavcan_protocol_SoftwareVersion_decode(transfer, (void*)&dest->software_version, offset);
    if (offset < 0)
    {
        ret = offset;
        goto protocol_GetNodeInfoResponse_error_exit;
    }

    /** Hardware Version. */
    offset = uavcan_protocol_HardwareVersion_decode(transfer, (void*)&dest->hardware_version, offset);
    if (offset < 0)
    {
        ret = offset;
        goto protocol_GetNodeInfoResponse_error_exit;
    }

    /** Get array len. */
    dest->name.len = ((transfer->payload_len * 8) - offset ) / 8;

    /** Get Array */
    for (uint8_t c = 0; c < dest->name.len; c++)
    {
        if (dest->name.data)
        {
            ret = canardDecodeScalar(transfer, offset, 8, false, (void*)(dest->name.data + c));
            if (ret != 8)
            {
                goto protocol_GetNodeInfoResponse_error_exit;
            }
        }
        offset += 8;
    }
    return offset;

protocol_GetNodeInfoResponse_error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}
#endif

/**
 * @brief 获取节点信息应答.
 * @arg ins, UAVCAN 实例
 * @arg transfer, 打包后的数据存储缓冲区
 * @ret None
 */
void uavcan_get_node_info_respond(CanardInstance* ins, CanardRxTransfer* transfer)
{
    uint8_t msg_buf[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];

    /** 更新节点状态. */
    memcpy(&nodeInfo.status, uavcan_get_node_status(), sizeof(uavcan_protocol_NodeStatus));

    /** 编码 */
    const uint32_t offset = uavcan_protocol_GetNodeInfoResponse_encode(&nodeInfo, msg_buf);

    const int16_t resp_res = canardRequestOrRespond(ins,
                             transfer->source_node_id,
                             UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
                             UAVCAN_PROTOCOL_GETNODEINFO_ID,
                             &transfer->transfer_id,
                             transfer->priority,
                             CanardResponse,
                             &msg_buf,
                             BIT_LEN_TO_SIZE(offset));
    if (resp_res <= 0)
    {

    }
}

/**
 * @brief 获取 UID.
 * @arg buf, 获取到的 UID 存储地址.
 * @ret None.
 */
static void get_unique_id(uint8_t* buf)
{
    uint32_t part_of_uid;

    part_of_uid = HAL_GetUIDw0();
    memcpy(buf + 0, (uint8_t*)&part_of_uid, sizeof(uint32_t));

    part_of_uid = HAL_GetUIDw1();
    memcpy(buf + 4, (uint8_t*)&part_of_uid, sizeof(uint32_t));

    part_of_uid = HAL_GetUIDw2();
    memcpy(buf + 8, (uint8_t*)&part_of_uid, sizeof(uint32_t));
}

/**
 * @brief 初始化节点信息.
 * @arg None
 * @ret None
 */
void uavcan_node_info_init(void)
{
    /** 节点状态 */
    memcpy(&nodeInfo.status, uavcan_get_node_status(), sizeof(uavcan_protocol_NodeStatus));

    /** 硬件版本 */
    get_unique_id(nodeInfo.hardware_version.unique_id);
    nodeInfo.hardware_version.major = HARDWARE_VERSION_MAJOR;
    nodeInfo.hardware_version.minor = HARDWARE_VERSION_MINOR;

    /** 防伪验证书 */
    const char* certificate_name = APP_NODE_CERTIFICATE;
    nodeInfo.hardware_version.certificate_of_authenticity.data = (uint8_t*)certificate_name;
    nodeInfo.hardware_version.certificate_of_authenticity.len = strlen((char*)nodeInfo.hardware_version.certificate_of_authenticity.data);

    /** 软件版本 */
    nodeInfo.software_version.major = SOFTWARE_VERSION_MAJOR;
    nodeInfo.software_version.minor = SOFTWARE_VERSION_MINOR;
    nodeInfo.software_version.vcs_commit = SOFTWARE_GIT_HASH;
    nodeInfo.software_version.image_crc = 0;
    nodeInfo.software_version.optional_field_flags = 0;

    /** 节点名称 */
    const char* node_name = APP_NODE_NAME;
    nodeInfo.name.data = (uint8_t*)node_name;
    nodeInfo.name.len = strlen((char*)nodeInfo.name.data);
}
