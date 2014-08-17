#ifndef __BEX_SERIALIZATION_CONFIG_H__
#define __BEX_SERIALIZATION_CONFIG_H__

//////////////////////////////////////////////////////////////////////////
// config serialization.

// 将未定义serialize和serialize_version的自定义类型当作pod类型处理
/**
 * 若非遇到兼容性问题, 不推荐设置此选项!
 */
#if defined(BEX_SERIALIZATION_POD_EXTEND)
# define BEX_SERIALIZATION_USE_POD_EXTEND 1
#endif //defined(BEX_SERIALIZATIOIN_POD_EXTEND)

#endif //__BEX_SERIALIZATION_CONFIG_H__